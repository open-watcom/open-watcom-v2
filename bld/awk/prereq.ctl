# AWK Builder Control file
# ==========================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<OWPREOBJDIR>
    rm -f -r <PROJDIR>/<OWPREOBJDIR>
    rm -f <OWBINDIR>/awk<CMDEXT>
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> dos386 os2386 nt386 ntaxp linux386 ]
#============================================================
    mkdir <PROJDIR>/<OWPREOBJDIR>
    cdsay <PROJDIR>/<OWPREOBJDIR>
    wmake -h -f ../<BUILD_PLATFORM>/makefile prebuild=1
    <CPCMD> awk.exe <OWBINDIR>/awk<CMDEXT>

[ BLOCK . . ]
#============

cdsay <PROJDIR>
