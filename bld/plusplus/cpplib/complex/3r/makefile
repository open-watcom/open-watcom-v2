#pmake: all build flat cpu_386 os_osi
#
# cplx3r.lib -- register based 32-bit COMPLEX library fpc
#
suffix = 3r
target_system = generic
target_lib = $(prefix)$(suffix).lib
production_lib = lib386
define_library_include = $(plusplus_complex_dir)\complex.mif
define_deps_include = $(plusplus_complex_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib32$(suffix).mif
text = -fpc
!include $(plusplus_dir)\mklib.mif
