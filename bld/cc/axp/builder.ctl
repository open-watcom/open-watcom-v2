# wccaxp Builder Control file
# ===========================

set PROJNAME=wccaxp

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
    <CPCMD> <OWOBJDIR>/bwccaxp.exe     <OWBINDIR>/bwccaxp<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwccdaxp<DYEXT> <OWBINDIR>/bwccdaxp<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwccaxp<CMDEXT>
    rm -f <OWBINDIR>/bwccaxp<CMDEXT>
    rm -f <OWBINDIR>/bwccdaxp<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/<OWOBJDIR>/wccaxp.exe        <OWRELROOT>/binw/
    <CCCMD> dos386/<OWOBJDIR>/wccaxp.sym        <OWRELROOT>/binw/
    <CCCMD> dos386/wccaxp01.int                 <OWRELROOT>/binw/
    <CCCMD> os2386.dll/<OWOBJDIR>/wccaxp.exe    <OWRELROOT>/binp/
    <CCCMD> os2386.dll/<OWOBJDIR>/wccaxp.sym    <OWRELROOT>/binp/
    <CCCMD> os2386.dll/<OWOBJDIR>/wccdaxp.dll   <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/<OWOBJDIR>/wccdaxp.sym   <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wccaxp01.int             <OWRELROOT>/binp/dll/
    <CCCMD> nt386.dll/<OWOBJDIR>/wccaxp.exe     <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/<OWOBJDIR>/wccaxp.sym     <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/<OWOBJDIR>/wccdaxp.dll    <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/<OWOBJDIR>/wccdaxp.sym    <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wccaxp01.int              <OWRELROOT>/binnt/
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wccaxp.exe     <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wccaxp.sym     <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wccdaxp.dll    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wccdaxp.sym    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wccaxp01.int              <OWRELROOT>/axpnt/
    <CCCMD> qnx386/<OWOBJDIR>/wccaxp.exe        <OWRELROOT>/qnx/binq/wccaxp
    <CCCMD> qnx386/<OWOBJDIR>/wccaxp.sym        <OWRELROOT>/qnx/sym/
    <CCCMD> dos386/wccaxp01.int                 <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/<OWOBJDIR>/wccaxp.exe      <OWRELROOT>/binl/wccaxp
    <CCCMD> linux386/<OWOBJDIR>/wccaxp.sym      <OWRELROOT>/binl/
    <CCCMD> linux386/wccaxp01.int               <OWRELROOT>/binl/

    <CCCMD> ntx64.dll/<OWOBJDIR>/wccaxp.exe     <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/<OWOBJDIR>/wccdaxp.dll    <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wccaxp01.int              <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/<OWOBJDIR>/wccaxp.exe      <OWRELROOT>/binl64/wccaxp
    <CCCMD> linuxx64/wccaxp01.int               <OWRELROOT>/binl64/
    <CCCMD> linuxarm/<OWOBJDIR>/wccaxp.exe      <OWRELROOT>/arml/wccaxp
    <CCCMD> linuxarm/wccaxp01.int               <OWRELROOT>/arml/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
