# WATCOM Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

    <CPCMD> <devdir>\watcom\lib\win95\*.* <relroot>\rel2\lib386\nt\
    <CPCMD> <devdir>\watcom\lib\os2.lib <relroot>\rel2\lib286\os2\
    <CPCMD> <devdir>\watcom\lib\winsock.lib <relroot>\rel2\lib286\win\
