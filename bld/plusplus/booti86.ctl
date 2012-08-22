# wpp Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> bootclean ]
#======================
    echo rm -f -r <PROJDIR>/<OWOBJDIR>.i86
    rm -f -r <PROJDIR>/<OWOBJDIR>.i86
    echo rm -f <OWBINDIR>/wpp<CMDEXT>
    rm -f <OWBINDIR>/wpp<CMDEXT>

[ BLOCK <1> boot ]
#=================
    echo Building the wpp bootstrap
    mkdir <PROJDIR>/<OWOBJDIR>.i86
    cdsay <PROJDIR>/<OWOBJDIR>.i86
    cp -f ../linux386.i86/target.h .
    wmake -h -f ../makei86
    <CPCMD> wcppi86.exe <OWBINDIR>/wpp<CMDEXT>
    cdsay <PROJDIR>
