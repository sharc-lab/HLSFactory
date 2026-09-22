import time
from pathlib import Path

import hlsfactory.utils
import pytest
from hlsfactory.utils import (
    ExecutionData,
    ExecutionDataStatus,
    FlowTimer,
    FlowExecutionData,
    flow_already_completed,
    read_execution_data,
    update_execution_data_with_flow_results,
)


def test_legacy_execution_time_writer_is_removed():
    assert not hasattr(hlsfactory.utils, "log_execution_time_to_file")
    assert not hasattr(hlsfactory.utils, "write_execution_data")


def test_update_and_read_execution_data(tmp_path: Path):
    flow = "VitisHLSSynthFlow"
    t0 = time.time()
    t1 = t0 + 12.345

    update_execution_data_with_flow_results(
        design_dir=tmp_path,
        flow_name=flow,
        status=ExecutionDataStatus.SUCCESS,
        t_start=t0,
        t_end=t1,
        core=4,
    )

    data = read_execution_data(tmp_path, flow_name=flow)
    assert isinstance(data, FlowExecutionData)
    assert data.status == ExecutionDataStatus.SUCCESS
    assert data.core == 4
    assert data.dt == round(t1 - t0, 4)


def test_error_and_timeout_status(tmp_path: Path):
    flow_err = "VitisHLSSynthFlow"
    flow_time = "VitisHLSImplFlow"

    update_execution_data_with_flow_results(
        design_dir=tmp_path,
        flow_name=flow_err,
        status=ExecutionDataStatus.ERROR,
        t_start=100.0,
        t_end=105.0,
        error_message="Synthesis failed",
    )

    update_execution_data_with_flow_results(
        design_dir=tmp_path,
        flow_name=flow_time,
        status=ExecutionDataStatus.TIMEOUT,
        t_start=200.0,
        t_end=260.0,
        error_message="Timeout reached",
    )

    all_data = read_execution_data(tmp_path)
    assert isinstance(all_data, ExecutionData)
    assert all_data.flows[flow_err].status == ExecutionDataStatus.ERROR
    assert all_data.flows[flow_err].error_message == "Synthesis failed"
    assert all_data.flows[flow_time].status == ExecutionDataStatus.TIMEOUT

    update_execution_data_with_flow_results(
        design_dir=tmp_path,
        flow_name=flow_err,
        status=ExecutionDataStatus.OTHER,
        t_start=300.0,
        t_end=301.0,
    )
    updated_data = read_execution_data(tmp_path)
    assert isinstance(updated_data, ExecutionData)
    assert updated_data.flows[flow_err].status == ExecutionDataStatus.OTHER
    assert updated_data.flows[flow_time].status == ExecutionDataStatus.TIMEOUT


def test_flow_timer_context_manager(tmp_path: Path):
    flow = "VitisHLSSynthFlow"
    with FlowTimer(flow, tmp_path):
        time.sleep(0.01)

    data = read_execution_data(tmp_path, flow_name=flow)
    assert isinstance(data, FlowExecutionData)
    assert data.status == ExecutionDataStatus.SUCCESS
    assert data.dt > 0


def test_flow_timer_records_exceptions_as_errors(tmp_path: Path):
    flow = "FailingFlow"

    with pytest.raises(ValueError, match="critical parse failure"):
        with FlowTimer(flow, tmp_path):
            raise ValueError("critical parse failure")

    data = read_execution_data(tmp_path, flow_name=flow)
    assert isinstance(data, FlowExecutionData)
    assert data.status == ExecutionDataStatus.ERROR
    assert data.error_message == "critical parse failure"


def test_flow_already_completed_requires_flow_entry(tmp_path: Path):
    flow = "VitisHLSSynthFlow"

    assert not flow_already_completed(tmp_path, flow)

    update_execution_data_with_flow_results(
        tmp_path, flow, ExecutionDataStatus.ERROR, 1.0, 2.0
    )
    assert flow_already_completed(tmp_path, flow)
    assert not flow_already_completed(tmp_path, "OtherFlow")

    update_execution_data_with_flow_results(
        tmp_path, flow, ExecutionDataStatus.SUCCESS, 1.0, 2.0
    )
    assert flow_already_completed(tmp_path, flow)

    # Legacy marker files from an earlier run must not override the latest
    # execution status.
    (tmp_path / f"error__{flow}.txt").touch()
    (tmp_path / f"timeout__{flow}.txt").touch()
    assert flow_already_completed(tmp_path, flow)


def test_execution_data_models_round_trip():
    execution_data = ExecutionData(
        flows={
            "CustomFlow": FlowExecutionData(
                status=ExecutionDataStatus.OTHER,
                t_start=1.0,
                t_end=2.5,
                dt=1.5,
                core=None,
                error_message="Custom result",
            )
        }
    )

    assert ExecutionData.from_json(execution_data.to_json()) == execution_data
