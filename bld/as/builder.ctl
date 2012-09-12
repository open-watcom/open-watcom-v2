# RISC assemblers Builder Control file
# ====================================

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ INCLUDE axp/builder.ctl ]
[ INCLUDE mps/builder.ctl ]
[ INCLUDE ppc/builder.ctl ]

[ BLOCK . . ]
#============
cdsay .
