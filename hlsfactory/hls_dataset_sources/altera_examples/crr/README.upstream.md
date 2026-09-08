# `CRR Binomial Tree` Sample

The `CRR Binomial Tree` sample demonstrates a Cox-Ross-Rubinstein (CRR) binomial tree model using five Greeks for American option pricing and exercising in the form of a field programmable gate array (FPGA)-optimized reference design.


| Optimized for       | Description                                                            |
| ------------------- | ---------------------------------------------------------------------- |
| What you will learn | How to implement a Cox-Ross-Rubinstein (CRR) binomial tree for an FPGA |
| Time to complete    | ~1 hr (excluding compile time)                                         |
| Category            | Reference Designs and End to End                                       |


## Purpose

This sample implements the Cox-Ross-Rubinstein (CRR) binomial tree model that is used in the finance field for American exercise options with five [Greeks](https://en.wikipedia.org/wiki/Greeks_(finance)) (delta, gamma, theta, vega, and rho). The code demonstrates how to model all possible asset price paths using a binomial tree.

## Prerequisites

This sample is part of the FPGA code samples.
It is categorized as a Tier 4 sample that demonstrates a reference design.

```mermaid
flowchart LR
   tier1("Tier 1: Get Started")
   tier2("Tier 2: Explore the Fundamentals")
   tier3("Tier 3: Explore the Advanced Techniques")
   tier4("Tier 4: Explore the Reference Designs")

   tier1 --> tier2 --> tier3 --> tier4

   style tier1 fill:#0071c1,stroke:#0071c1,stroke-width:1px,color:#fff
   style tier2 fill:#0071c1,stroke:#0071c1,stroke-width:1px,color:#fff
   style tier3 fill:#0071c1,stroke:#0071c1,stroke-width:1px,color:#fff
   style tier4 fill:#f96,stroke:#333,stroke-width:1px,color:#fff
```

Find more information about how to navigate this part of the code samples in the [FPGA top-level README.md](/README.md).
You can also find more information about [troubleshooting build errors](/README.md#troubleshooting), [links to selected documentation](/README.md#documentation), etc.


| Optimized for | Description                                                                                              |
| ------------- | -------------------------------------------------------------------------------------------------------- |
| OS            | Ubuntu* 20.04, Ubuntu* 22.04, Ubuntu* 24.04, RHEL* 9, SUSE* 15 **NOTE: Windows is not supported** |
| Hardware      | Agilex® 5, Agilex® 7, Stratix® 10 and Arria® 10 FPGAs                                                    |
| Software      | HLS IP Gen Compiler                                                                                      |


> **Note**: Even though the HLS IP Gen compiler is enough to compile for emulation, generating reports and generating RTL, there are extra software requirements for the simulation flow and FPGA compiles.
>
> For using the simulator flow, Quartus® Prime Pro Edition and one of the following simulators must be installed and accessible through your PATH:
>
> - Questa*-Altera® FPGA Edition
> - Questa*-Altera® FPGA Starter Edition
> - ModelSim® SE
>
> When using the hardware compile flow, Quartus® Prime Pro Edition must be installed and accessible through your PATH.
>
> :warning: Make sure you add the device files associated with the FPGA that you are targeting to your Quartus® Prime installation.

> **Note**: You'll need a large FPGA part to be able to fit this design

### Performance

Performance results are based on testing as of May 14, 2024.

> **Note**: Refer to the [Performance Disclaimers](/README.md#performance-disclaimers) section for important performance information.


| Device                        | Configuration                     | Throughput   |
| ----------------------------- | --------------------------------- | ------------ |
| Intel® FPGA SmartNIC N6001-PL | Outer unroll: 1; Inner unroll: 64 | 329 assets/s |


## Key Implementation Details

### Design Inputs

This design reads inputs from a CSV file (`ordered_inputs.csv` by default, or `small_ordered_inputs.csv` when built for the simulator). The input parameters are listed in the table.


| Input     | Description                                                                                  |
| --------- | -------------------------------------------------------------------------------------------- |
| `n_steps` | Number of time steps in the binomial tree. The maximum `n_steps` in this design is **8189**. |
| `cp`      | -1 or 1 represents put and call options, respectively.                                       |
| `spot`    | Spot price of the underlying price.                                                          |
| `fwd`     | Forward price of the underlying price.                                                       |
| `strike`  | Exercise price of the option.                                                                |
| `vol`     | Percent volatility that the design reads as a decimal value.                                 |
| `df`      | Discount factor to option expiry.                                                            |
| `t`       | Time, in years, to the maturity of the option.                                               |


### Design Outputs

This design writes outputs to the `ordered_outputs.csv` file. The outputs are:

| Output         | Description                                                                     |
|----------------|---------------------------------------------------------------------------------|
| `value`        | Option price                                                                    |
| `delta`        | Measures the rate of change of the theoretical option value with respect to changes in the underlying asset's price.|
| `gamma`        | Measures the rate of change in the `delta` with respect to changes in the underlying price.|
| `vega`         | Measures sensitivity to volatility.|
| `theta`        | Measures the sensitivity of the derivative's value to the passage of time.|
| `rho`          | Measures sensitivity to the interest of rate.|

### Design Correctness

This design tests the optimized FPGA code's correctness by comparing its output to a golden result computed on the CPU.

### Design Performance

This design measures the FPGA performance to determine how many assets can be processed per second.

### Additional Design Information

#### Source Code Explanation


| File             | Description                                                                                               |
| ---------------- | --------------------------------------------------------------------------------------------------------- |
| `main.cpp`       | Contains both host code and SYCL* kernel code.                                                            |
| `CRR_common.hpp` | Header file for `main.cpp`. Contains the data structures needed for both host code and SYCL* kernel code. |


#### Compiler Flags Used


| Flag                                | Description                                                                   |
| ----------------------------------- | ----------------------------------------------------------------------------- |
| `-Xshardware`                       | Target FPGA hardware (as opposed to FPGA emulator)                                                                    |
| `-Xsdaz`                            | Denormals are zero                                                                                                    |
| `-Xsrounding=faithful`              | Rounds results to either the upper or lower nearest single-precision numbers                                          |
| `-Xsparallel=2`                     | Uses 2 cores when compiling the bitstream through Quartus®                                                            |
| `-Xsseed=2`                         | Uses seed 2 during Quartus®, yields slightly higher f<sub>MAX</sub>                                                |
| `-Xshyper-optimized-handshaking=on` | Applied on Agilex5 device, on by default on Agilex7 and Stratix10 devices. Improves throughput/area on larger devices |


#### Preprocessor Define Flags


| Flag                          | Description                                                                                                                                                                  |
| ----------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `-DSET_OUTER_UNROLL=<N>`      | Sets the value for the constant OUTER_UNROLL to N, controls the number of CRRs that can be processed in parallel. The default value is 1 for all target platforms.           |
| `-DSET_INNER_UNROLL=<N>`      | Sets the value for the constant INNER_UNROLL to N, controls the degree of parallelization within the calculation of 1 CRR. The default value is 64 for all target platforms. |
| `-DSET_OUTER_UNROLL_POW2=<N>` | Sets the value for the constant OUTER_UNROLL_POW2 to N, controls the number of memory banks. The default value is 1 for all target platforms.                                 |


> **Note**: The `Xsseed` values differ depending on the board being targeted. You can find more information about the unroll factors in `/src/CRR_common.hpp`.

## Build the `CRR Binomial Tree` Sample

> **Note**: When working with the command-line interface (CLI), you should configure the HLS IP Gen Compiler using environment variables.
> Set up your CLI environment by sourcing the `fpgavars` script located in the root of your HLS IP Gen Compiler installation every time you open a new terminal window.
> This practice ensures that your compiler, libraries, and tools are ready for development.
>
> Linux*:
> - `source <install-dir>/fpgavars.sh`
> - For non-POSIX shells, like csh, use the following command: `bash -c 'source <install-dir>/fpgavars.sh ; exec csh'`

### On Linux*

1. Change to the sample directory.
2. Configure the build system for the Agilex® 7 device family, which is the default.
  ```
   mkdir build
   cd build
   cmake ..
  ```
  > **Note**: You can change the default target by using the following command. **Targeting a BSP is not supported.**
  ```
   cmake .. -DFPGA_DEVICE=<FPGA device family or FPGA part number>
  ```
3. Compile the design. (The provided targets match the recommended development flow.)
  1. Compile for emulation (fast compile time, targets emulated FPGA device).
    ```
     make fpga_emu
    ```
  2. Compile for simulation (fast compile time, targets simulator FPGA device).
    ```
     make fpga_sim
    ```
  3. Generate the HTML performance report.
    ```
     make report
    ```
     The report resides at `<project name>/reports/report.html`.
  4. Compile for FPGA hardware (longer compile time, targets FPGA device).
    ```
     make fpga
    ```

## Run the `CRR Binomial Tree` Program

1. Run the sample on the FPGA emulator (the kernel executes on the CPU).
  ```
   ./crr.fpga_emu <input_file> [-o=<output_file>]
  ```
   where:
  - `<input_file>` is an **optional** argument to specify the input data file name. The default input file is `src/data/ordered_inputs.csv`.
  - `-o=<output_file>`  is an **optional** argument to specify the name of the output file. The default name of the output file is `ordered_outputs.csv`.
2. Run the sample on the FPGA simulator.
  > **Note**: The simulator runs significantly slower than the emulator and FPGA hardware. Use the smaller input file `small_ordered_inputs.csv` (the default for simulator) instead of the larger `ordered_inputs.csv`. Large inputs can make the simulation take a very long time to finish.
  ```
   CL_CONTEXT_MPSIM_DEVICE_INTELFPGA=1 ./crr.fpga_sim small_ordered_inputs.csv [-o=<output_file>]
  ```

## Example Output

```
Running on device: ofs_n6001 : Intel OFS Platform (ofs_ec00000)

============= Correctness Test ============= 
Running analytical correctness checks... 
CPU-FPGA Equivalence: PASS

============= Throughput Test =============
   Avg throughput:   329.5 assets/s
```

## License

Code samples are licensed under the MIT license. See [License.txt](/License.txt) for details.

Third party program Licenses can be found here: [third-party-programs.txt](/third-party-programs.txt).