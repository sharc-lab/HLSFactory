# Core Concepts

This page introduces the key ideas behind HLSFactory for users familiar with High-Level Synthesis (HLS) but new to the framework.

## What HLSFactory Adds

HLSFactory is a **framework for building and working with HLS design datasets**. It enables you to construct, manage, and analyze collections of HLS designs across different tool flows (Vitis HLS, Vivado, Intel HLS Compiler, Quartus), providing standardized interfaces, automation, and data extraction. Rather than just orchestrating tool invocation, HLSFactory lets you treat HLS designs as programmatically accessible datasets, supporting a wide range of workflows:

- **Dataset construction and management** — Build, load, and manipulate large sets of HLS designs as `Design` and `DesignDataset` objects, supporting scalable experimentation and research.
- **Design-space exploration** — Automatically elaborate a single design into a diverse set of concrete variants using OptDSL, capturing many pragma and configuration combinations.
- **Parallelized synthesis and implementation** — Execute vendor tool flows across datasets with high-level control over parallelism, CPU affinity, and resource usage.
- **Structured data aggregation** — Consistently extract, organize, and package tool reports and flow outputs as CSV/JSON for downstream analysis, benchmarking, or machine learning.

While HLSFactory relies on existing vendor tools as backends, it provides the infrastructure, APIs, and abstractions needed to efficiently build, curate, and analyze HLS datasets at scale.

## Core Abstractions

- **Design** — A single HLS design: a directory containing source files, TCL scripts, and (after flows run) generated reports and artifacts
- **DesignDataset** — A collection of related `Design` objects (e.g., all PolyBench kernels)
- **DesignDatasetCollection** — A dictionary of datasets, used when running flows across multiple corpora
- **Frontend Flow** — Transforms designs (e.g., OptDSL elaborates abstract designs into concrete variants with different pragmas)
- **Tool Flow** — Runs vendor tools (Vitis HLS synthesis, Vivado implementation, etc.) and extracts data

Designs flow through: **Frontend** (optional) → **ToolFlow** → **Data** (JSON/CSV in each design directory).

## Abstract vs. Concrete Designs

- **Abstract** — A design that needs further elaboration before synthesis (e.g., has an OptDSL template but no fixed pragma values). Frontend flows like OptDSL turn abstract designs into multiple concrete ones.
- **Concrete** — A design ready for `csynth` and implementation. It has all source files and directives in place; tool flows can run directly on it.

You can also use HLSFactory with only concrete designs (e.g., pre-written Vitis HLS projects) and skip the frontend.

## Entry Points and Configuration

HLSFactory expects certain files in each design directory. The table below summarizes the main entry points:

| File | Purpose | Used By |
|------|---------|---------|
| `dataset_hls.tcl` | Create HLS project, add sources, run `csynth` | VitisHLSSynthFlow |
| `dataset_hls_ip_export.tcl` | Open synthesized project, run `export_design -flow impl` | VitisHLSImplFlow |
| `opt_template.tcl` | OptDSL design-space specification (pragma combinations) | OptDSLFrontend |
| `hlsfactory.toml` | Design metadata and flow-specific config | All flows (optional; legacy designs use TCL filenames) |

For full details on `hlsfactory.toml` and flow configuration, see [Design Configuration](framework/design_config). For integrating an existing Vitis HLS project, see the [Integrating Your HLS Project](extending) section in the Extending guide.

## Applications of HLSFactory

HLSFactory is designed to support a variety of advanced workflows in the HLS ecosystem by making it easy to build, manage, and analyze large HLS design datasets. With its abstractions and automation, you can:

- **Build datasets for machine learning**: Create and aggregate diverse sets of HLS designs and tool results as structured data (e.g., CSV, SQLite) for tasks like QoR (quality-of-result) prediction and feature extraction. See the [ML QoR tutorial](tutorials/ml_qor).
- **Benchmark and regression test tools**: Run suites of designs across different tool versions, compare results, and automate regression testing to track changes, as shown in the [Tool Benchmarking demo](tutorials/tool_bench).
- **Enable LLM and AI agent workflows**: Generate traceable datasets for training and evaluating language models, code agents, or reinforcement learning methods applied to HLS tasks.
- **Drive custom research and design-space exploration**: Perform large-scale design-space sweeps, curate study corpora, or mine results to investigate synthesis techniques and tool behaviors.
