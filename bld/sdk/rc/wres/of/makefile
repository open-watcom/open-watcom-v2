#pmake: build os_nt os_win cpu_386


host_OS = dos
host_CPU = 386
libname = f

!include ..\master.mif

lexusbld : .symbolic all
    @if not exist $(lexlib_dir)\wres\lib mkdir $(lexlib_dir)\wres\lib
    @newer ..\lib\wresf.lib $(lexlib_dir)\wres\lib "^copy &f $#f > nul"


