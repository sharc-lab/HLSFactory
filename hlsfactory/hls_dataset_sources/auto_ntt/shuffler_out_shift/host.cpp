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

void shuffler_out_shift(
    tapa::istreams<WORD, 2*V_BUG_SIZE>& fwd_inVal,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_inVal_inter_0,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_inVal_intra,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_outVal_inter_0,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_outVal_intra,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& inv_outVal,
    bool direction,
    VAR_TYPE_16 iter,
    VAR_TYPE_16 task_id);

int main() {
  tapa::istream<WORD> fwd_in[2 * V_BUG_SIZE];
  tapa::istream<WORD> inv_inter[2 * V_BUG_SIZE];
  tapa::istream<WORD> inv_intra[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd_inter[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd_intra[2 * V_BUG_SIZE];
  tapa::ostream<WORD> inv_out[2 * V_BUG_SIZE];

  const int dataCount = N / V_TOTAL_DATA;
  const int shuffleLimit = ((logN + (H_BUG_SIZE - 1)) / H_BUG_SIZE) - 1;
  const int beats = dataCount * shuffleLimit;
  for (int i = 0; i < beats; ++i) {
    for (int j = 0; j < 2 * V_BUG_SIZE; ++j) {
      hf_write(fwd_in[j], WORD(i * 16 + j + 1));
    }
  }

  shuffler_out_shift(fwd_in, inv_inter, inv_intra, fwd_inter, fwd_intra, inv_out,
                     true, /*iter=*/0, /*task_id=*/0);
  hf_drain_arr(fwd_inter);
  hf_drain_arr(fwd_intra);
  std::printf("shuffler_out_shift csim OK\n");
  return 0;
}
