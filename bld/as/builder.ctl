# RISC assemblers Builder Control file
# ====================================

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ INCLUDE axp/inline/builder.ctl ]
[ INCLUDE axp/builder.ctl ]
[ INCLUDE mps/inline/builder.ctl ]
[ INCLUDE mps/builder.ctl ]
[ INCLUDE ppc/inline/builder.ctl ]
[ INCLUDE ppc/builder.ctl ]

[ BLOCK . . ]
#============
cdsay .
