#ifndef HLS_VIDEO_H
#define HLS_VIDEO_H
// Minimal stand-in for the removed Xilinx hls_video.h (Window / LineBuffer).
// Sufficient for Rosetta optical_flow / BNN under Vitis HLS 2024+.

namespace hls {

template <int ROWS, int COLS, typename T>
class Window {
 public:
  T val[ROWS][COLS];

  Window() {
    for (int r = 0; r < ROWS; ++r)
      for (int c = 0; c < COLS; ++c) val[r][c] = T();
  }

  void shift_pixels_left() {
#pragma HLS inline
    for (int r = 0; r < ROWS; ++r)
#pragma HLS unroll
      for (int c = 0; c < COLS - 1; ++c)
#pragma HLS unroll
        val[r][c] = val[r][c + 1];
  }

  void insert_pixel(T v, int row, int col) {
#pragma HLS inline
    val[row][col] = v;
  }

  T getval(int row, int col) const {
#pragma HLS inline
    return val[row][col];
  }
};

template <int ROWS, int COLS, typename T>
class LineBuffer {
 public:
  T val[ROWS][COLS];

  LineBuffer() {
    for (int r = 0; r < ROWS; ++r)
      for (int c = 0; c < COLS; ++c) val[r][c] = T();
  }

  void shift_pixels_up(int col) {
#pragma HLS inline
    for (int r = 0; r < ROWS - 1; ++r)
#pragma HLS unroll
      val[r][col] = val[r + 1][col];
  }

  void insert_bottom_row(T v, int col) {
#pragma HLS inline
    val[ROWS - 1][col] = v;
  }

  T getval(int row, int col) const {
#pragma HLS inline
    return val[row][col];
  }
};

}  // namespace hls

#endif  // HLS_VIDEO_H
