#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#define DIJKSTRA_NUM_NODES 100
#define DIJKSTRA_NONE 9999 /* "no edge" / "no path" marker */
#define DIJKSTRA_NUM_QUERIES 20

/*
 * Runs the MiBench shortest path search for DIJKSTRA_NUM_QUERIES (start, end)
 * pairs: start = i and end = (NUM_NODES/2 + i) % NUM_NODES.  For each query,
 * dist[q] is the cost of the shortest path (DIJKSTRA_NONE if unreachable),
 * path_len[q] the number of nodes on it and path[q][0..path_len[q]-1] the
 * nodes from start to end.
 */
void dijkstra_queries(const int adj[DIJKSTRA_NUM_NODES][DIJKSTRA_NUM_NODES],
                      int dist[DIJKSTRA_NUM_QUERIES],
                      int path_len[DIJKSTRA_NUM_QUERIES],
                      int path[DIJKSTRA_NUM_QUERIES][DIJKSTRA_NUM_NODES]);

#endif
