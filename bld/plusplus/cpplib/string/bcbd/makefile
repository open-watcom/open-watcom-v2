#pmake: all build large cpu_i86 os_osi
#
# strgl.lib -- large memory-model STRING library
#
suffix = l
target_system = generic
target_lib = $(prefix)$(suffix).lib
production_lib = lib286
define_library_include = $(plusplus_string_dir)\string.mif
define_deps_include = $(plusplus_string_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
!include $(plusplus_dir)\mklib.mif
