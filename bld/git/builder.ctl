# WATCOM GIT filter driver Builder Control file
# ==============================================

set PROJNAME=git

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay "<PROJDIR>"
    <CPCMD> <OWOBJDIR>/cpyright.exe "<OWROOT>/build/<OWOBJDIR>/cpyright<CMDEXT>"
    <CPCMD> <OWOBJDIR>/cvtutf8.exe  "<OWROOT>/build/<OWOBJDIR>/cvtutf8<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/cpyright<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/cpyright<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/cvtutf8<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/cvtutf8<CMDEXT>"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
