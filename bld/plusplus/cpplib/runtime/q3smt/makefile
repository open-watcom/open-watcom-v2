#pmake: all build flat cpu_386 os_qnx
#
# plibmt3s -- 32-bit 3s PLIB multi-thread
#
suffix = mt3s
target_system = qnx
target_lib = $(prefix)$(suffix).lib
production_lib = lib386
define_library_include = $(plusplus_runtime_dir)\plib.mif
define_target_include = $(plusplus_dir)\lib$(suffix).mif
!include $(plusplus_dir)\mklib.mif
