# VIPER Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

[ INCLUDE lib/builder.ctl ]
[ INCLUDE ide2make/builder.ctl ]
[ INCLUDE ide/builder.ctl ]
[ INCLUDE cfg/builder.ctl ]

cdsay .
