// Auto-generated AutoNTT csim host (HLSFactory).
// Drive Pasta HLS stream shims (requires -D__SYNTHESIS__).
#include <tapa.h>
#include "ntt.h"
#include <cstdio>
#include <cstdint>

template <typename T>
static inline void hf_write(tapa::istream<T>& s, const T& v) {
  s._.write({v, false});
}

template <typename T>
static inline bool hf_read(tapa::ostream<T>& s, T& v) {
  if (s._.empty()) return false;
  v = s._.read().val;
  return true;
}

template <typename T>
static inline void hf_drain(tapa::ostream<T>& s) {
  T tmp;
  while (hf_read(s, tmp)) {
  }
}

template <typename T, int N>
static inline void hf_drain_arr(tapa::ostream<T> (&s)[N]) {
  for (int i = 0; i < N; ++i) hf_drain(s[i]);
}

void dual_interface_FIFO_receive(
    tapa::istream<WORD>& fwd_inVal,
    tapa::istream<WORD>& inv_inVal,
    tapa::ostream<WORD>& common_outVal,
    bool direction);

int main() {
  tapa::istream<WORD> fwd, inv;
  tapa::ostream<WORD> out;
  for (int i = 0; i < 8; ++i) hf_write(fwd, WORD(100 + i));
  dual_interface_FIFO_receive(fwd, inv, out, true);
  hf_drain(out);
  std::printf("dual_interface_FIFO_receive csim OK\n");
  return 0;
}
