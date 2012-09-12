# Default Builder Control File for tools created during build for internal use
# ============================================================================
#
# Build a tool which is internaly used by build and they are not created by 
# boot process, therefore they are build by new OW tools and copy it into 
# the binary directory (by example tools required C++ compiler).
# For a clean, also remove the tool from the binary directory.
# NB: PROJNAME must be the tool's base name.

cdsay <PROJDIR>

[ BLOCK .<PROJNAME> . ]
    error PROJNAME must be set!

[ BLOCK <1> build rel ]
#======================
    mkdir <PROJDIR>/<OWOBJDIR>
    cdsay <PROJDIR>/<OWOBJDIR>
    wmake -h -f ../binmake
    <CPCMD> <PROJNAME>.exe <OWBINDIR>/<PROJNAME><CMDEXT>

[ BLOCK <1> clean ]
#==================
    echo rm -r -f <PROJDIR>/<OWOBJDIR>
    rm -r -f <PROJDIR>/<OWOBJDIR>
    echo rm -f <OWBINDIR>/<PROJNAME><CMDEXT>
    rm -f <OWBINDIR>/<PROJNAME><CMDEXT>
    
[ BLOCK . . ]
#============
