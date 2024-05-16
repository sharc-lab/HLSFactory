# Flows

In HLSFactory, different `Flow` classes are defined to perform certain build steps, run external vendor tools on a design, or process designs with custom logic. These classes encapsulate the logic for executing these steps. The abstract base class `Flow` defines the interface for all flows and outlines a common set of functions to execute flows in parallel across multiple designs and datasets.

Each subclass of the `Flow` class defines an abstract method called `execute` with the following function signature:

```python
class Flow(ABC):
    ...
    @abstractmethod
    def execute(self, design: Design, timeout: float | None = None) -> list[Design]: ...
```

This method contains the core logic for each `Flow` subclass. For instance, the `VitisHLSSynthFlow(ToolFlow)` includes logic in its `execute` method to call the Vitis HLS synthesis tool on the design and return the resulting design object with the HLS synthesis results extracted and stored alongside the design source on disk.

The function signature might be confusing. For example, why would a flow that synthesizes a single design return a list of designs? There are two possible outcomes: 1) the synthesis is successful, and the design is returned as a list with a single element: `[design]`, or 2) the synthesis fails or times out and returns an empty list: `[]`. Other flows, like frontend flows, can elaborate a single base design into multiple new designs. In this case, the list of designs returned by the `execute` method would contain all the new designs created from the base design: `[design_new_1, design_new_2, ...]`. This pattern of returning a list of designs is used for all flows and has additional benefits, such as allowing ergonomic parallelization and combining the results of multiple designs and datasets executed in parallel.

We also include an optional timeout parameter in the `execute` method to allow user-defined flows to check for a timeout when calling and executing external tools, which may be long-running. For example, when exploring different combinations of optimization directives, a certain combination might lead a design to have unreasonably long synthesis times, such as by unrolling a huge loop or completely partitioning a large array. In this case, the user can set a timeout for the flow to stop the synthesis and return no results if the flow takes longer than the timeout.

Users will rarely use the `execute` function directly to call the flow on a single design. Instead, users will more commonly use the following helper functions of the `Flow` class to run the flow on a single design, a list of designs, or a dataset of designs:

```python
def execute_multiple_designs(
    self,
    designs: list[Design],
    n_jobs: int = 1,
    cpu_affinity: list[int] | None = None,
    timeout: float | None = None,
) -> list[Design]: ...

def execute_multiple_design_datasets_naive_parallel(
    self,
    design_datasets: DesignDatasetCollection,
    copy_dataset: bool,
    new_dataset_name_fn: Callable[[str], str] | None = None,
    n_jobs: int = 1,
    cpu_affinity: list[int] | None = None,
    timeout: float | None = None,
) -> DesignDatasetCollection: ...

def execute_multiple_design_datasets_fine_grained_parallel(
    self,
    design_datasets: DesignDatasetCollection,
    copy_dataset: bool,
    new_dataset_name_fn: Callable[[str], str] | None = None,
    n_jobs: int = 1,
    cpu_affinity: list[int] | None = None,
    par_chunksize: int | None = 1,
    timeout: float | None = None,
) -> DesignDatasetCollection: ...
```

However, users typically call these functions using the default values for the optional parameters:

```python
output_designs = my_flow.execute_multiple_designs(my_designs, n_jobs=N_JOBS)
my_datasets_v0 = my_flow.execute_multiple_design_datasets_naive_parallel(my_design_datasets, True, n_jobs=N_JOBS)
my_datasets_v1 = my_flow.execute_multiple_design_datasets_fine_grained_parallel(my_design_datasets, True, n_jobs=N_JOBS)
```

We always recommend using the `execute_multiple_design_datasets_fine_grained_parallel` function over the `execute_multiple_design_datasets_naive_parallel` function when running flows on multiple datasets, as it doesn't block the execution of the flow for each dataset independently and results in faster overall execution time.

## Frontend Flows vs. Tool Flows

In HLSFactory, we distinguish between `Frontend(Flow)` and `ToolFlow(Flow)`. These classes don't contain any logic themselves (literally just `...`) but are used for better code organization in case of more complex code structures and flow features in the future.

However, there are fundamental functional differences in the logic that these two types of flows implement in their `execute` methods. Frontend flows generally take a single design and elaborate it into multiple new designs, while tool flows generally take a single design, run a tool on it, and return the same design but with the output of the tool stored alongside the design source.

## Frontend Flows and OptDSL

As mentioned, frontend flows are used to generate multiple enumerated designs from a single base design. This can be as simple as templating a design with different values or as complex as custom source-to-source transformation of the HLS designs. The most common use case for frontend flows is to expand the design space of a single design by generating design variations of a base design with different combinations of optimization directives and values. For example, we may take a base design with a loop and generate four new designs, each with a loop unroll pragma and different unroll factors for the loop. This way, we have four different designs we can synthesize, each resulting in a different implementation.

## Xilinx Flows

## Intel Flows
