# WRES Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LEXLIB>\builder\logfile.ctl ]
[ LOG <LGFILE>.<LOGEXT> ]

echo WRES Control File
echo ------------------
echo.


[ INCLUDE <LEXLIB>\builder\builder.ctl ]
[ INCLUDE <LEXLIB>\builder\wproj.ctl ]

    
[ BLOCK <1> build buildmin rel2 ]
#=======================
    cdsay of
    wmake /h /i lexusbld
    cd <PROJDIR>
    cdsay of.dll
    wmake /h /i lexusbld
    cd <PROJDIR>
    
[ BLOCK <1> rel2 cprel2 ]
#========================
    # WRES has no installable components
    
[ BLOCK <1> clean ]
#==================
    cdsay of
    wmake /h /i clean
    cd <PROJDIR>
    cdsay of.dll
    wmake /h /i clean
    cd <PROJDIR>
