#pmake: all build medium cpu_i86 os_osi
#
# cplx7m.lib -- medium memory-model COMPLEX library fpi
#
suffix = m
target_system = generic
target_lib = $(prefix)7$(suffix).lib
production_lib = lib286
define_library_include = $(plusplus_complex_dir)\complex.mif
define_deps_include = $(plusplus_complex_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
text = -fpi
!include $(plusplus_dir)\mklib.mif
