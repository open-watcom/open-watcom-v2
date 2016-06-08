# Default Builder Control File for dynamic libraries
# ==================================================
#
# Build a dynamic lib and for boot builds, copy it into the binary directory.
# For a boot clean, also remove the dynlib from the binary directory.
# For normal builds, copying needs to be done from the .ctl file which
# included this one.
# NB: PROJNAME must be the library's base name.

cdsay <PROJDIR>

[ BLOCK .<PROJNAME> . ]
    error PROJNAME must be set!

[ BLOCK .<OWGUINOBUILD> .1 ]
    set PROJPMAKE=guitool .not .and

[ BLOCK <1> build rel ]
#======================
    pmake -d build <PROJPMAKE> <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> clean ]
#==================
    pmake -d build <PROJPMAKE> <2> <3> <4> <5> <6> <7> <8> <9> -h clean
    
[ BLOCK <1> boot ]
#=================
    mkdir <OWOBJDIR>
    cdsay <OWOBJDIR>
    wmake -h -f ../binmake bootstrap=1
    cdsay ..
    <CPCMD> <OWOBJDIR>/<PROJNAME><DYEXT> <OWBINDIR>/<PROJNAME><DYEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -rf <OWOBJDIR>
    rm -rf <OWOBJDIR>
    echo rm -f <OWBINDIR>/<PROJNAME><DYEXT>
    rm -f <OWBINDIR>/<PROJNAME><DYEXT>

[ BLOCK . . ]
    set PROJPMAKE=
