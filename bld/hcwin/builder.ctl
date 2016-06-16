# whc Builder Control file
# ========================

set PROJNAME=whc

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/whc.exe <OWBINDIR>/bwhc<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwhc<CMDEXT>
    rm -f <OWBINDIR>/bwhc<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/whc.exe      <OWRELROOT>/binw/
    <CCCMD> nt386/whc.exe       <OWRELROOT>/binnt/
    <CCCMD> os2386/whc.exe      <OWRELROOT>/binp/
    <CCCMD> linux386/whc.exe    <OWRELROOT>/binl/whc

    <CCCMD> ntx64/whc.exe       <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/whc.exe    <OWRELROOT>/binl64/whc

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
