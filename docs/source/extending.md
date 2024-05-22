# Extending HLSFactory

We encourage the FPGA and High-Level Synthesis (HLS) community to contribute their own HLS designs and tool flows to HLSFactory. This section provides a guide on how to extend HLSFactory with new built-in datasets and flows that can be utilized by other users.

The goal is to centralize design and flow support for different HLS tools, creating a common platform for research and experimentation.

## Contributing New Built-In Datasets

HLSFactory already supports loading user designs and design datasets at runtime. However, we encourage users to contribute their designs and design datasets to HLSFactory itself as a built-in package so that they can be shared and used by all HLSFactory users.

If you have any questions upfront or would like some guided assistance in integrating new designs and design datasets into HLSFactory, please reach out to the HLSFactory maintainers for help. You can contact us either by email or by opening an issue on the HLSFactory GitHub repository. We are happy to help and provide guidance on how to integrate your designs into HLSFactory.

To add a new design or collection of designs as a dataset, follow the concrete steps outlined below.

### Step 1: Prepare the Individual Designs

The first step is to prepare the individual designs that you want to add to the HLSFactory package. Each design should be a single directory that contains all the source files, data files, and build files needed for the designs.

Additionally, each design needs to implement the required entry points and other requirements needed for different kinds of flows.

#### Preparing a Xilinx-Based Design

Xilinx-based flows require two different entry point Tcl scripts that must be included in the top level of the design directory:

- `dataset_hls.tcl`: This script must be defined by the user to take a design, set up a Vitis HLS project, add the source files to the project, create a single solution, and run `csynth` on the solution successfully. The `VitisHLSSynthFlow` class will call this entry point using Vitis HLS and then look for the resulting HLS project and solution files in the design directory.
- `dataset_hls_ip_export.tcl`: This script must be defined by the user to open an existing project, open an existing synthesized solution, and then call the Vitis HLS `export_design` command with the argument `-flow impl` to export the synthesized design to Vivado and run the Vivado implementation flow. The `VitisHLSImplFlow` class will call this entry point using Vitis HLS and then look for the resulting Vivado project and solution files in the design directory.

If these files are not present, and one of these Xilinx flows is run on the design, the flow will fail, and helpful error messages will be raised for the user to indicate that the entry points are missing.

Your final design directory for a Xilinx-ready design should look as follows:

```text
- my_design/
    - dataset_hls.tcl
    - dataset_hls_ip_export.tcl
    - <the rest of your design files...>
```

#### Preparing an Intel-Based Design

```{todo}
Intel flow entry points are under construction. Please reach out for more information and help in the meantime.
```

#### Preparing an OptDSL Template

If you would like a design to be supported by the OptDSL frontend, you must provide an OptDSL template file named `opt_template.tcl` that follows the OptDSL syntax. This file should be placed in the top level of the design directory. The OptDSL frontend will look for this file and use it to generate enumerated designs based on the design space defined in the template file using the OptDSL syntax. If the file is not present, the OptDSL frontend flow will fail, and helpful error messages will be raised for the user to indicate that the entry point is missing.

For more details on the specific syntax of the OptDSL template file, please refer to the OptDSL documentation.

The design directory with OptDSL support should look as follows:

```text
- my_design/
    - opt_template.tcl
    - <the rest of your design files...>
```

Note that supporting OptDSL is optional, and if you do not want to support OptDSL for your design, you can skip this step. This makes sense in cases where you have design variations already prepared as separate designs, want to add very vendor-specific designs, or just want to add designs to be built "as is".

### Step 2: Organize Your Designs

If you have multiple designs that you want to add to the HLSFactory package, you should organize them in a single directory that contains all of the designs. This directory will represent a design dataset that can be loaded by HLSFactory.

For example, you may have a final organized design directory with the following directory structure:

```text
- my_dataset/
    - design1/
        - dataset_hls.tcl
        - dataset_hls_ip_export.tcl
        - <the rest of your design files...>
    - design2/
        - dataset_hls.tcl
        - dataset_hls_ip_export.tcl
        - <the rest of your design files...>
    - <more designs...>
```

If you are adding just a single design, you should make a directory for that design and place the design files in that directory. This will act as a dataset with a single design. This provides better organization and makes it easier to add more designs in the future.

### Step 3: Clone the HLSFactory Repository

To add the new designs to the HLSFactory package, you will need to clone the HLSFactory repository to your local machine. You can do this by running the following command:

Using SSH:
```bash
git clone git@github.com:sharc-lab/HLSFactory.git
```

Using HTTPS:
```bash
git clone https://github.com:sharc-lab/HLSFactory.git
```

### Step 4: Add the Designs to the Correct Directory in the HLSFactory Package

With the repository cloned to your local machine, you can add your designs to the HLSFactory package.

All built-in packages are located in the `hlsfactory/hls_dataset_sources` directory of the HLSFactory repository. You should directly copy your prepared design dataset directory into the `hls_dataset_sources` directory.

For example, if you have a design dataset directory named `my_dataset` that you want to add to the HLSFactory package, you should copy the `my_dataset` directory into the `hls_dataset_sources` directory.

```text
- HLSFactory/
    - hlsfactory/
        - hls_dataset_sources/
            - <polybench, machsuite, chstone, other built-in datasets...>
            - my_dataset/
                - design1/
                    - <design and build files...>
                - design2/
                    - <design and build files...>
                - <more designs...>
```

Keep in mind that whatever files are in your design dataset directory will be copied to the HLSFactory package, so make sure that you only include the necessary files for your designs. Including large and unnecessary data files can bloat the HLSFactory package and make it harder to maintain as well as hit the git file size limit. It's okay to include data files that are needed for testing or co-simulation, but try to manage these data files so that they can be included in the package in a reasonable way.

### Step 5: Add Support for the New Dataset in the `hlsfactory.datasets_builtin` Module

To allow users to load your new designs and design dataset in HLSFactory, you need to add support for the new dataset in the `hlsfactory.datasets_builtin` module. This module is implemented in the `hlsfactory/datasets_builtin.py` file in the HLSFactory repository.

There are several key updates to make to this module to add support for your new dataset. All examples assume the new dataset is named `my_dataset`.

#### Define the Relative Path to Your Dataset Directory

At the top of the module Python file, there are lines defining the relative paths to the different dataset directories in the HLSFactory package. You should add a new line to define the relative path to your new dataset directory.

```python
DIR_DATASET_POLYBENCH = HLS_DATASET_DIR / "polybench"
DIR_DATASET_MACHSUITE = HLS_DATASET_DIR / "machsuite"
...
DIR_DATASET_MY_DATASET = HLS_DATASET_DIR / "my_dataset" # New line for your dataset
```

#### Create a Dataset Builder Function for Your Dataset

Next, you need to create a new dataset builder function for your dataset. This function should be named `dataset_builder_my_dataset` and should take the `work_dir` and `dataset_labels` as arguments. The function should return a list of `Design` objects that represent the designs in your dataset.

The function should have the following function signature:

```python
def dataset_<your_dataset_name>_builder(name: str, work_dir: Path) -> DesignDataset: ...
```

In most cases, the dataset builder will look as follows and can be used as the same implementation for your dataset builder.

```python
def dataset_my_dataset_builder(name: str, work_dir: Path) -> DesignDataset:
    check_dataset_dir_exists(DIR_DATASET_MY_DATASET)
    new_dir = work_dir / DIR_DATASET_MY_DATASET.name
    shutil.copytree(DIR_DATASET_MY_DATASET, new_dir)
    return DesignDataset.from_dir(name, new_dir)
```

#### Add Your Dataset Builder to the `DATASET_STR_MAP` Dictionary

Finally, you need to add your new dataset builder function to the `DATASET_STR_MAP` dictionary in the `hlsfactory.datasets_builtin` module. This dictionary maps the dataset labels to the dataset builder functions. You should add a new entry to the dictionary with the key as the dataset label and the value as the dataset builder function.

```python
DATASET_STR_MAP = {
    "polybench": dataset_polybench_builder,
    "machsuite": dataset_machsuite_builder,
    ...
    "my_dataset": dataset_my_dataset_builder, # New line for your dataset
}
```

### Step 6: Update the Documentation to Describe the New Dataset

Please also update the HLSFactory documentation to include your new dataset in the list of built-in datasets. This will help other users know that your dataset is available and provide more information about it.

You will mainly need to update the `built_in_datasets.md` page, which is located in the `docs/source/built_in_datasets.md` file in the HLSFactory repository. The documentation is implemented with Sphinx with the Myst plugin. This means that the documentation is simply written in markdown, which should be familiar to most users.

When adding your dataset documentation, make a new subheading and include information about your dataset. Try to follow the same format as the other datasets in the documentation to keep it consistent and easy to read. This includes how many designs are in the dataset, what HLS tools and vendors are supported, OptDSL support, source links, and other relevant information about the dataset in general or specific designs in the dataset.

### Step 7: Create a Pull Request to the HLSFactory Repository

Once you have added your designs to the HLSFactory package, updated the built-in datasets module, and updated the documentation, you can create a pull request to the HLSFactory repository. This will allow the HLSFactory maintainers to review your changes to add a new dataset to the HLSFactory package. From here, the maintainers can provide feedback, discuss any changes that need to be made, and eventually merge your changes into the HLSFactory package.

## Contributing New Flows

Currently, HLSFactory has implemented flows for Xilinx and Intel HLS tools, along with an OptDSL frontend for both vendors.

However, users might want to use a different set of vendor HLS tools that are not currently built-in. These can be created by subclassing the `Flow` abstract base class (particularly the `Frontend(Flow)` or `ToolFlow(Flow)` class, depending on what kind of flow you are implementing, as these are just aliases) and adding the necessary functionality to interact with vendor tools, extract, and process data.

As with designs, these flows can be created locally by users in their own projects, but we encourage users to contribute these flows to the HLSFactory package so that they can be shared and used by all HLSFactory users.

```{todo}
Details on how to contribute new flows are under construction. Please reach out for more information and help in the meantime.
```
