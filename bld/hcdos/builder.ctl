# hcdos Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=hcdos

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/binrule.ctl ]

[ BLOCK <1> build rel ]
    cdsay <PROJDIR>

[ BLOCK <1> build rel ]
#======================
    <CPCMD> <OWOBJDIR>/hcdos.exe <OWBINDIR>/hcdos<CMDEXT>

[ BLOCK <1> clean ]
#==================
    echo rm -f <OWBINDIR>/hcdos<CMDEXT>
    rm -f <OWBINDIR>/hcdos<CMDEXT>

[ BLOCK . . ]
#============
cdsay <PROJDIR>
