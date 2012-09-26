# BUILDER Builder Control file
# =========================

set PROJDIR=<CWD>
set PROJNAME=build utilities

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
    cdsay <PROJDIR>

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/wsplice.exe  <OWBINDIR>/wsplice<CMDEXT>
    <CPCMD> <OWOBJDIR>/sweep.exe    <OWBINDIR>/sweep<CMDEXT>
    <CPCMD> <OWOBJDIR>/cdsay.exe    <OWBINDIR>/cdsay<CMDEXT>
    <CPCMD> <OWOBJDIR>/comstrip.exe <OWBINDIR>/comstrip<CMDEXT>
    <CPCMD> <OWOBJDIR>/diff.exe     <OWBINDIR>/diff<CMDEXT>
    <CPCMD> <OWOBJDIR>/aliasgen.exe <OWBINDIR>/aliasgen<CMDEXT>
    <CPCMD> <OWOBJDIR>/genverrc.exe <OWBINDIR>/genverrc<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
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
    echo rm -f <OWBINDIR>/genverrc<CMDEXT>
    rm -f <OWBINDIR>/genverrc<CMDEXT>

[ BLOCK . . ]
#============
cdsay <PROJDIR>
