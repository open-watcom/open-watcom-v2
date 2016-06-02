# Default actions Builder Control File 
# ====================================

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
    set BINTOOL=build

[ BLOCK <1> bootclean ]
#======================
    echo rm -rf <OWOBJDIR>
    rm -rf <OWOBJDIR>
    set BINTOOL=clean

[ BLOCK . . ]
#============
