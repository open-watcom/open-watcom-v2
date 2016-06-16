# vicomp Builder Control file
# ============================

set PROJNAME=vicomp

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
    <CPCMD> <OWOBJDIR>/vicomp.exe   <OWBINDIR>/vicomp<CMDEXT>
    <CPCMD> <OWOBJDIR>/parsectl.exe <OWBINDIR>/parsectl<CMDEXT>
    <CPCMD> <OWOBJDIR>/parsedyn.exe <OWBINDIR>/parsedyn<CMDEXT>
    <CPCMD> <OWOBJDIR>/edbind.exe   <OWBINDIR>/bedbind<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/vicomp<CMDEXT>
    rm -f <OWBINDIR>/vicomp<CMDEXT>
    echo rm -f <OWBINDIR>/parsectl<CMDEXT>
    rm -f <OWBINDIR>/parsectl<CMDEXT>
    echo rm -f <OWBINDIR>/parsedyn<CMDEXT>
    rm -f <OWBINDIR>/parsedyn<CMDEXT>
    echo rm -f <OWBINDIR>/bedbind<CMDEXT>
    rm -f <OWBINDIR>/bedbind<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
