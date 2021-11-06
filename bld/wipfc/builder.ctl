# WIPFC Builder Control file
# ==========================

set PROJNAME=wipfc

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
    <CPCMD> <OWOBJDIR>/wipfc.exe <OWBINDIR>/<OWOBJDIR>/bwipfc<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bwipfc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwipfc<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CPCMD> helper/*.nls          <OWRELROOT>/wipfc/
    <CPCMD> helper/*.txt          <OWRELROOT>/wipfc/
    <CPCMD> helper/*.cnv          <OWRELROOT>/wipfc/

    <CCCMD> dos386/wipfc.exe      <OWRELROOT>/binw/
    <CCCMD> os2386/wipfc.exe      <OWRELROOT>/binp/
    <CCCMD> nt386/wipfc.exe       <OWRELROOT>/binnt/
    <CCCMD> linux386/wipfc.exe    <OWRELROOT>/binl/wipfc

    <CCCMD> ntx64/wipfc.exe       <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wipfc.exe    <OWRELROOT>/binl64/wipfc

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
