import subprocess
import venv
from pathlib import Path
from tempfile import TemporaryDirectory

import pytest


@pytest.mark.skip
def test_package_install() -> None:
    github_url = "https://github.com/sharc-lab/HLSFactory"

    temp_dir = TemporaryDirectory()
    temp_dir_fp = Path(temp_dir.name)

    venv_dir = str(temp_dir_fp / "venv")

    subprocess.run(["python", "-m", "venv", venv_dir], check=True)

    p = subprocess.run(
        f"source {venv_dir}/bin/activate && pip install git+{github_url}",
        shell=True,
        check=False,
        capture_output=True,
        text=True,
    )
    if p.returncode != 0:
        print(p.stdout)
        print(p.stderr)
        raise RuntimeError("Failed to install HLSFactory")

    python_command_code = "import hlsfactory\nprint(dir(hlsfactory))"
    p = subprocess.run(
        f"source {venv_dir}/bin/activate && python -c '{python_command_code}'",
        shell=True,
        capture_output=True,
        text=True,
        check=False,
    )
    if p.returncode != 0:
        print(p.stdout)
        print(p.stderr)
        raise RuntimeError("Failed to import HLSFactory")

    print(p.stdout)
