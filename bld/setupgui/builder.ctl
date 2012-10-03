# setupgui Builder Control file
# =============================
 
set PROJDIR=<CWD>
set PROJNAME=setupgui
 
[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]
 
[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK . . ]
#============
cdsay <PROJDIR>

[ INCLUDE mkdisk/builder.ctl ]
