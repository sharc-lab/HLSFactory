import argparse
import shutil
from pathlib import Path
from pprint import pp

from hlsfactory.flow_balor import BalorGraphFlow
from hlsfactory.flow_altera import (
    AlteraHLSCosimFlow,
    AlteraHLSCsimFlow,
    AlteraHLSSynthFlow,
    AlteraQuartusImplFlow,
)

from hlsfactory.flow_catapult import CatapultHLSSynthFlow
from hlsfactory.flow_vitis import (
    VitisHLSCosimFlow,
    VitisHLSCosimSetupFlow,
    VitisHLSCsimFlow,
    VitisHLSImplFlow,
    VitisHLSSynthFlow,
)
from hlsfactory.flow_vitis_modern import (
    VitisHLSModernCsimFlow,
    VitisHLSModernSynthFlow,
)
from hlsfactory.flow_xls import XLSHLSSynthFlow
from hlsfactory.opt_dsl_frontend import OptDSLPassthroughFrontend
from hlsfactory.stratus_flow import StratusHLSSynthFlow
from hlsfactory.framework import (
    DesignDataset,
    Flow,
    count_total_designs_in_dataset_collection,
)
from hlsfactory.utils import (
    ToolPathsSource,
    get_tool_path_vitis_modern,
    get_tool_paths,
    get_work_dir,
    remove_and_make_new_dir_if_exists,
)

FLOWS = [
    BalorGraphFlow,
    AlteraHLSCosimFlow,
    AlteraHLSSynthFlow,
    AlteraHLSCsimFlow,
    AlteraQuartusImplFlow,
    CatapultHLSSynthFlow,
    StratusHLSSynthFlow,
    XLSHLSSynthFlow,
    VitisHLSSynthFlow,
    VitisHLSImplFlow,
    VitisHLSCosimSetupFlow,
    VitisHLSCosimFlow,
    VitisHLSCsimFlow,
    VitisHLSModernSynthFlow,
    VitisHLSModernCsimFlow,
]

FLOW_NAME_MAP = {
    "BalorGraphFlow": BalorGraphFlow,
    "AlteraHLSCosimFlow": AlteraHLSCosimFlow,
    "AlteraHLSSynthFlow": AlteraHLSSynthFlow,
    "AlteraHLSCsimFlow": AlteraHLSCsimFlow,
    "AlteraQuartusImplFlow": AlteraQuartusImplFlow,
    "CatapultHLSSynthFlow": CatapultHLSSynthFlow,
    "StratusHLSSynthFlow": StratusHLSSynthFlow,
    "XLSHLSSynthFlow": XLSHLSSynthFlow,
    "VitisHLSSynthFlow": VitisHLSSynthFlow,
    "VitisHLSImplFlow": VitisHLSImplFlow,
    "VitisHLSCosimSetupFlow": VitisHLSCosimSetupFlow,
    "VitisHLSCosimFlow": VitisHLSCosimFlow,
    "VitisHLSCsimFlow": VitisHLSCsimFlow,
    "VitisHLSModernSynthFlow": VitisHLSModernSynthFlow,
    "VitisHLSModernCsimFlow": VitisHLSModernCsimFlow,
    "OptDSLPassthroughFrontend": OptDSLPassthroughFrontend,
}


def main(args) -> None:
    top_work_dir = args.work_dir if args.work_dir is not None else get_work_dir()

    N_JOBS = args.n_jobs
    CPU_AFFINITY = None

    TIMEOUT_HLS_SYNTH = 60.0 * 12  # 12 minutes
    TIMEOUT_HLS_IMPL = 60.0 * 30  # 30 minutes
    TIMEOUT_ALTERA_COSIM = 60.0 * 60  # Includes FPGA simulator library compilation
    TIMEOUT_CATAPULT_SYNTH = 60.0 * 12  # 12 minutes
    TIMEOUT_STRATUS_SYNTH = 60.0 * 12  # 12 minutes
    TIMEOUT_XLS_SYNTH = 60.0 * 12  # 12 minutes
    TIMEOUT_VITIS_HLS_MODERN_SYNTH = 60.0 * 12  # 12 minutes
    TIMEOUT_BALOR_GRAPH = 60.0 * 10  # 10 minutes

    if args.name is None:
        dataset_name = args.dataset_source_directory.name
    else:
        dataset_name = args.name

    work_dir = top_work_dir / "dataset_validator" / f"dataset_validator__{dataset_name}"
    remove_and_make_new_dir_if_exists(work_dir)
    print(f"work_dir: {work_dir}")

    # copy the dataset source directory to the work directory
    shutil.copytree(
        args.dataset_source_directory,
        work_dir / "original",
    )

    dataset_instance = DesignDataset.from_dir(
        "original",
        work_dir / "original",
    )
    selected = getattr(args, "designs", None)
    if selected:
        unknown = set(selected) - {design.name for design in dataset_instance.designs}
        if unknown:
            raise ValueError(f"Unknown designs: {', '.join(sorted(unknown))}")
        dataset_instance.designs = [
            design for design in dataset_instance.designs if design.name in selected
        ]
    datasets = {
        "original": dataset_instance,
    }

    total_count = count_total_designs_in_dataset_collection(datasets)

    total_synth_time_estimation = total_count * TIMEOUT_HLS_SYNTH / N_JOBS
    total_impl_time_estimation = total_count * TIMEOUT_HLS_IMPL / N_JOBS

    print(f"total_count: {total_count}")
    print(f"total_synth_time_estimation: {total_synth_time_estimation}")
    print(f"total_impl_time_estimation: {total_impl_time_estimation}")

    if "+" in args.flow:
        flow = args.flow.split("+")
        flow = [flow_name.strip() for flow_name in flow]
    else:
        flow = [args.flow.strip()]

    for flow_name in flow:
        if flow_name not in FLOW_NAME_MAP:
            raise ValueError(f"Flow {flow_name} not found in FLOW_NAME_MAP")

    flow_classes = [FLOW_NAME_MAP[flow_name] for flow_name in flow]

    vitis_flow_classes = {
        VitisHLSSynthFlow,
        VitisHLSImplFlow,
        VitisHLSCsimFlow,
        VitisHLSCosimSetupFlow,
        VitisHLSCosimFlow,
    }
    if any(flow_class in vitis_flow_classes for flow_class in flow_classes):
        path_vitis_hls, path_vivado = get_tool_paths(ToolPathsSource.ENVFILE)
        bin_vitis_hls = path_vitis_hls / "bin" / "vitis_hls"

    vitis_modern_flow_classes = {
        VitisHLSModernSynthFlow,
        VitisHLSModernCsimFlow,
    }
    if any(flow_class in vitis_modern_flow_classes for flow_class in flow_classes):
        path_vitis_modern = get_tool_path_vitis_modern(ToolPathsSource.ENVFILE)
        bin_vpp_modern = path_vitis_modern / "bin" / "v++"
        bin_vitis_run_modern = path_vitis_modern / "bin" / "vitis-run"

    flow_instances: list[Flow] = []
    for flow in flow_classes:
        match flow:
            case cls if cls in (
                AlteraHLSCosimFlow,
                AlteraHLSSynthFlow,
                AlteraHLSCsimFlow,
                AlteraQuartusImplFlow,
            ):
                flow_instance = cls(altera_install_dir=args.altera_install_dir)
            case cls if cls is CatapultHLSSynthFlow:
                flow_instance = cls(
                    catapult_bin=(
                        str(args.catapult_bin)
                        if args.catapult_bin is not None
                        else None
                    ),
                )
            case cls if cls is StratusHLSSynthFlow:
                flow_instance = cls(
                    stratus_install_dir=args.stratus_install_dir,
                )
            case cls if cls is XLSHLSSynthFlow:
                flow_instance = cls()
            case cls if cls is BalorGraphFlow:
                flow_instance = cls(
                    balor_install_dir=args.balor_install_dir,
                )
            case cls if cls in (VitisHLSSynthFlow, VitisHLSImplFlow, VitisHLSCsimFlow):
                flow_instance = cls(
                    vitis_hls_bin=str(bin_vitis_hls),
                    env_var_xilinx_hls=str(path_vitis_hls),
                    env_var_xilinx_vivado=str(path_vivado),
                )
            case cls if cls in (
                VitisHLSCosimSetupFlow,
                VitisHLSCosimFlow,
            ):
                flow_instance = cls(
                    vitis_hls_bin=str(bin_vitis_hls),
                )
            case cls if cls is VitisHLSModernSynthFlow:
                flow_instance = cls(
                    vpp_bin=str(bin_vpp_modern),
                    env_var_xilinx_hls=str(path_vitis_modern),
                    env_var_xilinx_vitis=str(path_vitis_modern),
                )
            case cls if cls is VitisHLSModernCsimFlow:
                flow_instance = cls(
                    vitis_run_bin=str(bin_vitis_run_modern),
                    env_var_xilinx_hls=str(path_vitis_modern),
                    env_var_xilinx_vitis=str(path_vitis_modern),
                )
            case cls if cls is OptDSLPassthroughFrontend:
                flow_instance = cls(
                    work_dir=work_dir,
                )
            case _:
                raise NotImplementedError(f"Flow {flow!r} is not yet supported.")

        flow_instances.append(flow_instance)

    pp(flow_instances)

    in_datasets = datasets
    for flow in flow_instances:
        if isinstance(flow, AlteraQuartusImplFlow):
            timeout = TIMEOUT_HLS_IMPL
        elif isinstance(flow, AlteraHLSCosimFlow):
            timeout = TIMEOUT_ALTERA_COSIM
        elif isinstance(flow, (AlteraHLSSynthFlow, AlteraHLSCsimFlow)):
            timeout = TIMEOUT_HLS_SYNTH
        elif isinstance(flow, VitisHLSSynthFlow):
            timeout = TIMEOUT_HLS_SYNTH
        elif isinstance(flow, VitisHLSImplFlow):
            timeout = TIMEOUT_HLS_IMPL
        elif isinstance(flow, CatapultHLSSynthFlow):
            timeout = TIMEOUT_CATAPULT_SYNTH
        elif isinstance(flow, StratusHLSSynthFlow):
            timeout = TIMEOUT_STRATUS_SYNTH
        elif isinstance(flow, XLSHLSSynthFlow):
            timeout = TIMEOUT_XLS_SYNTH
        elif isinstance(flow, VitisHLSModernSynthFlow):
            timeout = TIMEOUT_VITIS_HLS_MODERN_SYNTH
        elif isinstance(flow, BalorGraphFlow):
            timeout = TIMEOUT_BALOR_GRAPH
        else:
            timeout = None

        if args.timeout is not None:
            timeout = args.timeout

        datasets_post_flow = (
            flow.execute_multiple_design_datasets_fine_grained_parallel(
                in_datasets,
                False,
                n_jobs=N_JOBS,
                cpu_affinity=CPU_AFFINITY,
                timeout=timeout,
            )
        )
        output_design_count = count_total_designs_in_dataset_collection(
            datasets_post_flow,
        )
        input_design_count = count_total_designs_in_dataset_collection(in_datasets)
        if output_design_count != input_design_count or output_design_count == 0:
            raise RuntimeError(
                f"Flow {flow.name} succeeded for {output_design_count}/{input_design_count} designs"
            )
        print(f"{flow.name}: {output_design_count}/{input_design_count} designs passed")
        in_datasets = datasets_post_flow


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Validate dataset files.")
    parser.add_argument(
        "--designs",
        nargs="+",
        help="Validate only these design names (defaults to the entire dataset).",
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=None,
        help="Override the per-design timeout in seconds for each selected flow.",
    )
    parser.add_argument(
        "--work-dir",
        type=Path,
        default=None,
        help="Output root (defaults to HLSFACTORY_WORK_DIR from .env).",
    )
    parser.add_argument(
        "--altera-install-dir",
        type=Path,
        default=None,
        help="Altera HLS IP Gen installation containing bin/ahls-sh.",
    )
    parser.add_argument(
        "dataset_source_directory",
        type=Path,
    )
    parser.add_argument(
        "-j",
        "--n_jobs",
        type=int,
        default=1,
        help="Number of jobs to run in parallel.",
    )

    parser.add_argument(
        "-n",
        "--name",
        type=str,
        help="Name of the test dataset to run. By default, its just the dataset source directory name.",
    )

    parser.add_argument(
        "--flow",
        type=str,
        default="VitisHLSSynthFlow",
        help="Flow that we are testing.",
    )
    parser.add_argument(
        "--catapult-bin",
        type=Path,
        help=(
            "Path to the Catapult executable (otherwise resolved from "
            "HLSFACTORY_CATAPULT_PATH or PATH)."
        ),
    )
    parser.add_argument(
        "--stratus-install-dir",
        type=Path,
        help="Stratus installation root containing bin/stratus and bin/bdw_makegen.",
    )
    parser.add_argument(
        "--balor-install-dir",
        type=Path,
        help=(
            "Balor checkout containing graph_compiler/run_graph_compiler.py "
            "(otherwise resolved from HLSFACTORY_BALOR_PATH, else Docker)."
        ),
    )

    args = parser.parse_args()
    main(args)
