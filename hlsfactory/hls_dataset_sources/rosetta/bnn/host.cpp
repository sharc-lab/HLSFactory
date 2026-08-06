#include <cstdio>
#include <cstring>
#include "Accel.h"

int main() {
  Word wt[WT_WORDS];
  Word kh[KH_WORDS];
  Word dmem_i[DMEM_WORDS];
  Word dmem_o[DMEM_O_WORDS];
  std::memset(wt, 0, sizeof(wt));
  std::memset(kh, 0, sizeof(kh));
  std::memset(dmem_i, 0, sizeof(dmem_i));
  std::memset(dmem_o, 0, sizeof(dmem_o));

  // Minimal conv-layer smoke: new layer, conv, 8-bit width, with norm.
  const unsigned S = 8;
  const unsigned M = CONVOLVERS * PIX_PER_PHASE / (S * S);
  top(wt, kh, dmem_i, dmem_o,
      /*n_inputs=*/M, /*n_outputs=*/1,
      /*input_words=*/(M * S * S + WORD_SIZE - 1) / WORD_SIZE,
      /*output_words=*/(S * S + WORD_SIZE - 1) / WORD_SIZE,
      /*layer_mode=*/ap_uint<3>(0b011),  // bit0=new, [2:1]=LAYER_CONV
      /*dmem_mode=*/ap_uint<1>(0),
      /*width_mode=*/ap_uint<2>(0),
      /*norm_mode=*/ap_uint<2>(1));

  std::printf("bnn_top csim OK\n");
  return 0;
}
