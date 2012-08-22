# MAD Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/x86/<OWOBJDIR>
    rm -f -r <PROJDIR>/x86/<OWOBJDIR>

[ BLOCK <1> boot ]
#=================
    echo Building the MAD bootstrap
    mkdir <PROJDIR>/x86/<OWOBJDIR>
    cdsay <PROJDIR>/x86/<OWOBJDIR>
    wmake -h -f ../../bootmake
    <CPCMD> <PROJDIR>/x86/<OWOBJDIR>/madx86.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/ppc/<OWOBJDIR>
    cdsay <PROJDIR>/ppc/<OWOBJDIR>
    wmake -h -f ../../bootmake
    <CPCMD> <PROJDIR>/ppc/<OWOBJDIR>/madppc.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/mips/<OWOBJDIR>
    cdsay <PROJDIR>/mips/<OWOBJDIR>
    wmake -h -f ../../bootmake
    <CPCMD> <PROJDIR>/mips/<OWOBJDIR>/madmips.so <DEVDIR>/build/bin/
    cdsay <PROJDIR>
