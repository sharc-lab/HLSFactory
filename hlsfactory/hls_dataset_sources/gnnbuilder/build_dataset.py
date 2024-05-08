import numpy as np

from pathlib import Path
import shutil
import sys


import torch.nn as nn
from torch_geometric.datasets import QM9


sys.path.append("/nethome/skaram7/gnn-builder")

import gnn_builder as gnnb
from gnn_builder.code_gen import FPX
from gnn_builder.models import MLP

CURRENT_SCRIPT_DIR = Path(__file__).parent

BUILD_DIR = CURRENT_SCRIPT_DIR / "designs"
BUILD_DIR.mkdir(parents=True, exist_ok=True)


DATASET_TEMP_DIR = CURRENT_SCRIPT_DIR / "tmp"

VITIS_HLS_PATH = Path("/tools/software/xilinx/Vitis_HLS/2023.1/")


def compute_max_nodes_and_edges(dataset):
    max_node = 0
    max_edge = 0
    for data in dataset:
        max_node = max(max_node, data.num_nodes)
        max_edge = max(max_edge, data.num_edges)
    return max_node, max_edge


def compute_average_nodes_and_edges(dataset, round_val=True):
    avg_nodes = 0
    avg_edges = 0
    count = 0
    for data in dataset:
        avg_nodes += data.num_nodes
        avg_edges += data.num_edges
        count += 1
    avg_nodes /= count
    avg_edges /= count
    if round_val:
        avg_nodes = int(round(avg_nodes))
        avg_edges = round(avg_edges)
    return avg_nodes, avg_edges


def compute_median_nodes_and_edges(dataset):
    nodes = []
    edges = []
    for data in dataset:
        nodes.append(data.num_nodes)
        edges.append(data.num_edges)
    median_nodes = int(np.median(nodes))
    median_edges = int(np.median(edges))

    return median_nodes, median_edges


def round_up_to_nearest_multiple(num, multiple):
    return multiple * ((num + multiple - 1) // multiple)


def build_model(
    dim_in,
    dim_out,
    conv,
    p_in_gnn=1,
    p_hidden_gnn=1,
    p_out_gnn=1,
    p_in_mlp=1,
    p_hidden_mlp=1,
    p_out_mlp=1,
):
    model = gnnb.GNNModel(
        graph_input_feature_dim=dim_in,
        graph_input_edge_dim=0,
        gnn_hidden_dim=128,
        gnn_num_layers=6,
        gnn_output_dim=64,
        gnn_conv=conv,
        gnn_activation=nn.ReLU,
        gnn_skip_connection=True,
        global_pooling=gnnb.GlobalPooling(["add", "mean", "max"]),
        mlp_head=MLP(
            in_dim=64 * 3,
            out_dim=dim_out,
            hidden_dim=64,
            hidden_layers=4,
            activation=nn.ReLU,
            p_in=p_in_mlp,
            p_hidden=p_hidden_mlp,
            p_out=p_out_mlp,
        ),
        output_activation=None,
        gnn_p_in=p_in_gnn,
        gnn_p_hidden=p_hidden_gnn,
        gnn_p_out=p_out_gnn,
    )

    return model


MAX_NODES = 600
MAX_EDGES = 600

PARALLEL_FACTOR = 8

CONVS = {
    "gcn": gnnb.GCNConv_GNNB,
    "gin": gnnb.GINConv_GNNB,
    "pna": gnnb.PNAConv_GNNB,
    "sage": gnnb.SAGEConv_GNNB,
}

DATASETS = {
    "qm9": QM9(root=f"{DATASET_TEMP_DIR.absolute()}/QM9").index_select(
        list(range(1000))
    ),
}

DATASET_TASK_TYPE = {
    "qm9": "regression",
    "esol": "regression",
    "freesolv": "regression",
    "lipo": "regression",
    "hiv": "classification",
}

combos = []
for conv in CONVS.keys():
    for dataset in DATASETS.keys():
        combos.append(
            {
                "conv_name": conv,
                "conv": CONVS[conv],
                "dataset_name": dataset,
                "dataset": DATASETS[dataset],
                "task_type": DATASET_TASK_TYPE[dataset],
                "max_nodes": MAX_NODES,
                "max_edges": MAX_EDGES,
                "parallel_factor": PARALLEL_FACTOR,
            }
        )


def make_project_single(combo):
    print(
        f"Making project for {combo['conv_name']} model on {combo['dataset_name']} dataset."
    )

    dataset = combo["dataset"]
    median_nodes, median_edges = compute_median_nodes_and_edges(dataset)
    # print(f"median_nodes: {median_nodes}")
    # print(f"median_edges: {median_edges}")

    median_degree = gnnb.utils.compute_median_degree(dataset)
    # print(f"median_degree: {median_degree}")

    num_features = dataset.num_features
    # print(f"num_features: {num_features}")

    task_type = combo["task_type"]
    # print(f"task_type: {task_type}")
    if task_type == "classification":
        dim_out = dataset.num_classes
    else:
        dim_out = dataset[0].y.ravel().shape[0]
    # print(f"dim_out: {dim_out}")

    if task_type == "classification":
        output_encoding = "classification_integer"
    elif task_type == "regression":
        output_encoding = "regression"
    else:
        raise ValueError(f"Unknown task_type: {task_type}")

    model = build_model(
        num_features,
        dim_out,
        combo["conv"],
        p_in_gnn=1,
        p_hidden_gnn=combo["parallel_factor"],
        p_out_gnn=combo["parallel_factor"],
        p_in_mlp=combo["parallel_factor"],
        p_hidden_mlp=combo["parallel_factor"],
        p_out_mlp=1,
    )
    project_name = f"fpga_{combo['conv_name']}_{combo['dataset_name']}"

    project = gnnb.Project(
        project_name,
        model,
        output_encoding,
        VITIS_HLS_PATH,
        BUILD_DIR,
        dataset=dataset,
        max_nodes=MAX_NODES,
        max_edges=MAX_EDGES,
        num_nodes_guess=median_nodes,
        num_edges_guess=median_edges,
        degree_guess=median_degree,
        float_or_fixed="fixed",
        fpx=FPX(32, 16),
        fpga_part="xcu280-fsvh2892-2L-e",
    )

    project.gen_hw_model()
    project.gen_testbench(gen_testbench_data=False)
    project.gen_makefile()
    project.gen_vitis_hls_tcl_script()


for combo in combos:
    make_project_single(combo)

shutil.rmtree(DATASET_TEMP_DIR)

print("Done.")
