#pmake: all build flat cpu_386 os_osi
#
# strgmt3r.lib -- register based multi-thread 32-bit STRING library
#
suffix = mt3r
target_system = generic
target_lib = $(prefix)$(suffix).lib
production_lib = lib386
define_library_include = $(plusplus_string_dir)\string.mif
define_deps_include = $(plusplus_string_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib$(suffix).mif
!include $(plusplus_dir)\mklib.mif
