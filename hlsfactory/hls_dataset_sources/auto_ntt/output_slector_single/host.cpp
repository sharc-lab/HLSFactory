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

void output_slector_single(
    tapa::istream<WORD>& fwd_valFromBU,
    tapa::istream<WORD>& inv_valFromLoad_0,
    tapa::istream<WORD>& inv_valFromShuf,
    tapa::ostream<WORD>& fwd_valToShuf,
    tapa::ostream<WORD>& fwd_valToStore_0,
    tapa::ostream<WORD>& inv_valToBU,
    bool direction,
    VAR_TYPE_16 iter);

int main() {
  tapa::istream<WORD> from_bu, from_load, from_shuf;
  tapa::ostream<WORD> to_shuf, to_store, to_bu;

  const int dataCount = N / V_TOTAL_DATA;
  const int dfLimit = (logN + (H_BUG_SIZE - 1)) / H_BUG_SIZE;
  for (int i = 0; i < dataCount * dfLimit; ++i) hf_write(from_bu, WORD(i + 7));

  output_slector_single(from_bu, from_load, from_shuf, to_shuf, to_store, to_bu,
                        true, /*iter=*/0);
  hf_drain(to_shuf);
  hf_drain(to_store);
  std::printf("output_slector_single csim OK\n");
  return 0;
}
