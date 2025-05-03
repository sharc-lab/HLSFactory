#include <ap_int.h>
#include <chrono>
#include <stdint.h>
#include <stdio.h>

#ifdef BIT16
#include "16.h"
#elif BIT32
#include "32.h"
#else
#include "64.h"
#endif

class Timer {
public:
  void start() { m_startTime = std::chrono::steady_clock::now(); }

  double elapsed() {
    auto endTime = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::micro> duration = endTime - m_startTime;
    return duration.count();
  }

private:
  std::chrono::steady_clock::time_point m_startTime;
};

void test_modmul() {
  spint x_vals[20] = {0x199d, 0x1d8b, 0x1fe8, 0x62,   0x11c8, 0xd68,  0x199c,
                      0x1f60, 0x1640, 0x1dee, 0x1495, 0xcd0,  0x1904, 0x189d,
                      0x18fa, 0xadb,  0x1924, 0x7a1,  0x1718, 0x23};
  spint y_vals[20] = {0xa9,   0x44b,  0x9ef,  0x1a3f, 0x11a3, 0x16b6, 0x14f0,
                      0x1675, 0x1c66, 0x5ec,  0x1854, 0x8cb,  0x34e,  0xe92,
                      0xa2e,  0x13f0, 0x1641, 0x11a0, 0x550,  0x97};

  spint x[20];
  spint y[20];
  spint z[20];

  for (int i = 0; i < Nlimbs; i++) {
    x[i] = x_vals[i];
    y[i] = y_vals[i];
  }

  Timer timer;
  timer.start();

//   for (int i = 0; i < 100000; i++)
    // for (int j = 0; j < 200; j++) {
      modmul(x, y, z);
    //   modmul(z, x, y);
    //   modmul(y, z, x);
    //   modmul(x, y, z);
    //   modmul(z, x, y);
    // }
  auto elapsed = timer.elapsed();
  printf("Time taken for modmul: %.8f us\n", elapsed);

  printf("modmul check 0x%06x\n", (int)z[0] & 0xFFFFFF);
}

int main() {

#ifdef BIT16
  printf("16-bit multiplication\n");
#elif BIT32
  printf("32-bit multiplication\n");
#else
  printf("64-bit multiplication\n");
#endif

  test_modmul();
  return 0;
}
