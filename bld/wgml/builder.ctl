# WGML Builder Control file
# =========================

set PROJNAME=wgml

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay "<PROJDIR>"
    <CPCMD> <OWOBJDIR>/wgml.exe "<OWROOT>/build/<OWOBJDIR>/wgml<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/wgml<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/wgml<CMDEXT>"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
