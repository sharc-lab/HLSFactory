# Data Aggregation and Packaging

Once designs have been built, the design directories contain reported data from tools such as reported timing, resource, and power usage, as well as HLS artifacts such as generated IR bitcode, scheduling and binding information, and generated IP/HDL components.

For most use cases, we would like to structure, aggregate, and package this data for end users to import and easily use for their own purposes. For example, we would like to aggregate tabular data across multiple designs into a common format like CSV or JSON, automatically find, compress, and archive artifacts, and automatically perform aggregation and packaging across multiple design datasets and vendors.

In some cases, users would like to combine their own data with previously generated and published data. For example, a paper may release the final QoR / reported data from HLS synthesis or design simulations, and we would like to cleanly fold it into the tabular and artifact data that might be generated from HLSFactory flows.

HLSFactory provides code to address all these use cases for the end user in an easy-to-use and easy-to-extend way.

## Output Files

Each flow writes JSON into the design directory. Aggregators read these files and merge them into one `CompleteHLSData` record.

| File | Written by | Contents |
|------|------------|----------|
| `data_design.json` | `VitisHLSSynthFlow` | `name`, `part`, `target_clock_period`, `version_vitis_hls` |
| `data_hls.json` | Each synthesis flow | Latency, clock period and area or resource estimates. The fields depend on the tool (see below). |
| `data_implementation.json` | `VitisHLSImplReportFlow` | `power__*`, `utilization__*` and `timing__*` (WNS, TNS, WHS, THS, clock period and frequency) |
| `execution_data.json` | All flows, when logging is enabled | Per flow: `status`, `t_start`, `t_end`, `dt`, `core`, `error_message` |

`data_hls.json` fields by flow:

- **Vitis HLS:** `clock_period`, best, average and worst latency in cycles, and LUT, FF, DSP, BRAM and URAM usage.
- **Catapult:** `latency_cycles`, `throughput_cycles`, `initiation_interval_cycles`, critical-path delay and slack, and total, combinational and sequential area. The area is a standard-cell estimate for the libraries in the design's Tcl script, not FPGA resources.
- **XLS:** `latency_cycles`, `initiation_interval`, `flop_count`, four estimated path delays in picoseconds (`null` when that path class doesn't exist), and operation counts. The delays come from the XLS estimator, not place-and-route, and device resources need a separate implementation flow.

`CompleteHLSData.to_flat_dict()` prefixes keys by section: `design__*`, `synthesis__*`, `implementation__*` and `execution__*`. For example, Catapult's `latency_cycles` becomes `synthesis__latency_cycles`.

Besides the JSON, the flows leave tool reports and generated files (RTL, IR, logs) in the design directory. {ref}`Data Aggregation for Other Vendors <other-vendor-aggregation>` lists which of these each aggregator can package. For the files each XLS option produces, see the [Google XLS tutorial](../tutorials/xls_flow).

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
from pathlib import Path

from hlsfactory.data_packaging import CompleteHLSData

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

### Abstract Methods

Below is the set of abstract methods a `DataAggregator` subclass can implement:

- `gather_hls_design_data(design)`: Gathers design data for the specified design.
- `gather_hls_synthesis_data(design)`: Gathers synthesis data for the specified design.
- `gather_implementation_data(design)`: Gathers implementation data for the specified design.
- `gather_execution_data(design)`: Gathers execution data for the specified design.

A `DataAggregator` subclass can implement one or more of these methods to gather data from different HLS tools and flows and does not need to implement all of them. This is because not all runs and flows may generate all types of data. This also means that end users can call a subset of these methods to gather only the data they are interested in.

## Xilinx Data Aggregation

The `DataAggregatorXilinx` class is an implementation of the `DataAggregator` abstract base class, specifically for handling data from Xilinx HLS solutions. It provides methods for gathering various types of data and artifacts from Xilinx HLS solutions.

```python
from pathlib import Path

from hlsfactory.data_packaging import DataAggregatorXilinx
from hlsfactory.framework import Design

# Create a DataAggregatorXilinx instance
aggregator = DataAggregatorXilinx()

# Assume we have a Design object
design = Design("design1", Path("/path/to/design1"))

# Gather design data
design_data = aggregator.gather_hls_design_data(design)
print(design_data)

# Gather synthesis data
synthesis_data = aggregator.gather_hls_synthesis_data(design)
print(synthesis_data)

# Gather implementation data
implementation_data = aggregator.gather_implementation_data(design)
print(implementation_data)

# Gather execution data
execution_data = aggregator.gather_execution_data(design)
print(execution_data)

# Gather synthesis artifacts
artifacts = aggregator.gather_hls_synthesis_artifacts(design)
print(artifacts)
```

(other-vendor-aggregation)=
## Data Aggregation for Other Vendors

Every flow writes its results as JSON in the design directory, so reading design, synthesis, implementation, and execution data is the same for all vendors. All aggregators share that logic through `JSONFileDataAggregator`. What differs per vendor is which **artifacts** a flow leaves on disk. The vendor aggregators below subclass `ArtifactAggregator`, which finds artifacts by file-name pattern and packages them in the archive.

| Aggregator | Flow whose status it checks | Artifact kinds |
|------------|----------------------------|----------------|
| `DataAggregatorXilinx` | `VitisHLSSynthFlow` | `ir`, `adb`, `report`, `hdl`, `ip` |
| `DataAggregatorVitisModern` | `VitisHLSModernSynthFlow` | `report`, `hdl` |
| `DataAggregatorIntel` | `IntelHLSSynthFlow` | `report`, `hdl`, `quartus` |
| `DataAggregatorAltera` | `AlteraHLSSynthFlow` | `report`, `interface`, `hdl`, `log` |
| `DataAggregatorCatapult` | `CatapultHLSSynthFlow` | `report`, `rtl` |
| `DataAggregatorStratus` | `StratusHLSSynthFlow` | `rtl`, `report`, `log` |
| `DataAggregatorXLS` | `XLSHLSSynthFlow` | `ir`, `rtl`, `metrics`, `interface`, `options`, `source_map`, `debug` |

Each aggregator has a matching enum of artifact kinds (`ArtifactsXLS`, `ArtifactsCatapult`, and so on). Usage is the same for all of them:

```python
from hlsfactory.data_packaging import ArtifactsXLS, DataAggregatorXLS

aggregator = DataAggregatorXLS()

# One design: JSON data plus an in-memory zip of every artifact kind
result = aggregator.gather_all_data(design)

# Only some artifact kinds; kinds with no files come back as an empty list
artifacts = aggregator.gather_hls_synthesis_artifacts_data(
    design,
    artifacts_to_extract={ArtifactsXLS.RTL, ArtifactsXLS.METRICS},
)
# {"rtl": [Path(...)], "metrics": [...], "ir": None, ...}

# Many designs, packaged into one archive
data = aggregator.gather_multiple_designs(dataset.designs, n_jobs=8)
aggregator.aggregated_data_to_archive(data, Path("data.zip"))
```

Behavior shared by the `ArtifactAggregator` subclasses:

- Artifacts are matched by glob patterns relative to the design directory; only files are collected. Unrequested kinds map to `None`.
- By default a requested kind that matches no files maps to `[]` and collection continues. Pass `error_if_missing_data=True` to raise `FileNotFoundError` instead. `DataAggregatorXilinx` follows the same rule.
- If `execution_data.json` records the vendor's synthesis flow as `error` or `timeout`, no artifacts are collected and an empty dictionary is returned.
- Inside the archive, files are stored as `artifacts/<kind>/<path relative to the design directory>`, so files with the same name in different directories don't collide. (`DataAggregatorXilinx` still stores `artifacts/<kind>/<file name>`.)

To support another tool, subclass `ArtifactAggregator`, define an enum of artifact kinds, and set `ARTIFACT_PATTERNS` and `SYNTH_FLOW_NAME`.

```{note}
The artifact patterns for the non-Xilinx vendors follow the file layouts that the HLSFactory flows read and write. They are checked against synthetic directory trees in the test suite, not against output from the vendor tools themselves, so adjust a pattern if your tool version places files elsewhere.
```

## HLSyn Data Aggregation

There is no dedicated HLSyn aggregator. The `hlsyn` and `hlsyn-vitis` sources are Vitis-based, so `DataAggregatorXilinx` applies to designs that have been run through the Xilinx flows.
