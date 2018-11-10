# UI lib Builder Control file
# ===========================

set PROJNAME=ui

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
#    <CCCMD> qnx/tix/*.tix <OWRELROOT>/qnx/tix/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
