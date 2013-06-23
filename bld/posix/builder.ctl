# POSIX Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=build POSIX tools

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/cat.exe   <OWBINDIR>/cat<CMDEXT>
    <CPCMD> <OWOBJDIR>/cp.exe    <OWBINDIR>/cp<CMDEXT>
    <CPCMD> <OWOBJDIR>/egrep.exe <OWBINDIR>/egrep<CMDEXT>
    <CPCMD> <OWOBJDIR>/head.exe  <OWBINDIR>/head<CMDEXT>
    <CPCMD> <OWOBJDIR>/chmod.exe <OWBINDIR>/chmod<CMDEXT>
    <CPCMD> <OWOBJDIR>/sed.exe   <OWBINDIR>/sed<CMDEXT>
    <CPCMD> <OWOBJDIR>/sleep.exe <OWBINDIR>/sleep<CMDEXT>
    <CPCMD> <OWOBJDIR>/tee.exe   <OWBINDIR>/tee<CMDEXT>
    <CPCMD> <OWOBJDIR>/tr.exe    <OWBINDIR>/tr<CMDEXT>
    <CPCMD> <OWOBJDIR>/uniq.exe  <OWBINDIR>/uniq<CMDEXT>
    <CPCMD> <OWOBJDIR>/wc.exe    <OWBINDIR>/wc<CMDEXT>
    <CPCMD> <OWOBJDIR>/which.exe <OWBINDIR>/which<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/cat<CMDEXT>
    rm -f <OWBINDIR>/cat<CMDEXT>
    echo rm -f <OWBINDIR>/cp<CMDEXT>
    rm -f <OWBINDIR>/cp<CMDEXT>
    echo rm -f <OWBINDIR>/egrep<CMDEXT>
    rm -f <OWBINDIR>/egrep<CMDEXT>
    echo rm -f <OWBINDIR>/head<CMDEXT>
    rm -f <OWBINDIR>/head<CMDEXT>
    echo rm -f <OWBINDIR>/chmod<CMDEXT>
    rm -f <OWBINDIR>/chmod<CMDEXT>
    echo rm -f <OWBINDIR>/sed<CMDEXT>
    rm -f <OWBINDIR>/sed<CMDEXT>
    echo rm -f <OWBINDIR>/sleep<CMDEXT>
    rm -f <OWBINDIR>/sleep<CMDEXT>
    echo rm -f <OWBINDIR>/tee<CMDEXT>
    rm -f <OWBINDIR>/tee<CMDEXT>
    echo rm -f <OWBINDIR>/tr<CMDEXT>
    rm -f <OWBINDIR>/tr<CMDEXT>
    echo rm -f <OWBINDIR>/uniq<CMDEXT>
    rm -f <OWBINDIR>/uniq<CMDEXT>
    echo rm -f <OWBINDIR>/wc<CMDEXT>
    rm -f <OWBINDIR>/wc<CMDEXT>
    echo rm -f <OWBINDIR>/which<CMDEXT>
    rm -f <OWBINDIR>/which<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    # Currently not built by default

[ BLOCK . . ]
#============
cdsay <PROJDIR>
