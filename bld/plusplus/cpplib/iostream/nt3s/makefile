#pmake: all build flat cpu_386 os_nt
#
# iost3s.lib -- stack based 32-bit IOSTREAM library
#
fs_registration = yes
target_system = nt
text = -d__USE_FS
suffix = 3s
target_lib = $(prefix)$(suffix).lib
production_lib = lib386
define_library_include = $(plusplus_iostream_dir)\iostream.mif
define_deps_include = $(plusplus_iostream_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib32$(suffix).mif
!include $(plusplus_dir)\mklib.mif
