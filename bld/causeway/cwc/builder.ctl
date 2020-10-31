# CauseWay compressor Builder Control file
# ========================================

set PROJNAME=cwc

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
    <CPCMD> <OWOBJDIR>/cwc.exe <OWBINDIR>/<OWOBJDIR>/bcwc<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bcwc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bcwc<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
