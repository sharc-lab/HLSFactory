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

void shuffler_out_shuff(
    tapa::istream<WORD>& fwd_inVal_inter0_0,
    tapa::istream<WORD>& fwd_inVal_inter0_1,
    tapa::istream<WORD>& fwd_inVal_inter0_2,
    tapa::istream<WORD>& fwd_inVal_inter0_3,
    tapa::istream<WORD>& fwd_inVal_inter0_4,
    tapa::istream<WORD>& fwd_inVal_inter0_5,
    tapa::istream<WORD>& fwd_inVal_inter0_6,
    tapa::istream<WORD>& fwd_inVal_inter0_7,
    tapa::istream<WORD>& fwd_inVal_inter0_8,
    tapa::istream<WORD>& fwd_inVal_inter0_9,
    tapa::istream<WORD>& fwd_inVal_inter0_10,
    tapa::istream<WORD>& fwd_inVal_inter0_11,
    tapa::istream<WORD>& fwd_inVal_inter0_12,
    tapa::istream<WORD>& fwd_inVal_inter0_13,
    tapa::istream<WORD>& fwd_inVal_inter0_14,
    tapa::istream<WORD>& fwd_inVal_inter0_15,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& fwd_inVal_intra,
    tapa::istreams<WORD, 2*V_BUG_SIZE>& inv_inVal,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& fwd_outVal,
    tapa::ostreams<WORD, 2*V_BUG_SIZE>& inv_outVal_intra,
    tapa::ostream<WORD>& inv_outVal_inter0_0,
    tapa::ostream<WORD>& inv_outVal_inter0_1,
    tapa::ostream<WORD>& inv_outVal_inter0_2,
    tapa::ostream<WORD>& inv_outVal_inter0_3,
    tapa::ostream<WORD>& inv_outVal_inter0_4,
    tapa::ostream<WORD>& inv_outVal_inter0_5,
    tapa::ostream<WORD>& inv_outVal_inter0_6,
    tapa::ostream<WORD>& inv_outVal_inter0_7,
    tapa::ostream<WORD>& inv_outVal_inter0_8,
    tapa::ostream<WORD>& inv_outVal_inter0_9,
    tapa::ostream<WORD>& inv_outVal_inter0_10,
    tapa::ostream<WORD>& inv_outVal_inter0_11,
    tapa::ostream<WORD>& inv_outVal_inter0_12,
    tapa::ostream<WORD>& inv_outVal_inter0_13,
    tapa::ostream<WORD>& inv_outVal_inter0_14,
    tapa::ostream<WORD>& inv_outVal_inter0_15,
    bool direction,
    VAR_TYPE_16 iter);

int main() {
  tapa::istream<WORD> fwd_inter0;
  tapa::istream<WORD> fwd_inter1;
  tapa::istream<WORD> fwd_inter2;
  tapa::istream<WORD> fwd_inter3;
  tapa::istream<WORD> fwd_inter4;
  tapa::istream<WORD> fwd_inter5;
  tapa::istream<WORD> fwd_inter6;
  tapa::istream<WORD> fwd_inter7;
  tapa::istream<WORD> fwd_inter8;
  tapa::istream<WORD> fwd_inter9;
  tapa::istream<WORD> fwd_inter10;
  tapa::istream<WORD> fwd_inter11;
  tapa::istream<WORD> fwd_inter12;
  tapa::istream<WORD> fwd_inter13;
  tapa::istream<WORD> fwd_inter14;
  tapa::istream<WORD> fwd_inter15;
  tapa::istream<WORD> fwd_intra[2 * V_BUG_SIZE];
  tapa::istream<WORD> inv_in[2 * V_BUG_SIZE];
  tapa::ostream<WORD> fwd_out[2 * V_BUG_SIZE];
  tapa::ostream<WORD> inv_intra[2 * V_BUG_SIZE];
  tapa::ostream<WORD> inv_inter0;
  tapa::ostream<WORD> inv_inter1;
  tapa::ostream<WORD> inv_inter2;
  tapa::ostream<WORD> inv_inter3;
  tapa::ostream<WORD> inv_inter4;
  tapa::ostream<WORD> inv_inter5;
  tapa::ostream<WORD> inv_inter6;
  tapa::ostream<WORD> inv_inter7;
  tapa::ostream<WORD> inv_inter8;
  tapa::ostream<WORD> inv_inter9;
  tapa::ostream<WORD> inv_inter10;
  tapa::ostream<WORD> inv_inter11;
  tapa::ostream<WORD> inv_inter12;
  tapa::ostream<WORD> inv_inter13;
  tapa::ostream<WORD> inv_inter14;
  tapa::ostream<WORD> inv_inter15;

  const int dataCount = N / V_TOTAL_DATA;
  // Stage 0 reads inter ports; stage 1 reads intra.
  for (int i = 0; i < dataCount; ++i) {
      hf_write(fwd_inter0, WORD(i * 16 + 0 + 1));
      hf_write(fwd_inter1, WORD(i * 16 + 1 + 1));
      hf_write(fwd_inter2, WORD(i * 16 + 2 + 1));
      hf_write(fwd_inter3, WORD(i * 16 + 3 + 1));
      hf_write(fwd_inter4, WORD(i * 16 + 4 + 1));
      hf_write(fwd_inter5, WORD(i * 16 + 5 + 1));
      hf_write(fwd_inter6, WORD(i * 16 + 6 + 1));
      hf_write(fwd_inter7, WORD(i * 16 + 7 + 1));
      hf_write(fwd_inter8, WORD(i * 16 + 8 + 1));
      hf_write(fwd_inter9, WORD(i * 16 + 9 + 1));
      hf_write(fwd_inter10, WORD(i * 16 + 10 + 1));
      hf_write(fwd_inter11, WORD(i * 16 + 11 + 1));
      hf_write(fwd_inter12, WORD(i * 16 + 12 + 1));
      hf_write(fwd_inter13, WORD(i * 16 + 13 + 1));
      hf_write(fwd_inter14, WORD(i * 16 + 14 + 1));
      hf_write(fwd_inter15, WORD(i * 16 + 15 + 1));
  }
  for (int i = 0; i < dataCount; ++i) {
    for (int j = 0; j < 2 * V_BUG_SIZE; ++j) {
      hf_write(fwd_intra[j], WORD(5000 + i * 16 + j));
    }
  }

  shuffler_out_shuff(
      fwd_inter0, fwd_inter1, fwd_inter2, fwd_inter3, fwd_inter4, fwd_inter5, fwd_inter6, fwd_inter7, fwd_inter8, fwd_inter9, fwd_inter10, fwd_inter11, fwd_inter12, fwd_inter13, fwd_inter14, fwd_inter15,
      fwd_intra, inv_in, fwd_out, inv_intra,
      inv_inter0, inv_inter1, inv_inter2, inv_inter3, inv_inter4, inv_inter5, inv_inter6, inv_inter7, inv_inter8, inv_inter9, inv_inter10, inv_inter11, inv_inter12, inv_inter13, inv_inter14, inv_inter15,
      true, /*iter=*/0);

  hf_drain_arr(fwd_out);
  hf_drain(inv_inter0);
  hf_drain(inv_inter1);
  hf_drain(inv_inter2);
  hf_drain(inv_inter3);
  hf_drain(inv_inter4);
  hf_drain(inv_inter5);
  hf_drain(inv_inter6);
  hf_drain(inv_inter7);
  hf_drain(inv_inter8);
  hf_drain(inv_inter9);
  hf_drain(inv_inter10);
  hf_drain(inv_inter11);
  hf_drain(inv_inter12);
  hf_drain(inv_inter13);
  hf_drain(inv_inter14);
  hf_drain(inv_inter15);
  std::printf("shuffler_out_shuff csim OK\n");
  return 0;
}
