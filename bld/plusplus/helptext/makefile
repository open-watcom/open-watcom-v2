# Make Help Text
#
!ifdef __dos__
util_bt = dos
!else ifdef __os2__
util_bt = os2
!else ifdef __nt__
util_bt = os2
!else
!error unknown OS!
!endif

p2r = ..\bin\ptf2rtf.exe

dir_awk=..\awk
dir_gml=..\gml

msgs=$(dir_gml)\messages.gml



########################################################
# OS/2
########################################################

help_os2=wpperrs.ipf

$(help_os2) : $(msgs) mkos2hlp.exe vlos2hlp.exe
    mkos2hlp.exe $(msgs) $(help_os2)
    vlos2hlp.exe $(help_os2)

mkos2hlp.exe : mkos2hlp.c
    wcl $[@ -zq-ox-d1 -l=$(util_bt)
    del mkos2hlp.obj

vlos2hlp.exe : vlos2hlp.c
    wcl $[@ -zq-ox-d1 -l=$(util_bt)
    del vlos2hlp.obj



########################################################
# WINDOWS
########################################################

help_win=wpp.hlp
awk_win_mak=$(dir_awk)\mkwinhlp.awk
temp_files = winhelp.ptf winhelp.idx wpp.ptf winhelp.rtf winhelp.gh

$(help_win) : $(msgs) mkwinhlp.exe $(p2r)
    mkwinhlp.exe $(msgs) winhelp.ptf
    $(p2r) -i -ip winhelp.ptf
    copy winhelp.ptf+winhelp.idx wpp.ptf
    $(p2r) -bl wpp.ptf winhelp.rtf
    hc wpp.hpj
!ifdef on_build_machine
    rm -f $(temp_files)
!endif

mkwinhlp.exe : mkwinhlp.c
    wcl $[@ -zq-ox-d1 -l=$(util_bt)
    del mkwinhlp.obj

global : .symbolic
    rm -f $(help_os2) $(help_win) $(temp_files) *.obj *.exe *.err *.dmp
