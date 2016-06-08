# MAD Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

[ INCLUDE x86/builder.ctl ]
[ INCLUDE axp/builder.ctl ]
[ INCLUDE ppc/builder.ctl ]
[ INCLUDE mps/builder.ctl ]

cdsay .
