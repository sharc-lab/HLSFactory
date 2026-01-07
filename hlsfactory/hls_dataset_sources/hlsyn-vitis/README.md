# HLSyn-Vitis

A reimplementation of HLSyn using Xilinx Vitis 2025.1, replacing the original Merlin-based implementation.

## Overview

This dataset source provides high-level synthesis (HLS) designs and configurations for design space exploration (DSE) targeting Xilinx FPGAs using Vitis HLS.

## Directory Structure

- `vivado-target/designs/` - HLS design benchmarks
- `vivado-target/sources/` - Source files for synthesis 


## Design Space Exploration Configuration for Vivado Target

### Pipeline Pragma Values

Pipeline pragmas can have the following values:
- `""` (empty string) - Pipeline pragma removed; inserted during initialization
- `"NA"` - Pipeline pragma removed; inserted as a DSE option
- `"   "` (three spaces) - Pipeline enabled without any additional options


### Unroll Pragma Values

Unroll factors are determined by:
- Loop bound integer divisors
- Powers of 2

### Synthesis Configuration

- **Timeout:** 15 minutes
- **Resource utilization target:** 75% of peak resources. U200 @ 250 MHz.

### Result Interpretation

#### Entry with `"valid": false`
If a design entry shows `"valid": false` but still contains resource utilization data, it likely **exceeded available resources**.

#### Entry without performance value
If a design entry does not show a performance value (`perf`), it likely **timed out** during synthesis (exceeded 15 minute limit).


## Ongoing

More optimized version using Vitis target and more optimized memory transfers. 



