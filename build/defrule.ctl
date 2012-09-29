# Default actions Builder Control File 
# ====================================

set BINTOOL=0

cdsay <PROJDIR>

[ BLOCK .<PROJNAME> . ]
    error PROJNAME must be set!

[ BLOCK <1> build rel ]
#======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    
[ BLOCK <1> boot ]
#=================
    mkdir <OWOBJDIR>
    cdsay <OWOBJDIR>
    wmake -h -f ../binmake bootstrap=1
    set BINTOOL=rel

[ BLOCK <1> bootclean ]
#======================
    echo rm -r -f <OWOBJDIR>
    rm -r -f <OWOBJDIR>
    set BINTOOL=clean

[ BLOCK . . ]
#============
