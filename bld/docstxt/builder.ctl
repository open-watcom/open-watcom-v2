# Text Documentation Builder Control file
# =======================================

set PROJNAME=docstxt

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> docs/*.txt      <OWRELROOT>/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
