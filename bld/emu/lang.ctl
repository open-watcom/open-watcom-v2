# EMU Builder Control file
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
    <CPCMD> <devdir>\emu\emudos.lib <relroot>\rel2\lib386\dos\emu387.lib
    <CPCMD> <devdir>\emu\emustub.lib <relroot>\rel2\lib386\netware\emu387.lib
    <CPCMD> <devdir>\emu\emustub.lib <relroot>\rel2\lib386\os2\emu387.lib
#    <CPCMD> <devdir>\emu\emustub.lib <relroot>\rel2\lib386\osi\emu387.lib
    <CPCMD> <devdir>\emu\emustub.lib <relroot>\rel2\lib386\win\emu387.lib
    <CPCMD> <devdir>\emu\emustub.lib <relroot>\rel2\lib386\qnx\emu387.lib
    <CPCMD> <devdir>\emu\emustub.lib <relroot>\rel2\lib386\nt\emu387.lib
    <CPCMD> <devdir>\emu\emustub.lib <relroot>\rel2\lib386\noemu387.lib
    <CPCMD> <devdir>\emu\emu387.qnx <relroot>\rel2\qnx\binq\emu387

[ BLOCK <1> clean ]
#==================
    sweep killobjs
