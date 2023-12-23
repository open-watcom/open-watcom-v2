# F77 errsrc Builder Control file
# ================================

set PROJNAME=errsrc

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay "<PROJDIR>"
    <CPCMD> <OWOBJDIR>/errsrc.exe  "<OWROOT>/build/<OWOBJDIR>/errsrc<CMDEXT>"
    <CPCMD> <OWOBJDIR>/errdoc.exe  "<OWROOT>/build/<OWOBJDIR>/errdoc<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/errsrc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/errsrc<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/errdoc<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/errdoc<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
