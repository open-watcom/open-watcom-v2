set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/wres/<OBJDIR>
    rm -f -r <PROJDIR>/wres/<OBJDIR>
    echo rm -f -r <PROJDIR>/rc/<OBJDIR>
    rm -f -r <PROJDIR>/rc/<OBJDIR>

[ BLOCK <1> boot ]
#=================
    echo Building the resource compiler bootstrap
    mkdir <PROJDIR>/wres/<OBJDIR>
    cdsay <PROJDIR>/wres/<OBJDIR>
    wmake -h -f ../bootmake
    mkdir ../../rc/<OBJDIR>
    cdsay ../../rc/<OBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> wrce.exe <OWROOT>/bld/build/bin/wrc
    <CPCMD> wrce.exe <OWROOT>/bld/build/bin/bwrc
    cdsay <PROJDIR>
