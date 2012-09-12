# BUILDER Builder Control file
# =========================

set PROJDIR=<CWD>
set PROJNAME=build utilities

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK <1> boot ]
#=================
    mkdir <PROJDIR>/<OWOBJDIR>
    cdsay <PROJDIR>/<OWOBJDIR>
    wmake -h -f ../bootmake

[ BLOCK <1> bootclean ]
#======================
    cdsay <PROJDIR>
    echo rm -f -r <PROJDIR>/<OWOBJDIR>
    rm -f -r <PROJDIR>/<OWOBJDIR>
    echo rm -f <OWBINDIR>/wsplice<CMDEXT>
    rm -f <OWBINDIR>/wsplice<CMDEXT>
    echo rm -f <OWBINDIR>/sweep<CMDEXT>
    rm -f <OWBINDIR>/sweep<CMDEXT>
    echo rm -f <OWBINDIR>/cdsay<CMDEXT>
    rm -f <OWBINDIR>/cdsay<CMDEXT>
    echo rm -f <OWBINDIR>/comstrip<CMDEXT>
    rm -f <OWBINDIR>/comstrip<CMDEXT>
    echo rm -f <OWBINDIR>/diff<CMDEXT>
    rm -f <OWBINDIR>/diff<CMDEXT>
    echo rm -f <OWBINDIR>/aliasgen<CMDEXT>
    rm -f <OWBINDIR>/aliasgen<CMDEXT>
    echo rm -f <OWBINDIR>/wgrep<CMDEXT>
    rm -f <OWBINDIR>/wgrep<CMDEXT>
    echo rm -f <OWBINDIR>/genverrc<CMDEXT>
    rm -f <OWBINDIR>/genverrc<CMDEXT>

[ BLOCK . . ]
#============
cdsay <PROJDIR>
