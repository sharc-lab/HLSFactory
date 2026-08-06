#include <cassert>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <ostream>
#include <random>
#include <stdexcept>
#include <vector>
#include <memory>
#include <functional>
#include <filesystem>
#include <omp.h>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <cmath>
#include <cassert>

#define NODES 2392
#define NN_LIST_SIZE 20
#define CHECKLIST_CAPACITY 500
#define ITERATIONS 1

uint32_t CHECKLIST_SIZE = 0;

using namespace std;

int64_t two_opt_nn(int32_t coords_in[NODES][2],
                   uint32_t neighbors_in[NODES][NN_LIST_SIZE],
                   uint32_t route[NODES],
                   uint32_t checklist[CHECKLIST_CAPACITY],
                   uint32_t nn_list_size,
                   uint32_t CHECKLIST_SIZE);

typedef struct {
    int32_t x;
    int32_t y;
} Point;

double get_distance(Point p1, Point p2) {
    return std::sqrt(std::pow(p1.x - p2.x,2) + std::pow(p1.y - p2.y,2));
}

typedef struct {
    std::vector<uint32_t> route;
    std::vector<uint32_t> corrected_route;
    std::vector<uint32_t> checklist;
    std::unordered_map<uint32_t, std::vector<uint32_t>> node_nearest_neighbor_map;
    uint32_t nn_list_size;
} local_search_info;


void parse_data(std::string filename, std::vector<local_search_info> &ls_info_list, std::vector<Point> &coordinates, std::vector<std::vector<uint32_t>> &nearest_neighbors)
{
	std::ifstream file(filename);
    std::string line;
    bool read_coordinates = false;
    bool read_neighbors = false;
    // std::vector<Point> coordinates;

    local_search_info ls_info = {};
    // std::vector<local_search_info> ls_info_list;
    while (std::getline(file, line)) {
        if (line.find("coordinates") != std::string::npos) {
            read_coordinates = true;
        } else if (line.find("New iteration") != std::string::npos) {
            break;
        } else if (line.find("get_nearest_neighbors") != std::string::npos) {
            // Process lines of style "get_nearest_neighbors of node 473: 474 472 475 943 469 476 468 477 471 944 470 942 478 949 467 465 945 940 941 948"
            std::stringstream ss(line);
            std::string token;
            ss >> token; // Discard "get_nearest_neighbors"
            ss >> token; // Discard "of"
            ss >> token; // Discard "node"
            uint32_t node;
            ss >> node; // Get node
            std::vector<uint32_t> nn_list;
            // Account for the colon
            ss >> token;
            while (ss >> token) {
                // std::cout << __LINE__ << " " << token << std::endl;
                nn_list.push_back(std::stoi(token));
            }
            nearest_neighbors.push_back(nn_list);
        } else if (read_coordinates) {
            int32_t x, y;
            std::stringstream ss(line);
            ss >> x >> y;
            coordinates.push_back(Point{x, y});
        }
    }
    
    while (std::getline(file, line)) {
        if (line.find("Done with iteration") != std::string::npos) {
            ls_info_list.push_back(ls_info);
            ls_info = {};
        } else if (line.find("ls_checklist") != std::string::npos) {
            // Process line of style "ls_checklist: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15"
            CHECKLIST_SIZE = 0;
            std::stringstream ss(line);
            std::string token;
            ss >> token; // Discard "ls_checklist:"
            while (ss >> token) {
                ls_info.checklist.push_back(std::stoi(token));
                CHECKLIST_SIZE++;
            }
        } else if (line.find("ant.route_ after two_opt_nn") != std::string::npos) {
            // Process line of style "ant.route_ after two_opt_nn: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15"
            std::stringstream ss(line);
            std::string token;
            ss >> token; // Discard "ant.route_"
            ss >> token; // Discard "after"
            ss >> token; // Discard "two_opt_nn:"
            while (ss >> token) {
                // std::cout << __LINE__ << " " << token << std::endl;
                ls_info.corrected_route.push_back(std::stoi(token));
            }
        } else if (line.find("ant.route_") != std::string::npos) {
            // Process line of style "ant.route_: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15"
            std::stringstream ss(line);
            std::string token;
            ss >> token; // Discard "ant.route_:"
            while (ss >> token) {
                // std::cout << __LINE__ << " " << token << std::endl;
                ls_info.route.push_back(std::stoi(token));
            }
        } else if (line.find("ls_cand_list_size_") != std::string::npos) {
            // Process line of style "ls_cand_list_size_: 20"
            std::stringstream ss(line);
            std::string token;
            ss >> token; // Discard "ls_cand_list_size_:"
            ss >> token;
            // std::cout << __LINE__ << " " << token << std::endl;
            ls_info.nn_list_size = std::stoi(token);
        } else if (line.find("get_nearest_neighbors") != std::string::npos) {
            // Process lines of style "get_nearest_neighbors of node 473: 474 472 475 943 469 476 468 477 471 944 470 942 478 949 467 465 945 940 941 948"
            std::stringstream ss(line);
            std::string token;
            ss >> token; // Discard "get_nearest_neighbors"
            ss >> token; // Discard "of"
            ss >> token; // Discard "node"
            uint32_t node;
            ss >> node; // Get node
            std::vector<uint32_t> nn_list;
            // Account for the colon
            ss >> token;
            while (ss >> token) {
                // std::cout << __LINE__ << " " << token << std::endl;
                nn_list.push_back(std::stoi(token));
            }
            ls_info.node_nearest_neighbor_map[node] = nn_list;
        }
    }
}

int main() {
      
    std::vector<local_search_info> ls_info_list;
    std::vector<Point> coordinates;
    std::vector<std::vector<uint32_t>> nearest_neighbors;
    parse_data("stats.txt", ls_info_list, coordinates, nearest_neighbors);
    
    int32_t coords[NODES][2];
    for (int i = 0; i < coordinates.size(); i++) {
        coords[i][0] = coordinates[i].x;
        coords[i][1] = coordinates[i].y;
    }
    uint32_t neighbors[NODES][NN_LIST_SIZE];
    for (int i = 0; i < NODES; i++) {
        std::copy(nearest_neighbors[i].begin(), nearest_neighbors[i].end(), neighbors[i]);
    }

    bool all_correct_distances = true;

    // auto time_start = chrono::steady_clock::now();
    for (int iter = 0; iter < ITERATIONS; iter++) {
        uint32_t route[NODES];
        std::copy(ls_info_list[iter].route.begin(), ls_info_list[iter].route.end(), route);
        uint32_t checklist[CHECKLIST_CAPACITY];
        std::copy(ls_info_list[iter].checklist.begin(), ls_info_list[iter].checklist.end(), checklist);
        uint32_t CHECKLIST_SIZE = ls_info_list[iter].checklist.size();
        
        double route_len = 0;
        for (int i = 1; i < NODES; i++) {
            route_len += get_distance(coordinates[route[i]], coordinates[route[i - 1]]);
        }
        std::cout << "Pre Two Opt route distance: " << route_len << std::endl;
        
        // auto time_start = chrono::steady_clock::now();
        two_opt_nn(coords, neighbors, route, checklist, iter == 0, CHECKLIST_SIZE);
        // auto time_end = chrono::steady_clock::now();
        // auto diff = time_end - time_start;
        // std::cout << "\nTime to run " << ITERATIONS << " iterations: " << chrono::duration <double, milli> (diff).count() << " ms" << std::endl;
        
        route_len = 0;
        for (int i = 1; i < NODES; i++) {
            route_len += get_distance(coordinates[route[i]], coordinates[route[i - 1]]);
        }
        std::cout << "Post Two Opt route distance: " << route_len << std::endl;
        double post_two_opt_route_len = route_len;
        
        route_len = 0;
        for (int i = 1; i < NODES; i++) {
            route_len += get_distance(coordinates[ls_info_list[iter].corrected_route[i]], coordinates[ls_info_list[iter].corrected_route[i - 1]]);
        }
        std::cout << "Correct Two Opt route distance: " << route_len << std::endl;

        if ((int)route_len != (int)post_two_opt_route_len) all_correct_distances = false;
    }
    // auto time_end = chrono::steady_clock::now();

    if (all_correct_distances) std::cout << "\n\033[1;32mPASSED ALL TESTS!\033[0m\n";
    else std::cout << "\n\033[1;31mFAILED :( - SOME TWO_OPT ROUTE LENGTH IS WRONG.\033[0m\n";

    // Store the time difference between start and end
    // auto diff = time_end - time_start;
    // std::cout << "\nTime to run " << ITERATIONS << " iterations: " << chrono::duration <double, milli> (diff).count() << " ms" << std::endl;
}