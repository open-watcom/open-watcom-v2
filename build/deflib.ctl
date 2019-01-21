# Default Builder Control File for libraries
# ==========================================
#
# Build libraries in place, do not copy any files around.

cdsay <PROJDIR>

[ BLOCK .<PROJNAME> . ]
    error PROJNAME must be set!

[ BLOCK .<OWGUINOBUILD> .1 ]
    set PROJPMAKE=guitool .not .and

[ BLOCK <BLDRULE> build rel ]
#============================
    pmake -d build <PROJPMAKE> <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <BLDRULE> clean ]
#========================
    pmake -d build <PROJPMAKE> <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK <BLDRULE> boot ]
#=======================
    mkdir <OWOBJDIR>
    cdsay <OWOBJDIR>
    wmake -h -f ../binmake bootstrap=1

[ BLOCK <BLDRULE> bootclean ]
#============================
    echo rm -rf <OWOBJDIR>
    rm -rf <OWOBJDIR>

[ BLOCK . . ]
    set PROJPMAKE=
