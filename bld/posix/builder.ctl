# POSIX Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=build POSIX tools

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BINTOOL> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> rel ]
#======================
    <CPCMD> <OWOBJDIR>/cat.exe   <OWBINDIR>/cat<CMDEXT>
    <CPCMD> <OWOBJDIR>/cp.exe    <OWBINDIR>/cp<CMDEXT>
    <CPCMD> <OWOBJDIR>/egrep.exe <OWBINDIR>/egrep<CMDEXT>
    <CPCMD> <OWOBJDIR>/chmod.exe <OWBINDIR>/chmod<CMDEXT>
    <CPCMD> <OWOBJDIR>/ls.exe    <OWBINDIR>/ls<CMDEXT>
    <CPCMD> <OWOBJDIR>/sed.exe   <OWBINDIR>/sed<CMDEXT>
    <CPCMD> <OWOBJDIR>/uniq.exe  <OWBINDIR>/uniq<CMDEXT>
    <CPCMD> <OWOBJDIR>/wc.exe    <OWBINDIR>/wc<CMDEXT>
    <CPCMD> <OWOBJDIR>/which.exe <OWBINDIR>/which<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    rm -f <OWBINDIR>/cat<CMDEXT>
    rm -f <OWBINDIR>/cp<CMDEXT>
    rm -f <OWBINDIR>/egrep<CMDEXT>
    rm -f <OWBINDIR>/chmod<CMDEXT>
    rm -f <OWBINDIR>/ls<CMDEXT>
    rm -f <OWBINDIR>/sed<CMDEXT>
    rm -f <OWBINDIR>/uniq<CMDEXT>
    rm -f <OWBINDIR>/wc<CMDEXT>
    rm -f <OWBINDIR>/which<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    # Currently not built by default

[ BLOCK . . ]
#============
cdsay <PROJDIR>
