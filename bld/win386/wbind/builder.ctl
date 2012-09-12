# wbind Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=wbind

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/wbind.exe <OWBINDIR>/bwbind<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwbind<CMDEXT>
    rm -f <OWBINDIR>/bwbind<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    # Currently not built by default

[ BLOCK . . ]
#============
cdsay <PROJDIR>
