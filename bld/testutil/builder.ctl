# testutil Builder Control file
# ==============================

set PROJNAME=testutil

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
    <CPCMD> <OWOBJDIR>/chk_exec.exe "<OWROOT>/build/<OWOBJDIR>/chk_exec<CMDEXT>"
    <CPCMD> <OWOBJDIR>/chktest.exe  "<OWROOT>/build/<OWOBJDIR>/chktest<CMDEXT>"
    <CPCMD> <OWOBJDIR>/catfile.exe  "<OWROOT>/build/<OWOBJDIR>/catfile<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/chk_exec<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/chk_exec<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/chktest<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/chktest<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/catfile<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/catfile<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
