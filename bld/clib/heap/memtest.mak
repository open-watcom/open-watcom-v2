#
# Memory test program makefile (see memtest.c)
#
!error copy to /tmp and remove this line

# code	platform		invocation
# ====	========		==========
# DOS	16-bit DOS		dos_s
#				dos_c
# PLS	PharLap			run386 pls_3s
#				run386 pls_3r
# RSI	Rational Systems	dos4g rsi_3s
#				dos4g rsi_3r
# ICB	Intel Code Builder	mkexe icb_3s
#				icb_3s
#				mkexe icb_3r
#				icb_3r
# OS2	OS/2 16-bit		os2_s
#				os2_c
# OS3	OS/2 32-bit 		os3_3r
# WIN	Windows 16-bit		create icons ; click
# W32	Windows 32-bit		create icon; click

.erase
.extensions:
.extensions: .exe .rex .exp .obj .c .asm

mem_test = $(new_clib)\heap\c\memtest.c

comp_opts = -ox -d1 -zq -fpc -fo=$^*.obj
link_opts = NAME $^* OPTION quiet DEBUG line

exes = &
    dos_s.exe dos_c.exe &
    pls_3s.exp pls_3r.exp &
    rsi_3s.exe rsi_3r.exe &
    icb_3s.rex icb_3r.rex &
    os2_s.exe os2_c.exe &
    os3_3r.exe os3_3s.exe &
    win_s.exe win_c.exe &
    win32_3s.exe win32_3r.exe

all : .symbolic $(exes)
    del *.obj

dos_s.exe : $(mem_test)
    $(comp286) $[@ $(comp_opts) -ms
    wlink SYSTEM dos $(link_opts) FILE $^*

dos_c.exe : $(mem_test)
    $(comp286) $[@ $(comp_opts) -mc
    wlink SYSTEM dos $(link_opts) FILE $^*

pls_3s.exp : $(mem_test)
    $(comp386) $[@ $(comp_opts) -mf -3s
    wlink SYSTEM pharlap option offset=4k $(link_opts) FILE $^*

pls_3r.exp : $(mem_test)
    $(comp386) $[@ $(comp_opts) -mf -3r
    wlink SYSTEM pharlap option offset=4k $(link_opts) FILE $^*

rsi_3s.exe : $(mem_test)
    $(comp386) $[@ $(comp_opts) -mf -3s
    wlink SYSTEM dos4g $(link_opts) FILE $^*

rsi_3r.exe : $(mem_test)
    $(comp386) $[@ $(comp_opts) -mf -3r
    wlink SYSTEM dos4g $(link_opts) FILE $^*

icb_3s.rex : $(mem_test)
    $(comp386) $[@ $(comp_opts) -mf -3s
    wlink SYSTEM codebuilder $(link_opts) FILE $^*

icb_3r.rex : $(mem_test)
    $(comp386) $[@ $(comp_opts) -mf -3r
    wlink SYSTEM codebuilder $(link_opts) FILE $^*

os2_s.exe : $(mem_test)
    $(comp286) $[@ $(comp_opts) -ms -2
    wlink SYSTEM os2 pmcompatible $(link_opts) FILE $^*

os2_c.exe : $(mem_test)
    $(comp286) $[@ $(comp_opts) -mc -2
    wlink SYSTEM os2 pmcompatible $(link_opts) FILE $^*

os3_3r.exe : $(mem_test)
    $(comp386) $[@ $(comp_opts) -mf -3r
    wlink SYSTEM os2v2 pmcompatible $(link_opts) FILE $^*

os3_3s.exe : $(mem_test)
    $(comp386) $[@ $(comp_opts) -mf -3s
    wlink SYSTEM os2v2 pmcompatible $(link_opts) FILE $^*

win_s.exe : $(mem_test) windows.lnk
    $(comp286) $[@ $(comp_opts) -ms -zw
    wlink SYSTEM windows $(link_opts) @$]@ FILE $^* LIBRARY clibs, windows
    if exist $^*.rc del $^*.rc
    wtouch $^*.rc
    rc $^*.rc $^@ -fe $^@
    del $^*.rc
    del $^*.res
    copy /b $^@+$^*.sym
    del $^*.sym

win_c.exe : $(mem_test) windows.lnk
    $(comp286) $[@ $(comp_opts) -mc -zw
    wlink SYSTEM windows $(link_opts) @$]@ FILE $^* LIBRARY clibc, windows
    if exist $^*.rc del $^*.rc
    wtouch $^*.rc
    rc $^*.rc $^@ -fe $^@
    del $^*.rc
    del $^*.res
    copy /b $^@+$^*.sym
    del $^*.sym

windows.lnk : .symbolic
    set $^@=OPTION heap=10k, stack=5k, symfile
    
win32_3r.exe : $(mem_test) win32r.lnk 
    $(comp386) $[@ $(comp_opts) -mf -3r -zw
    wlink SYSTEM win386 $(link_opts) @$]@ FILE $^*
    wtouch $^*.rc
    wbind $^*
    del $^*.rc

win32_3s.exe : $(mem_test) win32s.lnk 
    $(comp386) $[@ $(comp_opts) -mf -3s -zw
    wlink SYSTEM win386 $(link_opts) @$]@ FILE $^*
    wtouch $^*.rc
    wbind $^*
    del $^*.rc

win32r.lnk : .symbolic
    set $^@=opt mindata=32K,maxdata=32K
    
win32s.lnk : .symbolic
    set $^@=opt mindata=32K,maxdata=32K
