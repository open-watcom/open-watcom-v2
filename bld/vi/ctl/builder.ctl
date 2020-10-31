# victl Builder Control file
# ============================

set PROJNAME=victl

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
    <CPCMD> <OWOBJDIR>/parsectl.exe <OWBINDIR>/<OWOBJDIR>/parsectl<CMDEXT>
    <CPCMD> <OWOBJDIR>/parsedyn.exe <OWBINDIR>/<OWOBJDIR>/parsedyn<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/parsectl<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/parsectl<CMDEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/parsedyn<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/parsedyn<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
