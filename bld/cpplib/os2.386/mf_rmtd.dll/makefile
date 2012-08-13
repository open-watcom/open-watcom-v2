#pmake: build flat cpu_386 os_os2 dynamic debug
processor  = 386
system     = os2
model      = mf
convention = register
linkage    = dynamic
mthreaded  = 1
subdir     = mf_rmtd.dll

additional_cleanup  = export*.def

cpplib_release = 0

define_library_include = $(cpplib_dir)/plib.mif
!include $(cpplib_dir)/mrglib.mif
