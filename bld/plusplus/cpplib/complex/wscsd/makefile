#pmake: all build small cpu_i86 os_win
#
# cplxs.lib -- small memory-model COMPLEX library fpc for Windows
#
suffix = s
target_system = windows
target_lib = $(prefix)$(suffix).lib
production_lib = lib286\win
define_library_include = $(plusplus_complex_dir)\complex.mif
define_deps_include = $(plusplus_complex_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
text = -fpc-zW
!include $(plusplus_dir)\mklib.mif
