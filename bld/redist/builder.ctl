# Redistributable binary files control file
# =========================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel ]
#======================

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> dos4gw/*.exe            <OWRELROOT>/binw/
    <CPCMD> dos4gw/*.doc            <OWRELROOT>/binw/
    <CPCMD> dos32a/*                <OWRELROOT>/binw/
    <CPCMD> pmodew/*                <OWRELROOT>/binw/

[ BLOCK <1> clean ]
#==================

[ BLOCK . . ]
#============
cdsay <PROJDIR>
