#ifndef __DCL_H__
#define __DCL_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_EDGE 300
#define MAX_NODE 100
#define ND_FEATURE 9
#define EDGE_ATTR 3
#define EMB_DIM 100
#define NUM_TASK 1

#define MLP_0_IN 100
#define MLP_0_OUT 200
#define MLP_3_IN 200
#define MLP_3_OUT 100
#define MLP_IN_MAX 200
#define MLP_OUT_MAX 200
#define MLP_BN_DIM 200
#define CONV_BN_DIM 100
#define E_EPS 0.00001

// Constants for virtual node MLP
#define VN_MLP_0_IN 100
#define VN_MLP_0_OUT 200
#define VN_MLP_1_RELU 200
#define VN_MLP_2_IN 200
#define VN_MLP_2_OUT 100
#define VN_MLP_3_RELU 100
#define VN_MLP_IN_MAX 200
#define VN_MLP_OUT_MAX 200

extern float gnn_node_atom_encoder_atom_embedding_list_0_weight[119][100];
extern float gnn_node_atom_encoder_atom_embedding_list_1_weight[4][100];
extern float gnn_node_atom_encoder_atom_embedding_list_2_weight[12][100];
extern float gnn_node_atom_encoder_atom_embedding_list_3_weight[12][100];
extern float gnn_node_atom_encoder_atom_embedding_list_4_weight[10][100];
extern float gnn_node_atom_encoder_atom_embedding_list_5_weight[6][100];
extern float gnn_node_atom_encoder_atom_embedding_list_6_weight[6][100];
extern float gnn_node_atom_encoder_atom_embedding_list_7_weight[2][100];
extern float gnn_node_atom_encoder_atom_embedding_list_8_weight[2][100];
extern float gnn_node_virtualnode_embedding_weight[1][100];
extern float gnn_node_convs_0_eps[1];
extern float gnn_node_convs_0_mlp_0_weight[200][100];
extern float gnn_node_convs_0_mlp_0_bias[200];
extern float gnn_node_convs_0_mlp_2_weight[100][200];
extern float gnn_node_convs_0_mlp_2_bias[100];
extern float gnn_node_convs_0_bond_encoder_bond_embedding_list_0_weight[5][100];
extern float gnn_node_convs_0_bond_encoder_bond_embedding_list_1_weight[6][100];
extern float gnn_node_convs_0_bond_encoder_bond_embedding_list_2_weight[2][100];
extern float gnn_node_convs_1_eps[1];
extern float gnn_node_convs_1_mlp_0_weight[200][100];
extern float gnn_node_convs_1_mlp_0_bias[200];
extern float gnn_node_convs_1_mlp_2_weight[100][200];
extern float gnn_node_convs_1_mlp_2_bias[100];
extern float gnn_node_convs_1_bond_encoder_bond_embedding_list_0_weight[5][100];
extern float gnn_node_convs_1_bond_encoder_bond_embedding_list_1_weight[6][100];
extern float gnn_node_convs_1_bond_encoder_bond_embedding_list_2_weight[2][100];
extern float gnn_node_convs_2_eps[1];
extern float gnn_node_convs_2_mlp_0_weight[200][100];
extern float gnn_node_convs_2_mlp_0_bias[200];
extern float gnn_node_convs_2_mlp_2_weight[100][200];
extern float gnn_node_convs_2_mlp_2_bias[100];
extern float gnn_node_convs_2_bond_encoder_bond_embedding_list_0_weight[5][100];
extern float gnn_node_convs_2_bond_encoder_bond_embedding_list_1_weight[6][100];
extern float gnn_node_convs_2_bond_encoder_bond_embedding_list_2_weight[2][100];
extern float gnn_node_convs_3_eps[1];
extern float gnn_node_convs_3_mlp_0_weight[200][100];
extern float gnn_node_convs_3_mlp_0_bias[200];
extern float gnn_node_convs_3_mlp_2_weight[100][200];
extern float gnn_node_convs_3_mlp_2_bias[100];
extern float gnn_node_convs_3_bond_encoder_bond_embedding_list_0_weight[5][100];
extern float gnn_node_convs_3_bond_encoder_bond_embedding_list_1_weight[6][100];
extern float gnn_node_convs_3_bond_encoder_bond_embedding_list_2_weight[2][100];
extern float gnn_node_convs_4_eps[1];
extern float gnn_node_convs_4_mlp_0_weight[200][100];
extern float gnn_node_convs_4_mlp_0_bias[200];
extern float gnn_node_convs_4_mlp_2_weight[100][200];
extern float gnn_node_convs_4_mlp_2_bias[100];
extern float gnn_node_convs_4_bond_encoder_bond_embedding_list_0_weight[5][100];
extern float gnn_node_convs_4_bond_encoder_bond_embedding_list_1_weight[6][100];
extern float gnn_node_convs_4_bond_encoder_bond_embedding_list_2_weight[2][100];
extern float gnn_node_mlp_virtualnode_list_0_0_weight[200][100];
extern float gnn_node_mlp_virtualnode_list_0_0_bias[200];
extern float gnn_node_mlp_virtualnode_list_0_2_weight[100][200];
extern float gnn_node_mlp_virtualnode_list_0_2_bias[100];
extern float gnn_node_mlp_virtualnode_list_1_0_weight[200][100];
extern float gnn_node_mlp_virtualnode_list_1_0_bias[200];
extern float gnn_node_mlp_virtualnode_list_1_2_weight[100][200];
extern float gnn_node_mlp_virtualnode_list_1_2_bias[100];
extern float gnn_node_mlp_virtualnode_list_2_0_weight[200][100];
extern float gnn_node_mlp_virtualnode_list_2_0_bias[200];
extern float gnn_node_mlp_virtualnode_list_2_2_weight[100][200];
extern float gnn_node_mlp_virtualnode_list_2_2_bias[100];
extern float gnn_node_mlp_virtualnode_list_3_0_weight[200][100];
extern float gnn_node_mlp_virtualnode_list_3_0_bias[200];
extern float gnn_node_mlp_virtualnode_list_3_2_weight[100][200];
extern float gnn_node_mlp_virtualnode_list_3_2_bias[100];
extern float graph_pred_linear_weight[1][100];
extern float graph_pred_linear_bias[1];

void load_weights();
void fetch_one_graph(char* graph_name, int* node_feature, int* edge_list, int* edge_attr, int num_of_nodes, int num_of_edges);
void GIN_virtualnode_compute_one_graph(int* node_feature, int* edge_list, int* edge_attr, int* graph_attr);

#endif
