# FORTRAN Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

[ INCLUDE wfc/builder.ctl ]
[ INCLUDE wfl/builder.ctl ]
[ INCLUDE f77lib/builder.ctl ]

cdsay .
