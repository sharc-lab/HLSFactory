import argparse
import shutil
from pathlib import Path
from pprint import pp

from hlsfactory.flow_vitis import (
    VitisHLSCosimFlow,
    VitisHLSCosimSetupFlow,
    VitisHLSCsimFlow,
    VitisHLSImplFlow,
    VitisHLSSynthFlow,
)
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
    VitisHLSSynthFlow,
    VitisHLSImplFlow,
    VitisHLSCosimSetupFlow,
    VitisHLSCosimFlow,
    VitisHLSCsimFlow,
]

FLOW_NAME_MAP = {
    "VitisHLSSynthFlow": VitisHLSSynthFlow,
    "VitisHLSImplFlow": VitisHLSImplFlow,
    "VitisHLSCosimSetupFlow": VitisHLSCosimSetupFlow,
    "VitisHLSCosimFlow": VitisHLSCosimFlow,
    "VitisHLSCsimFlow": VitisHLSCsimFlow,
}


def main(args) -> None:
    top_work_dir = get_work_dir()

    PATH_VITIS_HLS, PATH_VIVADO = get_tool_paths(ToolPathsSource.ENVFILE)
    BIN_VITIS_HLS = PATH_VITIS_HLS / "bin" / "vitis_hls"
    BIN_VIVADO = PATH_VIVADO / "bin" / "vivado"

    N_JOBS = args.n_jobs
    CPU_AFFINITY = None

    TIMEOUT_HLS_SYNTH = 60.0 * 8  # 8 minutes
    TIMEOUT_HLS_IMPL = 60.0 * 30  # 30 minutes

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

    flow_instances: list[Flow] = []
    for flow in flow_classes:
        match flow:
            case cls if cls in (VitisHLSSynthFlow, VitisHLSImplFlow, VitisHLSCsimFlow):
                flow_instance = cls(
                    vitis_hls_bin=str(BIN_VITIS_HLS),
                    env_var_xilinx_hls=str(PATH_VITIS_HLS),
                    env_var_xilinx_vivado=str(PATH_VIVADO),
                )
            case cls if cls in (
                VitisHLSCosimSetupFlow,
                VitisHLSCosimFlow,
            ):
                flow_instance = cls(
                    vitis_hls_bin=str(BIN_VITIS_HLS),
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
        assert datasets_post_flow
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

    args = parser.parse_args()
    main(args)
