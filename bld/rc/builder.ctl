# Resource tools Builder Control file
# ===================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

[ INCLUDE rc/builder.ctl ]
[ INCLUDE mkcdpg/builder.ctl ]
[ INCLUDE exedmp/builder.ctl ]
[ INCLUDE restest/builder.ctl ]
[ INCLUDE wresdmp/builder.ctl ]

cdsay .
