# ce Builder Control file
# =======================

set PROJDIR=<CWD>

[ INCLUDE <LANG_BLD>\master.ctl ]
[ INCLUDE <LANG_BLD>\wproj.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
    wmake /h /i

[ BLOCK <1> rel2 cprel2 ]
#========================
    <CPCMD> cmdedit.exe <relroot>\rel2\binw\cmdedit.exe
    <CPCMD> cmdedit.txt <relroot>\rel2\binw\cmdedit.txt
    <CPCMD> os2edit.exe <relroot>\rel2\binp\os2edit.exe
    <CPCMD> os22edit.exe <relroot>\rel2\binp\os22edit.exe
    <CPCMD> os2edit.dll <relroot>\rel2\binp\dll\os2edit.dll

[ BLOCK <1> clean ]
#==================
    sweep killobjs
