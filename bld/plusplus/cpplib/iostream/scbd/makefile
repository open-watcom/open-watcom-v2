#pmake: all build compact cpu_i86 os_osi
#
# iostc.lib -- compact memory-model IOSTREAM library
#
suffix = c
target_system = generic
target_lib = $(prefix)$(suffix).lib
production_lib = lib286
define_library_include = $(plusplus_iostream_dir)\iostream.mif
define_deps_include = $(plusplus_iostream_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
!include $(plusplus_dir)\mklib.mif
