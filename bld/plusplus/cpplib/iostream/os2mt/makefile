#pmake: all build large cpu_i86 os_os2
#
# iostmt.lib -- large memory-model OS/2 multi-thread IOSTREAM library
#
suffix = mtl
target_lib = $(prefix)$(suffix).lib
production_lib = lib286\os2
target_system = os2
define_library_include = $(plusplus_iostream_dir)\iostream.mif
define_deps_include = $(plusplus_iostream_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib16$(suffix).mif
!include $(plusplus_dir)\mklib.mif
