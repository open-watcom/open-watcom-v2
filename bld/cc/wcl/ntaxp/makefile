#pmake: nomodel cpu_axp os_nt
host_CPU = axp
host_OS = nt
!include cproj.mif

wl_opt = option quiet

all: .SYMBOLIC wcl.exe wcl386.exe wclaxp.exe
        @%null

wcl.exe : ..\wcl.c ..\wclhelp.h ..\wclmsg.h
        $(compaxp) $[@ /dWCLI86 /d__NT /fo=wcl.abj /os /s /zq /d1 /bt=nt /i$(watcom_dir)\h
        wlink $(wl_opt) system ntaxp op stack=64K name $^@ file wcl.abj

wcl386.exe : ..\wcl.c ..\wclhelp.h ..\wclmsg.h
        $(compaxp) $[@ /dWCL386 /d__NT /fo=wcl386.abj /os /s /zq /d1 /bt=nt /i$(watcom_dir)\h
        wlink $(wl_opt) system ntaxp op stack=64K name $^@ file wcl386.abj

wclaxp.exe : ..\wcl.c ..\wclhelp.h ..\wclmsg.h
        $(compaxp) $[@ /dWCLAXP /d__NT /fo=wclaxp.abj /os /s /zq /d1 /bt=nt /i$(watcom_dir)\h
        wlink $(wl_opt) system ntaxp op stack=64K name $^@ file wclaxp.abj

!include deftarg.mif
