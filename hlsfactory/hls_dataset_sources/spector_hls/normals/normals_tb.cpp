#include "params.h"
#include <stdio.h>

int main() {
    int vmap[rows * cols * 3];
    int nmap[rows * cols * 3];
    for (int i = 0; i < rows * cols * 3; i++)
        vmap[i] = 1;
    for (int i = 0; i < rows * cols * 3; i++)
        nmap[i] = 0;
    normals(vmap, nmap);
    printf("normals csim done\n");
    return 0;
}
