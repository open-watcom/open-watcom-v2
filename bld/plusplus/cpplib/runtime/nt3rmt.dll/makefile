#pmake: all build flat cpu_386 os_nt dynamic
#
# plib3r -- 32-bit 3r MT PLIB, PLBX
#
fs_registration = yes
suffix = mt3r
linkage=dynamic
target_system = nt
target_lib = $(prefix)$(suffix).lib
production_lib = lib386
define_library_include = $(plusplus_runtime_dir)\plib.mif
define_target_include = $(plusplus_dir)\lib$(suffix).mif
text = -D__USE_FS
!include $(plusplus_dir)\mklib.mif
