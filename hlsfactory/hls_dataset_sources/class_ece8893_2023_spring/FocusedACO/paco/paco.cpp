#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <list>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "../FACO/src/problem_instance.h"

using namespace std;

// Define constants
const int N = 50; // Number of nodes
const int K = 10; // Size of population matrix
const int maxIterations = 500; // Maximum number of iterations
const double Q = 20.0; // Pheromone deposit amount
const int initPheromone = 10; // Initial pheromone strength
const int maxWeight = 100; // Maximum weight for edge weights
const int randomSeed = 12345; // Seed for random number generator

// Define struct to represent a node
struct Node {
    double x;
    double y;
};

// Define struct to represent an edge
struct Edge {
    int start;
    int end;
    int weight;
    double pheromone;
};

struct Neighbor {
    uint32_t index;
    double pheromone;
    bool selected;
};

// Define function to calculate distance between two nodes
double distance(Node a, Node b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

void generate_pheromone_matrix(vector<vector<Neighbor>>& pheromones, list<vector<uint32_t>>& population_matrix, vector<uint32_t>& best_path) {
    // Initialize all values in the pheromone matrix to initial_pheromone
    const int NUM_NODES = pheromones.size();
    const int nn_nodes = pheromones[0].size();
    for (int i = 0; i < NUM_NODES; i++) {
        for (int j = 0; j < nn_nodes; j++) {
            pheromones[i][j].pheromone = initPheromone;
            pheromones[i][j].selected = false;
        }
    }
    // Update the pheromone matrix based on the population matrix
    for (auto ant : population_matrix) {
        // double contribution = Q / cost;
        double contribution = Q;
        for (int i = 0; i < NUM_NODES - 1; i++) {
            // Broadcast the population matrix to the pheromone matrix by searching for the neighbor
            for (int j = 0; j < nn_nodes; j++) {
                if (pheromones[ant[i]][j].index == ant[i + 1]) {
                    pheromones[ant[i]][j].pheromone += contribution;
                    pheromones[ant[i]][j].selected = true;
                    break;
                }
            }

            for (int j = 0; j < nn_nodes; j++) {
                if (pheromones[ant[i + 1]][j].index == ant[i]) {
                    pheromones[ant[i + 1]][j].pheromone += contribution;
                    pheromones[ant[i + 1]][j].selected = true;
                    break;
                }
            }
        }
    }
    // Update the pheromone matrix based on the best path
    if (best_path.size() == 0) {
        return;
    }
    double contribution = Q * 2;
    for (int i = 0; i < best_path.size() - 1; i++) {
        // Broadcast the population matrix to the pheromone matrix by searching for the neighbor
        for (int j = 0; j < nn_nodes - 1; j++) {
            if (pheromones[best_path[i]][j].index == best_path[i + 1]) {
                pheromones[best_path[i]][j].pheromone += contribution;
                // pheromones[ant[i]][j].selected = true;
                break;
            }
        }

        for (int j = 0; j < nn_nodes - 1; j++) {
            if (pheromones[best_path[i + 1]][j].index == best_path[i]) {
                pheromones[best_path[i + 1]][j].pheromone += contribution;
                // pheromones[ant[i + 1]][j].selected = true;
                break;
            }
        }
    }
}

// Helper function to select the next node based on the edge weights
uint32_t select_next_node(int current_node, std::vector<uint32_t>& selected_nodes, std::vector<std::vector<Neighbor>>& pheromones, ProblemInstance& problem)
{
    const int NUM_NODES = pheromones.size();
    const int nn_nodes = pheromones[0].size();
    // Calculate the denominator of the probability function
    double denominator = 0.0;
    // for (int i = 0; i < nn_nodes; i++) {
    //     if (!pheromones[current_node][i].selected) {
    //         denominator += pheromones[current_node][i].pheromone;
    //     }
    // }
    // Calculate the probability of each node
    std::vector<double> probabilities(nn_nodes);
    for (int i = 0; i < nn_nodes; i++) {
        if (find(selected_nodes.begin(), selected_nodes.end(), pheromones[current_node][i].index) == selected_nodes.end()) {
            denominator += pheromones[current_node][i].pheromone;
            probabilities[i] = pheromones[current_node][i].pheromone;
        }
        else {
            probabilities[i] = 0.0;
        }
    }

    if (denominator == 0.0) {
        int i = 0;
        while (true) {
            uint32_t index = problem.all_nearest_neighbors_[current_node * problem.total_nn_per_node_ + nn_nodes + i++];
            if (find(selected_nodes.begin(), selected_nodes.end(), index) == selected_nodes.end()) {
                if (index >= problem.dimension_)
                    exit(1);
                return index;
            }
        }
    }
    // Normalize the probabilities
    for (int i = 0; i < nn_nodes; i++) {
        probabilities[i] /= denominator;
    }

    // Select the next node
    double random = (double)rand() / RAND_MAX;
    double sum = 0.0;
    for (int i = 0; i < nn_nodes; i++) {
        sum += probabilities[i];
        if (random < sum) {
            // Go through all Neighbors for all nodes to mark the node with the index as selected
            return pheromones[current_node][i].index;
        }
    }
    return -1;
}

void find_best_ant(vector<vector<uint32_t>>& ants, ProblemInstance& problem, double& cost, int& best_ant) {
    const int NUM_ANTS = ants.size();
    const int NUM_NODES = ants[0].size();
    // Find the best ant
    best_ant = 0;
    cost = numeric_limits<double>::max();
    for (int ant = 0; ant < NUM_ANTS; ant++) {
        double curr_cost = 0.0;
        curr_cost = problem.calculate_route_length(ants[ant]);
        if (curr_cost < cost) {
            cost = curr_cost;
            best_ant = ant;
        }
    }
}

int main(int argc, char** argv) {
    // Graph initialization
    cout << "Initializing graph from file: " << argv[1] << endl;

    ProblemInstance problem = load_tsplib_instance(argv[1]);
    int nn_count = 20;
    problem.compute_nn_lists(problem.dimension_);
    // Parameters
    int num_ants = 20;
    int num_iterations = maxIterations;

    // Best solution initialization
    // vector<uint32_t> best_path;
    list<vector<uint32_t>> population_matrix;
    double best_cost = numeric_limits<double>::max();
    double curr_cost = 0;
    int best_ant = 0;

    // vector<vector<double>> distances = read_tsp_file(argv[1]);
    int num_nodes = problem.dimension_;
    cout << "Number of nodes: " << num_nodes << endl;
    // Pheromone initialization
    vector<vector<Neighbor>> pheromones(num_nodes, vector<Neighbor>(nn_count, {0, 0.0, false}));
    for (int i = 0; i < num_nodes; i++) {
        // Get the nearest neighbors for each node
        NodeList neighbors = problem.get_nearest_neighbors(i, nn_count);
        // Print the neighbors
        // for (int j = 0; j < nn_count; j++) {
        //     cout << neighbors[j] << " ";
        // }
        // cout << endl;
        for (int j = 0; j < nn_count; j++) {
            pheromones[i][j].index = neighbors[j];
        }
    }

    std::vector<std::vector<uint32_t>> routes;
    for (int i = 0; i < num_ants; i++) {
        routes.push_back(problem.build_nn_tour(rand() % num_nodes));
    }

    // Pick best loop and set as best path
    // int best_ant = 0;
    for (int i = 0; i < num_ants; i++) {
        double curr_cost = problem.calculate_route_length(routes[i]);
        if (curr_cost < best_cost) {
            best_cost = curr_cost;
            best_ant = i;
        }
    }

    cout << "Best cost: " << best_cost << endl;
    vector<uint32_t> best_path(routes[best_ant].begin(), routes[best_ant].end());

    // Main loop
    for (int it = 0; it < num_iterations; it++) {
        // Ants initialization
        vector<vector<uint32_t>> ants(num_ants, vector<uint32_t>(num_nodes));

        // Generate population matrix
        generate_pheromone_matrix(pheromones, population_matrix, best_path);

        // Ants construction
        for (int ant = 0; ant < num_ants; ant++) {
            vector<uint32_t> path;;

            // Choose a random starting node
            int start_node = rand() % num_nodes;
            path.push_back(start_node);

            // Build the rest of the path
            for (int i = 1; i < num_nodes - 1; i++) {
                int next_node = select_next_node(path[i - 1], path, pheromones, problem);
                path.push_back(next_node);
            }

            // Find the last node
            int last_node = 0;
            for (int i = 0; i < num_nodes; i++) {
                if (find(path.begin(), path.end(), i) == path.end()) {
                    last_node = i;
                    break;
                }
            }
            path.push_back(last_node);

            // Save the ant's path
            ants[ant] = path;
        }

        find_best_ant(ants, problem, curr_cost, best_ant);

        // Update the best solution
        if (curr_cost < best_cost || it == 0) {
            best_path = ants[best_ant];
            best_cost = curr_cost;
        } else {
            // Add the best ant's path to the population matrix
            // If the population matrix has K paths, remove the least recent path
            if (population_matrix.size() == K) {
                population_matrix.pop_front();
            }
            population_matrix.push_back(ants[best_ant]);
        }

        // cout << "Iteration " << it << " best cost: " << best_cost << endl;
        cout << "Iteration " << it << " best cost: " << best_cost;
        cout << " best ant: " << best_ant << " ";//endl;
        // Print out distances of the routes in the population matrix
        for (auto route : population_matrix) {
            cout << problem.calculate_route_length(route) << " ";
        }
        cout << endl;
    }

    // Print the best solution
    cout << "Best path: ";
    for (int i = 0; i < num_nodes; i++) {
        cout << best_path[i] << " ";
    }
    cout << endl;
    cout << "Best cost: " << best_cost << endl;

    return 0;
}