# MAD Builder Control file
# ========================

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ INCLUDE x86/builder.ctl ]
[ INCLUDE axp/builder.ctl ]
[ INCLUDE ppc/builder.ctl ]
[ INCLUDE mips/builder.ctl ]

[ BLOCK . . ]
#============
cdsay .
