import argparse
import shutil
from pathlib import Path
from pprint import pp

from hlsfactory.flow_catapult import CatapultHLSSynthFlow
from hlsfactory.flow_vitis import (
    VitisHLSCosimFlow,
    VitisHLSCosimSetupFlow,
    VitisHLSCsimFlow,
    VitisHLSImplFlow,
    VitisHLSSynthFlow,
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
    get_tool_paths,
    get_work_dir,
    remove_and_make_new_dir_if_exists,
)

FLOWS = [
    CatapultHLSSynthFlow,
    StratusHLSSynthFlow,
    XLSHLSSynthFlow,
    VitisHLSSynthFlow,
    VitisHLSImplFlow,
    VitisHLSCosimSetupFlow,
    VitisHLSCosimFlow,
    VitisHLSCsimFlow,
]

FLOW_NAME_MAP = {
    "CatapultHLSSynthFlow": CatapultHLSSynthFlow,
    "StratusHLSSynthFlow": StratusHLSSynthFlow,
    "XLSHLSSynthFlow": XLSHLSSynthFlow,
    "VitisHLSSynthFlow": VitisHLSSynthFlow,
    "VitisHLSImplFlow": VitisHLSImplFlow,
    "VitisHLSCosimSetupFlow": VitisHLSCosimSetupFlow,
    "VitisHLSCosimFlow": VitisHLSCosimFlow,
    "VitisHLSCsimFlow": VitisHLSCsimFlow,
    "OptDSLPassthroughFrontend": OptDSLPassthroughFrontend,
}


def main(args) -> None:
    top_work_dir = get_work_dir()

    N_JOBS = args.n_jobs
    CPU_AFFINITY = None

    TIMEOUT_HLS_SYNTH = 60.0 * 12  # 12 minutes
    TIMEOUT_HLS_IMPL = 60.0 * 30  # 30 minutes
    TIMEOUT_CATAPULT_SYNTH = 60.0 * 12  # 12 minutes
    TIMEOUT_STRATUS_SYNTH = 60.0 * 12  # 12 minutes
    TIMEOUT_XLS_SYNTH = 60.0 * 12  # 12 minutes

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

    flow_instances: list[Flow] = []
    for flow in flow_classes:
        match flow:
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
        if isinstance(flow, VitisHLSSynthFlow):
            timeout = TIMEOUT_HLS_SYNTH
        elif isinstance(flow, VitisHLSImplFlow):
            timeout = TIMEOUT_HLS_IMPL
        elif isinstance(flow, CatapultHLSSynthFlow):
            timeout = TIMEOUT_CATAPULT_SYNTH
        elif isinstance(flow, StratusHLSSynthFlow):
            timeout = TIMEOUT_STRATUS_SYNTH
        elif isinstance(flow, XLSHLSSynthFlow):
            timeout = TIMEOUT_XLS_SYNTH
        else:
            timeout = None

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
        if output_design_count == 0:
            raise RuntimeError(f"Flow {flow.name} produced no successful designs")
        in_datasets = datasets_post_flow


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Validate dataset files.")
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
        help="Path to the Catapult executable (otherwise resolved from PATH).",
    )
    parser.add_argument(
        "--stratus-install-dir",
        type=Path,
        help="Stratus installation root containing bin/stratus and bin/bdw_makegen.",
    )

    args = parser.parse_args()
    main(args)
