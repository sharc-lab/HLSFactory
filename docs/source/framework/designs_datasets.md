# Designs and Design Datasets

In HLSFactory, a "Design" refers to a single HLS design, while a "Design Dataset" is a collection of multiple HLS designs. These concepts are encapsulated by the `Design` and `DesignDataset` classes within HLSFactory.

A "Design" is simply a directory/folder on the disk, and a "Design Dataset" is a directory containing multiple "Design" directories. The primary function of the `Design` and `DesignDataset` classes is to manage the directory paths of designs and design datasets, along with basic metadata such as the design name or dataset name.

These classes offer numerous helper functions to build, modify, copy, and move designs and design datasets. Users can leverage these classes to create their own HLS design datasets for executing HLSFactory workflows and data aggregation on their custom HLS designs.

Below is an example of what this directory structure might look like:

```text
- /polybench_dataset
    - /gemm
        - <contents for the gemm design>
    - /2mm
        - <contents for the 2mm design>
    - <other designs...>
```

In this structure, each subdirectory of `polybench_dataset` represents a "Design," and the entire `polybench_dataset` itself constitutes a "Design Dataset."

Generally, a design directory will contain the HLS source files, build scripts, and any other necessary files for building, testing, or running the HLS design. To ensure HLSFactory functions correctly across a broad range of designs, which may utilize different build systems or workflows, we specify certain requirements for what should be included in a design directory. These requirements are discussed in the flow-specific sections of the documentation.

## Creating `Design` Objects

This section demonstrates how to load a single design into a `Design` object.

Assume there is an HLS design directory on the disk. For instance, let's assume there is an HLS design for a generic matrix multiplication (GEMM) kernel in the directory `~/hls_designs/gemm/`.

```python
from hlsfactory.framework import Design

my_design = Design("~/hls_designs/gemm/", custom_single_design_fp)
```

This command creates a `Design` referencing the design source "in-place." For example, running a workflow on this design might modify the source files and generate project files directly within the local design directory. 

Typically, the goal is not to modify the original source files; ideally, source files should be kept clean and organized in one place, and the design copied to a new working or run directory where workflows can be executed. The following code illustrates how to use the `copy_to_new_parent_dir` function of the `Design` class to assist with moving, copying, and renaming designs.

Here, we simply copy the design to a new working directory using the `copy_to_new_parent_dir` function of the `Design` class. `WORK_DIR` is a `pathlib.Path` object that points to the new directory where we intend to copy the design.

```python
my_design = my_design.copy_to_new_parent_dir(WORK_DIR)
```

Several other helper functions are available for setting up and running custom experiments and workflows. These helper functions exist for both `DesignDataset` and `Design` classes.

## Creating `DesignDataset` Objects

This section demonstrates how to create a `DesignDataset` object containing a single design.

First, instantiate the `DesignDataset` object by specifying a dataset name, a directory for the dataset, and an initial empty list of designs. Here, we use the raw class initializer instead of one of the helper class methods.

Next, add a design to the dataset using the `add_design_copy` method from the `DesignDataset` class. This method copies the design directory into the dataset directory and updates the relevant metadata in each object.

```python
from hlsfactory.framework import DesignDataset

dataset_dir = WORK_DIR / "my_dataset_with_one_design"

my_dataset_with_one_design = DesignDataset(
    "my_dataset_with_one_design",
    dataset_dir,
    designs=[],
)

my_dataset_with_one_design.add_design_copy(my_design)
```

If you have a folder of individual design directories, you can also construct a `DesignDataset` object with all the designs in that directory using the `from_dir` helper class method of the `DesignDataset` class.

Here's how to do so using the local directory `./custom_dataset_folder`, which contains the directories `./custom_dataset_folder/design_a` and `./custom_dataset_folder/design_b` for two designs.

The directory structure would appear as follows:

```text
- /custom_dataset_folder
    - /design_a
        - <contents for design_a>
    - /design_b
        - <contents for design_b>
```

The following code snippet demonstrates how to load this design dataset into a `DesignDataset` object.

```python
dataset_sources = "custom_dataset_folder"
assert dataset_sources.exists()

my_dataset_with_several_designs = DesignDataset.from_dir(
    "my_dataset_with_several_designs",
    dataset_sources,
).copy_dataset(WORK_DIR)
```

## `DesignDatasetCollection` Objects

Sometimes, a user might want to manage multiple datasets simultaneously or work with a collection of datasets. For this convenience, HLSFactory defines a type alias `DesignDatasetCollection`, which is a dictionary of `DesignDataset` objects: `dict[str, DesignDataset]`. Many of the functions and interfaces that are part of the `Flow` classes, described later, either take a `DesignDatasetCollection` as an argument or return one as output.

## Loading Built-in Designs

To centralize the efforts of the HLS research community, we include commonly used HLS design datasets in the HLSFactory package, which can be loaded directly into a `DesignDataset` object and placed in a user-defined working directory. For this, we include `datasets_builtin`, which contains utility functions to load these built-in datasets.

For more information about the available built-in datasets, refer to the [built_in_datasets documentation](../built_in_datasets).

Below is an example of how to load the PolyBench dataset into a `DesignDataset` object.

```python
from hlsfactory.datasets_builtin import dataset_polybench_builder

my_polybench_dataset = dataset_polybench_builder("my_polybench_dataset", WORK_DIR)
```

Here is an example of how to load both the PolyBench and MachSuite datasets into a `DesignDatasetCollection` object.

```python
from hlsfactory.datasets_builtin import datasets_builder

my_design_datasets = datasets_builder(
    ["polybench", "machsuite"], # dataset_names: IDs used to uniquely identify the datasets
    ["my_polybench_dataset", "my_machsuite_dataset"], # dataset_labels: User-provided names for the created DesignDataset objects
    WORK_DIR
)
```

## "Abstract" vs. "Concrete" Designs

In the HLSFactory framework, it is possible to enumerate a single design source into multiple variations of that design via a "frontend" flow. The designs that enter a frontend flow may not be complete or synthesizable as they require further processing. For example, they might need source-to-source transformations, templating, preprocessing with different defines, or the generation of auxiliary directives. These designs are termed "abstract" designs, or designs that require further elaboration by a frontend flow.

In contrast, "concrete" designs are those that can be directly synthesized and built by vendor-specific HLS and implementation tools via a "tool" flow. These designs can be provided as is by the user, for instance, a completed hardware kernel ready to be processed by Vitis HLS.

Concrete designs also result from the output of a frontend flow, where the frontend has completed all necessary processing and transformation of an abstract design into a concrete design. The most common example of a frontend flow is the OptDSL flow, used to take designs with a user-specified design space of optimization directives and generate initial concrete designs with optimization directive values sampled from the design space. A simpler example, as previously mentioned, is a templating frontend which simply substitutes multiple template values into source files to create multiple resulting concrete designs.

There is no explicit labeling or tracking of whether a design is abstract or concrete within the HLSFactory framework. It is up to the user to keep track of this information and ensure that the correct designs are fed into the appropriate flows. This discussion is intended to offer the user a better conceptual model of how to use frontends and create their own to refine abstract designs, as well as to understand that they can take existing "concrete" designs and directly integrate them into backend vendor tool flows.
