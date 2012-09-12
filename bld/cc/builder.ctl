# C compiler Builder Control file
# ===============================

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ INCLUDE i86/builder.ctl ]
[ INCLUDE 386/builder.ctl ]
[ INCLUDE axp/builder.ctl ]
[ INCLUDE mps/builder.ctl ]
[ INCLUDE ppc/builder.ctl ]

[ BLOCK . . ]
#============
cdsay .
