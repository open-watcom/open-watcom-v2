# Installer builder control file
# ==============================

set PROJNAME=install

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

cdsay .

echo Installer Build: <1> <2> <3> <4> <5>

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> missing ]
#====================
    #########################################################
    # this is hack used by build server to create installers
    # mksetup -x create all missing files with zero length
    # it is dedicated for missing help files which are not
    # buildable on each host platform
    #########################################################
    wmake -h -f master.mif missing=1

[ BLOCK <1> clean ]
#==================
    rm -rf bin

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
