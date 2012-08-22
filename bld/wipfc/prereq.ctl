set TMP_BUILD_PLATFORM=<BUILD_PLATFORM>

[ BLOCK <1> clean ]
#==================
    echo rm -f -r <PROJDIR>/<OWPREOBJDIR>
    rm -f -r <PROJDIR>/<OWPREOBJDIR>
    rm -f <OWBINDIR>/wipfc<CMDEXT>
    set BUILD_PLATFORM=

[ BLOCK <BUILD_PLATFORM> dos386 os2386 nt386 linux386 ]
#================================
    mkdir <PROJDIR>/<OWPREOBJDIR>
    cdsay <PROJDIR>/<OWPREOBJDIR>
    wmake -h -f ../<BUILD_PLATFORM>/makefile
    <CPCMD> wipfc.exe <OWBINDIR>/wipfc<CMDEXT>

[ BLOCK . . ]
#============
set BUILD_PLATFORM=<TMP_BUILD_PLATFORM>
set TMP_BUILD_PLATFORM=
