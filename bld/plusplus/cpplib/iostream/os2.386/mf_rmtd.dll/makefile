#pmake: build flat cpu_386 os_os2 dynamic debug
#
# iostmt3r.lib -- register based multi-thread 32-bit IOSTREAM library
#
processor  = 386
system     = os2
model      = mf
convention = register
linkage    = dynamic
mthreaded  = 1
fs_registration = yes
cpplib_release  = 0

define_library_include = $(cpplib_dir)/iostream/iostream.mif
!include $(cpplib_dir)/mklib.mif
