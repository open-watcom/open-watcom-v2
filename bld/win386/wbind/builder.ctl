# wbind Builder Control file
# ============================

set PROJNAME=wbind

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/wbind.exe <OWBINDIR>/<OWOBJDIR>/bwbind<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bwbind<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwbind<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
