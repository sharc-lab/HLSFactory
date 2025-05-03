#include "dcl.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>   // for std::system() if you prefer, or mkdir()


static int      layer_id_arr     [NUM_EVENTS][MAX_HITS];
static int      n_pixels_arr     [NUM_EVENTS][MAX_HITS];
static data_t   hit_cartesian_arr[NUM_EVENTS][MAX_HITS][3];
static data_t   particle_id_arr  [NUM_EVENTS][MAX_HITS];
static data_t   energy_arr       [NUM_EVENTS][MAX_HITS];
static data_t   momentum_arr     [NUM_EVENTS][MAX_HITS][3];
static data_t   track_origin_arr [NUM_EVENTS][MAX_HITS][3];
static int      trigger_node_arr [NUM_EVENTS][MAX_HITS];
static data_t   particle_type_arr       [NUM_EVENTS][MAX_HITS];
static data_t   parent_particle_type_arr[NUM_EVENTS][MAX_HITS];
static data_t   interaction_point_arr   [NUM_EVENTS][3];
static data_t   model_edge_probability_arr[NUM_EVENTS][MAX_EDGES];
static int      edge_index_arr           [NUM_EVENTS][2][MAX_EDGES];
static bool     trigger_flag    [NUM_EVENTS];
static bool     has_trigger_pair[NUM_EVENTS];
static bool     intt_required   [NUM_EVENTS];
static int      num_edges       [NUM_EVENTS];
static int      num_hits        [NUM_EVENTS];
static EventInfo event_info     [NUM_EVENTS];
 
// ---------- Binary File Reader ----------
template<typename T>
void read_binary_file(const std::string& filename, std::vector<T>& container) {
    std::ifstream file(filename, std::ios::binary);
    assert(file.is_open());
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);
    container.resize(size / sizeof(T));
    file.read(reinterpret_cast<char*>(container.data()), size);
    file.close();
}
 
template<typename T>
int infer_size(const std::string& filename, int columns = 1) {
    std::ifstream file(filename, std::ios::binary);
    assert(file.is_open());
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.close();
    return size / (sizeof(T) * columns);
}

// POSIX: recursively walk converted/<class>/<event> directories
static std::vector<std::string> discover_events(const std::string& base) {
    std::vector<std::string> events;
    struct stat st;
    DIR* d1 = opendir(base.c_str());
    assert(d1 && "Cannot open base directory");
    struct dirent* e1;
    while ((e1 = readdir(d1))) {
        if (e1->d_name[0]=='.') continue;
        std::string cdir = base + "/" + e1->d_name;
        if (stat(cdir.c_str(), &st)==0 && S_ISDIR(st.st_mode)) {
            DIR* d2 = opendir(cdir.c_str());
            assert(d2);
            struct dirent* e2;
            while ((e2 = readdir(d2))) {
                if (e2->d_name[0]=='.') continue;
                std::string ev = cdir + "/" + e2->d_name;
                if (stat(ev.c_str(), &st)==0 && S_ISDIR(st.st_mode)) {
                    events.push_back(ev);
                }
            }
            closedir(d2);
        }
    }
    closedir(d1);
    std::sort(events.begin(), events.end());
    return events;
}

// mkdir -p: make each subpath in turn
static void mkdir_p(const std::string& path) {
    size_t pos = 0;
    do {
        pos = path.find('/', pos + 1);
        std::string sub = path.substr(0, pos);
        if (sub.size()>0) mkdir(sub.c_str(), 0755);
    } while (pos != std::string::npos);
}
 
int main() {
    // Set the directory path where the binary files reside.
    //std::string directory_path = "/nethome/sbommu3/FPGA/project/FPGA_Topic2_LU/2025_Spring/topic2/event000041188/"; // Adjust to your directory
    //std::string directory_path = "/nethome/sramanath3/project/FPGA_Topic2_LU/2025_Spring/topic2/event000041188/"; // Adjust to your directory
    // Raw data containers (vectors to temporarily hold file data)
    // const std::string input_base  = "/nethome/sbommu3/FPGA/project_2/FPGA_Topic2_LU/2025_Spring/topic2/converted";
    const std::string input_base  = "/nethome/sbommu3/FPGA/project_4/FPGA_Topic2_LU/2025_Spring/topic2/converted";
    //const std::string input_base  = "./converted";
    const std::string output_base = "./output";

    auto event_dirs = discover_events(input_base);

    // Use however many we found
    const size_t num_events = event_dirs.size();    
    //assert(num_events == NUM_EVENTS && "found != NUM_EVENTS");


    std::vector<int> edge_index_raw;
    std::vector<data_t> model_edge_prob;
    std::vector<int> layer_id_vec;
    std::vector<int> n_pixels_vec;
    std::vector<data_t> hit_cartesian_vec;
    std::vector<data_t> particle_id_vec;
    std::vector<data_t> energy_vec;
    std::vector<data_t> momentum_vec;
    std::vector<data_t> track_origin_vec;
    std::vector<int> trigger_node_vec;
    std::vector<data_t> particle_type_vec;
    std::vector<data_t> parent_particle_type_vec;
    std::vector<data_t> interaction_point_vec;
    std::vector<int> trigger_vec;  // Assuming stored as int: 0 or 1
    std::vector<uint8_t> has_trigger_pair_raw;
 
	// //Declarations of Inputs
    // int layer_id_arr[NUM_EVENTS][MAX_HITS];
    // int n_pixels_arr[NUM_EVENTS][MAX_HITS];
    // data_t hit_cartesian_arr[NUM_EVENTS][MAX_HITS][3];
    // data_t particle_id_arr[NUM_EVENTS][MAX_HITS];
    // data_t energy_arr[NUM_EVENTS][MAX_HITS];
    // data_t momentum_arr[NUM_EVENTS][MAX_HITS][3];
    // data_t track_origin_arr[NUM_EVENTS][MAX_HITS][3];
    // int trigger_node_arr[NUM_EVENTS][MAX_HITS];
    // data_t particle_type_arr[NUM_EVENTS][MAX_HITS];
    // data_t parent_particle_type_arr[NUM_EVENTS][MAX_HITS];
    // data_t interaction_point_arr[NUM_EVENTS][3];	
    // data_t model_edge_probability_arr[NUM_EVENTS][MAX_EDGES];
    // int edge_index_arr[NUM_EVENTS][2][MAX_EDGES];
    // bool trigger_flag[NUM_EVENTS];
    // bool has_trigger_pair[NUM_EVENTS];
    // bool intt_required[NUM_EVENTS];
    // int num_edges[NUM_EVENTS];
    // int num_hits[NUM_EVENTS];
    // EventInfo event_info[NUM_EVENTS]; // This structure will be filled by the kernel.
 
 
	//For Loop
	for(int event=0; event < NUM_EVENTS; event++) {
    	// Determine number of edges and hits using file sizes

        std::string directory_path = event_dirs[event] + "/";


    	num_edges[event] = infer_size<int>(directory_path + "edge_index.bin", 2);
    	num_hits[event]  = infer_size<data_t>(directory_path + "hit_cartesian.bin", 3);
 
    	std::cout << "Number of edges: " << num_edges << std::endl;
    	std::cout << "Number of hits: " << num_hits << std::endl;
 
    	// Read binary files
    	read_binary_file(directory_path + "edge_index.bin", edge_index_raw);
    	read_binary_file(directory_path + "model_edge_probability.bin", model_edge_prob);
    	read_binary_file(directory_path + "layer_id.bin", layer_id_vec);
    	read_binary_file(directory_path + "n_pixels.bin", n_pixels_vec);
    	read_binary_file(directory_path + "hit_cartesian.bin", hit_cartesian_vec);
    	read_binary_file(directory_path + "particle_id.bin", particle_id_vec);
    	read_binary_file(directory_path + "energy.bin", energy_vec);
    	read_binary_file(directory_path + "momentum.bin", momentum_vec);
    	read_binary_file(directory_path + "track_origin.bin", track_origin_vec);
    	read_binary_file(directory_path + "trigger_node.bin", trigger_node_vec);
    	read_binary_file(directory_path + "particle_type.bin", particle_type_vec);
    	read_binary_file(directory_path + "parent_particle_type.bin", parent_particle_type_vec);
    	read_binary_file(directory_path + "interaction_point.bin", interaction_point_vec);
    	read_binary_file(directory_path + "trigger.bin", trigger_vec);
    	read_binary_file(directory_path + "has_trigger_pair.bin", has_trigger_pair_raw);
 
    	trigger_flag[event] = (trigger_vec.size() > 0 && trigger_vec[0] != 0);
    	has_trigger_pair[event] = (has_trigger_pair_raw.size() > 0 && has_trigger_pair_raw[0] != 0);
    	intt_required[event] = false;
 
    	// ---------- Pack Data into Fixed-Size Arrays ----------
    	for (int i = 0; i < num_edges[event]; i++) {
    	    edge_index_arr[event][0][i] = edge_index_raw[2 * i];
    	    edge_index_arr[event][1][i] = edge_index_raw[2 * i + 1];
    	}
 
    	for (int i = 0; i < num_edges[event]; i++) {
    	    model_edge_probability_arr[event][i] = model_edge_prob[i];
    	}
 
    	for (int i = 0; i < num_hits[event]; i++) {
    	    layer_id_arr[event][i] = layer_id_vec[i];
    	    n_pixels_arr[event][i] = n_pixels_vec[i];
    	}
    	for (int i = 0; i < num_hits[event]; i++) {
    	    for (int j = 0; j < 3; j++) {
    	        hit_cartesian_arr[event][i][j] = hit_cartesian_vec[3 * i + j];
    	    }
    	}
    	for (int i = 0; i < num_hits[event]; i++) {
    	    particle_id_arr[event][i] = particle_id_vec[i];
    	}
    	for (int i = 0; i < num_hits[event]; i++) {
    	    energy_arr[event][i] = energy_vec[i];
    	}
    	for (int i = 0; i < num_hits[event]; i++) {
    	    for (int j = 0; j < 3; j++) {
    	        momentum_arr[event][i][j] = momentum_vec[3 * i + j];
    	    }
    	}
    	for (int i = 0; i < num_hits[event]; i++) {
    	    for (int j = 0; j < 3; j++) {
    	        track_origin_arr[event][i][j] = track_origin_vec[3 * i + j];
    	    }
    	}
    	for (int i = 0; i < num_hits[event]; i++) {
    	    trigger_node_arr[event][i] = trigger_node_vec[i];
    	}
    	for (int i = 0; i < num_hits[event]; i++) {
    	    particle_type_arr[event][i] = particle_type_vec[i];
    	    parent_particle_type_arr[event][i] = parent_particle_type_vec[i];
    	}
    	for (int i = 0; i < 3; i++) {
    	    interaction_point_arr[event][i] = interaction_point_vec[i];
    	}
	}
    // ---------- Call the Track Reconstruction Kernel ----------
    compute_tracks_HLS(
        edge_index_arr,
        model_edge_probability_arr,
        num_edges,
        layer_id_arr,
        n_pixels_arr,
        hit_cartesian_arr,
        particle_id_arr,
        energy_arr,
        momentum_arr,
        track_origin_arr,
        trigger_node_arr,
        particle_type_arr,
        parent_particle_type_arr,
        num_hits,
        interaction_point_arr,
        trigger_flag,
        has_trigger_pair,
        intt_required,  // Pass the new inner tracker requirement flag.
        event_info
    );
 
    // ---------- Output Results ----------
	for(int event=0; event<NUM_EVENTS; event++) {
        // get relative subpath: e.g. "0/event000041188"
        // strip "./converted/" prefix to get e.g. "0/event000041188"
        std::string full = event_dirs[event];
        std::string rel  = full.substr(input_base.size() + 1);
        std::string out_dir = output_base + "/" + rel;
        mkdir_p(out_dir);

        std::ofstream ofs(out_dir + "/result.txt");
        assert(ofs.is_open());

        ofs << "--- Reconstructed Event Information ---\n";
        ofs << "Global Interaction Point: ["
            << event_info[event].interaction_point[0] << ", "
            << event_info[event].interaction_point[1] << ", "
            << event_info[event].interaction_point[2] << "]\n";
        ofs << "Trigger: "          << event_info[event].trigger          << "\n";
        ofs << "Has Trigger Pair: " << event_info[event].has_trigger_pair << "\n";
        ofs << "Number of Tracks: " << event_info[event].num_tracks      << "\n\n";

        for (int t = 0; t < event_info[event].num_tracks; ++t) {
            ofs << "Track " << t << ":\n";
            ofs << "  Energy: " << event_info[event].energy[t] << "\n";
            ofs << "  Momentum: ["
                << event_info[event].momentum[t][0] << ", "
                << event_info[event].momentum[t][1] << ", "
                << event_info[event].momentum[t][2] << "]\n";
            ofs << "  Track Origin: ["
                << event_info[event].track_origin[t][0] << ", "
                << event_info[event].track_origin[t][1] << ", "
                << event_info[event].track_origin[t][2] << "]\n";
            ofs << "  Trigger Node: " << event_info[event].trigger_node[t] << "\n";
            ofs << "  Particle ID: "  << event_info[event].particle_id[t]    << "\n";
            ofs << "  Particle Type: " << event_info[event].particle_type[t]   << "\n";
            ofs << "  Parent Particle Type: "
                << event_info[event].parent_particle_type[t] << "\n";

            for (int layer = 0; layer < NUM_LAYERS; ++layer) {
                ofs << "  Layer Group " << layer << ":\n";
                ofs << "    n_pixels:    "
                    << event_info[event].n_pixels[t][layer]    << "\n";
                ofs << "    track_n_hits: "
                    << event_info[event].track_n_hits[t][layer] << "\n";
                ofs << "    track_hits: ["
                    << event_info[event].track_hits[t][3*layer]   << ", "
                    << event_info[event].track_hits[t][3*layer+1] << ", "
                    << event_info[event].track_hits[t][3*layer+2] << "]\n";
            }
            ofs << "\n";
        }
        ofs.close();
    }
	
    return 0;
}