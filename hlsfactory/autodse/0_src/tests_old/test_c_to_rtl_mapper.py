import json
from pathlib import Path
import sys
import os
from concrete_design.c_to_rtl_mapper import CToRTLMapper

log_dir = "/home/cy/github/DSE-HLS-FFT/0_work_dir/0_concrete_design/logs"
mapper = CToRTLMapper(log_dir)
success = mapper.run_analysis()

if success:
    print("\n✅ All C input to RTL modules successfully mapped!")
else:
    print("\n❌ Some modules could not be mapped or are missing")