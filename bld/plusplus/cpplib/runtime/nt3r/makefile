#pmake: all build flat cpu_386 os_nt
#
# plib3r -- 32-bit 3r PLIB, PLBX
#
fs_registration = yes
suffix = 3r
target_system = nt
production_lib = lib386
define_library_include = $(plusplus_runtime_dir)\plib.mif
define_target_include = $(plusplus_dir)\lib32$(suffix).mif
text = -D__USE_FS
!include $(plusplus_dir)\mklib.mif
