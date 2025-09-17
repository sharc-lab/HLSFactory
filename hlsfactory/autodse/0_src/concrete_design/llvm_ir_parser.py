#!/usr/bin/env python3
"""
llvm_ir_parser.py
  · Build a call hierarchy from a .ll file
  · Attach loop names from HLS pragmas
  · Attach *source-line numbers* via Vivado-style  !dbg !N →  metadata !{ i32 … }
"""

from __future__ import annotations
import re, json, textwrap
from pathlib import Path
from typing import Dict, Set, List, Optional, Tuple

# ── data records ────────────────────────────────────────────────────────────
class FunctionInfo:
    __slots__ = ("name", "callees", "loop_headers")
    def __init__(self, name: str):
        self.name          = name
        self.callees: Set[str] = set()
        self.loop_headers: Set[str] = set()

class CallEdge:
    __slots__ = ("callee", "dbg_id", "line", "loop_tag")
    def __init__(self, callee: str, dbg_id: Optional[int]):
        self.callee   = callee
        self.dbg_id   = dbg_id   # !dbg id; resolved later
        self.line     = None     # type: Optional[int]
        self.loop_tag = None     # type: Optional[str]

# ── main parser ─────────────────────────────────────────────────────────────
class LLVMIRParser:
    # --- core patterns ------------------------------------------------------
    _DEFINE_RE   = re.compile(r'^\s*define\s+[^@]*@(?:"([^"]+)"|([\w.$-]+))\s*\(')
    _CALL_RE     = re.compile(r'\bcall\b[^@]*@(?:"([^"]+)"|([\w.$-]+))')
    _DBGLINK_RE  = re.compile(r'!dbg\s*!([0-9]+)')

    # --- loop-pragma helpers -----------------------------------------------
    _GLOB_STR_RE = re.compile(r'^\s*@([\w.$-]+)\s*=.*c"([^"]*)')
    _LOOPNAME_RE = re.compile(r'@_ssdm_op_SpecLoopName\([^@]*@([\w.$-]+)\)')
    _LOOPTRIP_RE = re.compile(r'@_ssdm_op_SpecLoopTripCount')

    # --- Vivado-style metadata lines  (***NEW / FIX***: allow leading spaces)
    _META_LOC_RE  = re.compile(
        r'^\s*!([0-9]+)\s*=\s*metadata\s*!{\s*i32\s+([0-9]+),\s*i32\s+[0-9]+,'
        r'\s*metadata\s*!([0-9]+)')
    _META_LEX_RE  = re.compile(
        r'^\s*!([0-9]+)\s*=\s*metadata\s*!{\s*i32\s+786443,\s*metadata\s*!([0-9]+),'
        r'\s*i32\s+([0-9]+)')

    def __init__(self, ll_file: str | Path, input_c_hierarchy_path: str):
        self.ll_path = Path(ll_file)
        if not self.ll_path.exists():
            raise FileNotFoundError(self.ll_path)

        # data to fill
        self.functions: Dict[str, FunctionInfo] = {}
        self.edges:     Dict[str, List[CallEdge]] = {}
        self.loop_string: Dict[str, str] = {}   # @gv -> "HLSAUTOLOOP7"
        #            id -> (line, parent)
        self.meta_map: Dict[int, Tuple[Optional[int], Optional[int]]] = {}
        # ── C-source hierarchy: we use it to validate dbg line numbers ──
        self.valid_src_lines: Set[int] = set()
        self._load_source_hierarchy(self.ll_path.parent / input_c_hierarchy_path)



    # ── public helpers ------------------------------------------------------
    def build_llvm_ir_hierarchy_from_top(self, top: str):
        self._parse_two_passes()
        self._hierarchy = self._dfs_build(top, set())

    def save_hierarchy_to_json(self, out: str | Path):
        Path(out).write_text(json.dumps(self._hierarchy, indent=2), encoding="utf-8")

    def save_pretty_print_hierarchy_to_txt(self, out: str | Path, width: int = 120):
        with Path(out).open("w", encoding="utf-8") as fp:
            self._pretty(self._hierarchy, fp, 0, width)

    # ── parsing -------------------------------------------------------------
    def _parse_two_passes(self):
        lines = self.ll_path.read_text(encoding="utf-8", errors="ignore").splitlines()

        # pass-0 : gather loop strings & ALL metadata
        for ln in lines:
            if m := self._GLOB_STR_RE.match(ln):
                self.loop_string[m.group(1)] = m.group(2).split("\\00")[0]
                continue
            if m := self._META_LOC_RE.match(ln):
                self.meta_map[int(m.group(1))] = (int(m.group(2)), int(m.group(3)))
                continue
            if m := self._META_LEX_RE.match(ln):
                self.meta_map[int(m.group(1))] = (int(m.group(3)), int(m.group(2)))
                continue

        # pass-1 : walk functions & calls
        cur_fn: Optional[str] = None
        for ln in lines:
            if m := self._DEFINE_RE.match(ln):
                cur_fn = m.group(1) or m.group(2)
                self.functions.setdefault(cur_fn, FunctionInfo(cur_fn))
                self.edges.setdefault(cur_fn, [])
                continue
            if ln.strip() == '}' and cur_fn:
                cur_fn = None
                continue
            if cur_fn is None:
                continue

            # loop-pragma inside fn
            if self._LOOPTRIP_RE.search(ln) or self._LOOPNAME_RE.search(ln):
                for gv in self._LOOPNAME_RE.findall(ln):
                    if gv in self.loop_string:
                        self.functions[cur_fn].loop_headers.add(self.loop_string[gv])

            # calls
            if not (call_targets := [c1 or c2 for c1, c2 in self._CALL_RE.findall(ln)]):
                continue
            dbg_match = self._DBGLINK_RE.search(ln)
            dbg_id = int(dbg_match.group(1)) if dbg_match else None
            for callee in call_targets:
                if callee.startswith(('_ssdm_', 'llvm.')):  # skip intrinsics
                    continue
                self.functions[cur_fn].callees.add(callee)
                self.edges[cur_fn].append(CallEdge(callee, dbg_id))

        # resolve dbg-ids to line numbers + fallback loop tags
        for caller, elist in self.edges.items():
            outer_loop = next(iter(self.functions[caller].loop_headers), None)
            for e in elist:
                e.line = self._resolve_dbg_line(e.dbg_id)
                if e.line is None and outer_loop:
                    e.loop_tag = outer_loop

    # ---- dbg-line resolution (unchanged) -----------------------------------
    def _resolve_dbg_line(self, dbg_id: Optional[int]) -> Optional[int]:
        """Walk dbg → metadata chain until we hit a line **that exists** in
        input_c_hierarchy.json (or until we run out)."""
        cur = dbg_id
        visited: Set[int] = set()
        while cur is not None and cur not in visited:
            visited.add(cur)
            line, parent = self.meta_map.get(cur, (None, None))
            if line is not None:
                if not self.valid_src_lines or line in self.valid_src_lines:
                    return line        # good line
            cur = parent               # climb once more
        return None

    # ---- hierarchy / DFS ---------------------------------------------------
    def _dfs_build(self, fn: str, seen: Set[str]) -> dict:
        if fn in seen:
            return {"name": fn, "recursion": True}
        seen.add(fn)
        fi = self.functions.get(fn, FunctionInfo(fn))
        node = {"name": fi.name}
        if fi.loop_headers:
            node["loops"] = sorted(fi.loop_headers)

        children = []
        for edge in self.edges.get(fn, []):
            child = self._dfs_build(edge.callee, seen)
            # if edge.line is not None:
            #     child["line"] = edge.line
            # elif edge.loop_tag:
            #     child["loop"] = edge.loop_tag
            if edge.line is not None:
                child["line"] = edge.line
            else:
                child["line"] = "<no .dbg! in module ir>"
            children.append(child)
        if children:
            node["callees"] = children
        return node

    # ---- pretty printer (fixed) -------------------------------------------
    def _pretty(self, n: dict, fp, indent: int, width: int):
        pad   = "  " * indent           # two-space indent per depth
        parts = [n["name"]]

        if "loops" in n:
            parts.append(f"[loops: {', '.join(n['loops'])}]")
        if "line" in n:
            parts.append(f"[line {n['line']}]")
        if "loop" in n:
            parts.append(f"[loop: {n['loop']}]")
        if "recursion" in n:
            parts.append("[<recursion>]")

        line = pad + " ".join(parts)
        if width and len(line) > width:
            line = line[:width - 3] + "..."

        fp.write(line + "\n")
        for c in n.get("callees", []):
            self._pretty(c, fp, indent + 1, width)


    # ------------------------------------------------------------------ NEW
    def _load_source_hierarchy(self, json_path: Path):
        """Fill self.valid_src_lines with every 'line' that belongs to a
        loop or call in input_c_hierarchy.json (if the file exists)."""
        if not json_path.exists():
            return                                    # nothing to do
        import json
        def collect(node):
            if isinstance(node, dict):
                if node.get("type") in ("loop", "call") and "line" in node:
                    self.valid_src_lines.add(int(node["line"]))
                for key in ("children",):             # recursive walk
                    for child in node.get(key, []):
                        collect(child)
            elif isinstance(node, list):
                for item in node:
                    collect(item)
        with json_path.open() as fp:
            collect(json.load(fp))
