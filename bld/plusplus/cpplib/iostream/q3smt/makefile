#pmake: all build flat cpu_386 os_qnx
#
# iostmt3s.lib -- stack based multi-thread 32-bit IOSTREAM library
#
target_system = qnx
suffix = mt3s
target_lib = $(prefix)$(suffix).lib
production_lib = lib386
define_library_include = $(plusplus_iostream_dir)\iostream.mif
define_deps_include = $(plusplus_iostream_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib$(suffix).mif
!include $(plusplus_dir)\mklib.mif
