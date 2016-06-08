# SDK Builder Control file
# ============================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]

cdsay .

[ INCLUDE ddespy/builder.ctl ]
[ INCLUDE heapwalk/builder.ctl ]
[ INCLUDE spy/builder.ctl ]
[ INCLUDE drwat/builder.ctl ]
[ INCLUDE zoom/builder.ctl ]

[ INCLUDE fmedit/builder.ctl ]
[ INCLUDE wr/builder.ctl ]
[ INCLUDE wresedit/builder.ctl ]
[ INCLUDE wre/builder.ctl ]
[ INCLUDE wde/builder.ctl ]
[ INCLUDE imgedit/builder.ctl ]

cdsay .
