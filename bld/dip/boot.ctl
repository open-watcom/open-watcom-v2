# DIP Builder Control file
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
    echo Building the DIP bootstrap
    mkdir <PROJDIR>/dwarf/<OBJDIR>
    cdsay <PROJDIR>/dwarf/<OBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> <PROJDIR>/dwarf/<OBJDIR>/dwarf.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/watcom/<OBJDIR>
    cdsay <PROJDIR>/watcom/<OBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> <PROJDIR>/watcom/<OBJDIR>/watcom.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/codeview/<OBJDIR>
    cdsay <PROJDIR>/codeview/<OBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> <PROJDIR>/codeview/<OBJDIR>/codeview.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/export/<OBJDIR>
    cdsay <PROJDIR>/export/<OBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> <PROJDIR>/export/<OBJDIR>/export.so <DEVDIR>/build/bin/
    mkdir <PROJDIR>/mapsym/<OBJDIR>
    cdsay <PROJDIR>/mapsym/<OBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> <PROJDIR>/mapsym/<OBJDIR>/mapsym.so <DEVDIR>/build/bin/
    cdsay <PROJDIR>
