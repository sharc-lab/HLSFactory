# Data Aggregation and Packaging

Once designs have been built, the design directories contain reported data from tools such as reported timing, resource, and power usage, as well as HLS artifacts such as generated IR bitcode, scheduling and binding information, and generated IP/HDL components.

For most use cases, we would like to structure, aggregate, and package this data for end users to import and easily use for their own purposes. For example, we would like to aggregate tabular data across multiple designs into a common format like CSV or JSON, automatically find, compress, and archive artifacts, and automatically perform aggregation and packaging across multiple design datasets and vendors.

In some cases, users would like to combine their own data with previously generated and published data. For example, a paper may release the final QoR / reported data from HLS synthesis or design simulations, and we would like to cleanly fold it into the tabular and artifact data that might be generated from HLSFactory flows.

HLSFactory provides code to address all these use cases for the end user in an easy-to-use and easy-to-extend way.

## `CompleteHLSData` Class

The `CompleteHLSData` class is designed to store and manage data from different HLS and implementation flows. It also provides methods to convert this data into different formats such as flat dictionaries, JSON, and CSV.

### Attributes

- `design`: A dictionary containing design data.
- `synthesis`: A dictionary containing synthesis data.
- `implementation`: A dictionary containing implementation data.
- `execution`: A dictionary containing execution data.
- `artifacts`: An in-memory archive of artifacts.

### Methods

- `to_flat_dict()`: Converts the data to a flat dictionary.
- `to_json()`: Converts the data to a JSON string.
- `to_json_file(file_path)`: Saves the data as a JSON file.
- `to_csv()`: Converts the data to a CSV string.
- `to_csv_file(file_path)`: Saves the data as a CSV file.

### Example Usage

```python
from data_packaging import CompleteHLSData
from pathlib import Path

# Create a CompleteHLSData instance
hls_data = CompleteHLSData(
    design={"name": "design1", "version": "1.0"},
    synthesis={"timing": 10.5, "resource_usage": {"LUT": 1000}},
    implementation={"area": 2000, "power": 5.5},
    execution={"runtime": 100},
    artifacts=None
)

# Convert to flat dictionary
flat_dict = hls_data.to_flat_dict()
print(flat_dict)

# Convert to JSON
json_data = hls_data.to_json()
print(json_data)

# Save to JSON file
hls_data.to_json_file(Path("output.json"))

# Convert to CSV
csv_data = hls_data.to_csv()
print(csv_data)

# Save to CSV file
hls_data.to_csv_file(Path("output.csv"))
```

## `DataAggregator` Objects

The `DataAggregator` class is a utility class that provides a set of methods to aggregate and package data from multiple designs and datasets. The `DataAggregator` class is designed to be used in conjunction with the `DesignDataset` and `DesignDatasetCollection` classes to aggregate and package data across multiple designs and datasets.

### Abstarct Methods

Below is the set of abstract methods a `DataAggregator` subclass can implement:

- `gather_hls_design_data(design)`: Gathers design data for the specified design.
- `gather_hls_synthesis_data(design)`: Gathers synthesis data for the specified design.
- `gather_hls_implementation_data(design)`: Gathers implementation data for the specified design.
- `gather_hls_execution_data(design)`: Gathers execution data for the specified design. -->

A `DataAggregator` subclass can implement one or more of these methods to gather data from different HLS tools and flows and does not need to implement all of them. THis is because not all runs and flows may generate all types of data. This also means that end users can also call a subset of these methods to gather only the data they are interested in.

## Xilinx Data Aggregation

The `DataAggregatorXilinx` class is an implementation of the `DataAggregator` abstract base class, specifically for handling data from Xilinx HLS solutions. It provides methods for gathering various types of data and artifacts from Xilinx HLS solutions.

```python
from data_packaging import DataAggregatorXilinx, Design

# Create a DataAggregatorXilinx instance
aggregator = DataAggregatorXilinx()

# Assume we have a Design object
design = Design(name="design1", path="/path/to/design1")

# Gather design data
design_data = aggregator.gather_hls_design_data(design)
print(design_data)

# Gather synthesis data
synthesis_data = aggregator.gather_hls_synthesis_data(design)
print(synthesis_data)

# Gather implementation data
implementation_data = aggregator.gather_hls_implementation_data(design)
print(implementation_data)

# Gather execution data
execution_data = aggregator.gather_hls_execution_data(design)
print(execution_data)

# Gather synthesis artifacts
artifacts = aggregator.gather_hls_synthesis_artifacts(design)
print(artifacts)
```

## Intel Data Aggregation

```{todo}
Add details for Intel data aggregation implementation.
```

## HLSyn Data Aggregation

```{todo}
Add details for HLSyn data aggregation implementation.
```
