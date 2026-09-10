"""Generate four C++ preprocessor variants, then run Vitis HLS C synthesis."""

import argparse
from pathlib import Path
from tempfile import mkdtemp

from hlsfactory.define_frontend import CPPPreprocessorFrontend
from hlsfactory.flow_vitis import VitisHLSSynthFlow
from hlsfactory.framework import Design


# Each dictionary produces one concrete design. Macro values are strings.
CONFIGURATIONS = [
    {"VECTOR_SIZE": "16", "SCALE": "2"},
    {"VECTOR_SIZE": "16", "SCALE": "4"},
    {"VECTOR_SIZE": "32", "SCALE": "2"},
    {"VECTOR_SIZE": "32", "SCALE": "4"},
]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--work-dir",
        type=Path,
        help="Output directory (default: a new temporary directory).",
    )
    parser.add_argument(
        "--vitis-hls-bin", help="Vitis HLS executable (default: vitis_hls on PATH)."
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=300,
        help="Synthesis timeout per design in seconds.",
    )
    args = parser.parse_args()

    work_dir = args.work_dir or Path(mkdtemp(prefix="hlsfactory-demo-define-"))
    work_dir = work_dir.resolve()
    work_dir.mkdir(parents=True, exist_ok=True)
    print(f"Work directory: {work_dir}", flush=True)

    # Copy the parameterized source so generated files stay in the work directory.
    source_dir = Path(__file__).resolve().parent / "designs" / "design_define"
    design = Design.from_dir_with_config(source_dir)
    design.copy_to_new_parent_dir(work_dir)

    frontend = CPPPreprocessorFrontend(
        work_dir=work_dir,
        define_statements=CONFIGURATIONS,
    )
    variants = frontend.execute(design)

    synth_flow = VitisHLSSynthFlow(vitis_hls_bin=args.vitis_hls_bin, log_output=True)
    completed = []
    for config, variant in zip(CONFIGURATIONS, variants, strict=True):
        print(f"Synthesizing {config}: {variant.dir}", flush=True)
        completed.extend(synth_flow.execute(variant, timeout=args.timeout))

    if len(completed) != len(CONFIGURATIONS):
        raise RuntimeError(
            f"Only {len(completed)}/4 variants synthesized; inspect logs in {work_dir}."
        )
    print(f"Synthesized all four variants. Reports and data_hls.json files: {work_dir}")


if __name__ == "__main__":
    main()
