#pmake: all build large cpu_i86 os_win
#
# cplx7l.lib -- large memory-model COMPLEX library fpi for Windows
#
suffix = l
target_system = windows
target_lib = $(prefix)7$(suffix).lib
production_lib = lib286\win
define_library_include = $(plusplus_complex_dir)\complex.mif
define_deps_include = $(plusplus_complex_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
text = -fpi-zW
!include $(plusplus_dir)\mklib.mif
