#!/usr/bin/env python3
"""Add simple csim testbenches for all auto_ntt task designs.

Pasta forces TAPA_TARGET_=XILINX_HLS (no host runtime / glog). Csim compiles
design + TB with -DHLSFACTORY_CSIM so vendored tapa xilinx/hls headers expose
hls::stream-backed istream/ostream (without defining __SYNTHESIS__, which breaks
ap_int half types under Vitis clang).

dual_interface_FIFO_{send,receive} use for(;;) — patched under HLSFACTORY_CSIM
to exit when input streams are empty.
"""

from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SRC_ROOT = ROOT / "hlsfactory" / "hls_dataset_sources" / "auto_ntt"
DATASET = "auto_ntt"
PART = "xczu3eg-sbva484-1-e"
CLOCK = "4"
CFLAGS = '-Isrc/ -std=c++17 -DHLSFACTORY_CSIM'

TASKS = [
    "BU",
    "dual_interface_FIFO_receive",
    "dual_interface_FIFO_send",
    "fwd_load_inv_store_poly_1_limbs",
    "fwd_store_inv_load_poly_1_limbs",
    "input_selector_single",
    "Mmap2Stream_tf_0_1_limbs",
    "output_slector_single",
    "shuffler_buf",
    "shuffler_in",
    "shuffler_out_shift",
    "shuffler_out_shuff",
    "TFBuf_wiFW_0",
    "TFBuf_wiFW_1",
    "TFBuf_wiFW_2",
    "TFBuf_woFW_3",
]

COMMON_HDR = r"""// Auto-generated AutoNTT csim host (HLSFactory).
// Drive Pasta HLS stream shims (requires -D__SYNTHESIS__).
#include <tapa.h>
#include "ntt.h"
#include <cstdio>
#include <cstdint>

template <typename T>
static inline void hf_write(tapa::istream<T>& s, const T& v) {
  s._.write({v, false});
}

template <typename T>
static inline bool hf_read(tapa::ostream<T>& s, T& v) {
  if (s._.empty()) return false;
  v = s._.read().val;
  return true;
}

template <typename T>
static inline void hf_drain(tapa::ostream<T>& s) {
  T tmp;
  while (hf_read(s, tmp)) {
  }
}

template <typename T, int N>
static inline void hf_drain_arr(tapa::ostream<T> (&s)[N]) {
  for (int i = 0; i < N; ++i) hf_drain(s[i]);
}
"""


def write_csim_tcl(path: Path, top: str) -> None:
    path.write_text(
        "\n".join(
            [
                f"open_project {top}_prj",
                f"set_top {top}",
                "",
                f'add_files {top}.cpp -cflags "{CFLAGS}"',
                "",
                f'add_files -tb host.cpp -cflags "{CFLAGS}"',
                "",
                "open_solution sol1",
                f"set_part {{{PART}}}",
                f"create_clock -period {CLOCK} -name default",
                "",
                "csim_design",
                "exit",
                "",
            ]
        )
    )


def write_toml(path: Path, design_name: str) -> None:
    path.write_text(
        "\n".join(
            [
                f'design_name = "{design_name}"',
                f'dataset_name = "{DATASET}"',
                "",
                "[[flow_configs]]",
                'flow_name = "VitisHLSSynthFlow"',
                'synth_tcl = "dataset_hls.tcl"',
                "",
                "[[flow_configs]]",
                'flow_name = "VitisHLSCsimFlow"',
                'csim_tcl = "dataset_hls_csim.tcl"',
                "",
            ]
        )
    )


def patch_infinite_loops(cpp_path: Path) -> None:
    text = cpp_path.read_text()
    if "HLSFACTORY_CSIM" in text and "inpStreamNotEmpty" in text:
        # already patched
        if "break;" in text and "#ifdef HLSFACTORY_CSIM" in text:
            return

    # dual_interface_FIFO_receive / send share the same loop shape
    old = """  bool inpStreamNotEmpty;
  for(;;){
    
    inpStreamNotEmpty"""
    new = """  bool inpStreamNotEmpty;
  for(;;){
#ifdef HLSFACTORY_CSIM
    // Csim: exit when no pending input (kernels are otherwise infinite).
#endif
    inpStreamNotEmpty"""
    if old not in text:
        # try compact
        old2 = "  bool inpStreamNotEmpty;\n  for(;;){\n    \n    inpStreamNotEmpty"
        if old2 not in text:
            raise RuntimeError(f"Could not find infinite loop in {cpp_path}")
        old = old2

    # Prefer a surgical replace of the for(;;) body exit
    # Replace the whole for-loop with CSIM-aware version via regex
    pattern = re.compile(
        r"(  bool inpStreamNotEmpty;\n)"
        r"  for\(;;\)\{\n"
        r"(.*?)"
        r"\n  \}\n",
        re.S,
    )

    def repl(m: re.Match[str]) -> str:
        body = m.group(2)
        # Insert break when empty after computing inpStreamNotEmpty assignment block
        # Find first `if(inpStreamNotEmpty){` and insert before it
        marker = "    if(inpStreamNotEmpty){"
        if marker not in body:
            raise RuntimeError(f"unexpected loop body in {cpp_path}")
        insert = (
            "#ifdef HLSFACTORY_CSIM\n"
            "    if(!inpStreamNotEmpty) break;\n"
            "#endif\n"
            "    if(inpStreamNotEmpty){"
        )
        body2 = body.replace(marker, insert, 1)
        return m.group(1) + "  for(;;){\n" + body2 + "\n  }\n"

    new_text, n = pattern.subn(repl, text, count=1)
    if n != 1:
        raise RuntimeError(f"Failed to patch infinite loop in {cpp_path}")
    cpp_path.write_text(new_text)


def host_bu() -> str:
    return (
        COMMON_HDR
        + r"""
void BU(
    tapa::istream<WORD>& fwd_inVal0,
    tapa::istream<WORD>& fwd_inVal1,
    tapa::istream<WORD>& inv_inVal0,
    tapa::istream<WORD>& inv_inVal1,
    tapa::istream<WORD>& tf_inVal,
    tapa::ostream<WORD>& fwd_outVal0,
    tapa::ostream<WORD>& fwd_outVal1,
    tapa::ostream<WORD>& inv_outVal0,
    tapa::ostream<WORD>& inv_outVal1,
    WORD q,
    WORD twoInverse,
    WORD_PLUS3 factor,
    bool direction,
    VAR_TYPE_16 iter,
    VAR_TYPE_16 BUG_id,
    VAR_TYPE_16 layer_id
);

int main() {
  tapa::istream<WORD> fwd0, fwd1, inv0, inv1, tf;
  tapa::ostream<WORD> fwd_o0, fwd_o1, inv_o0, inv_o1;

  // iter=0 => 64 * 3 = 192 butterfly steps (direction=fwd)
  const int beats = (N / V_TOTAL_DATA) * ((logN + (H_BUG_SIZE - 1)) / H_BUG_SIZE);
  for (int i = 0; i < beats; ++i) {
    hf_write(fwd0, WORD(i + 1));
    hf_write(fwd1, WORD(i + 3));
    hf_write(tf, WORD(1));
  }

  WORD q = WORD(0x3b800001);  // sample Barrett modulus-shaped constant
  WORD twoInverse = WORD(0x1dc00001);
  WORD_PLUS3 factor = WORD_PLUS3(1);

  BU(fwd0, fwd1, inv0, inv1, tf, fwd_o0, fwd_o1, inv_o0, inv_o1,
     q, twoInverse, factor, /*direction=*/true, /*iter=*/0, /*BUG_id=*/0, /*layer_id=*/0);

  hf_drain(fwd_o0);
  hf_drain(fwd_o1);
  std::printf("BU csim OK (%d beats)\n", beats);
  return 0;
}
"""
    )


def host_dual_receive() -> str:
    return (
        COMMON_HDR
        + r"""
void dual_interface_FIFO_receive(
    tapa::istream<WORD>& fwd_inVal,
    tapa::istream<WORD>& inv_inVal,
    tapa::ostream<WORD>& common_outVal,
    bool direction);

int main() {
  tapa::istream<WORD> fwd, inv;
  tapa::ostream<WORD> out;
  for (int i = 0; i < 8; ++i) hf_write(fwd, WORD(100 + i));
  dual_interface_FIFO_receive(fwd, inv, out, true);
  hf_drain(out);
  std::printf("dual_interface_FIFO_receive csim OK\n");
  return 0;
}
"""
    )


def host_dual_send() -> str:
    return (
        COMMON_HDR
        + r"""
void dual_interface_FIFO_send(
    tapa::istream<WORD>& common_inVal,
    tapa::ostream<WORD>& fwd_outVal,
    tapa::ostream<WORD>& inv_outVal,
    bool direction);

int main() {
  tapa::istream<WORD> common;
  tapa::ostream<WORD> fwd, inv;
  for (int i = 0; i < 8; ++i) hf_write(common, WORD(200 + i));
  dual_interface_FIFO_send(common, fwd, inv, true);
  hf_drain(fwd);
  std::printf("dual_interface_FIFO_send csim OK\n");
  return 0;
}
"""
    )


def host_input_selector() -> str:
    return (
        COMMON_HDR
        + r"""
void input_selector_single(
    tapa::istream<WORD>& fwd_valFromLoad_0,
    tapa::istream<WORD>& fwd_valFromShuf,
    tapa::istream<WORD>& inv_valFromBU,
    tapa::ostream<WORD>& fwd_valToBU,
    tapa::ostream<WORD>& inv_valToShuf,
    tapa::ostream<WORD>& inv_valToStore_0,
    bool direction,
    VAR_TYPE_16 iter);

int main() {
  tapa::istream<WORD> from_load, from_shuf, from_bu;
  tapa::ostream<WORD> to_bu, to_shuf, to_store;

  const int dataCount = N / V_TOTAL_DATA;
  const int dfLimit = (logN + (H_BUG_SIZE - 1)) / H_BUG_SIZE;
  // dataFlowIter==0 reads load; later iters read shuf
  for (int i = 0; i < dataCount; ++i) hf_write(from_load, WORD(i + 1));
  for (int i = 0; i < dataCount * (dfLimit - 1); ++i) hf_write(from_shuf, WORD(1000 + i));

  input_selector_single(from_load, from_shuf, from_bu, to_bu, to_shuf, to_store,
                        true, /*iter=*/0);
  hf_drain(to_bu);
  std::printf("input_selector_single csim OK\n");
  return 0;
}
"""
    )


def host_output_selector() -> str:
    return (
        COMMON_HDR
        + r"""
void output_slector_single(
    tapa::istream<WORD>& fwd_valFromBU,
    tapa::istream<WORD>& inv_valFromLoad_0,
    tapa::istream<WORD>& inv_valFromShuf,
    tapa::ostream<WORD>& fwd_valToShuf,
    tapa::ostream<WORD>& fwd_valToStore_0,
    tapa::ostream<WORD>& inv_valToBU,
    bool direction,
    VAR_TYPE_16 iter);

int main() {
  tapa::istream<WORD> from_bu, from_load, from_shuf;
  tapa::ostream<WORD> to_shuf, to_store, to_bu;

  const int dataCount = N / V_TOTAL_DATA;
  const int dfLimit = (logN + (H_BUG_SIZE - 1)) / H_BUG_SIZE;
  for (int i = 0; i < dataCount * dfLimit; ++i) hf_write(from_bu, WORD(i + 7));

  output_slector_single(from_bu, from_load, from_shuf, to_shuf, to_store, to_bu,
                        true, /*iter=*/0);
  hf_drain(to_shuf);
  hf_drain(to_store);
  std::printf("output_slector_single csim OK\n");
  return 0;
}
"""
    )


def host_tfbuf(name: str, load_count: int, has_fwd: bool) -> str:
    decl = (
        f"""
void {name}(
    tapa::istream<DWORD>& loadTFStream,
"""
        + (
            "    tapa::ostream<DWORD>& loadTFToNext,\n"
            if has_fwd
            else ""
        )
        + f"""    tapa::ostream<WORD>& tfToBu_0,
    tapa::ostream<WORD>& tfToBu_1,
    bool direction,
    VAR_TYPE_8 BUG_id,
    VAR_TYPE_8 TFGen_offset,
    VAR_TYPE_8 unit_id,
    VAR_TYPE_16 iter);
"""
    )
    call_args = "load, next, tf0, tf1," if has_fwd else "load, tf0, tf1,"
    extras = (
        "  tapa::ostream<DWORD> next;\n" if has_fwd else ""
    )
    drain_next = "  hf_drain(next);\n" if has_fwd else ""
    return (
        COMMON_HDR
        + decl
        + f"""
int main() {{
  tapa::istream<DWORD> load;
  tapa::ostream<WORD> tf0, tf1;
{extras}  for (int i = 0; i < {load_count}; ++i) {{
    DWORD v = DWORD(i) | (DWORD(i + 1) << WORD_SIZE);
    hf_write(load, v);
  }}

  {name}({call_args}
         true, /*BUG_id=*/0, /*TFGen_offset=*/0, /*unit_id=*/0, /*iter=*/0);
{drain_next}  hf_drain(tf0);
  hf_drain(tf1);
  std::printf("{name} csim OK\\n");
  return 0;
}}
"""
    )


def host_shuffler_in() -> str:
    return (
        COMMON_HDR
        + r"""
void shuffler_in(
    tapa::istreams<WORD, 2*V_BUG_SIZE>& fwd_inVal,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_inVal,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_outVal,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& inv_outVal,
    bool direction,
    VAR_TYPE_16 iter,
    VAR_TYPE_16 task_idx);

int main() {
  tapa::istream<WORD> fwd_in[2 * V_BUG_SIZE];
  tapa::istream<WORD> inv_in[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd_out[2 * V_BUG_SIZE];
  tapa::ostream<WORD> inv_out[2 * V_BUG_SIZE];

  const int dataCount = N / V_TOTAL_DATA;
  const int shuffleLimit = ((logN + (H_BUG_SIZE - 1)) / H_BUG_SIZE) - 1;
  const int beats = dataCount * shuffleLimit;
  for (int i = 0; i < beats; ++i) {
    for (int j = 0; j < 2 * V_BUG_SIZE; ++j) {
      hf_write(fwd_in[j], WORD(i * 16 + j + 1));
    }
  }

  shuffler_in(fwd_in, inv_in, fwd_out, inv_out, true, /*iter=*/0, /*task_idx=*/0);
  hf_drain_arr(fwd_out);
  std::printf("shuffler_in csim OK\n");
  return 0;
}
"""
    )


def host_shuffler_buf() -> str:
    return (
        COMMON_HDR
        + r"""
void shuffler_buf(
    tapa::istreams<WORD, 2*V_BUG_SIZE>& fwd_inVal,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_inVal,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_outVal,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& inv_outVal,
    bool direction,
    VAR_TYPE_16 BUG_id,
    VAR_TYPE_16 iter);

int main() {
  tapa::istream<WORD> fwd_in[2 * V_BUG_SIZE];
  tapa::istream<WORD> inv_in[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd_out[2 * V_BUG_SIZE];
  tapa::ostream<WORD> inv_out[2 * V_BUG_SIZE];

  const int dataCount = N / V_TOTAL_DATA;
  const int shuffleLimit = ((logN + (H_BUG_SIZE - 1)) / H_BUG_SIZE) - 1;
  // Need a full receive window per shuffle stage.
  const int beats = dataCount * shuffleLimit;
  for (int i = 0; i < beats; ++i) {
    for (int j = 0; j < 2 * V_BUG_SIZE; ++j) {
      hf_write(fwd_in[j], WORD(i * 16 + j + 1));
    }
  }

  shuffler_buf(fwd_in, inv_in, fwd_out, inv_out, true, /*BUG_id=*/0, /*iter=*/0);
  hf_drain_arr(fwd_out);
  std::printf("shuffler_buf csim OK\n");
  return 0;
}
"""
    )


def host_shuffler_out_shift() -> str:
    return (
        COMMON_HDR
        + r"""
void shuffler_out_shift(
    tapa::istreams<WORD, 2*V_BUG_SIZE>& fwd_inVal,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_inVal_inter_0,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_inVal_intra,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_outVal_inter_0,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_outVal_intra,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& inv_outVal,
    bool direction,
    VAR_TYPE_16 iter,
    VAR_TYPE_16 task_id);

int main() {
  tapa::istream<WORD> fwd_in[2 * V_BUG_SIZE];
  tapa::istream<WORD> inv_inter[2 * V_BUG_SIZE];
  tapa::istream<WORD> inv_intra[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd_inter[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd_intra[2 * V_BUG_SIZE];
  tapa::ostream<WORD> inv_out[2 * V_BUG_SIZE];

  const int dataCount = N / V_TOTAL_DATA;
  const int shuffleLimit = ((logN + (H_BUG_SIZE - 1)) / H_BUG_SIZE) - 1;
  const int beats = dataCount * shuffleLimit;
  for (int i = 0; i < beats; ++i) {
    for (int j = 0; j < 2 * V_BUG_SIZE; ++j) {
      hf_write(fwd_in[j], WORD(i * 16 + j + 1));
    }
  }

  shuffler_out_shift(fwd_in, inv_inter, inv_intra, fwd_inter, fwd_intra, inv_out,
                     true, /*iter=*/0, /*task_id=*/0);
  hf_drain_arr(fwd_inter);
  hf_drain_arr(fwd_intra);
  std::printf("shuffler_out_shift csim OK\n");
  return 0;
}
"""
    )


def host_shuffler_out_shuff() -> str:
    inter_decls = "\n".join(
        f"  tapa::istream<WORD> fwd_inter{i};" for i in range(16)
    )
    inv_out_decls = "\n".join(
        f"  tapa::ostream<WORD> inv_inter{i};" for i in range(16)
    )
    feed_inter = "\n".join(
        f"      hf_write(fwd_inter{i}, WORD(i * 16 + {i} + 1));" for i in range(16)
    )
    call_inter = ", ".join(f"fwd_inter{i}" for i in range(16))
    call_inv = ", ".join(f"inv_inter{i}" for i in range(16))
    drain_inv = "\n".join(f"  hf_drain(inv_inter{i});" for i in range(16))
    return (
        COMMON_HDR
        + r"""
void shuffler_out_shuff(
    tapa::istream<WORD>& fwd_inVal_inter0_0,
    tapa::istream<WORD>& fwd_inVal_inter0_1,
    tapa::istream<WORD>& fwd_inVal_inter0_2,
    tapa::istream<WORD>& fwd_inVal_inter0_3,
    tapa::istream<WORD>& fwd_inVal_inter0_4,
    tapa::istream<WORD>& fwd_inVal_inter0_5,
    tapa::istream<WORD>& fwd_inVal_inter0_6,
    tapa::istream<WORD>& fwd_inVal_inter0_7,
    tapa::istream<WORD>& fwd_inVal_inter0_8,
    tapa::istream<WORD>& fwd_inVal_inter0_9,
    tapa::istream<WORD>& fwd_inVal_inter0_10,
    tapa::istream<WORD>& fwd_inVal_inter0_11,
    tapa::istream<WORD>& fwd_inVal_inter0_12,
    tapa::istream<WORD>& fwd_inVal_inter0_13,
    tapa::istream<WORD>& fwd_inVal_inter0_14,
    tapa::istream<WORD>& fwd_inVal_inter0_15,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& fwd_inVal_intra,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_inVal,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_outVal,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& inv_outVal_intra,
    tapa::ostream<WORD>& inv_outVal_inter0_0,
    tapa::ostream<WORD>& inv_outVal_inter0_1,
    tapa::ostream<WORD>& inv_outVal_inter0_2,
    tapa::ostream<WORD>& inv_outVal_inter0_3,
    tapa::ostream<WORD>& inv_outVal_inter0_4,
    tapa::ostream<WORD>& inv_outVal_inter0_5,
    tapa::ostream<WORD>& inv_outVal_inter0_6,
    tapa::ostream<WORD>& inv_outVal_inter0_7,
    tapa::ostream<WORD>& inv_outVal_inter0_8,
    tapa::ostream<WORD>& inv_outVal_inter0_9,
    tapa::ostream<WORD>& inv_outVal_inter0_10,
    tapa::ostream<WORD>& inv_outVal_inter0_11,
    tapa::ostream<WORD>& inv_outVal_inter0_12,
    tapa::ostream<WORD>& inv_outVal_inter0_13,
    tapa::ostream<WORD>& inv_outVal_inter0_14,
    tapa::ostream<WORD>& inv_outVal_inter0_15,
    bool direction,
    VAR_TYPE_16 iter);
"""
        + f"""
int main() {{
{inter_decls}
  tapa::istream<WORD> fwd_intra[2 * V_BUG_SIZE];
  tapa::istream<WORD> inv_in[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd_out[2 * V_BUG_SIZE];
  tapa::ostream<WORD> inv_intra[2 * V_BUG_SIZE];
{inv_out_decls}

  const int dataCount = N / V_TOTAL_DATA;
  // Stage 0 reads inter ports; stage 1 reads intra.
  for (int i = 0; i < dataCount; ++i) {{
{feed_inter}
  }}
  for (int i = 0; i < dataCount; ++i) {{
    for (int j = 0; j < 2 * V_BUG_SIZE; ++j) {{
      hf_write(fwd_intra[j], WORD(5000 + i * 16 + j));
    }}
  }}

  shuffler_out_shuff(
      {call_inter},
      fwd_intra, inv_in, fwd_out, inv_intra,
      {call_inv},
      true, /*iter=*/0);

  hf_drain_arr(fwd_out);
{drain_inv}
  std::printf("shuffler_out_shuff csim OK\\n");
  return 0;
}}
"""
    )


def host_mmap_tf() -> str:
    return (
        COMMON_HDR
        + r"""
void Mmap2Stream_tf_0_1_limbs(
    tapa::async_mmap<TF_WIDE_DATA>& tf_mmap,
    tapa::ostreams<DWORD, TF_CONCAT_FACTOR_PER_PARA_LIMB_PORT/2>& tf_stream_L0_0,
    bool direction,
    VAR_TYPE_16 iter);

int main() {
  tapa::async_mmap<TF_WIDE_DATA> tf_mmap;
  tapa::ostream<DWORD> tf_stream[TF_CONCAT_FACTOR_PER_PARA_LIMB_PORT / 2];

  const int total_depth = 242;  // fwd_total_depth in kernel
  for (int i = 0; i < total_depth; ++i) {
    TF_WIDE_DATA v = TF_WIDE_DATA(i + 1);
    hf_write(tf_mmap.read_data, v);
  }

  Mmap2Stream_tf_0_1_limbs(tf_mmap, tf_stream, true, /*iter=*/0);
  hf_drain_arr(tf_stream);
  // Drain addresses the kernel posted
  {
    tapa::async_mmap<TF_WIDE_DATA>::addr_t a;
    while (hf_read(tf_mmap.read_addr, a)) {
    }
  }
  std::printf("Mmap2Stream_tf_0_1_limbs csim OK\n");
  return 0;
}
"""
    )


def host_fwd_load() -> str:
    return (
        COMMON_HDR
        + r"""
void fwd_load_inv_store_poly_1_limbs(
    tapa::async_mmap<POLY_WIDE_DATA>& poly_mmap,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_in_poly_L0_stream0,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_in_poly_L0_stream1,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_in_poly_L0_stream2,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_in_poly_L0_stream3,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_out_poly_L0_stream0,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_out_poly_L0_stream1,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_out_poly_L0_stream2,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_out_poly_L0_stream3,
    bool direction,
    VAR_TYPE_16 iter);

int main() {
  tapa::async_mmap<POLY_WIDE_DATA> poly_mmap;
  tapa::istream<WORD> inv0[2 * V_BUG_SIZE], inv1[2 * V_BUG_SIZE];
  tapa::istream<WORD> inv2[2 * V_BUG_SIZE], inv3[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd0[2 * V_BUG_SIZE], fwd1[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd2[2 * V_BUG_SIZE], fwd3[2 * V_BUG_SIZE];

  const int totalDataCount = (N / V_TOTAL_DATA) * SEQ_BUG_PER_PARA_LIMB_POLY_PORT;
  for (int i = 0; i < totalDataCount; ++i) {
    hf_write(poly_mmap.read_data, POLY_WIDE_DATA(i + 1));
  }

  fwd_load_inv_store_poly_1_limbs(
      poly_mmap, inv0, inv1, inv2, inv3, fwd0, fwd1, fwd2, fwd3,
      /*direction=*/true, /*iter=*/0);

  hf_drain_arr(fwd0);
  hf_drain_arr(fwd1);
  hf_drain_arr(fwd2);
  hf_drain_arr(fwd3);
  {
    tapa::async_mmap<POLY_WIDE_DATA>::addr_t a;
    while (hf_read(poly_mmap.read_addr, a)) {
    }
  }
  std::printf("fwd_load_inv_store_poly_1_limbs csim OK\n");
  return 0;
}
"""
    )


def host_fwd_store() -> str:
    # Use inverse-load path (direction=false): mmap read -> inv output streams
    return (
        COMMON_HDR
        + r"""
void fwd_store_inv_load_poly_1_limbs(
    tapa::async_mmap<POLY_WIDE_DATA>& poly_mmap,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& fwd_in_poly_L0_stream0,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& fwd_in_poly_L0_stream1,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& fwd_in_poly_L0_stream2,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& fwd_in_poly_L0_stream3,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& inv_out_poly_L0_stream0,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& inv_out_poly_L0_stream1,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& inv_out_poly_L0_stream2,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& inv_out_poly_L0_stream3,
    bool direction,
    VAR_TYPE_16 iter);

int main() {
  tapa::async_mmap<POLY_WIDE_DATA> poly_mmap;
  tapa::istream<WORD> fwd0[2 * V_BUG_SIZE], fwd1[2 * V_BUG_SIZE];
  tapa::istream<WORD> fwd2[2 * V_BUG_SIZE], fwd3[2 * V_BUG_SIZE];
  tapa::ostream<WORD> inv0[2 * V_BUG_SIZE], inv1[2 * V_BUG_SIZE];
  tapa::ostream<WORD> inv2[2 * V_BUG_SIZE], inv3[2 * V_BUG_SIZE];

  const int totalDataCount = (N / V_TOTAL_DATA) * SEQ_BUG_PER_PARA_LIMB_POLY_PORT;
  for (int i = 0; i < totalDataCount; ++i) {
    hf_write(poly_mmap.read_data, POLY_WIDE_DATA(i + 11));
  }

  fwd_store_inv_load_poly_1_limbs(
      poly_mmap, fwd0, fwd1, fwd2, fwd3, inv0, inv1, inv2, inv3,
      /*direction=*/false, /*iter=*/0);

  hf_drain_arr(inv0);
  hf_drain_arr(inv1);
  hf_drain_arr(inv2);
  hf_drain_arr(inv3);
  {
    tapa::async_mmap<POLY_WIDE_DATA>::addr_t a;
    while (hf_read(poly_mmap.read_addr, a)) {
    }
  }
  std::printf("fwd_store_inv_load_poly_1_limbs csim OK\n");
  return 0;
}
"""
    )


HOST_BUILDERS = {
    "BU": host_bu,
    "dual_interface_FIFO_receive": host_dual_receive,
    "dual_interface_FIFO_send": host_dual_send,
    "input_selector_single": host_input_selector,
    "output_slector_single": host_output_selector,
    "TFBuf_wiFW_0": lambda: host_tfbuf("TFBuf_wiFW_0", 242, True),
    "TFBuf_wiFW_1": lambda: host_tfbuf("TFBuf_wiFW_1", 207, True),
    "TFBuf_wiFW_2": lambda: host_tfbuf("TFBuf_wiFW_2", 138, True),
    "TFBuf_woFW_3": lambda: host_tfbuf("TFBuf_woFW_3", 69, False),
    "shuffler_in": host_shuffler_in,
    "shuffler_buf": host_shuffler_buf,
    "shuffler_out_shift": host_shuffler_out_shift,
    "shuffler_out_shuff": host_shuffler_out_shuff,
    "Mmap2Stream_tf_0_1_limbs": host_mmap_tf,
    "fwd_load_inv_store_poly_1_limbs": host_fwd_load,
    "fwd_store_inv_load_poly_1_limbs": host_fwd_store,
}


def main() -> None:
    for task in TASKS:
        d = SRC_ROOT / task
        if not d.is_dir():
            raise SystemExit(f"Missing design dir {d}")

        if task in ("dual_interface_FIFO_receive", "dual_interface_FIFO_send"):
            patch_infinite_loops(d / f"{task}.cpp")
            # Keep cpp/ mirror in sync if present
            cpp_mirror = SRC_ROOT / "cpp" / f"{task}.cpp"
            if cpp_mirror.exists():
                patch_infinite_loops(cpp_mirror)

        host = HOST_BUILDERS[task]()
        (d / "host.cpp").write_text(host)
        write_csim_tcl(d / "dataset_hls_csim.tcl", task)
        write_toml(d / "hlsfactory.toml", task)
        print(f"OK  {task}")

    print(f"\nAdded csim hosts for {len(TASKS)} designs under {SRC_ROOT}")


if __name__ == "__main__":
    main()
