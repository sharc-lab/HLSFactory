#ifndef DCL_H
#define DCL_H

#include <ap_fixed.h>
#include <hls_math.h>
#include <cmath> // for std::isnan
#include <hls_stream.h>

// You may choose to use ap_fixed<16,5> if desired, but here we use float.
typedef float data_t;

// Define maximum sizes (adjust as necessary)
#define MAX_HITS       261    // Maximum number of hits per event
#define MAX_EDGES      80    // Maximum number of edges per event
#define MAX_TRACKS     108     // Maximum number of tracks (connected components)
#define MAX_TRACK_SIZE MAX_HITS  // Maximum number of hits in one track
#define NUM_LAYERS     5       // Number of detector layer groups
#define NUM_EVENTS     200       //

// Define the layer groups (matching Python's layers = [(0,), (1,), (2,), (3,4), (5,6)])
const int layer_start[NUM_LAYERS] = {0, 1, 2, 3, 5};
const int layer_end[NUM_LAYERS]   = {0, 1, 2, 4, 6};

// The EventInfo structure holds the processed event (track) attributes.
struct EventInfo {
    data_t n_pixels[MAX_TRACKS][NUM_LAYERS];       // Sum of pixel counts per track per layer group
    data_t energy[MAX_TRACKS];                     // Energy for each track
    data_t momentum[MAX_TRACKS][3];                // Momentum vector for each track
    data_t interaction_point[3];                   // Global interaction (collision) point
    bool trigger;                                  // Event trigger flag
    bool has_trigger_pair;                         // Indicates if the event has a paired trigger
    data_t track_origin[MAX_TRACKS][3];            // Estimated origin for each track
    int trigger_node[MAX_TRACKS];                  // Index of the trigger hit for each track
    data_t particle_id[MAX_TRACKS];                   // Representative particle ID for each track
    data_t particle_type[MAX_TRACKS];                 // Particle type classification for each track
    data_t parent_particle_type[MAX_TRACKS];          // Parent particle type for each track
    data_t track_hits[MAX_TRACKS][3 * NUM_LAYERS];   // Weighted hit positions (per layer group)
    int track_n_hits[MAX_TRACKS][NUM_LAYERS];        // Count of hits per layer group for each track
    int num_tracks;                                // Total number of tracks reconstructed
};

// Prototype for the HLS kernel function that reconstructs the tracks.
// The new parameter "intt_required" implements the inner tracker requirement.
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
);

#endif