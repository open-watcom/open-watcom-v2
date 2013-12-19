# wpp Builder Control file
# ========================

set PROJDIR=<CWD>
set PROJNAME=wpp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/bwpp.exe     <OWBINDIR>/bwpp<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwppdi86<DYEXT> <OWBINDIR>/bwppdi86<DYEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/bwpp<CMDEXT>
    rm -f <OWBINDIR>/bwpp<CMDEXT>
    rm -f <OWBINDIR>/bwppdi86<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wpp.exe          <OWRELROOT>/binw/
    <CCCMD> dos386/wpp.sym          <OWRELROOT>/binw/
    <CCCMD> dos386/wppi8601.int     <OWRELROOT>/binw/
    <CCCMD> os2386.dll/wpp.exe      <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wpp.sym      <OWRELROOT>/binp/
    <CCCMD> os2386.dll/wppdi86.dll  <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wppdi86.sym  <OWRELROOT>/binp/dll/
    <CCCMD> os2386.dll/wppi8601.int <OWRELROOT>/binp/dll/
    <CCCMD> nt386.dll/wpp.exe       <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wpp.sym       <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wppdi86.dll   <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wppdi86.sym   <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/wppi8601.int  <OWRELROOT>/binnt/
    <CCCMD> ntaxp.dll/wpp.exe       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wpp.sym       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wppdi86.dll   <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wppdi86.sym   <OWRELROOT>/axpnt/
    <CCCMD> ntaxp.dll/wppi8601.int  <OWRELROOT>/axpnt/
    <CCCMD> qnx386/wpp.exe          <OWRELROOT>/qnx/binq/wpp
    <CCCMD> qnx386/wpp.sym          <OWRELROOT>/qnx/sym/
    <CCCMD> qnx386/wppi8601.int     <OWRELROOT>/qnx/binq/
    <CCCMD> linux386/wpp.exe        <OWRELROOT>/binl/wpp
    <CCCMD> linux386/wpp.sym        <OWRELROOT>/binl/
    <CCCMD> linux386/wppi8601.int   <OWRELROOT>/binl/

    <CCCMD> ntx64.dll/wpp.exe       <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wppdi86.dll   <OWRELROOT>/binnt64/
    <CCCMD> ntx64.dll/wppi8601.int  <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wpp.exe        <OWRELROOT>/binl64/wpp
    <CCCMD> linuxx64/wppi8601.int   <OWRELROOT>/binl64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
