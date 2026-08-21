set LIB_PATH "[get_install_path]/share/stratus/techlibs/GPDK045/gsclib045_svt_v4.4/gsclib045/timing"
use_tech_lib "$LIB_PATH/slow_vdd1v2_basicCells.lib"

set_attr message_detail 1
set_attr auto_build_hls_libs on
set_attr clock_period 5.0
set_attr cycle_slack 0.5

define_hls_module multiply_accumulate multiply_accumulate.cc
define_io_config * TLM
define_io_config * PIN
define_hls_config multiply_accumulate BASIC
