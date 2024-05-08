#include "model.h"

//////////////////////
// Model Parameters //
//////////////////////

float mlp_head_linear_layers_0_weight_in[64][192];
float mlp_head_linear_layers_0_bias_in[64];
float mlp_head_linear_layers_1_weight_in[64][64];
float mlp_head_linear_layers_1_bias_in[64];
float mlp_head_linear_layers_2_weight_in[64][64];
float mlp_head_linear_layers_2_bias_in[64];
float mlp_head_linear_layers_3_weight_in[64][64];
float mlp_head_linear_layers_3_bias_in[64];
float mlp_head_linear_layers_4_weight_in[19][64];
float mlp_head_linear_layers_4_bias_in[19];
float gnn_convs_0_conv_bias_in[128];
float gnn_convs_0_conv_lin_weight_in[128][11];
float gnn_convs_1_conv_bias_in[128];
float gnn_convs_1_conv_lin_weight_in[128][128];
float gnn_convs_2_conv_bias_in[128];
float gnn_convs_2_conv_lin_weight_in[128][128];
float gnn_convs_3_conv_bias_in[128];
float gnn_convs_3_conv_lin_weight_in[128][128];
float gnn_convs_4_conv_bias_in[128];
float gnn_convs_4_conv_lin_weight_in[128][128];
float gnn_convs_5_conv_bias_in[64];
float gnn_convs_5_conv_lin_weight_in[64][128];

W_TYPE mlp_head_linear_layers_0_weight_fixed_in[64][192];
W_TYPE mlp_head_linear_layers_0_bias_fixed_in[64];
W_TYPE mlp_head_linear_layers_1_weight_fixed_in[64][64];
W_TYPE mlp_head_linear_layers_1_bias_fixed_in[64];
W_TYPE mlp_head_linear_layers_2_weight_fixed_in[64][64];
W_TYPE mlp_head_linear_layers_2_bias_fixed_in[64];
W_TYPE mlp_head_linear_layers_3_weight_fixed_in[64][64];
W_TYPE mlp_head_linear_layers_3_bias_fixed_in[64];
W_TYPE mlp_head_linear_layers_4_weight_fixed_in[19][64];
W_TYPE mlp_head_linear_layers_4_bias_fixed_in[19];
W_TYPE gnn_convs_0_conv_bias_fixed_in[128];
W_TYPE gnn_convs_0_conv_lin_weight_fixed_in[128][11];
W_TYPE gnn_convs_1_conv_bias_fixed_in[128];
W_TYPE gnn_convs_1_conv_lin_weight_fixed_in[128][128];
W_TYPE gnn_convs_2_conv_bias_fixed_in[128];
W_TYPE gnn_convs_2_conv_lin_weight_fixed_in[128][128];
W_TYPE gnn_convs_3_conv_bias_fixed_in[128];
W_TYPE gnn_convs_3_conv_lin_weight_fixed_in[128][128];
W_TYPE gnn_convs_4_conv_bias_fixed_in[128];
W_TYPE gnn_convs_4_conv_lin_weight_fixed_in[128][128];
W_TYPE gnn_convs_5_conv_bias_fixed_in[64];
W_TYPE gnn_convs_5_conv_lin_weight_fixed_in[64][128];

int num_nodes_in;
int num_edges_in;

float node_feature_table_in[600][11];
F_TYPE node_feature_table_fixed_in[600][11];
int edge_list_in[600][2];

float model_output_kernel_float[19];
F_TYPE model_output_kernel_fixed[19];
float model_output_golden_float[19];

float task_golden_output[19];


void load_tb_parameters(){
    load_data_2d<64, 192>("./tb_data/model_parameters/mlp_head_linear_layers_0_weight.bin", mlp_head_linear_layers_0_weight_in);
    load_data_1d<64>("./tb_data/model_parameters/mlp_head_linear_layers_0_bias.bin", mlp_head_linear_layers_0_bias_in);
    load_data_2d<64, 64>("./tb_data/model_parameters/mlp_head_linear_layers_1_weight.bin", mlp_head_linear_layers_1_weight_in);
    load_data_1d<64>("./tb_data/model_parameters/mlp_head_linear_layers_1_bias.bin", mlp_head_linear_layers_1_bias_in);
    load_data_2d<64, 64>("./tb_data/model_parameters/mlp_head_linear_layers_2_weight.bin", mlp_head_linear_layers_2_weight_in);
    load_data_1d<64>("./tb_data/model_parameters/mlp_head_linear_layers_2_bias.bin", mlp_head_linear_layers_2_bias_in);
    load_data_2d<64, 64>("./tb_data/model_parameters/mlp_head_linear_layers_3_weight.bin", mlp_head_linear_layers_3_weight_in);
    load_data_1d<64>("./tb_data/model_parameters/mlp_head_linear_layers_3_bias.bin", mlp_head_linear_layers_3_bias_in);
    load_data_2d<19, 64>("./tb_data/model_parameters/mlp_head_linear_layers_4_weight.bin", mlp_head_linear_layers_4_weight_in);
    load_data_1d<19>("./tb_data/model_parameters/mlp_head_linear_layers_4_bias.bin", mlp_head_linear_layers_4_bias_in);
    load_data_1d<128>("./tb_data/model_parameters/gnn_convs_0_conv_bias.bin", gnn_convs_0_conv_bias_in);
    load_data_2d<128, 11>("./tb_data/model_parameters/gnn_convs_0_conv_lin_weight.bin", gnn_convs_0_conv_lin_weight_in);
    load_data_1d<128>("./tb_data/model_parameters/gnn_convs_1_conv_bias.bin", gnn_convs_1_conv_bias_in);
    load_data_2d<128, 128>("./tb_data/model_parameters/gnn_convs_1_conv_lin_weight.bin", gnn_convs_1_conv_lin_weight_in);
    load_data_1d<128>("./tb_data/model_parameters/gnn_convs_2_conv_bias.bin", gnn_convs_2_conv_bias_in);
    load_data_2d<128, 128>("./tb_data/model_parameters/gnn_convs_2_conv_lin_weight.bin", gnn_convs_2_conv_lin_weight_in);
    load_data_1d<128>("./tb_data/model_parameters/gnn_convs_3_conv_bias.bin", gnn_convs_3_conv_bias_in);
    load_data_2d<128, 128>("./tb_data/model_parameters/gnn_convs_3_conv_lin_weight.bin", gnn_convs_3_conv_lin_weight_in);
    load_data_1d<128>("./tb_data/model_parameters/gnn_convs_4_conv_bias.bin", gnn_convs_4_conv_bias_in);
    load_data_2d<128, 128>("./tb_data/model_parameters/gnn_convs_4_conv_lin_weight.bin", gnn_convs_4_conv_lin_weight_in);
    load_data_1d<64>("./tb_data/model_parameters/gnn_convs_5_conv_bias.bin", gnn_convs_5_conv_bias_in);
    load_data_2d<64, 128>("./tb_data/model_parameters/gnn_convs_5_conv_lin_weight.bin", gnn_convs_5_conv_lin_weight_in);

    // cast the data to fixed point
    cast_2d<64, 192, float, W_TYPE>(mlp_head_linear_layers_0_weight_in, mlp_head_linear_layers_0_weight_fixed_in);
    cast_1d<64, float, W_TYPE>(mlp_head_linear_layers_0_bias_in, mlp_head_linear_layers_0_bias_fixed_in);
    cast_2d<64, 64, float, W_TYPE>(mlp_head_linear_layers_1_weight_in, mlp_head_linear_layers_1_weight_fixed_in);
    cast_1d<64, float, W_TYPE>(mlp_head_linear_layers_1_bias_in, mlp_head_linear_layers_1_bias_fixed_in);
    cast_2d<64, 64, float, W_TYPE>(mlp_head_linear_layers_2_weight_in, mlp_head_linear_layers_2_weight_fixed_in);
    cast_1d<64, float, W_TYPE>(mlp_head_linear_layers_2_bias_in, mlp_head_linear_layers_2_bias_fixed_in);
    cast_2d<64, 64, float, W_TYPE>(mlp_head_linear_layers_3_weight_in, mlp_head_linear_layers_3_weight_fixed_in);
    cast_1d<64, float, W_TYPE>(mlp_head_linear_layers_3_bias_in, mlp_head_linear_layers_3_bias_fixed_in);
    cast_2d<19, 64, float, W_TYPE>(mlp_head_linear_layers_4_weight_in, mlp_head_linear_layers_4_weight_fixed_in);
    cast_1d<19, float, W_TYPE>(mlp_head_linear_layers_4_bias_in, mlp_head_linear_layers_4_bias_fixed_in);
    cast_1d<128, float, W_TYPE>(gnn_convs_0_conv_bias_in, gnn_convs_0_conv_bias_fixed_in);
    cast_2d<128, 11, float, W_TYPE>(gnn_convs_0_conv_lin_weight_in, gnn_convs_0_conv_lin_weight_fixed_in);
    cast_1d<128, float, W_TYPE>(gnn_convs_1_conv_bias_in, gnn_convs_1_conv_bias_fixed_in);
    cast_2d<128, 128, float, W_TYPE>(gnn_convs_1_conv_lin_weight_in, gnn_convs_1_conv_lin_weight_fixed_in);
    cast_1d<128, float, W_TYPE>(gnn_convs_2_conv_bias_in, gnn_convs_2_conv_bias_fixed_in);
    cast_2d<128, 128, float, W_TYPE>(gnn_convs_2_conv_lin_weight_in, gnn_convs_2_conv_lin_weight_fixed_in);
    cast_1d<128, float, W_TYPE>(gnn_convs_3_conv_bias_in, gnn_convs_3_conv_bias_fixed_in);
    cast_2d<128, 128, float, W_TYPE>(gnn_convs_3_conv_lin_weight_in, gnn_convs_3_conv_lin_weight_fixed_in);
    cast_1d<128, float, W_TYPE>(gnn_convs_4_conv_bias_in, gnn_convs_4_conv_bias_fixed_in);
    cast_2d<128, 128, float, W_TYPE>(gnn_convs_4_conv_lin_weight_in, gnn_convs_4_conv_lin_weight_fixed_in);
    cast_1d<64, float, W_TYPE>(gnn_convs_5_conv_bias_in, gnn_convs_5_conv_bias_fixed_in);
    cast_2d<64, 128, float, W_TYPE>(gnn_convs_5_conv_lin_weight_in, gnn_convs_5_conv_lin_weight_fixed_in);


}

void print_graph(int graph_idx,
                 int num_nodes,
                 int num_edges,
                 int edge_list[600][2],
                 float node_feature_table[600][11],
                 float model_output_golden_float[19],
                 float task_golden_output[19]){
    printf("=== graph %d ===\n", graph_idx);
    printf("num_nodes: %d\n", num_nodes);
    printf("num_edges: %d\n", num_edges);
    printf("edge_list:\n");
    for(int i = 0; i < num_edges; i++){
        printf("%d %d\n", edge_list[i][0], edge_list[i][1]);
    }
    printf("node_feature_table:\n");
    for(int i = 0; i < num_nodes; i++){
        for(int j = 0; j < 11; j++){
            printf("%f ", node_feature_table[i][j]);
        }
        printf("\n");
    }
    printf("model_output_golden_float:\n");
    for(int i = 0; i < 19; i++){
        printf("%f ", model_output_golden_float[i]);
    }
    printf("\n");
    printf("task_golden_output:\n");
    for(int i = 0; i < 19; i++){
        printf("%f ", task_golden_output[i]);
    }
    printf("\n");
}

void load_graph(int idx){
    int graph_info[2];
    // int num_nodes;
    // int num_edges;

    // files:
    // ./tb_data/graphs/graph_<idx>_info.bin
    // ./tb_data/graphs/graph_<idx>_coo.bin
    // ./tb_data/graphs/graph_<idx>_node_features.bin
    // ./tb_data/graphs/graph_<idx>_model_golden_output.bin
    // ./tb_data/graphs/graph_<idx>_task_golden_output.bin

    std::string fp_string;

    fp_string = "./tb_data/graphs/graph_" + std::to_string(idx) + "_info.bin";  
    load_data_var_1d<2, int>(fp_string.c_str(), graph_info, 2);
    num_nodes_in = graph_info[0];
    num_edges_in = graph_info[1];
    
    fp_string = "./tb_data/graphs/graph_" + std::to_string(idx) + "_coo.bin";
    load_data_var_2d<600, 2, int>(fp_string.c_str(), edge_list_in, num_edges_in, 2);

    fp_string = "./tb_data/graphs/graph_" + std::to_string(idx) + "_node_features.bin";
    load_data_var_2d<600, 11, float>(fp_string.c_str(), node_feature_table_in, num_nodes_in, 11);
    cast_2d<600, 11, float, W_TYPE>(node_feature_table_in, node_feature_table_fixed_in);

    fp_string = "./tb_data/graphs/graph_" + std::to_string(idx) + "_model_golden_output.bin";
    load_data_var_1d<19, float>(fp_string.c_str(), model_output_golden_float, 19);

    fp_string = "./tb_data/graphs/graph_" + std::to_string(idx) + "_task_golden_output.bin";
    load_data_var_1d<19, float>(fp_string.c_str(), task_golden_output, 19);
}

int read_total_num_of_graphs(){
    int total_num_of_graphs;
    FILE *fp = fopen("./tb_data/dataset_info.txt", "r");
    fscanf(fp, "num_graphs %d", &total_num_of_graphs);
    fclose(fp);
    return total_num_of_graphs;

}

void call_kernel(int copy_parameters_flag){
    fpga_gcn_qm9_top(
            node_feature_table_fixed_in,
            edge_list_in,
            model_output_kernel_fixed,
            num_nodes_in,
            num_edges_in,
            copy_parameters_flag,
            mlp_head_linear_layers_0_weight_fixed_in,
            mlp_head_linear_layers_0_bias_fixed_in,
            mlp_head_linear_layers_1_weight_fixed_in,
            mlp_head_linear_layers_1_bias_fixed_in,
            mlp_head_linear_layers_2_weight_fixed_in,
            mlp_head_linear_layers_2_bias_fixed_in,
            mlp_head_linear_layers_3_weight_fixed_in,
            mlp_head_linear_layers_3_bias_fixed_in,
            mlp_head_linear_layers_4_weight_fixed_in,
            mlp_head_linear_layers_4_bias_fixed_in,
            gnn_convs_0_conv_bias_fixed_in,
            gnn_convs_0_conv_lin_weight_fixed_in,
            gnn_convs_1_conv_bias_fixed_in,
            gnn_convs_1_conv_lin_weight_fixed_in,
            gnn_convs_2_conv_bias_fixed_in,
            gnn_convs_2_conv_lin_weight_fixed_in,
            gnn_convs_3_conv_bias_fixed_in,
            gnn_convs_3_conv_lin_weight_fixed_in,
            gnn_convs_4_conv_bias_fixed_in,
            gnn_convs_4_conv_lin_weight_fixed_in,
            gnn_convs_5_conv_bias_fixed_in,
            gnn_convs_5_conv_lin_weight_fixed_in
        );
}


int main() {
    // printf("########################\n");
    // printf("# GNNBuilder Testbench #\n");
    // printf("########################\n");
    // printf("Loading model parameters...\n");
    std::cout << "########################" << std::endl;
    std::cout << "# GNNBuilder Testbench #" << std::endl;
    std::cout << "########################" << std::endl;

    std::cout << "Loading model parameters..." << std::endl;
    load_tb_parameters();

    std::cout << "Loading weights to the kernel..." << std::endl;
    int copy_parameters_flag_in = 1;
    call_kernel(copy_parameters_flag_in);
    copy_parameters_flag_in = 0;
    
    std::cout << "Evaluating graphs in the dataset..." << std::endl;
    int total_number_of_graphs = read_total_num_of_graphs();

    std::cout << "total_number_of_graphs: " << total_number_of_graphs << std::endl;
    
    // loop over all graphs
    float model_output_mae_sum = 0.0;
    int model_output_mae_count = 0;

    float model_runtime_sum = 0.0;
    int model_runtime_count = 0;

    // make a text file to log the results
    std::ofstream run_log_file ("./tb_data/run_log.txt");

    for(int idx = 0; idx < total_number_of_graphs; idx++){
        std::cout << "graph_idx: " << idx << std::endl;
        std::cout << "loading graph..." << std::endl;
        
        run_log_file << "graph_idx: " << idx << std::endl;
        run_log_file << "loading graph..." << std::endl;


        load_graph(idx);
        // print_graph(idx, num_nodes_in, num_edges_in, edge_list_in, node_feature_table_in, model_output_golden_float, task_golden_output);
        
        auto start = std::chrono::steady_clock::now();
        call_kernel(copy_parameters_flag_in);
        auto end = std::chrono::steady_clock::now();
        auto elapsed_nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        float runtime = (float)elapsed_nanoseconds.count() / (float)1e9;

        cast_1d<19, W_TYPE, float>(model_output_kernel_fixed, model_output_kernel_float);
        
        // print the golden output and the kenrnel output
        std::cout << "model_output_golden_float:" << std::endl;
        run_log_file << "model_output_golden_float:" << std::endl;

        for(int i = 0; i < 19; i++){
            std::cout << model_output_golden_float[i] << " ";
            run_log_file << model_output_golden_float[i] << " ";
        }
        std::cout << std::endl;
        run_log_file << std::endl;

        std::cout << "model_output_kernel_float:" << std::endl;
        run_log_file << "model_output_kernel_float:" << std::endl;
        for(int i = 0; i < 19; i++){

            std::cout << model_output_kernel_float[i] << " ";
            run_log_file << model_output_kernel_float[i] << " ";
        }
        std::cout << std::endl;
        run_log_file << std::endl;


        std::cout << "task_golden_output:" << std::endl;
        run_log_file << "task_golden_output:" << std::endl;
        for(int i = 0; i < 19; i++){
            std::cout << task_golden_output[i] << " ";
            run_log_file << task_golden_output[i] << " ";
        }
        std::cout << std::endl;
        run_log_file << std::endl;

        std::cout << "--------------" << std::endl;
        run_log_file << "--------------" << std::endl;

        for(int i = 0; i < 19; i++){
            model_output_mae_sum += std::abs(model_output_golden_float[i] - model_output_kernel_float[i]);
            model_output_mae_count += 1;
        }

        model_runtime_sum += runtime;
        model_runtime_count += 1;
    }

    // close log file
    run_log_file.close();

    float model_output_mae = model_output_mae_sum / model_output_mae_count;
    std::cout << "model_output_mae: " << model_output_mae << std::endl;
    std::ofstream mae_file ("./tb_data/model_output_mae.txt");
    mae_file << "model_output_mae " << model_output_mae << std::endl;
    mae_file.close();

    float model_runtime = model_runtime_sum / model_runtime_count;
    std::cout << "model_runtime: " << model_runtime << std::endl;
    std::ofstream runtime_file ("./tb_data/model_runtime.txt");
    runtime_file << "model_runtime " << model_runtime << std::endl;
    runtime_file.close();
}