#pmake: all build large cpu_i86 os_osi
#
# cplxl.lib -- large memory-model COMPLEX library fpc
#
suffix = l
target_system = generic
target_lib = $(prefix)$(suffix).lib
production_lib = lib286
define_library_include = $(plusplus_complex_dir)\complex.mif
define_deps_include = $(plusplus_complex_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
text = -fpc
!include $(plusplus_dir)\mklib.mif
