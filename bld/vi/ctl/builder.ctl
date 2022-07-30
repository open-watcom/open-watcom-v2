# v.ctl" Builder Control file
# ============================

set PROJNAME=victl

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
    <CPCMD> <OWOBJDIR>/parsectl.exe "<OWROOT>/build/<OWOBJDIR>/parsectl<CMDEXT>"
    <CPCMD> <OWOBJDIR>/parsedyn.exe "<OWROOT>/build/<OWOBJDIR>/parsedyn<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/parsectl<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/parsectl<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/parsedyn<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/parsedyn<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
