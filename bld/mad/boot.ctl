# MAD Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/x86/<OBJDIR>
    rm -f -r <PROJDIR>/x86/<OBJDIR>

[ BLOCK <1> boot ]
#=================
    echo Building the MAD bootstrap
    mkdir <PROJDIR>/x86/<OBJDIR>
    cdsay <PROJDIR>/x86/<OBJDIR>
    wmake -h -f ../../bootmake
    <CPCMD> <PROJDIR>/x86/<OBJDIR>/madx86.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/ppc/<OBJDIR>
    cdsay <PROJDIR>/ppc/<OBJDIR>
    wmake -h -f ../../bootmake
    <CPCMD> <PROJDIR>/ppc/<OBJDIR>/madppc.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/mips/<OBJDIR>
    cdsay <PROJDIR>/mips/<OBJDIR>
    wmake -h -f ../../bootmake
    <CPCMD> <PROJDIR>/mips/<OBJDIR>/madmips.so <DEVDIR>/build/bin/
    cdsay <PROJDIR>
