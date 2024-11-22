import shutil
from pathlib import Path
from unittest.mock import patch

from hlsfactory.datasets_builtin import (
    dataset_polybench_builder,
    dataset_vitis_examples_builder,
)
from hlsfactory.flow_vitis import (
    VitisHLSImplFlow,
    VitisHLSImplReportFlow,
    VitisHLSSynthFlow,
)
from hlsfactory.framework import (
    count_total_designs_in_dataset_collection,
)
from hlsfactory.opt_dsl_frontend import OptDSLFrontend
from hlsfactory.utils import (
    ToolPathsSource,
    get_tool_paths,
    get_work_dir,
    remove_and_make_new_dir_if_exists,
)

top_work_dir = get_work_dir()

PATH_VITIS_HLS, PATH_VIVADO = get_tool_paths(ToolPathsSource.ENVFILE)
BIN_VITIS_HLS = PATH_VITIS_HLS / "bin" / "vitis_hls"
BIN_VIVADO = PATH_VIVADO / "bin" / "vivado"

N_JOBS = 2
CPU_AFFINITY = list(range(N_JOBS))

TIMEOUT_HLS_SYNTH = 60.0 * 8  # 8 minutes
TIMEOUT_HLS_IMPL = 60.0 * 30  # 30 minutes


def test_concrete_vitis_hls_flow() -> None:
    work_dir = top_work_dir / "test_flows" / "test_concrete_vitis_hls_flow"
    remove_and_make_new_dir_if_exists(work_dir)
    print(f"work_dir: {work_dir}")

    dataset_vitis_examples = dataset_vitis_examples_builder("vitis_examples", work_dir)
    n_keep = 2
    designs_to_keep = dataset_vitis_examples.designs[:n_keep]
    designs_to_remove = dataset_vitis_examples.designs[n_keep:]
    dataset_vitis_examples.designs = designs_to_keep
    for design in designs_to_remove:
        shutil.rmtree(design.dir)

    datasets = {
        "vitis_examples": dataset_vitis_examples,
    }

    total_count = count_total_designs_in_dataset_collection(datasets)

    total_time_estimation = (
        total_count * (TIMEOUT_HLS_SYNTH + TIMEOUT_HLS_IMPL) / N_JOBS
    )

    print(
        f"Estimated worst-case build time:\n"
        f"{total_time_estimation} seconds\n"
        f"{total_time_estimation / 60} minutes"
        f"\n{total_time_estimation / 3600} hours",
    )

    toolflow_vitis_hls_synth = VitisHLSSynthFlow(
        vitis_hls_bin=str(BIN_VITIS_HLS),
        env_var_xilinx_hls=str(PATH_VITIS_HLS),
        env_var_xilinx_vivado=str(PATH_VIVADO),
    )
    datasets_post_hls_synth = (
        toolflow_vitis_hls_synth.execute_multiple_design_datasets_fine_grained_parallel(
            datasets,
            False,
            n_jobs=N_JOBS,
            cpu_affinity=CPU_AFFINITY,
            timeout=TIMEOUT_HLS_SYNTH,
        )
    )

    toolflow_vitis_hls_implementation = VitisHLSImplFlow(
        vitis_hls_bin=str(BIN_VITIS_HLS),
        env_var_xilinx_hls=str(PATH_VITIS_HLS),
        env_var_xilinx_vivado=str(PATH_VIVADO),
    )
    datasets_post_hls_implementation = toolflow_vitis_hls_implementation.execute_multiple_design_datasets_fine_grained_parallel(
        datasets_post_hls_synth,
        False,
        n_jobs=N_JOBS,
        cpu_affinity=CPU_AFFINITY,
        timeout=TIMEOUT_HLS_IMPL,
    )

    toolflow_vitis_hls_impl_report = VitisHLSImplReportFlow(
        vitis_hls_bin=str(BIN_VITIS_HLS),
        vivado_bin=str(BIN_VIVADO),
        env_var_xilinx_hls=str(PATH_VITIS_HLS),
        env_var_xilinx_vivado=str(PATH_VIVADO),
    )
    toolflow_vitis_hls_impl_report.execute_multiple_design_datasets_fine_grained_parallel(
        datasets_post_hls_implementation,
        False,
        n_jobs=N_JOBS,
        cpu_affinity=CPU_AFFINITY,
    )


def test_concrete_vitis_hls_flow_induced_tool_error(monkeypatch) -> None:
    # want to induce a tool error by mocking calls to suborocess to return non-zero exit code
    work_dir = (
        top_work_dir / "test_flows" / "test_concrete_vitis_hls_flow_induced_tool_error"
    )
    remove_and_make_new_dir_if_exists(work_dir)
    print(f"work_dir: {work_dir}")

    dataset_vitis_examples = dataset_vitis_examples_builder("vitis_examples", work_dir)
    n_keep = 4
    designs_to_keep = dataset_vitis_examples.designs[:n_keep]
    designs_to_remove = dataset_vitis_examples.designs[n_keep:]
    dataset_vitis_examples.designs = designs_to_keep
    for design in designs_to_remove:
        shutil.rmtree(design.dir)

    datasets = {
        "vitis_examples": dataset_vitis_examples,
    }

    toolflow_vitis_hls_synth = VitisHLSSynthFlow(
        vitis_hls_bin=str(BIN_VITIS_HLS),
        env_var_xilinx_hls=str(PATH_VITIS_HLS),
        env_var_xilinx_vivado=str(PATH_VIVADO),
    )

    with patch("subprocess.Popen.wait") as mock_wait:
        mock_wait.return_value = 1

        datasets_post_hls_synth = toolflow_vitis_hls_synth.execute_multiple_design_datasets_fine_grained_parallel(
            datasets,
            False,
            n_jobs=N_JOBS,
            cpu_affinity=CPU_AFFINITY,
            timeout=TIMEOUT_HLS_SYNTH,
        )

    assert len(datasets_post_hls_synth["vitis_examples"].designs) == 0
    for d in datasets["vitis_examples"].designs:
        d_dir = d.dir
        assert Path(d_dir).exists()
        assert Path(d_dir / "error__VitisHLSSynthFlow.txt").exists()


N_KEEP = 2
N_RANDOM_SAMPLES = 4
RANDOM_SAMPLE_SEED = 7


def test_frontend_vitis_hls_flow() -> None:
    work_dir = top_work_dir / "test_flows" / "test_frontend_vitis_hls_flow"
    remove_and_make_new_dir_if_exists(work_dir)
    print(f"work_dir: {work_dir}")

    dataset_polybench = dataset_polybench_builder("polybench", work_dir)
    designs_to_keep = dataset_polybench.designs[:N_KEEP]
    designs_to_remove = dataset_polybench.designs[N_KEEP:]
    dataset_polybench.designs = designs_to_keep
    for design in designs_to_remove:
        shutil.rmtree(design.dir)

    datasets = {
        "polybench": dataset_polybench,
    }

    total_count = count_total_designs_in_dataset_collection(datasets)

    # Create the OptDSLFrontend instance
    opt_dsl_frontend = OptDSLFrontend(
        work_dir,
        random_sample=True,
        random_sample_num=N_RANDOM_SAMPLES,
        random_sample_seed=RANDOM_SAMPLE_SEED,
        log_execution_time=True,
    )
    # Run the frontend (in parallel)
    datasets_post_frontend = (
        opt_dsl_frontend.execute_multiple_design_datasets_fine_grained_parallel(
            datasets,
            True,
            lambda x: f"{x}__post_frontend",
            n_jobs=N_JOBS,
            cpu_affinity=CPU_AFFINITY,
        )
    )

    # Count the total number of designs in the dataset collection post-frontend
    total_count_post_frontend = count_total_designs_in_dataset_collection(
        datasets_post_frontend,
    )

    print(f"Total Designs: {total_count}")
    print(f"Total Designs post-frontend: {total_count_post_frontend}")

    assert total_count_post_frontend == N_KEEP * N_RANDOM_SAMPLES
