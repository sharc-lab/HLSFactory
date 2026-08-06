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

void BU(
    tapa::istream<WORD>& fwd_inVal0,
    tapa::istream<WORD>& fwd_inVal1,
    tapa::istream<WORD>& inv_inVal0,
    tapa::istream<WORD>& inv_inVal1,
    tapa::istream<WORD>& tf_inVal,
    tapa::ostream<WORD>& fwd_outVal0,
    tapa::ostream<WORD>& fwd_outVal1,
    tapa::ostream<WORD>& inv_outVal0,
    tapa::ostream<WORD>& inv_outVal1,
    WORD q,
    WORD twoInverse,
    WORD_PLUS3 factor,
    bool direction,
    VAR_TYPE_16 iter,
    VAR_TYPE_16 BUG_id,
    VAR_TYPE_16 layer_id
);

int main() {
  tapa::istream<WORD> fwd0, fwd1, inv0, inv1, tf;
  tapa::ostream<WORD> fwd_o0, fwd_o1, inv_o0, inv_o1;

  // iter=0 => 64 * 3 = 192 butterfly steps (direction=fwd)
  const int beats = (N / V_TOTAL_DATA) * ((logN + (H_BUG_SIZE - 1)) / H_BUG_SIZE);
  for (int i = 0; i < beats; ++i) {
    hf_write(fwd0, WORD(i + 1));
    hf_write(fwd1, WORD(i + 3));
    hf_write(tf, WORD(1));
  }

  WORD q = WORD(0x3b800001);  // sample Barrett modulus-shaped constant
  WORD twoInverse = WORD(0x1dc00001);
  WORD_PLUS3 factor = WORD_PLUS3(1);

  BU(fwd0, fwd1, inv0, inv1, tf, fwd_o0, fwd_o1, inv_o0, inv_o1,
     q, twoInverse, factor, /*direction=*/true, /*iter=*/0, /*BUG_id=*/0, /*layer_id=*/0);

  hf_drain(fwd_o0);
  hf_drain(fwd_o1);
  std::printf("BU csim OK (%d beats)\n", beats);
  return 0;
}
