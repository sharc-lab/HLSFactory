// Read text from a file and store results in vectors
// Text should be converted to int32_t

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

typedef struct {
    uint32_t x;
    uint32_t y;
} Point;

typedef struct {
    std::vector<uint32_t> route;
    std::vector<uint32_t> corrected_route;
    std::vector<uint32_t> checklist;
    std::unordered_map<uint32_t, std::vector<uint32_t>> node_nearest_neighbor_map;
    uint32_t nn_list_size;
} local_search_info;

// Read text from a file and store results in vectors

int main(int argc, char *argv[]) {
    std::string filename = argv[1];
    std::ifstream file(filename);
    std::string line;
    bool read_coordinates = false;

    std::vector<Point> coordinates;

    local_search_info ls_info = {};
    std::vector<local_search_info> ls_info_list;
    while (std::getline(file, line)) {
        if (line.find("coordinates") != std::string::npos) {
            read_coordinates = true;
        } else if (line.find("New iteration") != std::string::npos) {
            break;
        } else if (read_coordinates) {
            uint32_t x, y;
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
            std::stringstream ss(line);
            std::string token;
            ss >> token; // Discard "ls_checklist:"
            while (ss >> token) {
                ls_info.checklist.push_back(std::stoi(token));
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
    // Output coordinates
    for (auto &p : coordinates) {
        std::cout << p.x << " " << p.y << std::endl;
    }

    return 0;
}