#pmake: all build flat cpu_386 os_nt
#
# contmt3s.lib -- stack based multi-thread 32-bit CONTAINER library
#
fs_registration = yes
target_system = nt
text = -d__USE_FS
suffix = mt3s
target_lib = $(prefix)$(suffix).lib
production_lib = lib386
define_library_include = $(plusplus_contain_dir)\contain.mif
define_target_include = $(plusplus_dir)\lib$(suffix).mif
!include $(plusplus_dir)\mklib.mif
