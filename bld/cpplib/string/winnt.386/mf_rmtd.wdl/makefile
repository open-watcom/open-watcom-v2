#pmake: build flat cpu_386 os_nt dynamic
#
# strgmt3r.lib -- register based multi-thread 32-bit STRING library
#
processor  = 386
system     = winnt
model      = mf
convention = register
linkage    = dynamic
export_only=1
mthreaded  = 1
fs_registration = yes
cpplib_release = 0
define_library_include = $(cpplib_dir)/string/string.mif
!include $(cpplib_dir)/mklib.mif
