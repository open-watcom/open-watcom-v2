# Resource Compiler Builder Control file
# ======================================

set PROJNAME=wrc

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
    <CPCMD> <OWOBJDIR>/bwrc.exe     <OWBINDIR>/<OWOBJDIR>/bwrc<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwrcd<DYEXT> <OWBINDIR>/<OWOBJDIR>/bwrcd<DYEXT>
    <CPCMD> <OWOBJDIR>/kanji.uni    <OWBINDIR>/<OWOBJDIR>/
    <CPCMD> <OWOBJDIR>/to932.uni    <OWBINDIR>/<OWOBJDIR>/

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/bwrc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwrc<CMDEXT>
    rm -f <OWBINDIR>/<OWOBJDIR>/bwrcd<DYEXT>
    echo rm -f <OWBINDIR>/<OWOBJDIR>/kanji.uni
    rm -f <OWBINDIR>/<OWOBJDIR>/kanji.uni
    echo rm -f <OWBINDIR>/<OWOBJDIR>/to932.uni
    rm -f <OWBINDIR>/<OWOBJDIR>/to932.uni

[ BLOCK <BLDRULE> rel cprel ]
#============================

    <CCCMD> dos386/wrc.exe          <OWRELROOT>/binw/
    <CCCMD> dos386/wrc.sym          <OWRELROOT>/binw/
    <CCCMD> dos386/*.uni            <OWRELROOT>/binw/
    <CCCMD> os2386.dll/wrc.exe      <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wrc.sym      <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wrcd.dll     <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wrcd.sym     <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/*.uni        <OWRELROOT>/binp/
    <CCCMD> nt386.dll/wrc.exe       <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wrc.sym       <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wrcd.dll      <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wrcd.sym      <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/*.uni         <OWRELROOT>/binnt/
    <CCCMD> ntaxp.dll/wrc.exe       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wrc.sym       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wrcd.dll      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wrcd.sym      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/*.uni         <OWRELROOT>/axpnt/
    <CCCMD> linux386/wrc.exe        <OWRELROOT>/binl/wrc
    <CCCMD> linux386/wrc.sym        <OWRELROOT>/binl/
    <CCCMD> linux386/*.uni          <OWRELROOT>/binl/
    <CCCMD> rdos386/wrc.exe         <OWRELROOT>/rdos/
    <CCCMD> rdos386/wrc.sym         <OWRELROOT>/rdos/
    <CCCMD> rdos386/*.uni           <OWRELROOT>/rdos/

    <CCCMD> linuxx64/wrc.exe        <OWRELROOT>/binl64/wrc
    <CCCMD> linuxx64/*.uni          <OWRELROOT>/binl64/
    <CCCMD> ntx64.dll/wrc.exe       <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wrcd.dll      <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/*.uni         <OWRELROOT>/binnt64/

    <CCCMD> linuxarm/wrc.exe        <OWRELROOT>/arml/wrc
    <CCCMD> linuxarm/*.uni          <OWRELROOT>/arml/
    <CCCMD> osxx64/wrc.exe          <OWRELROOT>/osx64/wrc
    <CCCMD> osxx64/*.uni            <OWRELROOT>/osx64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
