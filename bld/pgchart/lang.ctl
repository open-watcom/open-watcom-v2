# PGCHART Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    echo <PROJDIR> builds under GRAPHLIB

[ BLOCK <1> clean ]
#==================
    echo <PROJDIR> cleans under GRAPHLIB

