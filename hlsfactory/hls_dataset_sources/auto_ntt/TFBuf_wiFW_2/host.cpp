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

void TFBuf_wiFW_2(
    tapa::istream<DWORD>& loadTFStream,
    tapa::ostream<DWORD>& loadTFToNext,
    tapa::ostream<WORD>& tfToBu_0,
    tapa::ostream<WORD>& tfToBu_1,
    bool direction,
    VAR_TYPE_8 BUG_id,
    VAR_TYPE_8 TFGen_offset,
    VAR_TYPE_8 unit_id,
    VAR_TYPE_16 iter);

int main() {
  tapa::istream<DWORD> load;
  tapa::ostream<WORD> tf0, tf1;
  tapa::ostream<DWORD> next;
  for (int i = 0; i < 138; ++i) {
    DWORD v = DWORD(i) | (DWORD(i + 1) << WORD_SIZE);
    hf_write(load, v);
  }

  TFBuf_wiFW_2(load, next, tf0, tf1,
         true, /*BUG_id=*/0, /*TFGen_offset=*/0, /*unit_id=*/0, /*iter=*/0);
  hf_drain(next);
  hf_drain(tf0);
  hf_drain(tf1);
  std::printf("TFBuf_wiFW_2 csim OK\n");
  return 0;
}
