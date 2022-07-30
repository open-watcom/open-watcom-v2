# BUILDER Builder Control file
# =========================

set PROJNAME=buildutl

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay "<PROJDIR>"

[ BLOCK <BINTOOL> build ]
#========================
    cdsay "<PROJDIR>"
    <CPCMD> <OWOBJDIR>/wsplice.exe  "<OWROOT>/build/<OWOBJDIR>/wsplice<CMDEXT>"
    <CPCMD> <OWOBJDIR>/sweep.exe    "<OWROOT>/build/<OWOBJDIR>/sweep<CMDEXT>"
    <CPCMD> <OWOBJDIR>/comstrip.exe "<OWROOT>/build/<OWOBJDIR>/comstrip<CMDEXT>"
    <CPCMD> <OWOBJDIR>/genverrc.exe "<OWROOT>/build/<OWOBJDIR>/genverrc<CMDEXT>"
    <CCCMD> <OWOBJDIR>/diff.exe     "<OWROOT>/build/<OWOBJDIR>/diff<CMDEXT>"
    <CCCMD> <OWOBJDIR>/crlf.exe     "<OWROOT>/build/<OWOBJDIR>/crlf<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/wsplice<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/wsplice<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/sweep<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/sweep<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/comstrip<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/comstrip<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/genverrc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/genverrc<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/diff<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/diff<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/crlf<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/crlf<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
