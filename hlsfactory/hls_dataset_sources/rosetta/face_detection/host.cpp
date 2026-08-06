#include <cstdio>
#include "face_detect.h"

int main() {
  unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH];
  for (int y = 0; y < IMAGE_HEIGHT; ++y)
    for (int x = 0; x < IMAGE_WIDTH; ++x)
      Data[y][x] = (unsigned char)((x + y * 3) & 0xff);

  int result_x[RESULT_SIZE];
  int result_y[RESULT_SIZE];
  int result_w[RESULT_SIZE];
  int result_h[RESULT_SIZE];
  int res_size = 0;

  // SDSoC host streams one row per call; final call runs the detector.
  for (int i = 0; i < IMAGE_HEIGHT - 1; ++i)
    face_detect(Data[i], result_x, result_y, result_w, result_h, &res_size);
  face_detect(Data[IMAGE_HEIGHT - 1], result_x, result_y, result_w, result_h, &res_size);

  std::printf("face_detect csim OK (res_size=%d)\n", res_size);
  return 0;
}
