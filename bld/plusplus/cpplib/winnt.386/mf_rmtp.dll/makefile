#pmake: build flat cpu_386 os_nt dynamic profile
processor  = 386
system     = winnt
model      = mf
convention = register
linkage    = dynamic
mthreaded  = 1
profile    = 1
subdir     = mf_rmtp.dll

additional_cleanup  = export*.def

define_library_include = $(cpplib_dir)/plib.mif
!include $(cpplib_dir)/mrglib.mif
