#pmake: all build flat cpu_386 os_nt
#
# cont3s.lib -- stack based 32-bit CONTAINER library
#
fs_registration = yes
target_system = nt
text = -d__USE_FS
suffix = 3s
target_lib = $(prefix)$(suffix).lib
production_lib = lib386
define_library_include = $(plusplus_contain_dir)\contain.mif
define_target_include = $(plusplus_dir)\lib32$(suffix).mif
!include $(plusplus_dir)\mklib.mif
