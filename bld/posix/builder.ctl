# POSIX Builder Control file
# ==========================

set PROJNAME=posixutl

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/cat.exe   <OWBINDIR>/<OWOBJDIR>/cat<CMDEXT>
    <CPCMD> <OWOBJDIR>/egrep.exe <OWBINDIR>/<OWOBJDIR>/egrep<CMDEXT>
    <CPCMD> <OWOBJDIR>/head.exe  <OWBINDIR>/<OWOBJDIR>/head<CMDEXT>
    <CPCMD> <OWOBJDIR>/chmod.exe <OWBINDIR>/<OWOBJDIR>/chmod<CMDEXT>
    <CPCMD> <OWOBJDIR>/sed.exe   <OWBINDIR>/<OWOBJDIR>/sed<CMDEXT>
    <CPCMD> <OWOBJDIR>/sleep.exe <OWBINDIR>/<OWOBJDIR>/sleep<CMDEXT>
    <CPCMD> <OWOBJDIR>/sort.exe  <OWBINDIR>/<OWOBJDIR>/sort<CMDEXT>
    <CPCMD> <OWOBJDIR>/tee.exe   <OWBINDIR>/<OWOBJDIR>/tee<CMDEXT>
    <CPCMD> <OWOBJDIR>/tr.exe    <OWBINDIR>/<OWOBJDIR>/tr<CMDEXT>
    <CPCMD> <OWOBJDIR>/uniq.exe  <OWBINDIR>/<OWOBJDIR>/uniq<CMDEXT>
    <CPCMD> <OWOBJDIR>/wc.exe    <OWBINDIR>/<OWOBJDIR>/wc<CMDEXT>
    <CPCMD> <OWOBJDIR>/which.exe <OWBINDIR>/<OWOBJDIR>/which<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/cat<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/cat<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/egrep<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/egrep<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/head<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/head<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/chmod<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/chmod<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/sed<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/sed<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/sleep<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/sleep<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/sort<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/sort<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/tee<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/tee<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/tr<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/tr<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/uniq<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/uniq<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/wc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/wc<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/which<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/which<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
