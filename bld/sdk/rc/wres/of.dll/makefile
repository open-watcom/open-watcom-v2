#pmake: build os_nt os_win cpu_386
host_OS = nt
host_CPU = 386
wres_rtdll = 1
libname = fdll

!include ..\master.mif

lexusbld : .symbolic all
    @if not exist $(lexlib_dir)\wres\lib mkdir $(lexlib_dir)\wres\lib
    @newer ..\lib\wresfdll.lib $(lexlib_dir)\wres\lib "^copy &f $#f > nul"
