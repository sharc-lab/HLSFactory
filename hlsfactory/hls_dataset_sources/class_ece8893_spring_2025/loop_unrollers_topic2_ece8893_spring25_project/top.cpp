#include "dcl.h"
// #define DEBUG
#define MAX_NEIGHBORS 64

void filter_and_adjacency_list(
    int event[1],
    int edge_index[NUM_EVENTS][2][MAX_EDGES],
    data_t model_edge_probability[NUM_EVENTS][MAX_EDGES],
    int num_edges[NUM_EVENTS],
    int edge_index_local[2][MAX_EDGES],
    data_t model_edge_probability_local[MAX_EDGES],
    int num_edges_local[1],
    int num_nodes[1],
    int num_hits[NUM_EVENTS],
    int adj_list[MAX_HITS][MAX_NEIGHBORS],
    int adj_count[MAX_HITS],
    int max_node[1],
    int event2[1],
    int num_nodes3[1]
)
{
    event2[0] = event[0];
    int filtered_edges[2][MAX_EDGES];
	int filtered_count[1];
    // ----- Step 1: Filter Edges -----
    for (int i = 0; i < num_edges[event[0]]; i++) {
        edge_index_local[0][i] = edge_index[event[0]][0][i];
        edge_index_local[1][i] = edge_index[event[0]][1][i];
        model_edge_probability_local[i] = model_edge_probability[event[0]][i];
    }
    num_edges_local[0] = num_edges[event[0]];
	filtered_count[0] = 0;
    data_t threshold = 0.5;
    for (int i = 0; i < num_edges_local[0]; i++) {
        if (model_edge_probability_local[i] >= threshold) {
            filtered_edges[0][filtered_count[0]] = edge_index_local[0][i];
            filtered_edges[1][filtered_count[0]] = edge_index_local[1][i];
            filtered_count[0]++;
        }
    }
    num_nodes[0] = num_hits[event[0]];
    num_nodes3[0] = num_nodes[0];
    // ----- Step 2: Build the Adjacency List -----
    for (int i = 0; i < num_nodes[0]; i++) {
        adj_count[i] = 0;
    }
	max_node[0] = 0;
    for (int i = 0; i < filtered_count[0]; i++) {
        int u = filtered_edges[0][i];
        int v = filtered_edges[1][i];
        if (u > max_node[0]) max_node[0] = u;
        if (v > max_node[0]) max_node[0] = v;
        if (adj_count[u] < MAX_NEIGHBORS) {
            adj_list[u][adj_count[u]] = v;
            adj_count[u]++;
        }
        if (adj_count[v] < MAX_NEIGHBORS) {
            adj_list[v][adj_count[v]] = u;
            adj_count[v]++;
        }
    }
}

void iterative_dfs (
    int event2[1],
    int num_nodes[1],
    int max_node[1],
    int adj_list[MAX_HITS][MAX_NEIGHBORS],
    int adj_count[MAX_HITS],
    int tracks[MAX_TRACKS][MAX_TRACK_SIZE],
    int track_sizes[MAX_TRACKS],
    int track_count[1],
    int event3[1]
) {
    event3[0] = event2[0];
    // ----- Step 3: Connected Components via Iterative DFS -----
    bool visited[MAX_HITS];
    for (int i = 0; i < num_nodes[0]; i++) {
        visited[i] = false;
    }
	track_count[0] = 0;
    for (int i = 0; i < max_node[0]+1; i++) {
        if (!visited[i]) {
            int current_track[MAX_TRACK_SIZE];
            int current_size = 0;
            int stack[MAX_HITS];
            int stack_ptr = 0;
            stack[stack_ptr++] = i;
            visited[i] = true;
            while (stack_ptr > 0) {
                int node = stack[--stack_ptr];
                current_track[current_size++] = node;
                for (int j = 0; j < adj_count[node]; j++) {
                    int neigh = adj_list[node][j];
                    if (!visited[neigh]) {
                        visited[neigh] = true;
                        stack[stack_ptr++] = neigh;
                    }
                }
            }
            for (int k = 0; k < current_size; k++) {
                tracks[track_count[0]][k] = current_track[k];
            }
            track_sizes[track_count[0]] = current_size;
            track_count[0]++;
        }
    }
}

void process_tracks (
    int event[1],
    int track_count[1],
    int tracks[MAX_TRACKS][MAX_TRACK_SIZE],
    int track_sizes[MAX_TRACKS],
    int layer_id[NUM_EVENTS][MAX_HITS],
    int n_pixels_arr[NUM_EVENTS][MAX_HITS],
    data_t hit_cartesian[NUM_EVENTS][MAX_HITS][3],
    data_t particle_id_arr[NUM_EVENTS][MAX_HITS],
    data_t energy_arr[NUM_EVENTS][MAX_HITS],
    data_t momentum_arr[NUM_EVENTS][MAX_HITS][3],
    data_t track_origin_arr[NUM_EVENTS][MAX_HITS][3],
    int trigger_node_arr[NUM_EVENTS][MAX_HITS],
    data_t particle_type_arr[NUM_EVENTS][MAX_HITS],
    data_t parent_particle_type_arr[NUM_EVENTS][MAX_HITS],
    data_t interaction_point_arr[NUM_EVENTS][3],
    bool trigger[NUM_EVENTS],
    bool has_trigger_pair[NUM_EVENTS],
    bool intt_required[NUM_EVENTS],
    int layer_id_local[MAX_HITS],
    int n_pixels_arr_local[MAX_HITS],
    data_t hit_cartesian_local[MAX_HITS][3],
    data_t particle_id_arr_local[MAX_HITS],
    data_t energy_arr_local[MAX_HITS],
    data_t momentum_arr_local[MAX_HITS][3],
    data_t track_origin_arr_local[MAX_HITS][3],
    int trigger_node_arr_local[MAX_HITS],
    data_t particle_type_arr_local[MAX_HITS],
    data_t parent_particle_type_arr_local[MAX_HITS],
    data_t interaction_point_arr_local[3],
    bool trigger_local[1],
    bool has_trigger_pair_local[1],
    bool intt_required_local[1],
    int event4[1],
    int track_count4[1],
    int processed_tracks[1],
    data_t weighted_sum[MAX_TRACKS][NUM_LAYERS][3],
    int sum_pixels[MAX_TRACKS][NUM_LAYERS],
    int count_hits[MAX_TRACK_SIZE][NUM_LAYERS],
    int rep_hit[MAX_TRACKS]
) {
    event4[0] = event[0];
    track_count4[0] = track_count[0];
    for (int i = 0; i < MAX_HITS; i++) {
        layer_id_local[i] = layer_id[event[0]][i];
        n_pixels_arr_local[i] = n_pixels_arr[event[0]][i];
        for (int j = 0; j < 3; j++) {
            hit_cartesian_local[i][j] = hit_cartesian[event[0]][i][j];
        }
        particle_id_arr_local[i] = particle_id_arr[event[0]][i];
        energy_arr_local[i] = energy_arr[event[0]][i];
        for (int j = 0; j < 3; j++) {
            momentum_arr_local[i][j] = momentum_arr[event[0]][i][j];
            track_origin_arr_local[i][j] = track_origin_arr[event[0]][i][j];
        }
        trigger_node_arr_local[i] = trigger_node_arr[event[0]][i];
        particle_type_arr_local[i] = particle_type_arr[event[0]][i];
        parent_particle_type_arr_local[i] = parent_particle_type_arr[event[0]][i];
    }
    for (int i = 0; i < 3; i++) {
        interaction_point_arr_local[i] = interaction_point_arr[event[0]][i];
    }
    trigger_local[0] = trigger[event[0]];
    has_trigger_pair_local[0] = has_trigger_pair[event[0]];
    intt_required_local[0] = intt_required[event[0]];

    processed_tracks[0] = 0;
    for (int t = 0; t < track_count[0] && processed_tracks[0] < MAX_TRACKS; t++) {
        if (intt_required_local[0]) {
            bool valid_track = false;
            for (int k = 0; k < track_sizes[t]; k++) {
                int hit_idx = tracks[t][k];
                if (layer_id_local[hit_idx] >= 3) {
                    valid_track = true;
                    break;
                }
            }
            if (!valid_track) continue;
        }
        for (int j = 0; j < NUM_LAYERS; j++) {
            for (int d = 0; d < 3; d++) {
                weighted_sum[t][j][d] = 0;
            }
            sum_pixels[t][j] = 0;
            count_hits[t][j] = 0;
            for (int k = 0; k < track_sizes[t]; k++) {
                int hit_idx = tracks[t][k];
                int hit_layer = layer_id_local[hit_idx];
                if (hit_layer >= layer_start[j] && hit_layer <= layer_end[j]) {
                    int pix = n_pixels_arr_local[hit_idx];
                    sum_pixels[t][j] += pix;
                    for (int d = 0; d < 3; d++) {
                        weighted_sum[t][j][d] += pix * hit_cartesian_local[hit_idx][d];
                    }
                    count_hits[t][j]++;
                }
            }
        }
        data_t pids[MAX_TRACK_SIZE][3];
        for (int i = 0; i < MAX_TRACK_SIZE; i++) {
            pids[i][0] = -1;
            pids[i][1] = -1;
            pids[i][2] = -1;
        }
        for (int i = 0; i < track_sizes[t]; i++) {
            data_t p_id = particle_id_arr_local[tracks[t][i]];
            for (int j = 0; j < MAX_TRACK_SIZE; j++) {
                if (pids[j][0] == -1) {
                    pids[j][0] = p_id;
                    pids[j][1] = 1;
                    pids[j][2] = tracks[t][i];
                    break;
                } else if (pids[j][0] == p_id) {
                    pids[j][1]++;
                    break;
                }
            }
        }
        int max_count = 0;
        int mode_particle_id = -1;
        int track_particle_idx = -1;
        for (int i = 0; i < MAX_TRACK_SIZE; i++) {
            if (pids[i][1] > max_count) {
                max_count = pids[i][1];
                mode_particle_id = pids[i][0];
                track_particle_idx = pids[i][2];
            }
        }
        rep_hit[t] = tracks[t][track_particle_idx];
    }
}

void process_tracks_update (
    int event[1],
    int track_count[1],
    int processed_tracks[1],
    data_t weighted_sum[MAX_TRACKS][NUM_LAYERS][3],
    int sum_pixels[MAX_TRACKS][NUM_LAYERS],
    int count_hits[MAX_TRACK_SIZE][NUM_LAYERS],
    int rep_hit[MAX_TRACKS],
    EventInfo event_info[NUM_EVENTS],
    data_t particle_id_arr_local[MAX_HITS],
    data_t energy_arr_local[MAX_HITS],
    data_t momentum_arr_local[MAX_HITS][3],
    data_t track_origin_arr_local[MAX_HITS][3],
    int trigger_node_arr_local[MAX_HITS],
    data_t particle_type_arr_local[MAX_HITS],
    data_t parent_particle_type_arr_local[MAX_HITS],
    data_t interaction_point_arr_local[3],
    bool trigger_local[1],
    bool has_trigger_pair_local[1]
) {
    for (int t = 0; t < track_count[0] && processed_tracks[0] < MAX_TRACKS; t++) {
        for (int j = 0; j < NUM_LAYERS; j++) {
            for (int d = 0; d < 3; d++) {
                if (sum_pixels[t][j] > 0)
                    event_info[event[0]].track_hits[processed_tracks[0]][3*j + d] = weighted_sum[t][j][d] / sum_pixels[t][j];
                else
                    event_info[event[0]].track_hits[processed_tracks[0]][3*j + d] = 0;
            }
            event_info[event[0]].n_pixels[processed_tracks[0]][j] = sum_pixels[t][j];
            event_info[event[0]].track_n_hits[processed_tracks[0]][j] = count_hits[t][j];
        }
        event_info[event[0]].energy[processed_tracks[0]] = energy_arr_local[rep_hit[t]];
        for (int d = 0; d < 3; d++) {
            event_info[event[0]].momentum[processed_tracks[0]][d] = momentum_arr_local[rep_hit[t]][d];
            event_info[event[0]].track_origin[processed_tracks[0]][d] = track_origin_arr_local[rep_hit[t]][d];
        }
        event_info[event[0]].trigger_node[processed_tracks[0]] = trigger_node_arr_local[rep_hit[t]];
        event_info[event[0]].particle_id[processed_tracks[0]] = particle_id_arr_local[rep_hit[t]];
        event_info[event[0]].particle_type[processed_tracks[0]] = particle_type_arr_local[rep_hit[t]];
        event_info[event[0]].parent_particle_type[processed_tracks[0]] = parent_particle_type_arr_local[rep_hit[t]];
        processed_tracks[0]++;
    }
    event_info[event[0]].num_tracks = processed_tracks[0];
    // ----- Step 5: Store Global Event Information -----
    for (int d = 0; d < 3; d++) {
        event_info[event[0]].interaction_point[d] = interaction_point_arr_local[d];
    }
    event_info[event[0]].trigger = trigger_local[0];
    event_info[event[0]].has_trigger_pair = has_trigger_pair_local[0];
}




// ------------------------------
// Kernel Function: compute_tracks_HLS
// ------------------------------
void compute_tracks_HLS(
    int edge_index[NUM_EVENTS][2][MAX_EDGES],
    data_t model_edge_probability[NUM_EVENTS][MAX_EDGES],
    int num_edges[NUM_EVENTS],
    int layer_id[NUM_EVENTS][MAX_HITS],
    int n_pixels_arr[NUM_EVENTS][MAX_HITS],
    data_t hit_cartesian[NUM_EVENTS][MAX_HITS][3],
    data_t particle_id_arr[NUM_EVENTS][MAX_HITS],
    data_t energy_arr[NUM_EVENTS][MAX_HITS],
    data_t momentum_arr[NUM_EVENTS][MAX_HITS][3],
    data_t track_origin_arr[NUM_EVENTS][MAX_HITS][3],
    int trigger_node_arr[NUM_EVENTS][MAX_HITS],
    data_t particle_type_arr[NUM_EVENTS][MAX_HITS],
    data_t parent_particle_type_arr[NUM_EVENTS][MAX_HITS],
    int num_hits[NUM_EVENTS],
    data_t interaction_point_arr[NUM_EVENTS][3],
    bool trigger[NUM_EVENTS],
    bool has_trigger_pair[NUM_EVENTS],
    bool intt_required[NUM_EVENTS],
    EventInfo event_info[NUM_EVENTS]
) {
	
#pragma HLS interface m_axi port=edge_index offset=slave bundle=mem1
#pragma HLS interface m_axi port=model_edge_probability offset=slave bundle=mem1
#pragma HLS interface m_axi port=num_edges offset=slave bundle=mem1

#pragma HLS interface m_axi port=num_hits offset=slave bundle=mem1

#pragma HLS interface m_axi port=layer_id offset=slave bundle=mem2
#pragma HLS interface m_axi port=n_pixels_arr offset=slave bundle=mem2
#pragma HLS interface m_axi port=hit_cartesian offset=slave bundle=mem2
#pragma HLS interface m_axi port=particle_id_arr offset=slave bundle=mem2
#pragma HLS interface m_axi port=energy_arr offset=slave bundle=mem2
#pragma HLS interface m_axi port=momentum_arr offset=slave bundle=mem2
#pragma HLS interface m_axi port=track_origin_arr offset=slave bundle=mem2
#pragma HLS interface m_axi port=trigger_node_arr offset=slave bundle=mem2
#pragma HLS interface m_axi port=particle_type_arr offset=slave bundle=mem2
#pragma HLS interface m_axi port=parent_particle_type_arr offset=slave bundle=mem2
#pragma HLS interface m_axi port=interaction_point_arr offset=slave bundle=mem2
#pragma HLS interface m_axi port=trigger offset=slave bundle=mem2
#pragma HLS interface m_axi port=has_trigger_pair offset=slave bundle=mem2
#pragma HLS interface m_axi port=intt_required offset=slave bundle=mem2
#pragma HLS interface m_axi port=event_info offset=slave bundle=mem3

#pragma HLS interface s_axilite port=return

    //Local Data Structures
    int edge_index_local[2][MAX_EDGES];
    data_t model_edge_probability_local[MAX_EDGES];
    int num_edges_local[1];
    int layer_id_local[MAX_HITS];
    int n_pixels_arr_local[MAX_HITS];
    data_t hit_cartesian_local[MAX_HITS][3];
    data_t particle_id_arr_local[MAX_HITS];
    data_t energy_arr_local[MAX_HITS];
    data_t momentum_arr_local[MAX_HITS][3];
    data_t track_origin_arr_local[MAX_HITS][3];
    int trigger_node_arr_local[MAX_HITS];
    data_t particle_type_arr_local[MAX_HITS];
    data_t parent_particle_type_arr_local[MAX_HITS];
    data_t interaction_point_arr_local[3];
    bool trigger_local[1];
    bool has_trigger_pair_local[1];
    bool intt_required_local[1];
    trigger_local[0] = false;
	has_trigger_pair_local[0] = false;
	intt_required_local[0] = false;

    for (int event_it = 0; event_it < NUM_EVENTS; event_it++) {
		#pragma HLS dataflow

		int event[1];
		int adj_list[MAX_HITS][MAX_NEIGHBORS];
		int adj_count[MAX_HITS];
		int max_node[1];
		int tracks[MAX_TRACKS][MAX_TRACK_SIZE];
		int track_sizes[MAX_TRACKS];
		int track_count[1];
		int event1[1];
		int event2[1];
		int event3[1];
        int event4[1];
		int num_nodes_local2[1];
		int num_nodes_local3[1];

        int track_count4[1];

        int processed_tracks[1];
        data_t weighted_sum[MAX_TRACKS][NUM_LAYERS][3];
        int sum_pixels[MAX_TRACKS][NUM_LAYERS];
        int count_hits[MAX_TRACK_SIZE][NUM_LAYERS];
        int rep_hit[MAX_TRACKS];

		event[0] = event_it;

        // ----- Step 1: Filter Edges and adjaceny list creation -----
        filter_and_adjacency_list(
            event,
            edge_index,
            model_edge_probability,
            num_edges,
            edge_index_local,
            model_edge_probability_local,
            num_edges_local,
            num_nodes_local2,
            num_hits,
            adj_list,
            adj_count,
            max_node,
            event2,
            num_nodes_local3
        );

        // ----- Step 2: Connected Components via Iterative DFS -----
        iterative_dfs(
            event2,
            num_nodes_local3,
            max_node,
            adj_list,
            adj_count,
            tracks,
            track_sizes,
            track_count,
            event3 
        );
        // ----- Step 3: Process Each Track -----
        process_tracks(
            event3,
            track_count,
            tracks,
            track_sizes,
            layer_id,
            n_pixels_arr,
            hit_cartesian,
            particle_id_arr,
            energy_arr,
            momentum_arr,
            track_origin_arr,
            trigger_node_arr,
            particle_type_arr,
            parent_particle_type_arr,
            interaction_point_arr,
            trigger,
            has_trigger_pair,
            intt_required,
            layer_id_local,
            n_pixels_arr_local,
            hit_cartesian_local,
            particle_id_arr_local,
            energy_arr_local,
            momentum_arr_local,
            track_origin_arr_local,
            trigger_node_arr_local,
            particle_type_arr_local,
            parent_particle_type_arr_local,
            interaction_point_arr_local,
            trigger_local,
            has_trigger_pair_local,
            intt_required_local,
            event4,
            track_count4,
            processed_tracks,
            weighted_sum,
            sum_pixels,
            count_hits,
            rep_hit
        );

        //Step 4: Update each track to event_info
        process_tracks_update(
            event4,
            track_count4,
            processed_tracks,
            weighted_sum,
            sum_pixels,
            count_hits,
            rep_hit,
            event_info,
            particle_id_arr_local,
            energy_arr_local,
            momentum_arr_local,
            track_origin_arr_local,
            trigger_node_arr_local,
            particle_type_arr_local,
            parent_particle_type_arr_local,
            interaction_point_arr_local,
            trigger_local,
            has_trigger_pair_local
        );
    }
}