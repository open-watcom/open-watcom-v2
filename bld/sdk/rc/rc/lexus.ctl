# RC Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LEXLIB>\builder\logfile.ctl ]
[ LOG <LGFILE>.<LOGEXT> ]

echo RC Control File
echo --------------
echo.


[ INCLUDE <LEXLIB>\builder\builder.ctl ]
[ INCLUDE <LEXLIB>\builder\wproj.ctl ]

    
[ BLOCK <1> build buildmin rel2 ]
#=======================
    cdsay ntrtdll
    wmake /h /i
    cd <PROJDIR>
    
[ BLOCK <1> rel2 cprel2 ]
#========================
    
[ BLOCK <1> clean ]
#==================
    cdsay ntrtdll
    wmake /h /i clean
    cd <PROJDIR>
