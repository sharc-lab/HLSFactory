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

void input_selector_single(
    tapa::istream<WORD>& fwd_valFromLoad_0,
    tapa::istream<WORD>& fwd_valFromShuf,
    tapa::istream<WORD>& inv_valFromBU,
    tapa::ostream<WORD>& fwd_valToBU,
    tapa::ostream<WORD>& inv_valToShuf,
    tapa::ostream<WORD>& inv_valToStore_0,
    bool direction,
    VAR_TYPE_16 iter);

int main() {
  tapa::istream<WORD> from_load, from_shuf, from_bu;
  tapa::ostream<WORD> to_bu, to_shuf, to_store;

  const int dataCount = N / V_TOTAL_DATA;
  const int dfLimit = (logN + (H_BUG_SIZE - 1)) / H_BUG_SIZE;
  // dataFlowIter==0 reads load; later iters read shuf
  for (int i = 0; i < dataCount; ++i) hf_write(from_load, WORD(i + 1));
  for (int i = 0; i < dataCount * (dfLimit - 1); ++i) hf_write(from_shuf, WORD(1000 + i));

  input_selector_single(from_load, from_shuf, from_bu, to_bu, to_shuf, to_store,
                        true, /*iter=*/0);
  hf_drain(to_bu);
  std::printf("input_selector_single csim OK\n");
  return 0;
}
