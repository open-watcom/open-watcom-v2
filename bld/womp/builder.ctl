# WOMP Builder Control file
# =========================

set PROJNAME=womp

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

cdsay .

[ BLOCK <BLDRULE> build rel ]
#============================
#    cdsay h
#    wmake -h -i
#   cdsay ../release
#   wmake -h -i

[ BLOCK <BLDRULE> rel cprel ]
#============================
#   <CPCMD> womp.exe  <OWRELROOT>/binw/womp.exe
#   <CPCMD> wompj.exe <OWRELROOT>/binw/japan/womp.exe

[ BLOCK <BLDRULE> clean ]
#========================
#     rm -f h/wompmsg.gh
#    sweep killobjs

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
