#pmake: all build large cpu_i86 os_win
#
# strgl.lib -- large memory-model STRING library for Windows
#
suffix = l
target_system = windows
target_lib = $(prefix)$(suffix).lib
production_lib = lib286\win
define_library_include = $(plusplus_string_dir)\string.mif
define_deps_include = $(plusplus_string_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
text = -zW
!include $(plusplus_dir)\mklib.mif
