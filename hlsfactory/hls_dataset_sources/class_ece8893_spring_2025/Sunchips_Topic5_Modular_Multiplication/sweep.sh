#!/bin/bash

# List of radix values to sweep
for k in 13 17 19 24 29 32 51 64 96 128 256; do
  vitis_hls -cflags "-DRADIX=${k}" -f script.tcl > "log_radix_${k}.txt" 2>&1

  rpt_path="project1/solution1/impl/report/mod_mul_export.rpt"

  if [[ -f "$rpt_path" ]]; then
    DSP=$(grep "DSP:" "$rpt_path" | awk '{print $3}')
    LUT=$(grep "LUT:" "$rpt_path" | awk '{print $3}')
    FF=$(grep "FF:" "$rpt_path" | awk '{print $3}')
    CLK=$(grep "achieved post-implementation:" "$rpt_path" | awk '{print $3}')

    echo "RADIX: $k"
    echo "  Clock     : $CLK ns"
    echo "  DSPs used : $DSP"
    echo "  LUTs used : $LUT"
    echo "  FFs used  : $FF"
  else
    echo " No report found found for RADIX $k"
  fi

  echo ""
done
