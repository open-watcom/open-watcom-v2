#pmake: all build flat cpu_386 os_qnx
#
# contmt3s.lib -- stack based multi-thread 32-bit CONTAINER library
#
target_system = qnx
suffix = mt3s
target_lib = $(prefix)$(suffix).lib
production_lib = lib386
define_library_include = $(plusplus_contain_dir)\contain.mif
define_target_include = $(plusplus_dir)\lib$(suffix).mif
!include $(plusplus_dir)\mklib.mif
