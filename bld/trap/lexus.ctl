# Trap Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <LEXLIB>\builder\logfile.ctl ]
[ LOG <LGFILE>.<LOGEXT> ]

echo Trap Control File
echo ------------------
echo.

[ INCLUDE <LEXLIB>\builder\builder.ctl ]
[ INCLUDE <LEXLIB>\builder\wproj.ctl ]

    
[ BLOCK <1> build buildmin rel2 ]
#=======================
    cdsay <PROJDIR>\lcl\nt\std
    wmake /h /i w lexusbld=1
    cdsay <PROJDIR>\tcp\nt.srv
    wmake /h /i w lexusbld=1
    cdsay <PROJDIR>\tcp\nt.trp
    wmake /h /i w lexusbld=1
    cdsay <PROJDIR>\lcl\msj\nt.trp
    wmake /h /i w lexusbld=1
    
    
[ BLOCK <1> rel2 cprel2 ]
#========================
    echo ******* TODO! REL2 copy rule for Lexus ********
    
[ BLOCK <1> clean ]
#==================
    cdsay <PROJDIR>\lcl\nt\std
    rm *.obj
    cdsay <PROJDIR>\tcp\nt.srv
    rm *.obj
    cdsay <PROJDIR>\tcp\nt.trp
    rm *.obj
    cdsay <PROJDIR>\msj\nt.srv
    rm *.obj
