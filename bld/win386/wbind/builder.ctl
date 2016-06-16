# wbind Builder Control file
# ============================

set PROJNAME=wbind

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
    <CPCMD> <OWOBJDIR>/wbind.exe <OWBINDIR>/bwbind<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwbind<CMDEXT>
    rm -f <OWBINDIR>/bwbind<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
