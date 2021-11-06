# wcc Builder Control file
# ========================

set PROJNAME=wcc

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
    <CPCMD> <OWOBJDIR>/bwcc.exe     <OWBINDIR>/<OWOBJDIR>/bwcc<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwccd<DYEXT> <OWBINDIR>/<OWOBJDIR>/bwccd<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bwcc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwcc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwccd<DYEXT>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/<OWOBJDIR>/wcc.exe           <OWRELROOT>/binw/
    <CCCMD> dos386/<OWOBJDIR>/wcc.sym           <OWRELROOT>/binw/
    <CCCMD> dos386/<OWOBJDIR>/wcci8601.int      <OWRELROOT>/binw/
    <CCCMD> os2386.dll/<OWOBJDIR>/wcc.exe       <OWRELROOT>/binp/
    <CCCMD> os2386.dll/<OWOBJDIR>/wcc.sym       <OWRELROOT>/binp/
    <CCCMD> os2386.dll/<OWOBJDIR>/wccd.dll      <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/<OWOBJDIR>/wccd.sym      <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/<OWOBJDIR>/wcci8601.int  <OWRELROOT>/binp/dll/
    <CCCMD> nt386.dll/<OWOBJDIR>/wcc.exe        <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/<OWOBJDIR>/wcc.sym        <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/<OWOBJDIR>/wccd.dll       <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/<OWOBJDIR>/wccd.sym       <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/<OWOBJDIR>/wcci8601.int   <OWRELROOT>/binnt/
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wcc.exe        <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wcc.sym        <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wccd.dll       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wccd.sym       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/<OWOBJDIR>/wcci8601.int   <OWRELROOT>/axpnt/
    <CCCMD> qnx386/<OWOBJDIR>/wcc.exe           <OWRELROOT>/qnx/binq/wcc
    <CCCMD> qnx386/<OWOBJDIR>/wcc.sym           <OWRELROOT>/qnx/sym/
    <CCCMD> qnx386/<OWOBJDIR>/wcci8601.int      <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/<OWOBJDIR>/wcc.exe         <OWRELROOT>/binl/wcc
    <CCCMD> linux386/<OWOBJDIR>/wcc.sym         <OWRELROOT>/binl/
    <CCCMD> linux386/<OWOBJDIR>/wcci8601.int    <OWRELROOT>/binl/

    <CCCMD> ntx64.dll/<OWOBJDIR>/wcc.exe        <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/<OWOBJDIR>/wccd.dll       <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/<OWOBJDIR>/wcci8601.int   <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/<OWOBJDIR>/wcc.exe         <OWRELROOT>/binl64/wcc
    <CCCMD> linuxx64/<OWOBJDIR>/wcci8601.int    <OWRELROOT>/binl64/
    <CCCMD> linuxarm/<OWOBJDIR>/wcc.exe         <OWRELROOT>/arml/wcc
    <CCCMD> linuxarm/<OWOBJDIR>/wcci8601.int    <OWRELROOT>/arml/
    <CCCMD> osxx64/<OWOBJDIR>/wcc.exe           <OWRELROOT>/osx64/wcc
    <CCCMD> osxx64/<OWOBJDIR>/wcci8601.int      <OWRELROOT>/osx64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
