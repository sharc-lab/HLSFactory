#include <stdio.h>
#include "dijkstra.h"
#include "input_matrix.h"

/* Bellman-Ford reference */
static int ref_dist(int s, int t)
{
    int d[DIJKSTRA_NUM_NODES];
    for (int i = 0; i < DIJKSTRA_NUM_NODES; i++)
        d[i] = DIJKSTRA_NONE;
    d[s] = 0;
    for (int it = 0; it < DIJKSTRA_NUM_NODES; it++)
        for (int u = 0; u < DIJKSTRA_NUM_NODES; u++) {
            if (d[u] == DIJKSTRA_NONE)
                continue;
            for (int v = 0; v < DIJKSTRA_NUM_NODES; v++)
                if (input_matrix[u][v] != DIJKSTRA_NONE &&
                    (d[v] == DIJKSTRA_NONE || d[u] + input_matrix[u][v] < d[v]))
                    d[v] = d[u] + input_matrix[u][v];
        }
    return d[t];
}

int main(void)
{
    static int dist[DIJKSTRA_NUM_QUERIES];
    static int path_len[DIJKSTRA_NUM_QUERIES];
    static int path[DIJKSTRA_NUM_QUERIES][DIJKSTRA_NUM_NODES];
    int errors = 0;

    dijkstra_queries(input_matrix, dist, path_len, path);

    for (int q = 0; q < DIJKSTRA_NUM_QUERIES; q++) {
        int s = q;
        int t = (DIJKSTRA_NUM_NODES / 2 + q) % DIJKSTRA_NUM_NODES;
        int exp = ref_dist(s, t);
        if (dist[q] != exp) {
            printf("query %d (%d->%d): dist %d expected %d\n", q, s, t, dist[q], exp);
            errors++;
            continue;
        }
        /* the reported path must start/end correctly and add up to the distance */
        int cost = 0;
        if (path_len[q] < 2 || path[q][0] != s || path[q][path_len[q] - 1] != t) {
            printf("query %d: malformed path\n", q);
            errors++;
            continue;
        }
        for (int k = 0; k + 1 < path_len[q]; k++)
            cost += input_matrix[path[q][k]][path[q][k + 1]];
        if (cost != dist[q]) {
            printf("query %d: path cost %d != dist %d\n", q, cost, dist[q]);
            errors++;
        }
    }

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS\n");
    return 0;
}
