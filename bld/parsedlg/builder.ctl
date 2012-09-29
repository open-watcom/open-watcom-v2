# parsedlg Builder Control file
# =============================

set PROJDIR=<CWD>
set PROJNAME=parsedlg

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/bintool.ctl ]

[ BLOCK <1> rel cprel ]
#======================
    # Currently not built by default

[ BLOCK . . ]
#============
cdsay <PROJDIR>
