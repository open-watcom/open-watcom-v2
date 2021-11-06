# BUILDER Builder Control file
# =========================

set PROJNAME=buildutl

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/wsplice.exe  <OWBINDIR>/<OWOBJDIR>/wsplice<CMDEXT>
    <CPCMD> <OWOBJDIR>/sweep.exe    <OWBINDIR>/<OWOBJDIR>/sweep<CMDEXT>
    <CPCMD> <OWOBJDIR>/comstrip.exe <OWBINDIR>/<OWOBJDIR>/comstrip<CMDEXT>
    <CPCMD> <OWOBJDIR>/genverrc.exe <OWBINDIR>/<OWOBJDIR>/genverrc<CMDEXT>
    <CCCMD> <OWOBJDIR>/diff.exe     <OWBINDIR>/<OWOBJDIR>/diff<CMDEXT>
    <CCCMD> <OWOBJDIR>/crlf.exe     <OWBINDIR>/<OWOBJDIR>/crlf<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/wsplice<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/wsplice<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/sweep<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/sweep<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/comstrip<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/comstrip<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/genverrc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/genverrc<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/diff<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/diff<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/crlf<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/crlf<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
