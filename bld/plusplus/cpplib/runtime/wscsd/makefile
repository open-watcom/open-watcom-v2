#pmake: all build small cpu_i86 os_win
#
# plibs -- 16-bit small memory-model PLIB for Windows
#
suffix = s
target_system = windows
target_lib = $(prefix)$(suffix).lib
production_lib = lib286\win
define_library_include = $(plusplus_runtime_dir)\plib.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
text = -zW
!include $(plusplus_dir)\mklib.mif
