#pmake: nomodel cpu_axp os_nt
#
# iost -- 32-bit axp COMPLEX library
#
suffix = nt
target_system = ntaxp
target_lib = $(prefix)$(suffix).lib
production_lib = libaxp
uncompressed=1
define_library_include = $(plusplus_complex_dir)\complex.mif
define_deps_include = $(plusplus_complex_dir)\deps.mif
define_target_include = $(plusplus_dir)\libaxp$(suffix).mif
!include $(plusplus_dir)\mklib.mif
