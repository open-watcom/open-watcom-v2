# whc Builder Control file
# ========================

set PROJNAME=whc

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/whc.exe <OWBINDIR>/<OWOBJDIR>/bwhc<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bwhc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwhc<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/whc.exe      <OWRELROOT>/binw/
    <CCCMD> nt386/whc.exe       <OWRELROOT>/binnt/
    <CCCMD> os2386/whc.exe      <OWRELROOT>/binp/
    <CCCMD> linux386/whc.exe    <OWRELROOT>/binl/whc

    <CCCMD> ntx64/whc.exe       <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/whc.exe    <OWRELROOT>/binl64/whc

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
