#pmake: all build nomodel cpu_i86 cpu_386
all: .SYMBOLIC wclnt.exe wcl.exe wcl386nt.exe wcl386.exe

#all: .SYMBOLIC wclnt.exe wcl.exe wcl386nt.exe wcl386.exe wclaxpnt.exe wclaxp.exe
#       cd ntaxp
#       wmake
#       @%null

!ifdef tools_9_0
ntlink = wlink9xx
!else
ntlink = wlink
!endif

wl_opt = option quiet

.before:
        set INCLUDE=$(%INCLUDE);$(watcom_dir)\h

wclnt.exe : wcl.c swchar.nbj wclhelp.h wclmsg.h
        $(comp386) wcl /dWCLI86 /fo=wcl.nbj /os /s /zq /d1 /d__NT /bt=nt
        $(ntlink) $(wl_opt) system nt file wcl.nbj,swchar.nbj name wclnt

wcl386nt.exe : wcl.c swchar.nbj wclhelp.h wclmsg.h
        $(comp386) wcl /dWCL386 /fo=wcl386.nbj /os /s /zq /d1 /d__NT /bt=nt
        $(ntlink) $(wl_opt) system nt file wcl386.nbj,swchar.nbj name wcl386nt

wclaxpnt.exe : wcl.c swchar.nbj wclhelp.h wclmsg.h
        $(comp386) wcl /dWCLAXP /fo=wclaxp.nbj /os /s /zq /d1 /d__NT /bt=nt
        $(ntlink) $(wl_opt) system nt file wclaxp.nbj,swchar.nbj name wclaxpnt

wcl.exe: wcl.c swchar.obj wclhelp.h wclmsg.h
        $(comp286) wcl /dWCLI86 /os /s /d1 /zq /d__DOS /bt=dos
        $(comp386) wcl /dWCLI86 /os /s /d1 /zq /d__OS2 /bt=os2 /fo=.2bj
        wlink $(wl_opt) system dos name wcldos file wcl,swchar option map, stack=2K
        wlink $(wl_opt) system os2v2 pmc op stack=8K, newfile file wcl.2bj option stub=wcldos.exe
        erase wcldos.exe

wcl386.exe: wcl.c swchar.obj wclhelp.h wclmsg.h
        $(comp286) wcl /dWCL386 /d__DOS /fo=wcl386.obj /os /s /zq /d1 /bt=dos
        $(comp386) wcl /dWCL386 /d__OS2 /fo=wcl386.2bj /os /s /zq /d1 /bt=os2
        wlink $(wl_opt) system dos name wcl386d file wcl386,swchar option stack=2K
        wlink $(wl_opt) system os2v2 pmc op stack=8K, newfile file wcl386.2bj opt stub=wcl386d.exe
        erase wcl386d.exe

wclaxp.exe: wcl.c swchar.obj wclhelp.h wclmsg.h
        $(comp286) wcl /dWCLAXP /d__DOS /fo=wclaxp.obj /os /s /zq /d1 /bt=dos
        $(comp386) wcl /dWCLAXP /d__OS2 /fo=wclaxp.2bj /os /s /zq /d1 /bt=os2
        wlink $(wl_opt) system dos name wclaxpd file wclaxp,swchar option stack=2K
        wlink $(wl_opt) system os2v2 pmc op stack=8K, newfile file wclaxp.2bj opt stub=wclaxpd.exe
        erase wclaxpd.exe

wclp16.exe: wcl.c swchar.obj wclhelp.h wclmsg.h
        $(comp286) wcl /dWCLP16 /d__DOS /fo=wclp16.obj /os /s /zq /bt=dos
        $(comp386) wcl /dWCLP16 /d__OS2 /fo=wclp16.2bj /os /s /zq /bt=os2
        wlink $(wl_opt) system dos name wclp16d file wclp16,swchar option stack=2K
        wlink $(wl_opt) system os2v2 pmc op stack=8K, newfile file wclp16.2bj opt stub=wclp16d.exe
        erase wclp16d.exe

wcl.qnx: wcl.c wclhelp.h wclmsg.h
        $(comp286) wcl /dWCLI86 /os /s /d1 /zq /d__QNX /fo=.qbj /bt=qnx
        wlink format qnx file wcl.qbj option stack=1024

swchar.obj: ..\c\swchar.c
        $(comp286) swchar /os /s /d1 /zq

swchar.nbj: ..\c\swchar.c
        $(comp386) swchar /os /s /d1 /zq /d__NT /fo=.nbj /bt=nt

!include deftarg.mif
