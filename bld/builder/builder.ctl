# BUILDER Builder Control file
# =========================

set PROJNAME=buildutl

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/wsplice.exe  <OWBINDIR>/wsplice<CMDEXT>
    <CPCMD> <OWOBJDIR>/sweep.exe    <OWBINDIR>/sweep<CMDEXT>
    <CPCMD> <OWOBJDIR>/comstrip.exe <OWBINDIR>/comstrip<CMDEXT>
    <CPCMD> <OWOBJDIR>/genverrc.exe <OWBINDIR>/genverrc<CMDEXT>
    <CCCMD> <OWOBJDIR>/diff.exe     <OWBINDIR>/diff<CMDEXT>
    <CCCMD> <OWOBJDIR>/crlf.exe     <OWBINDIR>/crlf<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/wsplice<CMDEXT>
    rm -f <OWBINDIR>/wsplice<CMDEXT>
    echo rm -f <OWBINDIR>/sweep<CMDEXT>
    rm -f <OWBINDIR>/sweep<CMDEXT>
    echo rm -f <OWBINDIR>/comstrip<CMDEXT>
    rm -f <OWBINDIR>/comstrip<CMDEXT>
    echo rm -f <OWBINDIR>/genverrc<CMDEXT>
    rm -f <OWBINDIR>/genverrc<CMDEXT>
    echo rm -f <OWBINDIR>/diff<CMDEXT>
    rm -f <OWBINDIR>/diff<CMDEXT>
    echo rm -f <OWBINDIR>/crlf<CMDEXT>
    rm -f <OWBINDIR>/crlf<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
