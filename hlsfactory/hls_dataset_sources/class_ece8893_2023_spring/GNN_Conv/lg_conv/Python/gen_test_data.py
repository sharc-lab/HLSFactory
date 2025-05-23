from pathlib import Path
import os
import shutil

from rich.pretty import pprint as pt
import numpy as np
import networkx as nx
import torch

from torch_geometric.utils import from_networkx

from gnn_builder import (
    GCNConv_GNNB,
    GINConv_GNNB,
    PNAConv_GNNB,
    SAGEConv_GNNB,
    LGConv_GNNB,
)
from gnn_builder.utils import compute_in_deg_histogram


def rand_tenssor(shape, min=-1, max=1, dtype=torch.float32):
    return torch.rand(shape, dtype=dtype) * (max - min) + min


def linspace_tensor(n, min=-1, max=1, dtype=torch.float32):
    return torch.linspace(min, max, n, dtype=dtype)


def serialize_numpy(array: np.ndarray, fp: Path, np_type=np.float32):
    casted_array: np.ndarray = array.astype(np_type)
    casted_array.tofile(fp)


def gen_test_activations(test_data_dir: Path):
    SIZE = 64

    x_torch = linspace_tensor(SIZE, min=-10, max=10, dtype=torch.float32)
    x_np = x_torch.numpy()

    activations_torch_functions = {
        "elu": torch.nn.ELU(),
        "hardtanh": torch.nn.Hardtanh(),
        "leakyrelu": torch.nn.LeakyReLU(0.1),
        "relu": torch.nn.ReLU(),
        "gelu": torch.nn.GELU(),
        "gelu_approx_tanh": torch.nn.GELU(approximate="tanh"),
        "sigmoid": torch.nn.Sigmoid(),
        "silu": torch.nn.SiLU(),
        "tanh": torch.nn.Tanh(),
        "softsign": torch.nn.Softsign(),
        "sin": torch.sin,
        "cos": torch.cos,
        "identity": torch.nn.Identity(),
    }

    y_torch = {}
    for activation_name, activation_function in activations_torch_functions.items():
        y_torch[activation_name] = activation_function(x_torch)
    y_np = {}
    for activation_name, activation_function in activations_torch_functions.items():
        y_np[activation_name] = y_torch[activation_name].numpy()

    os.makedirs(test_data_dir, exist_ok=True)
    for activation_name, activation_function in activations_torch_functions.items():
        serialize_numpy(
            x_np, test_data_dir / f"test_activations_x_in_{activation_name}.bin"
        )
        serialize_numpy(
            y_np[activation_name],
            test_data_dir / f"test_activations_x_out_{activation_name}.bin",
        )


def gen_graph_data(test_data_dir: Path, seed=0):
    np.random.seed(seed)
    torch.manual_seed(seed)

    MAX_NODES = 1000
    MAX_EDGES = 1000

    INPUT_NODE_FEATURE_SIZE = 8

    coo_matrix = np.zeros((MAX_EDGES, 2), dtype=np.int64)

    G = nx.erdos_renyi_graph(100, 0.05, seed=seed, directed=True)
    G.remove_nodes_from(list(nx.isolates(G)))

    for node in G.nodes():
        G.nodes[node]["node_feature_array"] = (
            (np.random.rand(INPUT_NODE_FEATURE_SIZE) * 2) - 1
        ).astype(np.float32)

    num_nodes = G.number_of_nodes()
    num_edges = G.number_of_edges()
    print(f"num_nodes: {num_nodes}")
    print(f"num_edges: {num_edges}")

    assert num_nodes <= MAX_NODES
    assert num_edges <= MAX_EDGES

    coo_matrix = np.array(list(G.edges()))

    in_degree_table = np.array(list(dict(G.in_degree(G.nodes())).values()))
    out_degree_table = np.array(list(dict(G.out_degree(G.nodes())).values()))

    neighbor_table_offsets = np.cumsum(in_degree_table, dtype=int)
    neighbor_table_offsets = np.concatenate(
        (np.zeros(1, dtype=int), neighbor_table_offsets[:-1])
    )
    node_offsets_copy = neighbor_table_offsets.copy()
    neighbor_table = np.zeros(num_edges, dtype=np.int64)
    for e in range(num_edges):
        n_from = coo_matrix[e, 0]
        n_to = coo_matrix[e, 1]
        n_to_offset = node_offsets_copy[n_to]
        neighbor_table[int(n_to_offset)] = n_from
        node_offsets_copy[n_to] += 1

    for node_id in range(num_nodes):
        coo_slice = coo_matrix[coo_matrix[:, 1] == node_id][:, 0]
        set_0 = set(coo_slice.tolist())
        neighbor_table_slice = neighbor_table[
            neighbor_table_offsets[node_id] : neighbor_table_offsets[node_id]
            + in_degree_table[node_id]
        ]
        set_1 = set(neighbor_table_slice.tolist())
        in_nodes = list(G.predecessors(node_id))
        set_2 = set(in_nodes)
        check = set_0 == set_1 == set_2
        if not check:
            raise ValueError("Neighbor table computation not correct")

    pyg_graph = from_networkx(G, group_node_attrs=["node_feature_array"])
    input_node_features = pyg_graph.x.detach().numpy()
    # print("input_node_features")
    # print(input_node_features)

    OUTPUT_FEATURE_SIZE = 16

    # GCN
    gcn_layer = GCNConv_GNNB(INPUT_NODE_FEATURE_SIZE, OUTPUT_FEATURE_SIZE)

    gcn_weights = gcn_layer.conv.lin.weight.detach().numpy()
    gcn_bias = gcn_layer.conv.bias.detach().numpy()
    gcn_output = gcn_layer(pyg_graph.x, pyg_graph.edge_index).detach().numpy()

    # GIN
    GIN_HIDDEN_FEATURE_SIZE = 64
    GIN_EPS = 0.2

    gin_layer = GINConv_GNNB(
        INPUT_NODE_FEATURE_SIZE,
        OUTPUT_FEATURE_SIZE,
        hidden_dim=GIN_HIDDEN_FEATURE_SIZE,
        eps=GIN_EPS,
    )

    gin_mlp_0_weights = gin_layer.mlp.linear_0.weight.detach().numpy()
    gin_mlp_0_bias = gin_layer.mlp.linear_0.bias.detach().numpy()
    gin_mlp_1_weights = gin_layer.mlp.linear_1.weight.detach().numpy()
    gin_mlp_1_bias = gin_layer.mlp.linear_1.bias.detach().numpy()

    gin_output = gin_layer(pyg_graph.x, pyg_graph.edge_index).detach().numpy()

    # PNA
    pna_layer = PNAConv_GNNB(INPUT_NODE_FEATURE_SIZE, OUTPUT_FEATURE_SIZE, delta=0.1)

    pna_avg_degree_log = pna_layer.delta_scaler
    print(f"pna_avg_degree_log: {pna_avg_degree_log}")

    pna_transform_lin = pna_layer.conv.pre_nns[0][0]
    pna_transform_lin_weights = pna_transform_lin.weight.detach().numpy()
    pna_transform_lin_bias = pna_transform_lin.bias.detach().numpy()

    pna_apply_lin = pna_layer.conv.post_nns[0][0]
    pna_apply_lin_weights = pna_apply_lin.weight.detach().numpy()
    pna_apply_lin_bias = pna_apply_lin.bias.detach().numpy()

    pna_final_lin = pna_layer.conv.lin
    pna_final_lin_weights = pna_final_lin.weight.detach().numpy()
    pna_final_lin_bias = pna_final_lin.bias.detach().numpy()

    pna_output = pna_layer(pyg_graph.x, pyg_graph.edge_index).detach().numpy()

    # SAGE
    sage_layer = SAGEConv_GNNB(INPUT_NODE_FEATURE_SIZE, OUTPUT_FEATURE_SIZE)
    sage_neighbor_lin_weights = sage_layer.conv.lin_l.weight.detach().numpy()
    sage_neighbor_lin_bias = sage_layer.conv.lin_l.bias.detach().numpy()
    sage_self_lin_weights = sage_layer.conv.lin_r.weight.detach().numpy()
    sage_output = sage_layer(pyg_graph.x, pyg_graph.edge_index).detach().numpy()

    # LGConv
    lgconv_layer = LGConv_GNNB(INPUT_NODE_FEATURE_SIZE, OUTPUT_FEATURE_SIZE)
    lgconv_output = lgconv_layer(pyg_graph.x, pyg_graph.edge_index).detach().numpy()

    ##############

    os.makedirs(test_data_dir, exist_ok=True)
    np.savetxt(test_data_dir / "tb_max_nodes.txt", [MAX_NODES], fmt="%d")
    np.savetxt(test_data_dir / "tb_max_edges.txt", [MAX_EDGES], fmt="%d")
    np.savetxt(test_data_dir / "tb_num_nodes.txt", [num_nodes], fmt="%d")
    np.savetxt(test_data_dir / "tb_num_edges.txt", [num_edges], fmt="%d")
    np.savetxt(test_data_dir / "tb_coo_matrix.txt", coo_matrix, fmt="%d")
    serialize_numpy(
        np.array([MAX_NODES]),
        Path(test_data_dir / "tb_max_nodes.bin"),
        np_type=np.int32,
    )
    serialize_numpy(
        np.array([MAX_EDGES]),
        Path(test_data_dir / "tb_max_edges.bin"),
        np_type=np.int32,
    )
    serialize_numpy(
        np.array([num_nodes]),
        Path(test_data_dir / "tb_num_nodes.bin"),
        np_type=np.int32,
    )
    serialize_numpy(
        np.array([num_edges]),
        Path(test_data_dir / "tb_num_edges.bin"),
        np_type=np.int32,
    )
    serialize_numpy(
        coo_matrix, Path(test_data_dir / "tb_coo_matrix.bin"), np_type=np.int32
    )

    np.savetxt(test_data_dir / "tb_in_degree_table.txt", in_degree_table, fmt="%d")
    np.savetxt(test_data_dir / "tb_out_degree_table.txt", out_degree_table, fmt="%d")
    serialize_numpy(
        in_degree_table,
        Path(test_data_dir / "tb_in_degree_table.bin"),
        np_type=np.int32,
    )
    serialize_numpy(
        out_degree_table,
        Path(test_data_dir / "tb_out_degree_table.bin"),
        np_type=np.int32,
    )

    np.savetxt(
        test_data_dir / "tb_neighbor_table_offsets.txt",
        neighbor_table_offsets,
        fmt="%d",
    )
    np.savetxt(test_data_dir / "tb_neighbor_table.txt", neighbor_table, fmt="%d")
    serialize_numpy(
        neighbor_table_offsets,
        Path(test_data_dir / "tb_neighbor_table_offsets.bin"),
        np_type=np.int32,
    )
    serialize_numpy(
        neighbor_table, Path(test_data_dir / "tb_neighbor_table.bin"), np_type=np.int32
    )

    np.savetxt(
        test_data_dir / "tb_input_node_feature_size.txt",
        [INPUT_NODE_FEATURE_SIZE],
        fmt="%d",
    )
    np.savetxt(
        test_data_dir / "tb_input_node_features.txt", input_node_features, fmt="%.20f"
    )
    serialize_numpy(
        np.array([INPUT_NODE_FEATURE_SIZE]),
        Path(test_data_dir / "tb_input_node_feature_size.bin"),
        np_type=np.int32,
    )
    serialize_numpy(
        input_node_features,
        Path(test_data_dir / "tb_input_node_features.bin"),
        np_type=np.float32,
    )

    np.savetxt(
        test_data_dir / "tb_output_feature_size.txt", [OUTPUT_FEATURE_SIZE], fmt="%d"
    )
    serialize_numpy(
        np.array([OUTPUT_FEATURE_SIZE]),
        Path(test_data_dir / "tb_output_feature_size.bin"),
        np_type=np.int32,
    )

    serialize_numpy(
        gcn_weights, Path(test_data_dir / "tb_gcn_weights.bin"), np_type=np.float32
    )
    serialize_numpy(
        gcn_bias, Path(test_data_dir / "tb_gcn_bias.bin"), np_type=np.float32
    )
    serialize_numpy(
        gcn_output, Path(test_data_dir / "tb_gcn_output.bin"), np_type=np.float32
    )

    np.savetxt(
        test_data_dir / "tb_gin_hidden_feature_size.txt",
        [GIN_HIDDEN_FEATURE_SIZE],
        fmt="%d",
    )
    serialize_numpy(
        np.array([GIN_HIDDEN_FEATURE_SIZE]),
        Path(test_data_dir / "tb_gin_hidden_feature_size.bin"),
        np_type=np.int32,
    )
    serialize_numpy(
        gin_mlp_0_weights,
        Path(test_data_dir / "tb_gin_mlp_0_weights.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        gin_mlp_0_bias,
        Path(test_data_dir / "tb_gin_mlp_0_bias.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        gin_mlp_1_weights,
        Path(test_data_dir / "tb_gin_mlp_1_weights.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        gin_mlp_1_bias,
        Path(test_data_dir / "tb_gin_mlp_1_bias.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        np.array([GIN_EPS]), Path(test_data_dir / "tb_gin_eps.bin"), np_type=np.float32
    )
    serialize_numpy(
        gin_output, Path(test_data_dir / "tb_gin_output.bin"), np_type=np.float32
    )

    serialize_numpy(
        np.array(pna_avg_degree_log),
        Path(test_data_dir / "tb_pna_avg_degree_log.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        pna_transform_lin_weights,
        Path(test_data_dir / "tb_pna_transform_lin_weights.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        pna_transform_lin_bias,
        Path(test_data_dir / "tb_pna_transform_lin_bias.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        pna_apply_lin_weights,
        Path(test_data_dir / "tb_pna_apply_lin_weights.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        pna_apply_lin_bias,
        Path(test_data_dir / "tb_pna_apply_lin_bias.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        pna_final_lin_weights,
        Path(test_data_dir / "tb_pna_final_lin_weights.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        pna_final_lin_bias,
        Path(test_data_dir / "tb_pna_final_lin_bias.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        pna_output, Path(test_data_dir / "tb_pna_output.bin"), np_type=np.float32
    )

    serialize_numpy(
        sage_neighbor_lin_weights,
        test_data_dir / Path("tb_sage_neighbor_lin_weights.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        sage_neighbor_lin_bias,
        test_data_dir / Path("tb_sage_neighbor_lin_bias.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        sage_self_lin_weights,
        test_data_dir / Path("tb_sage_self_lin_weights.bin"),
        np_type=np.float32,
    )
    serialize_numpy(
        sage_output, test_data_dir / Path("tb_sage_output.bin"), np_type=np.float32
    )

    # serialize_numpy(lgconv_weights, Path(test_data_dir / "tb_lgconv_weights.bin"), np_type=np.float32)
    # serialize_numpy(lgconv_bias, Path(test_data_dir / "tb_lgconv_bias.bin"), np_type=np.float32)
    serialize_numpy(
        lgconv_output, Path(test_data_dir / "tb_lgconv_output.bin"), np_type=np.float32
    )


if __name__ == "__main__":
    test_data_dir = Path(__file__).parent / "tb_data"

    if test_data_dir.exists():
        shutil.rmtree(test_data_dir)

    gen_test_activations(test_data_dir)
    gen_graph_data(test_data_dir)
