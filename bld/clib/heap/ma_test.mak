#
# MA test program makefile (see ma_test.c)
#

# code	platform		invocation
# ====	========		==========
# DOS	16-bit DOS		dos_s
#				dos_c
#				dos_h
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
#				os2_h
# OS3	OS/2 32-bit 		os3_3r
#				os2_3s
# WIN	Windows 16-bit		create icons ; click
# W32	Windows 32-bit		create icon; click
# WNT	Windows NT		wnt_3r
#				wnt_3s

# Copy this file to a directory where you want the executables and
# in that directory, type "wmake -h -f ma_test.mak"
!error copy to \tmpdir and remove this line

.erase
.extensions:
.extensions: .exe .rex .exp .obj .c .asm

ma_test = $(new_clib)\heap\c\ma_test.c

comp_opts = -ox -d2 -zq -fpc -fo=$^*.obj
link_opts = NAME $^* OPTION quiet DEBUG all

exes = &
    dos_s.exe dos_c.exe dos_h.exe &
    pls_3s.exp pls_3r.exp &
    rsi_3s.exe rsi_3r.exe &
    icb_3s.rex icb_3r.rex &
    os2_s.exe os2_c.exe os2_h.exe &
    os3_3r.exe os3_3s.exe &
    win_s.exe win_c.exe &
    win32_3s.exe win32_3r.exe &
    wnt_3s.exe wnt_3r.exe

all : .symbolic $(exes)
    del *.obj

dos_s.exe : $(ma_test)
    $(comp286) $[@ $(comp_opts) -ms
    wlink SYSTEM dos $(link_opts) FILE $^*

dos_c.exe : $(ma_test)
    $(comp286) $[@ $(comp_opts) -mc
    wlink SYSTEM dos $(link_opts) FILE $^*

dos_h.exe : $(ma_test)
    $(comp286) $[@ $(comp_opts) -mh
    wlink SYSTEM dos $(link_opts) FILE $^*

pls_3s.exp : $(ma_test)
    $(comp386) $[@ $(comp_opts) -mf -3s
    wlink SYSTEM pharlap option offset=4k $(link_opts) FILE $^*

pls_3r.exp : $(ma_test)
    $(comp386) $[@ $(comp_opts) -mf -3r
    wlink SYSTEM pharlap option offset=4k $(link_opts) FILE $^*

rsi_3s.exe : $(ma_test)
    $(comp386) $[@ $(comp_opts) -mf -3s
    wlink SYSTEM dos4g $(link_opts) FILE $^*

rsi_3r.exe : $(ma_test)
    $(comp386) $[@ $(comp_opts) -mf -3r
    wlink SYSTEM dos4g $(link_opts) FILE $^*

icb_3s.rex : $(ma_test)
    $(comp386) $[@ $(comp_opts) -mf -3s
    wlink SYSTEM codebuilder $(link_opts) FILE $^*

icb_3r.rex : $(ma_test)
    $(comp386) $[@ $(comp_opts) -mf -3r
    wlink SYSTEM codebuilder $(link_opts) FILE $^*

os2_s.exe : $(ma_test)
    $(comp286) $[@ $(comp_opts) -ms -2
    wlink SYSTEM os2 pmcompatible $(link_opts) FILE $^*

os2_c.exe : $(ma_test)
    $(comp286) $[@ $(comp_opts) -mc -2
    wlink SYSTEM os2 pmcompatible $(link_opts) FILE $^*

os2_h.exe : $(ma_test)
    $(comp286) $[@ $(comp_opts) -mh -2
    wlink SYSTEM os2 pmcompatible $(link_opts) FILE $^*

os3_3r.exe : $(ma_test)
    $(comp386) $[@ $(comp_opts) -mf -3r
    wlink SYSTEM os2v2 pmcompatible $(link_opts) FILE $^*

os3_3s.exe : $(ma_test)
    $(comp386) $[@ $(comp_opts) -mf -3s
    wlink SYSTEM os2v2 pmcompatible $(link_opts) FILE $^*

win_s.exe : $(ma_test) windows.lnk
    $(comp286) $[@ $(comp_opts) -ms -zw -bw -bt=windows
    wlink SYSTEM windows $(link_opts) @$]@ FILE $^* LIBRARY clibs, windows
    if exist $^*.rc del $^*.rc
    wtouch $^*.rc
    rc $^*.rc $^@ -fe $^@
    del $^*.rc
    del $^*.res
    copy /b $^@+$^*.sym
    del $^*.sym

win_c.exe : $(ma_test) windows.lnk
    $(comp286) $[@ $(comp_opts) -mc -zw -bw -bt=windows
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
    
win32_3r.exe : $(ma_test) win32r.lnk 
    $(comp386) $[@ $(comp_opts) -mf -3r -zw -bw -bt=win386
    wlink SYSTEM win386 $(link_opts) @$]@ FILE $^*
    wbind -n $^*
    del $^*.rex

win32_3s.exe : $(ma_test) win32s.lnk 
    $(comp386) $[@ $(comp_opts) -mf -3s -zw -bw -bt=win386
    wlink SYSTEM win386 $(link_opts) @$]@ FILE $^*
    wbind -n $^*
    del $^*.rex

win32r.lnk : .symbolic
    set $^@=opt mindata=32K,maxdata=32K
    
win32s.lnk : .symbolic
    set $^@=opt mindata=32K,maxdata=32K

wnt_3r.exe : $(ma_test)
    $(comp386) $[@ $(comp_opts) -mf -3r -bt=nt
    wlink SYSTEM nt $(link_opts) FILE $^*

wnt_3s.exe : $(ma_test)
    $(comp386) $[@ $(comp_opts) -mf -3s -bt=nt
    wlink SYSTEM nt $(link_opts) FILE $^*
