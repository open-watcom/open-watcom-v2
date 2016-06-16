# RISC assemblers Builder Control file
# ====================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

[ INCLUDE axp/builder.ctl ]
[ INCLUDE mps/builder.ctl ]
[ INCLUDE ppc/builder.ctl ]

cdsay .
