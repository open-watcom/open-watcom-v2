# Wlink Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\wl\qnx\wl.exe <relroot>\rel2\qnx\binq\wlink
    <CPCMD> <devdir>\wl\qnx\wl.sym <relroot>\rel2\qnx\sym\wlink.sym
    <CPCMD> <devdir>\wl\ms2wlink\o\ms2wlink.exe <relroot>\rel2\binw\ms2wlink.exe
    <CPCMD> <devdir>\wl\fcenable\o\fcenable.exe <relroot>\rel2\binw\fcenable.exe
# Removed: KB
#    <CPCMD> <devdir>\wl\ovlldr\wovl.lib <relroot>\rel2\lib286\dos\wovl.lib
#    <CPCMD> <devdir>\wl\ovlldr\wmovl.lib <relroot>\rel2\lib286\dos\wmovl.lib
# /Removed: KB
    <CPCMD> <devdir>\wl\wlbin.lnk <relroot>\rel2\binw\wlink.lnk
    <CPCMD> <devdir>\wl\wlbinp.lnk <relroot>\rel2\binp\wlink.lnk
    <CPCMD> <devdir>\wl\wlbinnt.lnk <relroot>\rel2\binnt\wlink.lnk
#    <CPCMD> <devdir>\wl\wlaxpnt.lnk <relroot>\rel2\axpnt\wlink.lnk
    <CPCMD> <devdir>\wl\wlsystem.lnk <relroot>\rel2\binw\wlsystem.lnk
    <CPCMD> <devdir>\wl\386prod\wl.exe <relroot>\rel2\binw\wlink.exe
#    <CPCMD> <devdir>\wl\386nec\wl.exe <relroot>\rel2\n98bw\wlink.exe
    <CPCMD> <devdir>\wl\os2dll\wlstub.exe <relroot>\rel2\binp\wlink.exe
    <CPCMD> <devdir>\wl\os2dll\wl.dll <relroot>\rel2\binp\dll\wlink.dll
    <CPCMD> <devdir>\wl\ntdll\wlstub.exe <relroot>\rel2\binnt\wlink.exe
    <CPCMD> <devdir>\wl\ntdll\wl.dll <relroot>\rel2\binnt\wlink.dll
    <CPCMD> <devdir>\wl\ntrtdll\wl.dll <relroot>\rel2\binnt\rtdll\wlink.dll
#    <CPCMD> <devdir>\wl\axp\wlstub.exe <relroot>\rel2\axpnt\wlink.exe
#    <CPCMD> <devdir>\wl\axp\wl.dll <relroot>\rel2\axpnt\wlink.dll
#   cat <devdir>\wl\wlsystem.lnk <devdir>\wl\wlqnx.lnk ><relroot>\rel2\qnx\etc\wlink.lnk

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK <1> relink ]
#===================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h killnonobj
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
