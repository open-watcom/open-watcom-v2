# wcc386 Builder Control file
# ===========================

set PROJNAME=wcc386

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
    <CPCMD> <OWOBJDIR>/bwcc386.exe     <OWBINDIR>/bwcc386<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwccd386<DYEXT> <OWBINDIR>/bwccd386<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwcc386<CMDEXT>
    rm -f <OWBINDIR>/bwcc386<CMDEXT>
    rm -f <OWBINDIR>/bwccd386<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wcc386.exe        <OWRELROOT>/binw/
    <CCCMD> dos386/wcc386.sym        <OWRELROOT>/binw/
    <CCCMD> dos386/wcc38601.int      <OWRELROOT>/binw/
    <CCCMD> os2386.dll/wcc386.exe    <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wcc386.sym    <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wccd386.dll   <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wccd386.sym   <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wcc38601.int  <OWRELROOT>/binp/dll/
    <CCCMD> nt386.dll/wcc386.exe     <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wcc386.sym     <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wccd386.dll    <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wccd386.sym    <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wcc38601.int   <OWRELROOT>/binnt/
    <CCCMD> ntaxp.dll/wcc386.exe     <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wcc386.sym     <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wccd386.dll    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wccd386.sym    <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wcc38601.int   <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wcc386.exe        <OWRELROOT>/qnx/binq/wcc386
    <CCCMD> qnx386/wcc386.sym        <OWRELROOT>/qnx/sym/
    <CCCMD> qnx386/wcc38601.int      <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/wcc386.exe      <OWRELROOT>/binl/wcc386
    <CCCMD> linux386/wcc386.sym      <OWRELROOT>/binl/
    <CCCMD> linux386/wcc38601.int    <OWRELROOT>/binl/

    <CCCMD> ntx64.dll/wcc386.exe     <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wccd386.dll    <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wcc38601.int   <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wcc386.exe      <OWRELROOT>/binl64/wcc386
    <CCCMD> linuxx64/wcc38601.int    <OWRELROOT>/binl64/
    <CCCMD> linuxarm/wcc386.exe      <OWRELROOT>/arml/wcc386
    <CCCMD> linuxarm/wcc38601.int    <OWRELROOT>/arml/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
