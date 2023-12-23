# OMF tools Builder Control file
# ==============================

set PROJNAME=omftools

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BINTOOL> build ]
#========================
    cdsay "<PROJDIR>"
    <CPCMD> <OWOBJDIR>/objchg.exe   "<OWROOT>/build/<OWOBJDIR>/objchg<CMDEXT>"
    <CPCMD> <OWOBJDIR>/objfind.exe  "<OWROOT>/build/<OWOBJDIR>/objfind<CMDEXT>"
    <CPCMD> <OWOBJDIR>/objlist.exe  "<OWROOT>/build/<OWOBJDIR>/objlist<CMDEXT>"
    <CPCMD> <OWOBJDIR>/objxdef.exe  "<OWROOT>/build/<OWOBJDIR>/objxdef<CMDEXT>"
    <CPCMD> <OWOBJDIR>/objxref.exe  "<OWROOT>/build/<OWOBJDIR>/objxref<CMDEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/objchg<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/objchg<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/objfind<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/objfind<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/objlist<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/objlist<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/objxdef<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/objxdef<CMDEXT>"
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/objxref<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/objxref<CMDEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
