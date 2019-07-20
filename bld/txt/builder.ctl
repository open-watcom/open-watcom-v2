# Text Documentation Builder Control file
# =======================================

set PROJNAME=txt

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> txt/*.txt       <OWRELROOT>/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
