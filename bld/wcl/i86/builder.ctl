# wcl Builder Control file
# ========================

set PROJNAME=wcl

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
    <CPCMD> <OWOBJDIR>/bwcl.exe <OWBINDIR>/<OWOBJDIR>/bwcl<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bwcl<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwcl<CMDEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dosi86/wcl.exe        <OWRELROOT>/binw/
    <CCCMD> dosi86/wcl.sym        <OWRELROOT>/binw/
    <CCCMD> nt386/wcl.exe         <OWRELROOT>/binnt/
    <CCCMD> nt386/wcl.sym         <OWRELROOT>/binnt/
    <CCCMD> os2386/wcl.exe        <OWRELROOT>/binp/
    <CCCMD> os2386/wcl.sym        <OWRELROOT>/binp/
    <CCCMD> linux386/wcl.exe      <OWRELROOT>/binl/wcl
    <CCCMD> linux386/wcl.sym      <OWRELROOT>/binl/
    <CCCMD> ntaxp/wcl.exe         <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wcl.sym         <OWRELROOT>/axpnt/

    <CCCMD> ntx64/wcl.exe         <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wcl.exe      <OWRELROOT>/binl64/wcl
    <CCCMD> linuxarm/wcl.exe      <OWRELROOT>/arml/wcl
    <CCCMD> osxx64/wcl.exe        <OWRELROOT>/osx64/wcl

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
