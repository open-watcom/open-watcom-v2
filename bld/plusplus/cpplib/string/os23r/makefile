#pmake: all build flat cpu_386 os_os2
#
# strg3r.lib -- register based 32-bit STRING library
#
fs_registration = yes
target_system = os2v2
text = -d__USE_FS
suffix = 3r
production_lib = lib386
define_library_include = $(plusplus_string_dir)\string.mif
define_deps_include = $(plusplus_string_dir)\deps.mif
define_target_include = $(plusplus_dir)\lib32$(suffix).mif
!include $(plusplus_dir)\mklib.mif
