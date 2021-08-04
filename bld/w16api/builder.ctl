# Windows 16-bit API Builder Control file
# =======================================

set PROJNAME=w16api

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> wini86/lib/*                <OWRELROOT>/lib286/win/
    <CCCMD> wini86/h/*                  <OWRELROOT>/h/win/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
