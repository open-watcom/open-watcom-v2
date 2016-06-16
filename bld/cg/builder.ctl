# codegen Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

[ INCLUDE intel/386/builder.ctl ]
[ INCLUDE intel/i86/builder.ctl ]
[ INCLUDE risc/axp/builder.ctl ]
[ INCLUDE risc/mps/builder.ctl ]
[ INCLUDE risc/ppc/builder.ctl ]

cdsay .
