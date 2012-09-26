# Default Builder Control File for libraries
# ==========================================
#
# Build libraries in place, do not copy any files around.

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
    mkdir <PROJDIR>/<OWOBJDIR>
    cdsay <PROJDIR>/<OWOBJDIR>
    wmake -h -f ../binmake bootstrap=1

[ BLOCK <1> bootclean ]
#======================
    echo rm -r -f <PROJDIR>/<OWOBJDIR>
    rm -r -f <PROJDIR>/<OWOBJDIR>

[ BLOCK . . ]
#============
