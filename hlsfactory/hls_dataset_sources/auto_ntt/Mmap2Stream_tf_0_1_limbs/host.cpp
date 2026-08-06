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

void Mmap2Stream_tf_0_1_limbs(
    tapa::async_mmap<TF_WIDE_DATA>& tf_mmap,
    tapa::ostreams<DWORD, TF_CONCAT_FACTOR_PER_PARA_LIMB_PORT/2>& tf_stream_L0_0,
    bool direction,
    VAR_TYPE_16 iter);

int main() {
  tapa::async_mmap<TF_WIDE_DATA> tf_mmap;
  tapa::ostream<DWORD> tf_stream[TF_CONCAT_FACTOR_PER_PARA_LIMB_PORT / 2];

  const int total_depth = 242;  // fwd_total_depth in kernel
  for (int i = 0; i < total_depth; ++i) {
    TF_WIDE_DATA v = TF_WIDE_DATA(i + 1);
    hf_write(tf_mmap.read_data, v);
  }

  Mmap2Stream_tf_0_1_limbs(tf_mmap, tf_stream, true, /*iter=*/0);
  hf_drain_arr(tf_stream);
  // Drain addresses the kernel posted
  {
    tapa::async_mmap<TF_WIDE_DATA>::addr_t a;
    while (hf_read(tf_mmap.read_addr, a)) {
    }
  }
  std::printf("Mmap2Stream_tf_0_1_limbs csim OK\n");
  return 0;
}
