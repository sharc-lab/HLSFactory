# Data Aggregation and Packaging

After running synthesis and implementation flows, each design directory contains JSON files (e.g., `data_hls.json`, `data_implementation.json`) with latency, resource usage, timing, and power data. This tutorial shows how to aggregate that data into tabular formats (CSV, JSON) and archives.

## Aggregating Single Designs

Use `DataAggregatorXilinx` to gather data from Xilinx HLS solutions:

```python
from pathlib import Path
from hlsfactory.data_packaging import DataAggregatorXilinx
from hlsfactory.framework import Design

design = Design("atax", Path("/path/to/design_dir"))
aggregator = DataAggregatorXilinx()

# Gather all data (design metadata, synthesis, implementation, execution)
hls_data = aggregator.gather_all_data(design)

# Export to CSV or JSON
hls_data.to_csv_file(Path("output.csv"))
hls_data.to_json_file(Path("output.json"))
```

## Aggregating Multiple Designs

For many designs:

```python
from hlsfactory.data_packaging import DataAggregatorXilinx
from hlsfactory.framework import DesignDataset

dataset = DesignDataset.from_dir("polybench", Path("/path/to/work_dir/polybench"))
aggregator = DataAggregatorXilinx()

data_list = aggregator.gather_multiple_designs(dataset.designs, n_jobs=8)

# Aggregate to a single CSV
csv_str = aggregator.aggregated_data_to_csv(data_list)
Path("all_data.csv").write_text(csv_str)

# Or archive everything (CSV, JSON, and artifacts per design)
aggregator.aggregated_data_to_archive(data_list, Path("data.zip"))
```

## Key Fields

The aggregated data includes synthesis metrics (latency, clock period, LUT/FF/BRAM/DSP estimates), implementation metrics (WNS, TNS, power, final resource usage), and execution times. See the [Data Aggregation and Packaging](../framework/data) framework guide (Output Files section) for the full structure.

For the `CompleteHLSData` class, `DataAggregator` interface, and vendor-specific aggregation (Xilinx, Intel), see the [Data Aggregation and Packaging](../framework/data) framework guide.
