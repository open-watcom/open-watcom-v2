# WRESEDIT Builder Control file
# =============================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
#waccel
    <CPCMD> <devdir>\sdk\wresedit\waccel\win16\wacc.dll <relroot>\rel2\binw\wacc.dll
    <CPCMD> <devdir>\sdk\wresedit\waccel\nt\wacc.dll <relroot>\rel2\binnt\wacc.dll
#    <CPCMD> <devdir>\sdk\wresedit\waccel\axp\wacc.dll <relroot>\rel2\axpnt\wacc.dll
#wmenu
    <CPCMD> <devdir>\sdk\wresedit\wmenu\win16\wmenu.dll <relroot>\rel2\binw\wmenu.dll
    <CPCMD> <devdir>\sdk\wresedit\wmenu\nt\wmenu.dll <relroot>\rel2\binnt\wmenu.dll
#    <CPCMD> <devdir>\sdk\wresedit\wmenu\axp\wmenu.dll <relroot>\rel2\axpnt\wmenu.dll
#wstring
    <CPCMD> <devdir>\sdk\wresedit\wstring\win16\wstring.dll <relroot>\rel2\binw\wstring.dll
    <CPCMD> <devdir>\sdk\wresedit\wstring\nt\wstring.dll <relroot>\rel2\binnt\wstring.dll
#    <CPCMD> <devdir>\sdk\wresedit\wstring\axp\wstring.dll <relroot>\rel2\axpnt\wstring.dll

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
