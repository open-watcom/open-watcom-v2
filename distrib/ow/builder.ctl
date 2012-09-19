# Installer builder control file
# ==============================

set INSTDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

echo Installer Build: <1> <2> <3> <4> <5>

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> missing ]
    #########################################################
    # this is hack used by build server to create installers
    # mksetup -x create all missing files with zero length
    # it is dedicated for missing help files which are not
    # buildable on each host platform
    #########################################################
    langdat c
    mkinf -x -i../include c filelist <OWRELROOT>
    langdat f77
    mkinf -x -i../include f77 filelist <OWRELROOT>
    rm filelist

[ BLOCK . . ]
cdsay .
