import time
from pathlib import Path
import pytest
from hlsfactory.utils import write_execution_data, read_execution_data, FlowTimer


def test_write_and_read_execution_data(tmp_path: Path):
    flow = "VitisHLSSynthFlow"
    t0 = time.time()
    t1 = t0 + 12.345

    write_execution_data(
        design_dir=tmp_path,
        flow_name=flow,
        status="success",
        t_start=t0,
        t_end=t1,
        core=4,
        return_code=0,
    )

    data = read_execution_data(tmp_path, flow_name=flow)
    assert data["status"] == "success"
    assert data["core"] == 4
    assert data["return_code"] == 0
    assert data["dt"] == round(t1 - t0, 4)


def test_error_and_timeout_status(tmp_path: Path):
    flow_err = "VitisHLSSynthFlow"
    flow_time = "VitisHLSImplFlow"

    write_execution_data(
        design_dir=tmp_path,
        flow_name=flow_err,
        status="error",
        t_start=100.0,
        t_end=105.0,
        return_code=1,
        error_message="Synthesis failed",
    )

    write_execution_data(
        design_dir=tmp_path,
        flow_name=flow_time,
        status="timeout",
        t_start=200.0,
        t_end=260.0,
        return_code=-1,
        error_message="Timeout reached",
    )

    all_data = read_execution_data(tmp_path)
    assert all_data[flow_err]["status"] == "error"
    assert all_data[flow_err]["error_message"] == "Synthesis failed"
    assert all_data[flow_time]["status"] == "timeout"


def test_flow_timer_context_manager(tmp_path: Path):
    flow = "VitisHLSSynthFlow"
    with FlowTimer(flow, tmp_path):
        time.sleep(0.01)

    data = read_execution_data(tmp_path, flow_name=flow)
    assert data["status"] == "success"
    assert data["dt"] > 0