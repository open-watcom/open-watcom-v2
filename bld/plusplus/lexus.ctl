# PLUSPLUS Builder Control file
# =============================

set PROJDIR=<CWD>

[ INCLUDE <LEXLIB>\builder\logfile.ctl ]
[ LOG <LGFILE>.<LOGEXT> ]

echo PLUSPLUS Control File
echo ---------------------
echo.


[ INCLUDE <LEXLIB>\builder\builder.ctl ]
[ INCLUDE <LEXLIB>\builder\wproj.ctl ]

    
[ BLOCK <1> build rel2 ]
#=======================
    # nothing to build: only the source is used
    
[ BLOCK <1> rel2 cprel2 ]
#========================
    
[ BLOCK <1> clean ]
#==================
    # nothing is built in PLUSPLUS, so nothing to clean
    
