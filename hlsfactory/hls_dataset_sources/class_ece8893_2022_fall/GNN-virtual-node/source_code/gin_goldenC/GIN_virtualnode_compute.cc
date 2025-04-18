#include "dcl.h"
#define _PRINT_

/// h_x: node feature vectors
/// e_x: edge attribute vectors
/// message: received message of each node
/// mlp_in/out: buffers for mlp

float message[MAX_NODE][EMB_DIM];
float mlp_in[MAX_NODE][MLP_IN_MAX];
float mlp_out[MAX_NODE][MLP_OUT_MAX];

float h_0[MAX_NODE][EMB_DIM];
float e_0[MAX_EDGE][EMB_DIM];

float h_1[MAX_NODE][EMB_DIM];
float e_1[MAX_EDGE][EMB_DIM];

float h_2[MAX_NODE][EMB_DIM];
float e_2[MAX_EDGE][EMB_DIM];

float h_3[MAX_NODE][EMB_DIM];
float e_3[MAX_EDGE][EMB_DIM];

float h_4[MAX_NODE][EMB_DIM];
float e_4[MAX_EDGE][EMB_DIM];

float h_5[MAX_NODE][EMB_DIM];

// Virtual node embedding
float virtualnode_emb[EMB_DIM];

// Buffers for virtual node MLP
float vn_mlp_in[VN_MLP_IN_MAX];
float vn_mlp_out[VN_MLP_OUT_MAX];

float h_graph[EMB_DIM];
float task[NUM_TASK];

void message_passing(float ed[MAX_EDGE][EMB_DIM], float h[MAX_NODE][EMB_DIM], int* edge_list, int num_of_nodes, int num_of_edges)
{
    memset(message, 0, MAX_NODE * EMB_DIM * sizeof(float));
    for(int e = 0; e < num_of_edges; e++) {
        int u = edge_list[e*2];     // source node id
        int v = edge_list[e*2+1];   // target node id

        for(int dim = 0; dim < EMB_DIM; dim++) {
            // accumulate the embedding vector for edge [u -> v]
            float msg = ed[e][dim] + h[u][dim];
            if(msg < 0) msg = 0.0;
            message[v][dim] += msg;            
        }
    }

#ifdef _PRINT_
    printf("\nMessage of Conv\n");
    for(int nd = 0; nd < num_of_nodes; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", message[nd][dim]);
        }
        printf("...\n");
    }
#endif
}



void MLP_BatchNorm_Relu(float d_in[MAX_NODE][MLP_BN_DIM], float d_out[MAX_NODE][MLP_BN_DIM], 
                    // float (*weight), float (*bias), 
                    // float (*running_mean),
                    // float (*running_var),
                    int num_of_nodes)
{

    for(int nd = 0; nd < num_of_nodes; nd++) {
        for(int dim_out = 0; dim_out < MLP_BN_DIM; dim_out++) {
            // d_in[nd][dim_out] = (d_in[nd][dim_out] - running_mean[dim_out]) / sqrt((running_var[dim_out] + E_EPS))
            //                        * weight[dim_out] + bias[dim_out];
        
            d_out[nd][dim_out] = d_in[nd][dim_out] > 0 ? d_in[nd][dim_out] : 0.0;
        }
    }
}



void Conv_BatchNorm_Relu(float d_in[MAX_NODE][MLP_OUT_MAX], float d_out[MAX_NODE][EMB_DIM], 
                    // float (*weight), float (*bias), 
                    // float (*running_mean),
                    // float (*running_var),
                    int num_of_nodes, bool last_layer = false)
{

    for(int nd = 0; nd < num_of_nodes; nd++) {
        for(int dim_out = 0; dim_out < CONV_BN_DIM; dim_out++) {
            // d_in[nd][dim_out] = (d_in[nd][dim_out] - running_mean[dim_out]) / sqrt((running_var[dim_out] + E_EPS))
            //                        * weight[dim_out] + bias[dim_out];
        
            d_out[nd][dim_out] = (d_in[nd][dim_out] < 0 && !last_layer ) ? 0.0 : d_in[nd][dim_out];
        }
    }
}



void MLP(float mlp_in[MAX_NODE][MLP_IN_MAX], float mlp_out[MAX_NODE][MLP_OUT_MAX], float h[MAX_NODE][EMB_DIM], int num_of_nodes,
         float mlp_0_weight[MLP_0_OUT][MLP_0_IN], float (*mlp_0_bias), 
         float mlp_3_weight[MLP_3_OUT][MLP_3_IN], float (*mlp_3_bias),
         //float (*bn_weight), float (*bn_bias), float (*bn_running_mean), float (*bn_running_var), 
         float eps)
{
    /// MLP input by aggregating messages and self features
    for(int nd = 0; nd < num_of_nodes; nd++) {
        for(int dim = 0; dim < EMB_DIM; dim++) {
            mlp_in[nd][dim] = message[nd][dim] + (1 + eps) * h[nd][dim];
        }
    }

#ifdef _PRINT_
    printf("\nInput of MLP\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", mlp_in[nd][dim]);
        }
        printf("...\n");
    }
#endif


    /// MLP 0 (linear)
    memset(mlp_out, 0, MAX_NODE * EMB_DIM * sizeof(float));
    for(int nd = 0; nd < num_of_nodes; nd++) {
        for(int dim_out = 0; dim_out < MLP_0_OUT; dim_out++) {
            mlp_out[nd][dim_out] = mlp_0_bias[dim_out];
            for(int dim_in = 0; dim_in < MLP_0_IN; dim_in++) {
                mlp_out[nd][dim_out] += mlp_in[nd][dim_in] * mlp_0_weight[dim_out][dim_in];
            }
        }
    }

    /// MLP 1 (batch-norm) + Relu
    MLP_BatchNorm_Relu(mlp_out, mlp_in, 
                    //bn_weight, bn_bias,
                    //bn_running_mean, bn_running_var, 
                    num_of_nodes);


    /// MLP 3 (linear)
    for(int nd = 0; nd < num_of_nodes; nd++) {
        for(int dim_out = 0; dim_out < MLP_3_OUT; dim_out++) {
            mlp_out[nd][dim_out] = mlp_3_bias[dim_out];
            for(int dim_in = 0; dim_in < MLP_3_IN; dim_in++) {
                mlp_out[nd][dim_out] += mlp_in[nd][dim_in] * mlp_3_weight[dim_out][dim_in];
            }
        }
    }

#ifdef _PRINT_
    printf("\nOutput of MLP\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", mlp_out[nd][dim]);
        }
        printf("...\n");
    }
#endif
}

// MLP for virtual node
void MLP_Virtualnode(float* vn_mlp_in, float* vn_mlp_out, float* vn_emb, float h[MAX_NODE][EMB_DIM], int num_of_nodes,
                     float vn_mlp_0_weight[VN_MLP_0_OUT][VN_MLP_0_IN], float* vn_mlp_0_bias,
                     float vn_mlp_2_weight[VN_MLP_2_OUT][VN_MLP_2_IN], float* vn_mlp_2_bias)
{
    /// Compute vn_mlp_in by taking the global add pool of graph nodes
    /// and adding it to the virtual node embedding
    for(int dim = 0; dim < EMB_DIM; dim++)
    {
        vn_mlp_in[dim] = vn_emb[dim];
        for(int nd = 0; nd < num_of_nodes; nd++)
        {
            vn_mlp_in[dim] += h[nd][dim];
        }       
    }

#ifdef _PRINT_
    printf("\nInput of Virtualnode MLP 0:\n");
    for(int dim = 0; dim < 10; dim++) {
        printf("%.5f ", vn_mlp_in[dim]);
    }
    printf("...\n");
#endif

    /// Virtual node MLP 0 (linear)
    for(int dim_out = 0; dim_out < VN_MLP_0_OUT; dim_out++)
    {
        vn_mlp_out[dim_out] = vn_mlp_0_bias[dim_out];
        for(int dim_in = 0; dim_in < VN_MLP_0_IN; dim_in++)
        {
            vn_mlp_out[dim_out] += (vn_mlp_in[dim_in] * vn_mlp_0_weight[dim_out][dim_in]);
        }
    }

    /// Virtual node MLP 1 (RelU)
    for(int dim_out = 0; dim_out < VN_MLP_1_RELU; dim_out++)
    {
        vn_mlp_in[dim_out] = ((vn_mlp_out[dim_out] > 0) ? vn_mlp_out[dim_out] : 0.0);
    }

    /// Virtual node MLP 2 (linear)
    for(int dim_out = 0; dim_out < VN_MLP_2_OUT; dim_out++)
    {
        vn_mlp_out[dim_out] = vn_mlp_2_bias[dim_out];
        for(int dim_in = 0; dim_in < VN_MLP_2_IN; dim_in++)
        {
            vn_mlp_out[dim_out] += (vn_mlp_in[dim_in] * vn_mlp_2_weight[dim_out][dim_in]);
        }
    }

    /// Virtual node MLP 3 (RelU)
    for(int dim_out = 0; dim_out < VN_MLP_3_RELU; dim_out++)
    {
        vn_emb[dim_out] = ((vn_mlp_out[dim_out] > 0) ? vn_mlp_out[dim_out] : 0.0);
    }

#ifdef _PRINT_
    printf("\nOutput of Virtualnode MLP 3:\n");
    for(int dim = 0; dim < 10; dim++) {
        printf("%.5f ", vn_emb[dim]);
    }
    printf("...\n");
#endif
}

void CONV_0(int* node_feature, int* edge_list, int* edge_attr, int num_of_nodes, int num_of_edges)
{
    printf("\n---- Computing CONV 0 ----\n");
    
    ////////////// Embedding: Add virtual node embedding to graph nodes
    for(int nd = 0; nd < num_of_nodes; nd++)
    {
        for(int dim = 0; dim < EMB_DIM; dim++)
        {
            h_0[nd][dim] += virtualnode_emb[dim];   
        }
    }

#ifdef _PRINT_
    printf("\nNode embeddings after addition of virtual node:\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", h_0[nd][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Embedding: compute edge embedding
    memset(e_0, 0, MAX_EDGE * EMB_DIM * sizeof(float));
    for(int e = 0; e < num_of_edges; e++) {
        for(int ef = 0; ef < EDGE_ATTR; ef++) {
            int e_f = edge_attr[e * EDGE_ATTR + ef];
            for(int dim = 0; dim < EMB_DIM; dim++) {
                float emb_value = 0;
                switch (ef) {
                case 0:
                    emb_value = gnn_node_convs_0_bond_encoder_bond_embedding_list_0_weight[e_f][dim];
                    break;
                case 1:
                    emb_value = gnn_node_convs_0_bond_encoder_bond_embedding_list_1_weight[e_f][dim];
                    break;
                case 2:
                    emb_value = gnn_node_convs_0_bond_encoder_bond_embedding_list_2_weight[e_f][dim];
                    break;
                }
                e_0[e][dim] += emb_value;
            }   
        }
    }

#ifdef _PRINT_
    printf("\nInitial edge embedding:\n");
    for(int e = 0; e < 5; e++) {
        printf("Edge %d: ", e);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", e_0[e][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Message Passing
    message_passing(e_0, h_0, edge_list, num_of_nodes, num_of_edges);

    ////////////// MLP of Conv 0
    float eps = gnn_node_convs_0_eps[0];
    MLP(mlp_in, mlp_out, h_0, num_of_nodes,
        gnn_node_convs_0_mlp_0_weight, gnn_node_convs_0_mlp_0_bias, gnn_node_convs_0_mlp_2_weight, gnn_node_convs_0_mlp_2_bias,
        //gnn_node_convs_0_mlp_1_weight, gnn_node_convs_0_mlp_1_bias, gnn_node_convs_0_mlp_1_running_mean, gnn_node_convs_0_mlp_1_running_var, 
        eps);


    ////////////// Batchnorm + Relu of Conv 0
    Conv_BatchNorm_Relu(mlp_out, h_1, 
                    //gnn_node_batch_norms_0_weight, gnn_node_batch_norms_0_bias,
                    //gnn_node_batch_norms_0_running_mean, gnn_node_batch_norms_0_running_var, 
                    num_of_nodes);

#ifdef _PRINT_
    printf("\nOutput of BatchNorm and Relu of Conv 0\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", h_1[nd][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Update the virtual node
    ////////////// Virtual node MLP of layer 0
    MLP_Virtualnode(vn_mlp_in, vn_mlp_out, virtualnode_emb, h_0, num_of_nodes, gnn_node_mlp_virtualnode_list_0_0_weight, gnn_node_mlp_virtualnode_list_0_0_bias,
                    gnn_node_mlp_virtualnode_list_0_2_weight, gnn_node_mlp_virtualnode_list_0_2_bias);
    
#ifdef _PRINT_
    printf("\nUpdated Virtualnode after layer 0:\n");
    for(int dim = 0; dim < 10; dim++) {
        printf("%.5f ", virtualnode_emb[dim]);
    }
    printf("...\n");
#endif
}




void CONV_1(int* node_feature, int* edge_list, int* edge_attr, int num_of_nodes, int num_of_edges)
{
    printf("\n---- Computing CONV 1 ----\n");

    ////////////// Embedding: Add virtual node embedding to graph nodes
    for(int nd = 0; nd < num_of_nodes; nd++)
    {
        for(int dim = 0; dim < EMB_DIM; dim++)
        {
            h_1[nd][dim] += virtualnode_emb[dim];   
        }
    }

#ifdef _PRINT_
    printf("\nNode embeddings after addition of virtual node:\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", h_1[nd][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Embedding: compute edge embedding
    memset(e_1, 0, MAX_EDGE * EMB_DIM * sizeof(float));
    for(int e = 0; e < num_of_edges; e++) {
        for(int ef = 0; ef < EDGE_ATTR; ef++) {
            int e_f = edge_attr[e * EDGE_ATTR + ef];
            for(int dim = 0; dim < EMB_DIM; dim++) {
                float emb_value = 0;
                switch (ef) {
                case 0:
                    emb_value = gnn_node_convs_1_bond_encoder_bond_embedding_list_0_weight[e_f][dim];
                    break;
                case 1:
                    emb_value = gnn_node_convs_1_bond_encoder_bond_embedding_list_1_weight[e_f][dim];
                    break;
                case 2:
                    emb_value = gnn_node_convs_1_bond_encoder_bond_embedding_list_2_weight[e_f][dim];
                    break;
                }
                e_1[e][dim] += emb_value;
            }   
        }
    }

#ifdef _PRINT_
    printf("\nInitial edge embedding:\n");
    for(int e = 0; e < 5; e++) {
        printf("Edge %d: ", e);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", e_1[e][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Message Passing
    message_passing(e_1, h_1, edge_list, num_of_nodes, num_of_edges);

    ////////////// MLP of Conv 1
    float eps = gnn_node_convs_1_eps[0];
    MLP(mlp_in, mlp_out, h_1, num_of_nodes,
        gnn_node_convs_1_mlp_0_weight, gnn_node_convs_1_mlp_0_bias, gnn_node_convs_1_mlp_2_weight, gnn_node_convs_1_mlp_2_bias,
        //gnn_node_convs_1_mlp_1_weight, gnn_node_convs_1_mlp_1_bias, gnn_node_convs_1_mlp_1_running_mean, gnn_node_convs_1_mlp_1_running_var, 
        eps);


    ////////////// Batchnorm + Relu of Conv 1
    Conv_BatchNorm_Relu(mlp_out, h_2, 
                    //gnn_node_batch_norms_1_weight, gnn_node_batch_norms_1_bias,
                    //gnn_node_batch_norms_1_running_mean, gnn_node_batch_norms_1_running_var, 
                    num_of_nodes);

#ifdef _PRINT_
    printf("\nOutput of BatchNorm and Relu of Conv 1\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", h_2[nd][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Update the virtual node
    ////////////// Virtual node MLP of layer 1
    MLP_Virtualnode(vn_mlp_in, vn_mlp_out, virtualnode_emb, h_1, num_of_nodes, gnn_node_mlp_virtualnode_list_1_0_weight, gnn_node_mlp_virtualnode_list_1_0_bias,
                    gnn_node_mlp_virtualnode_list_1_2_weight, gnn_node_mlp_virtualnode_list_1_2_bias);
    
#ifdef _PRINT_
    printf("\nUpdated Virtualnode after layer 1:\n");
    for(int dim = 0; dim < 10; dim++) {
        printf("%.5f ", virtualnode_emb[dim]);
    }
    printf("...\n");
#endif
}




void CONV_2(int* node_feature, int* edge_list, int* edge_attr, int num_of_nodes, int num_of_edges)
{
    printf("\n---- Computing CONV 2 ----\n");

    ////////////// Embedding: Add virtual node embedding to graph nodes
    for(int nd = 0; nd < num_of_nodes; nd++)
    {
        for(int dim = 0; dim < EMB_DIM; dim++)
        {
            h_2[nd][dim] += virtualnode_emb[dim];   
        }
    }

#ifdef _PRINT_
    printf("\nNode embeddings after addition of virtual node:\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", h_2[nd][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Embedding: compute edge embedding
    memset(e_2, 0, MAX_EDGE * EMB_DIM * sizeof(float));
    for(int e = 0; e < num_of_edges; e++) {
        for(int ef = 0; ef < EDGE_ATTR; ef++) {
            int e_f = edge_attr[e * EDGE_ATTR + ef];
            for(int dim = 0; dim < EMB_DIM; dim++) {
                float emb_value = 0;
                switch (ef) {
                case 0:
                    emb_value = gnn_node_convs_2_bond_encoder_bond_embedding_list_0_weight[e_f][dim];
                    break;
                case 1:
                    emb_value = gnn_node_convs_2_bond_encoder_bond_embedding_list_1_weight[e_f][dim];
                    break;
                case 2:
                    emb_value = gnn_node_convs_2_bond_encoder_bond_embedding_list_2_weight[e_f][dim];
                    break;
                }
                e_2[e][dim] += emb_value;
            }   
        }
    }

#ifdef _PRINT_
    printf("\nInitial edge embedding:\n");
    for(int e = 0; e < 5; e++) {
        printf("Edge %d: ", e);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", e_2[e][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Message Passing
    message_passing(e_2, h_2, edge_list, num_of_nodes, num_of_edges);

    ////////////// MLP of Conv 2
    float eps = gnn_node_convs_2_eps[0];
    MLP(mlp_in, mlp_out, h_2, num_of_nodes,
        gnn_node_convs_2_mlp_0_weight, gnn_node_convs_2_mlp_0_bias, gnn_node_convs_2_mlp_2_weight, gnn_node_convs_2_mlp_2_bias,
        //gnn_node_convs_2_mlp_1_weight, gnn_node_convs_2_mlp_1_bias, gnn_node_convs_2_mlp_1_running_mean, gnn_node_convs_2_mlp_1_running_var, 
        eps);


    ////////////// Batchnorm + Relu of Conv 2
    Conv_BatchNorm_Relu(mlp_out, h_3, 
                    //gnn_node_batch_norms_2_weight, gnn_node_batch_norms_2_bias,
                    //gnn_node_batch_norms_2_running_mean, gnn_node_batch_norms_2_running_var, 
                    num_of_nodes);

#ifdef _PRINT_
    printf("\nOutput of BatchNorm and Relu of Conv 2\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", h_3[nd][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Update the virtual node
    ////////////// Virtual node MLP of layer 2
    MLP_Virtualnode(vn_mlp_in, vn_mlp_out, virtualnode_emb, h_2, num_of_nodes, gnn_node_mlp_virtualnode_list_2_0_weight, gnn_node_mlp_virtualnode_list_2_0_bias,
                    gnn_node_mlp_virtualnode_list_2_2_weight, gnn_node_mlp_virtualnode_list_2_2_bias);
    
#ifdef _PRINT_
    printf("\nUpdated Virtualnode after layer 2:\n");
    for(int dim = 0; dim < 10; dim++) {
        printf("%.5f ", virtualnode_emb[dim]);
    }
    printf("...\n");
#endif
}





void CONV_3(int* node_feature, int* edge_list, int* edge_attr, int num_of_nodes, int num_of_edges)
{
    printf("\n---- Computing CONV 3 ----\n");

    ////////////// Embedding: Add virtual node embedding to graph nodes
    for(int nd = 0; nd < num_of_nodes; nd++)
    {
        for(int dim = 0; dim < EMB_DIM; dim++)
        {
            h_3[nd][dim] += virtualnode_emb[dim];   
        }
    }

#ifdef _PRINT_
    printf("\nNode embeddings after addition of virtual node:\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", h_3[nd][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Embedding: compute edge embedding
    memset(e_3, 0, MAX_EDGE * EMB_DIM * sizeof(float));
    for(int e = 0; e < num_of_edges; e++) {
        for(int ef = 0; ef < EDGE_ATTR; ef++) {
            int e_f = edge_attr[e * EDGE_ATTR + ef];
            for(int dim = 0; dim < EMB_DIM; dim++) {
                float emb_value = 0;
                switch (ef) {
                case 0:
                    emb_value = gnn_node_convs_3_bond_encoder_bond_embedding_list_0_weight[e_f][dim];
                    break;
                case 1:
                    emb_value = gnn_node_convs_3_bond_encoder_bond_embedding_list_1_weight[e_f][dim];
                    break;
                case 2:
                    emb_value = gnn_node_convs_3_bond_encoder_bond_embedding_list_2_weight[e_f][dim];
                    break;
                }
                e_3[e][dim] += emb_value;
            }   
        }
    }

#ifdef _PRINT_
    printf("\nInitial edge embedding:\n");
    for(int e = 0; e < 5; e++) {
        printf("Edge %d: ", e);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", e_3[e][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Message Passing
    message_passing(e_3, h_3, edge_list, num_of_nodes, num_of_edges);

    ////////////// MLP of Conv 3
    float eps = gnn_node_convs_3_eps[0];
    MLP(mlp_in, mlp_out, h_3, num_of_nodes,
        gnn_node_convs_3_mlp_0_weight, gnn_node_convs_3_mlp_0_bias, gnn_node_convs_3_mlp_2_weight, gnn_node_convs_3_mlp_2_bias,
        //gnn_node_convs_3_mlp_1_weight, gnn_node_convs_3_mlp_1_bias, gnn_node_convs_3_mlp_1_running_mean, gnn_node_convs_3_mlp_1_running_var, 
        eps);


    ////////////// Batchnorm + Relu of Conv 3
    Conv_BatchNorm_Relu(mlp_out, h_4, 
                    //gnn_node_batch_norms_3_weight, gnn_node_batch_norms_3_bias,
                    //gnn_node_batch_norms_3_running_mean, gnn_node_batch_norms_3_running_var, 
                    num_of_nodes);

#ifdef _PRINT_
    printf("\nOutput of BatchNorm and Relu of Conv 3\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", h_4[nd][dim]);
        }
        printf("...\n");
    }
#endif


    ////////////// Update the virtual node
    ////////////// Virtual node MLP of layer 3
    MLP_Virtualnode(vn_mlp_in, vn_mlp_out, virtualnode_emb, h_3, num_of_nodes, gnn_node_mlp_virtualnode_list_3_0_weight, gnn_node_mlp_virtualnode_list_3_0_bias,
                    gnn_node_mlp_virtualnode_list_3_2_weight, gnn_node_mlp_virtualnode_list_3_2_bias);
    
#ifdef _PRINT_
    printf("\nUpdated Virtualnode after layer 3:\n");
    for(int dim = 0; dim < 10; dim++) {
        printf("%.5f ", virtualnode_emb[dim]);
    }
    printf("...\n");
#endif
}




void CONV_4(int* node_feature, int* edge_list, int* edge_attr, int num_of_nodes, int num_of_edges)
{
    printf("\n---- Computing CONV 4 ----\n");

    ////////////// Embedding: Add virtual node embedding to graph nodes
    for(int nd = 0; nd < num_of_nodes; nd++)
    {
        for(int dim = 0; dim < EMB_DIM; dim++)
        {
            h_4[nd][dim] += virtualnode_emb[dim];   
        }
    }

#ifdef _PRINT_
    printf("\nNode embeddings after addition of virtual node:\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", h_4[nd][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Embedding: compute edge embedding
    memset(e_4, 0, MAX_EDGE * EMB_DIM * sizeof(float));
    for(int e = 0; e < num_of_edges; e++) {
        for(int ef = 0; ef < EDGE_ATTR; ef++) {
            int e_f = edge_attr[e * EDGE_ATTR + ef];
            for(int dim = 0; dim < EMB_DIM; dim++) {
                float emb_value = 0;
                switch (ef) {
                case 0:
                    emb_value = gnn_node_convs_4_bond_encoder_bond_embedding_list_0_weight[e_f][dim];
                    break;
                case 1:
                    emb_value = gnn_node_convs_4_bond_encoder_bond_embedding_list_1_weight[e_f][dim];
                    break;
                case 2:
                    emb_value = gnn_node_convs_4_bond_encoder_bond_embedding_list_2_weight[e_f][dim];
                    break;
                }
                e_4[e][dim] += emb_value;
            }   
        }
    }

#ifdef _PRINT_
    printf("\nInitial edge embedding:\n");
    for(int e = 0; e < 5; e++) {
        printf("Edge %d: ", e);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", e_4[e][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// Message Passing
    message_passing(e_4, h_4, edge_list, num_of_nodes, num_of_edges);

    ////////////// MLP of Conv 4
    float eps = gnn_node_convs_4_eps[0];
    MLP(mlp_in, mlp_out, h_4, num_of_nodes,
        gnn_node_convs_4_mlp_0_weight, gnn_node_convs_4_mlp_0_bias, gnn_node_convs_4_mlp_2_weight, gnn_node_convs_4_mlp_2_bias,
        //gnn_node_convs_4_mlp_1_weight, gnn_node_convs_4_mlp_1_bias, gnn_node_convs_4_mlp_1_running_mean, gnn_node_convs_4_mlp_1_running_var, 
        eps);


    ////////////// Batchnorm + Relu of Conv 4
    Conv_BatchNorm_Relu(mlp_out, h_5, 
                    //gnn_node_batch_norms_4_weight, gnn_node_batch_norms_4_bias,
                    //gnn_node_batch_norms_4_running_mean, gnn_node_batch_norms_4_running_var, 
                    num_of_nodes, true);

#ifdef _PRINT_
    printf("\nOutput of BatchNorm and Relu of Conv 4\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", h_5[nd][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// No need to update virtual node in the last layer
}



void GIN_virtualnode_compute_one_graph(int* node_feature, int* edge_list, int* edge_attr, int* graph_attr)
{
    int num_of_nodes = graph_attr[0];
    int num_of_edges = graph_attr[1];


    printf("Computing GIN ...\n");

    ////////////// Embedding: compute initial virtual node embedding
    for(int dim = 0; dim < EMB_DIM; dim++) {
        virtualnode_emb[dim] = gnn_node_virtualnode_embedding_weight[0][dim];
    }

#ifdef _PRINT_
    printf("\nInitial virtual node embedding:\n");
    for(int dim = 0; dim < 10; dim++) {
        printf("%.5f ", virtualnode_emb[dim]);
    }
    printf("...\n");
#endif

    ////////////// Embedding: compute input node embedding
    memset(h_0, 0, MAX_NODE * EMB_DIM * sizeof(float));
    for(int nd = 0; nd < num_of_nodes; nd++) {
        for(int nf = 0; nf < ND_FEATURE; nf++) {
            int nd_f = node_feature[nd * ND_FEATURE + nf];
            for(int dim = 0; dim < EMB_DIM; dim++) {
                float emb_value = 0;
                switch (nf) {
                case 0:
                    emb_value = gnn_node_atom_encoder_atom_embedding_list_0_weight[nd_f][dim];
                    break;
                case 1:
                    emb_value = gnn_node_atom_encoder_atom_embedding_list_1_weight[nd_f][dim];
                    break;
                case 2:
                    emb_value = gnn_node_atom_encoder_atom_embedding_list_2_weight[nd_f][dim];
                    break;
                case 3:
                    emb_value = gnn_node_atom_encoder_atom_embedding_list_3_weight[nd_f][dim];
                    break;
                case 4:
                    emb_value = gnn_node_atom_encoder_atom_embedding_list_4_weight[nd_f][dim];
                    break;
                case 5:
                    emb_value = gnn_node_atom_encoder_atom_embedding_list_5_weight[nd_f][dim];
                    break;
                case 6:
                    emb_value = gnn_node_atom_encoder_atom_embedding_list_6_weight[nd_f][dim];
                    break;
                case 7:
                    emb_value = gnn_node_atom_encoder_atom_embedding_list_7_weight[nd_f][dim];
                    break;
                case 8:
                    emb_value = gnn_node_atom_encoder_atom_embedding_list_8_weight[nd_f][dim];
                    break;
                }
                h_0[nd][dim] += emb_value;
            }   
        }
    }

#ifdef _PRINT_
    printf("\nInitial node embedding:\n");
    for(int nd = 0; nd < 5; nd++) {
        printf("Node %d: ", nd);
        for(int dim = 0; dim < 10; dim++) {
            printf("%.5f ", h_0[nd][dim]);
        }
        printf("...\n");
    }
#endif

    ////////////// CONV 0 //////////////////////////////////
    CONV_0(node_feature, edge_list, edge_attr, num_of_nodes, num_of_edges);
    ////////////// CONV 1 //////////////////////////////////
    CONV_1(node_feature, edge_list, edge_attr, num_of_nodes, num_of_edges);
    ////////////// CONV 2 //////////////////////////////////
    CONV_2(node_feature, edge_list, edge_attr, num_of_nodes, num_of_edges);
    ////////////// CONV 3 //////////////////////////////////
    CONV_3(node_feature, edge_list, edge_attr, num_of_nodes, num_of_edges);
    ////////////// CONV 4 //////////////////////////////////
    CONV_4(node_feature, edge_list, edge_attr, num_of_nodes, num_of_edges);

    
    ////////////// Global mean pooling //////////////////////
    // node representation is h_5
    memset(h_graph, 0, EMB_DIM * sizeof(float));
    for(int dim = 0; dim < EMB_DIM; dim++) {
        for(int nd = 0; nd < num_of_nodes; nd++) {
            h_graph[dim] += h_5[nd][dim];
        }
        h_graph[dim] = h_graph[dim] / num_of_nodes;
    }

#ifdef _PRINT_
    printf("\nGlobal h_graph (global mean pool):\n");
    for(int dim = 0; dim < 10; dim++) {
        printf("%.5f ", h_graph[dim]);
    }
    printf("...\n");
#endif
    
    ////////////// Graph prediction linear ///////////////////
    memset(task, 0, NUM_TASK * sizeof(float));
    for(int tsk = 0; tsk < NUM_TASK; tsk++) {
        task[tsk] = graph_pred_linear_bias[tsk];
        for(int dim = 0; dim < EMB_DIM; dim++) {
            task[tsk] += h_graph[dim] * graph_pred_linear_weight[tsk][dim];
        }
    }

//#ifdef _PRINT_
    printf("Final graph prediction:\n");
    for(int tsk = 0; tsk < NUM_TASK; tsk++) {
        printf("%.7f\n", task[tsk]);
    }
    printf("\nGIN computation done.\n");
//#endif

}
