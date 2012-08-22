set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/wres/<OWOBJDIR>
    rm -f -r <PROJDIR>/wres/<OWOBJDIR>
    echo rm -f -r <PROJDIR>/rc/<OWOBJDIR>
    rm -f -r <PROJDIR>/rc/<OWOBJDIR>

[ BLOCK <1> boot ]
#=================
    echo Building the resource compiler bootstrap
    mkdir <PROJDIR>/wres/<OWOBJDIR>
    cdsay <PROJDIR>/wres/<OWOBJDIR>
    wmake -h -f ../bootmake
    mkdir ../../rc/<OWOBJDIR>
    cdsay ../../rc/<OWOBJDIR>
    wmake -h -f ../bootmake
    <CPCMD> wrce.exe <OWROOT>/bld/build/bin/wrc
    <CPCMD> wrce.exe <OWROOT>/bld/build/bin/bwrc
    cdsay <PROJDIR>
