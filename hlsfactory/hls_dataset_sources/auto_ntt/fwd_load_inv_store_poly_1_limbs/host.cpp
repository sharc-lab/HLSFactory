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

void fwd_load_inv_store_poly_1_limbs(
    tapa::async_mmap<POLY_WIDE_DATA>& poly_mmap,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_in_poly_L0_stream0,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_in_poly_L0_stream1,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_in_poly_L0_stream2,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_in_poly_L0_stream3,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_out_poly_L0_stream0,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_out_poly_L0_stream1,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_out_poly_L0_stream2,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_out_poly_L0_stream3,
    bool direction,
    VAR_TYPE_16 iter);

int main() {
  tapa::async_mmap<POLY_WIDE_DATA> poly_mmap;
  tapa::istream<WORD> inv0[2 * V_BUG_SIZE], inv1[2 * V_BUG_SIZE];
  tapa::istream<WORD> inv2[2 * V_BUG_SIZE], inv3[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd0[2 * V_BUG_SIZE], fwd1[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd2[2 * V_BUG_SIZE], fwd3[2 * V_BUG_SIZE];

  const int totalDataCount = (N / V_TOTAL_DATA) * SEQ_BUG_PER_PARA_LIMB_POLY_PORT;
  for (int i = 0; i < totalDataCount; ++i) {
    hf_write(poly_mmap.read_data, POLY_WIDE_DATA(i + 1));
  }

  fwd_load_inv_store_poly_1_limbs(
      poly_mmap, inv0, inv1, inv2, inv3, fwd0, fwd1, fwd2, fwd3,
      /*direction=*/true, /*iter=*/0);

  hf_drain_arr(fwd0);
  hf_drain_arr(fwd1);
  hf_drain_arr(fwd2);
  hf_drain_arr(fwd3);
  {
    tapa::async_mmap<POLY_WIDE_DATA>::addr_t a;
    while (hf_read(poly_mmap.read_addr, a)) {
    }
  }
  std::printf("fwd_load_inv_store_poly_1_limbs csim OK\n");
  return 0;
}
