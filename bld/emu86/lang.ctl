# EMU86 Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    wmake -h -i
    cd <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> <devdir>\emu86\emu87.lib <relroot>\rel2\lib286\dos\emu87.lib
    <CPCMD> <devdir>\emu86\emuos2.lib <relroot>\rel2\lib286\os2\emu87.lib
    <CPCMD> <devdir>\emu86\emuwin.lib <relroot>\rel2\lib286\win\emu87.lib
    <CPCMD> <devdir>\emu86\noemu87.lib <relroot>\rel2\lib286\qnx\emu87.lib
    <CPCMD> <devdir>\emu86\noemu87.lib <relroot>\rel2\lib286\noemu87.lib
    <CPCMD> <devdir>\emu86\emu87.qnx <relroot>\rel2\qnx\binq\emu86
    <CPCMD> <devdir>\emu86\emu87_16.qnx <relroot>\rel2\qnx\binq\emu86_16
    <CPCMD> <devdir>\emu86\emu87_32.qnx <relroot>\rel2\qnx\binq\emu86_32

[ BLOCK <1> clean ]
#==================
    sweep killobjs
