#pmake: all build compact cpu_i86 os_win
#
# plibc -- 16-bit compact memory-model PLIB for Windows
#
suffix = c
target_system = windows
target_lib = $(prefix)$(suffix).lib
production_lib = lib286\win
define_library_include = $(plusplus_runtime_dir)\plib.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
text = -zW
!include $(plusplus_dir)\mklib.mif
