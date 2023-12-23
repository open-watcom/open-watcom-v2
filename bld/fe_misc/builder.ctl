# FEMISC Builder Control file
# ===========================

set PROJNAME=femisc

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay "<PROJDIR>"
    <CPCMD> <OWOBJDIR>/optencod.exe  "<OWROOT>/build/<OWOBJDIR>/optencod<CMDEXT>"
    <CPCMD> <OWOBJDIR>/msgencod.exe  "<OWROOT>/build/<OWOBJDIR>/msgencod<CMDEXT>"
    <CPCMD> <OWOBJDIR>/findhash.exe  "<OWROOT>/build/<OWOBJDIR>/findhash<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/optencod<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/optencod<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/msgencod<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/msgencod<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/findhash<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/findhash<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
