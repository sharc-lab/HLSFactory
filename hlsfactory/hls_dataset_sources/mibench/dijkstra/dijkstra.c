/*
 * MiBench network/dijkstra (dijkstra_small.c): single-pair shortest path
 * on a dense adjacency matrix using a FIFO work queue (label-correcting
 * Dijkstra variant).
 *
 * HLS adaptation: the malloc'ed linked-list queue is a circular buffer,
 * the global state is local, file input and printf are removed, and
 * the recursive print_path is replaced by an iterative path reconstruction.
 */
#include "dijkstra.h"

#define NUM_NODES DIJKSTRA_NUM_NODES
#define NONE DIJKSTRA_NONE
#define QCAP 1024 /* power of two; peak occupancy on the benchmark input is 315 */

typedef struct {
    int iDist;
    int iPrev;
} NODE;

static int dijkstra(const int adj[NUM_NODES][NUM_NODES], int chStart, int chEnd,
                    int *path_len, int path[NUM_NODES])
{
    NODE rgnNodes[NUM_NODES];
    int q_node[QCAP];
    int q_dist[QCAP];
    int q_head = 0, q_tail = 0, q_count = 0;

    for (int ch = 0; ch < NUM_NODES; ch++) {
        rgnNodes[ch].iDist = NONE;
        rgnNodes[ch].iPrev = NONE;
    }

    if (chStart != chEnd) {
        rgnNodes[chStart].iDist = 0;
        rgnNodes[chStart].iPrev = NONE;
        q_node[q_tail] = chStart;
        q_dist[q_tail] = 0;
        q_tail = (q_tail + 1) & (QCAP - 1);
        q_count++;

        while (q_count > 0) {
            int iNode = q_node[q_head];
            int iDist = q_dist[q_head];
            q_head = (q_head + 1) & (QCAP - 1);
            q_count--;

            for (int i = 0; i < NUM_NODES; i++) {
                int iCost = adj[iNode][i];
                if (iCost != NONE) {
                    if ((NONE == rgnNodes[i].iDist) ||
                        (rgnNodes[i].iDist > (iCost + iDist))) {
                        rgnNodes[i].iDist = iDist + iCost;
                        rgnNodes[i].iPrev = iNode;
                        q_node[q_tail] = i;
                        q_dist[q_tail] = iDist + iCost;
                        q_tail = (q_tail + 1) & (QCAP - 1);
                        q_count++;
                    }
                }
            }
        }
    } else {
        rgnNodes[chStart].iDist = 0;
    }

    /* walk the predecessor chain back from the end node, then reverse it */
    int len = 0;
    int tmp[NUM_NODES];
    if (rgnNodes[chEnd].iDist != NONE) {
        int n = chEnd;
        for (int k = 0; k < NUM_NODES; k++) {
            tmp[len++] = n;
            if (rgnNodes[n].iPrev == NONE)
                break;
            n = rgnNodes[n].iPrev;
        }
    }
    for (int k = 0; k < NUM_NODES; k++) {
        if (k < len)
            path[k] = tmp[len - 1 - k];
    }
    *path_len = len;

    return rgnNodes[chEnd].iDist;
}

void dijkstra_queries(const int adj[DIJKSTRA_NUM_NODES][DIJKSTRA_NUM_NODES],
                      int dist[DIJKSTRA_NUM_QUERIES],
                      int path_len[DIJKSTRA_NUM_QUERIES],
                      int path[DIJKSTRA_NUM_QUERIES][DIJKSTRA_NUM_NODES])
{
    for (int q = 0; q < DIJKSTRA_NUM_QUERIES; q++) {
        int start = q;
        int end = (NUM_NODES / 2 + q) % NUM_NODES;
        int len = 0;
        dist[q] = dijkstra(adj, start, end, &len, path[q]);
        path_len[q] = len;
    }
}
