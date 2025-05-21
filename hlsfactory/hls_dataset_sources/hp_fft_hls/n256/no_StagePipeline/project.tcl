# Source the common script

# Get the current working directory
set currentDir [pwd]
# Split into a list of path segments
set dirList [file split $currentDir]
# Get the total number of segments
set length [llength $dirList]
set lastTwo [lrange $dirList [expr {$length - 2}] end]
# Join them back with a slash (or the platform's directory separator)
set shortPath [join $lastTwo "-"]
puts $shortPath

source ../../common.tcl




