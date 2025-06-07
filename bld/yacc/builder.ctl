# YACC Builder Control file
# =========================

set PROJNAME=yacc

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK <BINTOOL> build ]
#========================
    cdsay "<PROJDIR>"
    <CPCMD> <OWOBJDIR>/byacc.exe "<OWROOT>/build/<OWOBJDIR>/byacc<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/byacc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/byacc<CMDEXT>"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]

