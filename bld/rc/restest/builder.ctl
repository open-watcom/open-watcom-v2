# Resource tools Builder Control file
# ===================================

set PROJNAME=restest

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
    # Currently not built by default

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
