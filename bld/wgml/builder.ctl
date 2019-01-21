# WGML Builder Control file
# =========================

set PROJNAME=wgml

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> wgml ]
#=================
    pmake -d wgml <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <BLDRULE> wgmlclean ]
#============================
    pmake -d wgml <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
