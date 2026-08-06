#include <cstdio>
#include <cstdint>
#include "rendering.h"

int main() {
  bit32 input[3 * NUM_3D_TRI];
  bit32 output[NUM_FB];
  for (int i = 0; i < NUM_3D_TRI; ++i) {
    // Simple synthetic triangles inside the 256x256 viewport.
    bit8 x0 = (bit8)((i * 3) % 200);
    bit8 y0 = (bit8)((i * 5) % 200);
    bit8 z0 = (bit8)((i * 7) % 200);
    bit8 x1 = (bit8)((x0 + 20) % 256);
    bit8 y1 = (bit8)((y0 + 10) % 256);
    bit8 z1 = z0;
    bit8 x2 = (bit8)((x0 + 10) % 256);
    bit8 y2 = (bit8)((y0 + 25) % 256);
    bit8 z2 = z0;
    input[3 * i](7, 0) = x0;
    input[3 * i](15, 8) = y0;
    input[3 * i](23, 16) = z0;
    input[3 * i](31, 24) = x1;
    input[3 * i + 1](7, 0) = y1;
    input[3 * i + 1](15, 8) = z1;
    input[3 * i + 1](23, 16) = x2;
    input[3 * i + 1](31, 24) = y2;
    input[3 * i + 2](7, 0) = z2;
    input[3 * i + 2](31, 8) = 0;
  }
  rendering(input, output);
  std::printf("rendering csim OK (fb0=%u)\n", (unsigned)output[0]);
  return 0;
}
