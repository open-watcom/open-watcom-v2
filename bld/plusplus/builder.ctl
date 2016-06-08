# C++ compiler Builder Control file
# =================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

[ INCLUDE i86/builder.ctl ]
[ INCLUDE 386/builder.ctl ]
[ INCLUDE axp/builder.ctl ]

cdsay .
