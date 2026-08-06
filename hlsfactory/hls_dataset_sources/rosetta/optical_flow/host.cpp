#include <cstdio>
#include <cstring>
#include "optical_flow.h"

int main() {
  static frames_t frames[MAX_HEIGHT][MAX_WIDTH];
  static velocity_t outputs[MAX_HEIGHT][MAX_WIDTH];
  for (int r = 0; r < MAX_HEIGHT; ++r) {
    for (int c = 0; c < MAX_WIDTH; ++c) {
      // Pack 5 synthetic grayscale samples into the 64-bit frames word.
      frames_t v = 0;
      for (int f = 0; f < 5; ++f) {
        unsigned char pix = (unsigned char)((r + c + f * 17) & 0xff);
        v.range(8 * (f + 1) - 1, 8 * f) = pix;
      }
      frames[r][c] = v;
    }
  }
  optical_flow(frames, outputs);
  std::printf("optical_flow csim OK (vx=%f)\n", (double)outputs[0][0].x);
  return 0;
}
