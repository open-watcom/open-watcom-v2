# wcl Builder Control file
# ========================

set PROJNAME=wcl386

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
    <CPCMD> <OWOBJDIR>/bwcl386.exe <OWBINDIR>/<OWOBJDIR>/bwcl386<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bwcl386<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwcl386<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dosi86/wcl386.exe     <OWRELROOT>/binw/
    <CCCMD> dosi86/wcl386.sym     <OWRELROOT>/binw/
    <CCCMD> nt386/wcl386.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386/wcl386.sym      <OWRELROOT>/binnt/
    <CCCMD> os2386/wcl386.exe     <OWRELROOT>/binp/
    <CCCMD> os2386/wcl386.sym     <OWRELROOT>/binp/
    <CCCMD> linux386/wcl386.exe   <OWRELROOT>/binl/wcl386
    <CCCMD> linux386/wcl386.sym   <OWRELROOT>/binl/
    <CCCMD> ntaxp/wcl386.exe      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wcl386.sym      <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wcl386.exe      <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wcl386.exe   <OWRELROOT>/binl64/wcl386
    <CCCMD> linuxarm/wcl386.exe   <OWRELROOT>/arml/wcl386
    <CCCMD> osxx64/wcl386.exe     <OWRELROOT>/osx64/wcl386

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
