#pmake: all build large cpu_i86 os_win
#
# iostl.lib -- large memory-model IOSTREAM library for Windows
#
suffix = l
target_system = windows
target_lib = $(prefix)$(suffix).lib
production_lib = lib286\win
define_library_include = $(plusplus_iostream_dir)\iostream.mif
define_deps_include = $(plusplus_iostream_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
text = -zW
!include $(plusplus_dir)\mklib.mif
