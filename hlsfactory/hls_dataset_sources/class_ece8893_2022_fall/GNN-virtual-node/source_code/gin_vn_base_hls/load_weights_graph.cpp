#include <stdlib.h>
#include <stdio.h>
#include "dcl.hpp"


extern WT_TYPE gnn_node_mlp_1_weights_fixed[LAYER_NUM * MLP_1_OUT * MLP_1_IN];
extern WT_TYPE gnn_node_mlp_1_bias_fixed[LAYER_NUM * MLP_1_OUT];
extern WT_TYPE gnn_node_mlp_2_weights_fixed[LAYER_NUM * MLP_2_OUT * MLP_2_IN];
extern WT_TYPE gnn_node_mlp_2_bias_fixed[LAYER_NUM * MLP_2_OUT];
extern WT_TYPE gnn_node_embedding_table_fixed[ND_FEATURE_TOTAL * EMB_DIM];
extern WT_TYPE gnn_edge_embedding_table_fixed[EG_FEATURE_TOTAL * EMB_DIM];
extern WT_TYPE graph_pred_linear_weight_fixed[NUM_TASK * MLP_2_OUT];
extern WT_TYPE graph_pred_linear_bias_fixed[NUM_TASK];
extern WT_TYPE eps_fixed[LAYER_NUM];
extern FM_TYPE gnn_node_virtualnode_embedding_weight_fixed[1 * EMB_DIM];
extern WT_TYPE gnn_node_virtualnode_mlp_1_weights_fixed[VN_LAYER_NUM * VN_MLP_1_OUT * VN_MLP_1_IN];
extern WT_TYPE gnn_node_virtualnode_mlp_1_bias_fixed[VN_LAYER_NUM * VN_MLP_1_OUT];
extern WT_TYPE gnn_node_virtualnode_mlp_2_weights_fixed[VN_LAYER_NUM * VN_MLP_2_OUT * VN_MLP_2_IN];
extern WT_TYPE gnn_node_virtualnode_mlp_2_bias_fixed[VN_LAYER_NUM * VN_MLP_2_OUT];

void load_weights()
{
	printf("Loading weights for GIN Virtual Node...\n");

    FILE* f;

    f = fopen("gin-virtual_ep1_mlp_1_weights_dim100.bin", "r");
	fread(gnn_node_mlp_1_weights, sizeof(float), LAYER_NUM * MLP_1_OUT * MLP_1_IN, f);
	fclose(f);

    f = fopen("gin-virtual_ep1_mlp_1_bias_dim100.bin", "r");
	fread(gnn_node_mlp_1_bias, sizeof(float), LAYER_NUM * MLP_1_OUT, f);
	fclose(f);

	f = fopen("gin-virtual_ep1_mlp_2_weights_dim100.bin", "r");
	fread(gnn_node_mlp_2_weights, sizeof(float), LAYER_NUM * MLP_2_OUT * MLP_2_IN, f);
	fclose(f);

	f = fopen("gin-virtual_ep1_mlp_2_bias_dim100.bin", "r");
	fread(gnn_node_mlp_2_bias, sizeof(float), LAYER_NUM * MLP_2_OUT, f);
	fclose(f);

	f = fopen("gin-virtual_ep1_eps_dim100.bin", "r");
	fread(eps, sizeof(float), LAYER_NUM, f);
	fclose(f);

    f = fopen("gin-virtual_ep1_nd_embed_dim100.bin", "r");
	fread(gnn_node_embedding_table, sizeof(float), ND_FEATURE_TOTAL * EMB_DIM, f);
	fclose(f);

	f = fopen("gin-virtual_ep1_ed_embed_dim100.bin", "r");
	fread(gnn_edge_embedding_table, sizeof(float), EG_FEATURE_TOTAL * EMB_DIM, f);
	fclose(f);

	f = fopen("gin-virtual_ep1_pred_weights_dim100.bin", "r");
	fread(graph_pred_linear_weight, sizeof(float), NUM_TASK * MLP_2_OUT, f);
	fclose(f);

	f = fopen("gin-virtual_ep1_pred_bias_dim100.bin", "r");
	fread(graph_pred_linear_bias, sizeof(float), NUM_TASK, f);
	fclose(f);

	f = fopen("gin-virtual_ep1_virtualnode_mlp_0_weights_dim100.bin", "r");
	fread(gnn_node_virtualnode_mlp_1_weights, sizeof(float), VN_LAYER_NUM * VN_MLP_1_OUT * VN_MLP_1_IN, f);
	fclose(f);

	f = fopen("gin-virtual_ep1_virtualnode_mlp_0_bias_dim100.bin", "r");
	fread(gnn_node_virtualnode_mlp_1_bias, sizeof(float), VN_LAYER_NUM * VN_MLP_1_OUT, f);
	fclose(f);

	f = fopen("gin-virtual_ep1_virtualnode_mlp_2_weights_dim100.bin", "r");
	fread(gnn_node_virtualnode_mlp_2_weights, sizeof(float), VN_LAYER_NUM * VN_MLP_2_OUT * VN_MLP_2_IN, f);
	fclose(f);

	f = fopen("gin-virtual_ep1_virtualnode_mlp_2_bias_dim100.bin", "r");
	fread(gnn_node_virtualnode_mlp_2_bias, sizeof(float), VN_LAYER_NUM * VN_MLP_2_OUT, f);
	fclose(f);

	f = fopen("gin-virtual_ep1_virtualnode_embed_dim100.bin", "r");
	fread(gnn_node_virtualnode_embedding_weight, sizeof(float), 1 * EMB_DIM, f);
	fclose(f);

	/// convert to fixed point
	for(int l = 0; l < LAYER_NUM; l++) {
		eps_fixed[l] = (WT_TYPE)eps[l];
		for(int dim_out = 0; dim_out < MLP_1_OUT; dim_out++) {
			gnn_node_mlp_1_bias_fixed[l * MLP_1_OUT + dim_out] = (WT_TYPE)gnn_node_mlp_1_bias[l][dim_out];
			for(int dim_in = 0; dim_in < MLP_1_IN; dim_in++) {
				gnn_node_mlp_1_weights_fixed[l * MLP_1_OUT * MLP_1_IN + dim_out * MLP_1_IN + dim_in] = (WT_TYPE)gnn_node_mlp_1_weights[l][dim_out][dim_in];
			}
		}
		for(int dim_out = 0; dim_out < MLP_2_OUT; dim_out++) {
			gnn_node_mlp_2_bias_fixed[l * MLP_2_OUT + dim_out] = (WT_TYPE)gnn_node_mlp_2_bias[l][dim_out];
			for(int dim_in = 0; dim_in < MLP_2_IN; dim_in++) {
				gnn_node_mlp_2_weights_fixed[l * MLP_2_OUT * MLP_2_IN + dim_out * MLP_2_IN + dim_in] = (WT_TYPE)gnn_node_mlp_2_weights[l][dim_out][dim_in];
			}
		}
	}

    // Convert Virtual node weights to fixed point
	for(int l = 0; l < VN_LAYER_NUM; l++) {
		for(int dim_out = 0; dim_out < VN_MLP_1_OUT; dim_out++) {
			gnn_node_virtualnode_mlp_1_bias_fixed[l * VN_MLP_1_OUT + dim_out] = (WT_TYPE) gnn_node_virtualnode_mlp_1_bias[l][dim_out];
			for(int dim_in = 0; dim_in < VN_MLP_1_IN; dim_in++) {
				gnn_node_virtualnode_mlp_1_weights_fixed[l * VN_MLP_1_OUT * VN_MLP_1_IN + dim_out * VN_MLP_1_IN + dim_in] = (WT_TYPE) gnn_node_virtualnode_mlp_1_weights[l][dim_out][dim_in];
			}
		}
		for(int dim_out = 0; dim_out < VN_MLP_2_OUT; dim_out++) {
			gnn_node_virtualnode_mlp_2_bias_fixed[l * VN_MLP_2_OUT + dim_out] = (WT_TYPE) gnn_node_virtualnode_mlp_2_bias[l][dim_out];
			for(int dim_in = 0; dim_in < VN_MLP_2_IN; dim_in++) {
				gnn_node_virtualnode_mlp_2_weights_fixed[l * VN_MLP_2_OUT * VN_MLP_2_IN + dim_out * VN_MLP_2_IN + dim_in] = (WT_TYPE) gnn_node_virtualnode_mlp_2_weights[l][dim_out][dim_in];
			}
		}
	}

    // Virtual node embedding weight    
    for(int dim = 0; dim < EMB_DIM; dim++) {
    	gnn_node_virtualnode_embedding_weight_fixed[dim] = (FM_TYPE) gnn_node_virtualnode_embedding_weight[0][dim];
    }

	
	for(int i = 0; i < ND_FEATURE_TOTAL; i++) {
		for(int dim = 0; dim < EMB_DIM; dim++) {
			gnn_node_embedding_table_fixed[i * EMB_DIM + dim] = (WT_TYPE)gnn_node_embedding_table[i][dim];
		}
	}

	for(int i = 0; i < EG_FEATURE_TOTAL; i++) {
		for(int dim = 0; dim < EMB_DIM; dim++) {
			gnn_edge_embedding_table_fixed[i * EMB_DIM + dim] = (WT_TYPE)gnn_edge_embedding_table[i][dim];
		}
	}

	for(int t = 0; t < NUM_TASK; t++) {
		graph_pred_linear_bias_fixed[t] = (WT_TYPE)graph_pred_linear_bias[t];
		eps_fixed[t] = (WT_TYPE)eps[t];
		for(int dim_in = 0; dim_in < MLP_2_OUT; dim_in++ ) {
			graph_pred_linear_weight_fixed[t * MLP_2_OUT + dim_in] = (WT_TYPE)graph_pred_linear_weight[t][dim_in];
		}
	}
}



void fetch_one_graph(char* graph_name, int* node_feature, int* edge_list, int* edge_attr, int num_of_nodes, int num_of_edges)
{
    printf("Loading graph ...\n");
        
    FILE* f;

	char f_node_feature[128];
	char f_edge_list[128];
	char f_edge_attr[128];

	sprintf(f_node_feature, "%s_node_feature.bin", graph_name);
	sprintf(f_edge_list, "%s_edge_list.bin", graph_name);
	sprintf(f_edge_attr, "%s_edge_attr.bin", graph_name);
	
	int node_feature_in[ND_FEATURE * MAX_NODE];
	int edge_list_in[2 * MAX_EDGE];
    int edge_attr_in[EDGE_ATTR * MAX_EDGE];
        

    f = fopen(f_node_feature, "r");
	fread(node_feature_in, sizeof(int), num_of_nodes * ND_FEATURE, f);
    fclose(f);

    f = fopen(f_edge_list, "r");
    fread(edge_list_in, sizeof(int), 2 * num_of_edges, f);
    fclose(f);

    f = fopen(f_edge_attr, "r");
    fread(edge_attr_in, sizeof(int), EDGE_ATTR * num_of_edges, f);
    fclose(f);

	for(int i = 0; i < num_of_nodes * ND_FEATURE; i++) {
		node_feature[i] = node_feature_in[i];
	}

	for(int i = 0; i < 2 * num_of_edges; i++) {
		edge_list[i] = edge_list_in[i];
	}

	for(int i = 0; i < num_of_edges * EDGE_ATTR; i++) {
		edge_attr[i] = edge_attr_in[i];
	}


#ifdef _PRINT_
		printf("Node features:\n");
		for(int i = 0; i < num_of_nodes; i++) {
			for(int j = 0; j < ND_FEATURE; j++) {
				printf("%d ", node_feature[i * ND_FEATURE + j]);
			}
			printf("\n");
		}

		printf("Edges:\n");
		for(int i = 0; i < num_of_edges; i++) {
			printf("%d -> %d\n", edge_list[i*2], edge_list[i*2+1]);
		}

		printf("Edge attributes:\n");
		for(int i = 0; i < num_of_edges; i++) {
			for(int j = 0; j < EDGE_ATTR; j++) {
				printf("%d ", edge_attr[i * EDGE_ATTR + j]);
			}
			printf("\n");
		}
	}
#endif
}
