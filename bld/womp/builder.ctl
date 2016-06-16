# WOMP Builder Control file
# =========================

set PROJNAME=womp

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

cdsay .

[ BLOCK <1> build rel ]
#======================
#    cdsay h
#    wmake -h -i
#   cdsay ../release
#   wmake -h -i

[ BLOCK <1> rel cprel ]
#======================
#   <CPCMD> womp.exe  <OWRELROOT>/binw/womp.exe
#   <CPCMD> wompj.exe <OWRELROOT>/binw/japan/womp.exe

[ BLOCK <1> clean ]
#==================
#     rm -f h/wmpmsg.gh
#    sweep killobjs

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
