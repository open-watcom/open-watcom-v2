#pmake: all build compact cpu_i86 os_win
#
# contc.lib -- compact memory-model CONTAINER library for Windows
#
suffix = c
target_system = windows
target_lib = $(prefix)$(suffix).lib
production_lib = lib286\win
define_library_include = $(plusplus_contain_dir)\contain.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
text = -zW
!include $(plusplus_dir)\mklib.mif
