"""Fetch upstream XLS DSLX designs into the `xls_upstream` dataset.

Sources are pinned to the same commit the installed XLS binaries were built
from, so the designs and the toolchain always agree.
"""

import argparse
import json
import re
import urllib.request
from dataclasses import dataclass, field
from pathlib import Path

XLS_COMMIT = "998cf6aec2a7b09c9ad14c492fe4fca63f831fb2"
XLS_RELEASE = "v0.0.0-10684-g998cf6aec"
RAW_BASE = f"https://raw.githubusercontent.com/google/xls/{XLS_COMMIT}"

DATASET_NAME = "xls_upstream"
DATASET_DIR = (
    Path(__file__).resolve().parents[1]
    / "hlsfactory"
    / "hls_dataset_sources"
    / DATASET_NAME
)

APACHE = "Apache-2.0"
SOFTFLOAT = "BSD-3-Clause (Berkeley SoftFloat derived)"
GO_MATH = "BSD-3-Clause (Go math library derived)"


@dataclass
class DesignSpec:
    name: str
    upstream_path: str
    top: str
    license: str = APACHE
    tags: list[str] = field(default_factory=list)
    # Imported modules copied in under their repository-relative path, so a
    # dotted DSLX import such as `third_party.xls_go_math.fpsqrt_32` resolves
    # against the design directory itself. Designs stay self-contained.
    extra_files: list[str] = field(default_factory=list)
    # Import roots passed to the flow as `dslx_path`; "." means the design dir.
    dslx_path: str = ""
    # Some designs cannot be scheduled into a single stage; XLS reports the
    # minimum it needs.
    # Recursively vendor the full DSLX import closure instead of listing
    # every dependency by hand. Needed for the larger `modules/` families.
    resolve_imports: bool = False
    pipeline_stages: str = "1"
    extra_settings: dict[str, str] = field(default_factory=dict)


DESIGNS = [
    DesignSpec(
        name="adler32",
        upstream_path="xls/examples/adler32/adler32.x",
        top="main",
        tags=["checksum", "dslx", "upstream"],
    ),
    DesignSpec(
        name="crc32",
        upstream_path="xls/examples/crc32/crc32.x",
        top="main",
        tags=["checksum", "bitwise", "dslx", "upstream"],
    ),
    DesignSpec(
        name="sha256",
        upstream_path="xls/examples/sha256.x",
        top="main",
        tags=["crypto", "hash", "bitwise", "dslx", "upstream"],
    ),
    DesignSpec(
        name="cubic_bezier",
        upstream_path="xls/examples/cubic_bezier.x",
        top="main",
        tags=["geometry", "arithmetic", "dslx", "upstream"],
    ),
    DesignSpec(
        name="idct_chen",
        upstream_path="xls/examples/jpeg/idct_chen.x",
        top="idct",
        tags=["image", "transform", "jpeg", "dslx", "upstream"],
    ),
    DesignSpec(
        name="riscv_simple",
        upstream_path="xls/examples/riscv_simple.x",
        top="run_instruction",
        tags=["processor", "decode", "dslx", "upstream"],
    ),
    DesignSpec(
        name="hack_cpu",
        upstream_path="xls/examples/hack_cpu.x",
        top="cpu",
        tags=["processor", "alu", "dslx", "upstream"],
    ),
    DesignSpec(
        name="fpdiv_2x32",
        upstream_path="third_party/xls_berkeley_softfloat/fpdiv_2x32.x",
        top="fpdiv_2x32",
        license=SOFTFLOAT,
        tags=["floating-point", "division", "dslx", "upstream"],
    ),
    DesignSpec(
        name="fpsqrt_32",
        upstream_path="third_party/xls_go_math/fpsqrt_32.x",
        top="fpsqrt_32",
        license=GO_MATH,
        tags=["floating-point", "sqrt", "dslx", "upstream"],
    ),
    # --- second batch ---------------------------------------------------
    DesignSpec(
        name="prefix_sum",
        upstream_path="xls/examples/prefix_sum.x",
        top="prefix_sum",
        tags=["scan", "array", "dslx", "upstream"],
    ),
    DesignSpec(
        name="sparse_prefix_sum",
        upstream_path="xls/examples/sparse_prefix_sum.x",
        top="sparse_prefix_sum",
        tags=["scan", "array", "dslx", "upstream"],
    ),
    DesignSpec(
        name="lfsr",
        upstream_path="xls/examples/lfsr.x",
        top="lfsr8",
        tags=["prng", "bitwise", "dslx", "upstream"],
    ),
    DesignSpec(
        name="serialized_decomposer",
        upstream_path="xls/examples/serialized_decomposer.x",
        top="serialized_decomposer",
        tags=["proc", "channel", "stream", "dslx", "upstream"],
        extra_settings={"reset": "rst"},
    ),
    DesignSpec(
        name="reorder_queue",
        upstream_path="xls/examples/reorder_queue/reorder_queue.x",
        top="reorder_queue_32_16",
        tags=["proc", "queue", "control", "stateful", "dslx", "upstream"],
        extra_settings={"reset": "rst"},
    ),
    DesignSpec(
        name="fp_trig_reduce",
        upstream_path="third_party/xls_go_math/fp_trig_reduce.x",
        top="fp_trig_reduce_32",
        license=GO_MATH,
        tags=["floating-point", "trig", "dslx", "upstream"],
    ),
    # --- require dslx_path (imports beyond the DSLX stdlib) --------------
    DesignSpec(
        name="fpexp_32",
        upstream_path="third_party/xls_go_math/fpexp_32.x",
        top="fpexp_32",
        license=GO_MATH,
        tags=["floating-point", "exponential", "dslx", "upstream"],
        extra_files=["third_party/xls_berkeley_softfloat/fpdiv_2x32.x"],
        dslx_path=".",
    ),
    DesignSpec(
        name="fp_sincos_32",
        upstream_path="third_party/xls_go_math/fp_sincos_32.x",
        top="fp_sincos_32",
        license=GO_MATH,
        tags=["floating-point", "trig", "dslx", "upstream"],
        extra_files=["third_party/xls_go_math/fp_trig_reduce.x"],
        dslx_path=".",
    ),
    # --- section 1 (substantial families) -------------------------------
    DesignSpec(
        name="aes_encrypt",
        upstream_path="xls/modules/aes/aes.x",
        top="encrypt",
        tags=["crypto", "aes", "block-cipher", "dslx", "upstream"],
        extra_files=[
            "xls/modules/aes/aes_common.x",
            "xls/modules/aes/constants.x",
        ],
        dslx_path=".",
    ),
    DesignSpec(
        name="aes_decrypt",
        upstream_path="xls/modules/aes/aes.x",
        top="decrypt",
        tags=["crypto", "aes", "block-cipher", "dslx", "upstream"],
        extra_files=[
            "xls/modules/aes/aes_common.x",
            "xls/modules/aes/constants.x",
        ],
        dslx_path=".",
    ),
    DesignSpec(
        name="aes_ghash_gf128_mul",
        upstream_path="xls/modules/aes/ghash.x",
        top="gf128_mul",
        tags=["crypto", "aes", "galois-field", "dslx", "upstream"],
        extra_files=[
            "xls/modules/aes/aes.x",
            "xls/modules/aes/aes_common.x",
            "xls/modules/aes/constants.x",
        ],
        dslx_path=".",
    ),
    DesignSpec(
        name="aes_key_schedule",
        upstream_path="xls/modules/aes/aes.x",
        top="create_key_schedule",
        tags=["crypto", "aes", "key-expansion", "dslx", "upstream"],
        extra_files=[
            "xls/modules/aes/aes_common.x",
            "xls/modules/aes/constants.x",
        ],
        dslx_path=".",
    ),
    DesignSpec(
        name="aes_ctr_encrypt",
        upstream_path="xls/modules/aes/aes_ctr.x",
        top="aes_ctr_encrypt",
        tags=["crypto", "aes", "counter-mode", "dslx", "upstream"],
        extra_files=[
            "xls/modules/aes/aes.x",
            "xls/modules/aes/aes_common.x",
            "xls/modules/aes/constants.x",
        ],
        dslx_path=".",
    ),
    DesignSpec(
        name="ws_matmul",
        upstream_path="xls/modules/ml/ws_matmul.x",
        top="ws_2x2",
        tags=["systolic", "matmul", "floating-point", "proc", "dslx", "upstream"],
        pipeline_stages="4",
        extra_settings={"reset": "rst"},
    ),
    DesignSpec(
        name="rle_enc",
        upstream_path="xls/modules/rle/rle_enc.x",
        top="RunLengthEncoder32",
        tags=["compression", "rle", "proc", "stateful", "dslx", "upstream"],
        extra_files=["xls/modules/rle/rle_common.x"],
        dslx_path=".",
        extra_settings={"reset": "rst"},
    ),
    DesignSpec(
        name="rle_dec",
        upstream_path="xls/modules/rle/rle_dec.x",
        top="RunLengthDecoder32",
        tags=["compression", "rle", "proc", "stateful", "dslx", "upstream"],
        extra_files=["xls/modules/rle/rle_common.x"],
        dslx_path=".",
        extra_settings={"reset": "rst"},
    ),
]


def fetch(path: str) -> str:
    with urllib.request.urlopen(f"{RAW_BASE}/{path}") as response:  
        return response.read().decode("utf-8")


DSLX_STDLIB_MODULES = frozenset({
    "std", "apfloat", "float32", "float64", "bfloat16", "hfloat16",
    "fixed_point", "round", "abs_diff", "acm_random", "lza", "float_instances",
})


def resolve_import_closure(root_path: str) -> dict[str, str]:
    """Fetch a module and everything it transitively imports."""
    seen: dict[str, str] = {}
    queue = [root_path]
    while queue:
        path = queue.pop()
        if path in seen:
            continue
        try:
            text = fetch(path)
        except Exception:  
            continue
        seen[path] = text
        for module in re.findall(r"^\s*import\s+([\w.]+)", text, re.M):
            if module.split(".")[0] in DSLX_STDLIB_MODULES:
                continue
            queue.append(module.replace(".", "/") + ".x")
    return seen


def render_toml(spec: DesignSpec, source_name: str) -> str:
    tags = ", ".join(f'"{tag}"' for tag in spec.tags)
    extra = ""
    if spec.dslx_path:
        extra += f'dslx_path = "{spec.dslx_path}"\n'
    for key, value in spec.extra_settings.items():
        extra += f'{key} = "{value}"\n'
    return f"""design_name = "{spec.name}"
dataset_name = "{DATASET_NAME}"
tags = [{tags}]

[[flow_configs]]
flow_name = "XLSHLSSynthFlow"
dslx_file = "{source_name}"
top = "{spec.top}"
generator = "pipeline"
pipeline_stages = "{spec.pipeline_stages}"
delay_model = "unit"
{extra}"""


def render_provenance(spec: DesignSpec, source_name: str) -> str:
    return json.dumps(
        {
            "upstream_repo": "https://github.com/google/xls",
            "upstream_commit": XLS_COMMIT,
            "upstream_release": XLS_RELEASE,
            "upstream_path": spec.upstream_path,
            "local_file": source_name,
            "top": spec.top,
            "license": spec.license,
            "provenance": "human-authored upstream",
        },
        indent=4,
    )


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    print(f"XLS commit : {XLS_COMMIT}")
    print(f"XLS release: {XLS_RELEASE}")
    print(f"target     : {DATASET_DIR}")
    print()

    for spec in DESIGNS:
        source_name = Path(spec.upstream_path).name
        design_dir = DATASET_DIR / spec.name

        if args.dry_run:
            print(f"DRY {spec.name:<14} {spec.upstream_path} (top={spec.top})")
            continue

        design_dir.mkdir(parents=True, exist_ok=True)
        text = fetch(spec.upstream_path)
        (design_dir / source_name).write_text(text, encoding="utf-8")
        (design_dir / "hlsfactory.toml").write_text(
            render_toml(spec, source_name),
            encoding="utf-8",
        )
        (design_dir / "PROVENANCE.json").write_text(
            render_provenance(spec, source_name),
            encoding="utf-8",
        )

        if spec.resolve_imports:
            closure = resolve_import_closure(spec.upstream_path)
            for dep_path, dep_text in closure.items():
                if dep_path == spec.upstream_path:
                    continue
                dep_dest = design_dir / dep_path
                dep_dest.parent.mkdir(parents=True, exist_ok=True)
                dep_dest.write_text(dep_text, encoding="utf-8")

        for extra in spec.extra_files:
            dest = design_dir / extra
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_text(fetch(extra), encoding="utf-8")

        lines = len([x for x in text.splitlines() if x.strip()])
        print(f"OK  {spec.name:<14} {lines:>4} lines  top={spec.top}")

    if not args.dry_run:
        print()
        print(f"{len(DESIGNS)} designs written to {DATASET_DIR}")


if __name__ == "__main__":
    main()
