# wpp386 Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> bootclean ]
#======================
    echo rm -f -r <PROJDIR>/<OBJDIR>.386
    rm -f -r <PROJDIR>/<OBJDIR>.386
    echo rm -f <OWBINDIR>/wpp386<CMDEXT>
    rm -f <OWBINDIR>/wpp386<CMDEXT>

[ BLOCK <1> boot ]
#=================
    echo Building the wpp386 bootstrap
    mkdir <PROJDIR>/<OBJDIR>.386
    cdsay <PROJDIR>/<OBJDIR>.386
    cp -f ../linux386.386/target.h .
    wmake -h -f ../make386
    <CPCMD> wcpp386.exe <OWBINDIR>/wpp386<CMDEXT>
    cdsay <PROJDIR>
