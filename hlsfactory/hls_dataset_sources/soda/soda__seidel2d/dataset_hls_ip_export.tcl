set project prj
set phase syn

set argv [list $project $phase]
set argc [llength $argv]


open_project hls_[lindex $argv 0]
open_solution "solution1"

