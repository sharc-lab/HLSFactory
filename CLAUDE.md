# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

HLSFactory is a framework for buildinbg and using High-Level Synthesis (HLS) design datasets. It enables collecting, building, and analyzing HLS designs across multiple vendor tool flows (AMD/Xilinx Vitis HLS, Intel HLS Compiler, Bambu, HARP) and custom user-defined flows.

## Common Commands

```bash
# Install dependencies (recommended)
uv sync

# Run tests with coverage
uv run pytest

# Run a single test file
uv run pytest tests/test_flows.py

# Run specific test
uv run pytest tests/test_flows.py::test_function_name -v

# Lint code
uv run ruff check .

# Fix auto-fixable lint issues
uv run ruff check . --fix

# Build documentation
uv run sphinx-build -M html docs/source docs/build
```

## Architecture

### Core Abstractions (hlsfactory/framework.py)

- **Design**: Single HLS design with name and directory path. Provides methods to find source files (.cpp, .h, .tcl).
- **DesignDataset**: Collection of related Design objects. Factory methods: `from_dir()`, `from_empty_dir()`.
- **Flow**: Abstract base class for all tool flows with abstract `execute(design)` method.
  - **Frontend**: Subclass that transforms designs (e.g., applies optimizations)
  - **ToolFlow**: Subclass that runs vendor tools
- **DesignDatasetCollection**: Type alias for `dict[str, DesignDataset]`

### Flow Pipeline Pattern

Designs flow through: Frontend transforms → ToolFlow execution → Data collection

Each flow's `execute_multiple_designs()` supports parallel execution with CPU affinity.

### Key Modules

| Module | Purpose |
|--------|---------|
| `framework.py` | Core Design, DesignDataset, Flow abstractions |
| `design_config.py` | Configuration parsing for `hlsfactory.toml` |
| `flow_vitis.py` | AMD/Xilinx Vitis HLS synthesis and implementation flows |
| `flow_intel.py` | Intel HLS Compiler flow |
| `opt_dsl_frontend.py`, `opt_dsl_frontend_v2.py` | Optimization DSL frontends for design space exploration |
| `jinja_frontend.py` | Jinja2-based design generation |
| `datasets_builtin.py` | Factory builders for built-in datasets (PolyBench, MachSuite, CHStone, etc.) |
| `data_packaging.py` | Data aggregation to CSV/JSON/SQLite |
| `utils.py` | Tool calling with subprocess, timeout, CPU affinity support |

### Configuration

Environment variables (set in `.env` file, template at `.env.template`):
- `HLSFACTORY_WORK_DIR`: Working directory for tool outputs
- `HLSFACTORY_VITIS_HLS_PATH`: Path to Vitis HLS installation
- `HLSFACTORY_VIVADO_PATH`: Path to Vivado installation

Design-specific flows configured via `hlsfactory.toml` files.

### Built-in Datasets

Located in `hlsfactory/hls_dataset_sources/`. Builders in `datasets_builtin.py` create isolated copies:
- PolyBench, MachSuite, CHStone, PP4FPGA, Vitis Examples, FlowGNN, and 15+ more

## Testing

- Framework: pytest with coverage
- Config in `pyproject.toml`: coverage reports to `htmlcov/`
- Tests organized by area: `test_flows.py`, `test_datasets.py`, `test_aggregation.py`, etc.

## Code Style

- Ruff linter with most rules enabled (see `pyproject.toml`)
- Docstring rules (D) are disabled
- `hlsfactory/hls_dataset_sources/` is excluded from linting
- Type hints used throughout
