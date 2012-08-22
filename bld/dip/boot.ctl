# DIP Builder Control file
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
    echo Building the DIP bootstrap
    mkdir <PROJDIR>/dwarf/<OWOBJDIR>
    cdsay <PROJDIR>/dwarf/<OWOBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> <PROJDIR>/dwarf/<OWOBJDIR>/dwarf.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/watcom/<OWOBJDIR>
    cdsay <PROJDIR>/watcom/<OWOBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> <PROJDIR>/watcom/<OWOBJDIR>/watcom.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/codeview/<OWOBJDIR>
    cdsay <PROJDIR>/codeview/<OWOBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> <PROJDIR>/codeview/<OWOBJDIR>/codeview.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/export/<OWOBJDIR>
    cdsay <PROJDIR>/export/<OWOBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> <PROJDIR>/export/<OWOBJDIR>/export.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/mapsym/<OWOBJDIR>
    cdsay <PROJDIR>/mapsym/<OWOBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> <PROJDIR>/mapsym/<OWOBJDIR>/mapsym.so <DEVDIR>/build/bin/
    cdsay <PROJDIR>
