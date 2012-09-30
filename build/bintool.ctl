# Default Builder Control File for tools created during build for internal use
# ============================================================================
#
# Build a tool which is internaly used by build and they are not created by 
# boot process, therefore they are build by new OW tools and copy it into 
# the binary directory (by example tools required C++ compiler).
# For a clean, also remove the tool from the binary directory.
# NB: PROJNAME must be the tool's base name.

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
    
[ BLOCK <1> build rel ]
#======================
    mkdir <OWOBJDIR>
    cdsay <OWOBJDIR>
    wmake -h -f ../binmake
    cdsay ..
    <CPCMD> <OWOBJDIR>/<PROJNAME>.exe <OWBINDIR>/<PROJNAME><CMDEXT>
    set BINTOOL=rel

[ BLOCK <1> clean ]
#==================
    cdsay .
    echo rm -r -f <OWOBJDIR>
    rm -r -f <OWOBJDIR>
    echo rm -f <OWBINDIR>/<PROJNAME><CMDEXT>
    rm -f <OWBINDIR>/<PROJNAME><CMDEXT>
    set BINTOOL=clean

[ BLOCK . . ]
#============
