# wccaxp Builder Control file
# ===========================

set PROJDIR=<CWD>
set PROJNAME=wccaxp

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/bwcaxp.exe      <OWBINDIR>/bwccaxp<CMDEXT>
    <CCCMD> <OWOBJDIR>/bwccdaxp<DYEXT> <OWBINDIR>/bwccdaxp<DYEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwccaxp<CMDEXT>
    rm -f <OWBINDIR>/bwccaxp<CMDEXT>
    rm -f <OWBINDIR>/bwccdaxp<DYEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wcaxp.exe        <OWRELROOT>/binw/wccaxp.exe
    <CCCMD> dos386/wcaxp.sym        <OWRELROOT>/binw/wccaxp.sym
    <CCCMD> dos386/wcaxp01.int      <OWRELROOT>/binw/wccaxp01.int
    <CCCMD> os2386/wcaxp.exe        <OWRELROOT>/binp/wccaxp.exe
    <CCCMD> os2386/wcaxp.sym        <OWRELROOT>/binp/wccaxp.sym
    <CCCMD> os2386/wcdaxp.dll       <OWRELROOT>/binp/dll/wccdaxp.dll
    <CCCMD> os2386/wcdaxp.sym       <OWRELROOT>/binp/dll/wccdaxp.sym
    <CCCMD> os2386/wcaxp01.int      <OWRELROOT>/binp/dll/wccaxp01.int
    <CCCMD> nt386/wcaxp.exe         <OWRELROOT>/binnt/wccaxp.exe
    <CCCMD> nt386/wcaxp.sym         <OWRELROOT>/binnt/wccaxp.sym
    <CCCMD> nt386/wcdaxp.dll        <OWRELROOT>/binnt/wccdaxp.dll
    <CCCMD> nt386/wcdaxp.sym        <OWRELROOT>/binnt/wccdaxp.sym
    <CCCMD> nt386/wcaxp01.int       <OWRELROOT>/binnt/wccaxp01.int
    <CCCMD> ntaxp/wcaxp.exe         <OWRELROOT>/axpnt/wccaxp.exe
    <CCCMD> ntaxp/wcaxp.sym         <OWRELROOT>/axpnt/wccaxp.sym
    <CCCMD> ntaxp/wcdaxp.dll        <OWRELROOT>/axpnt/wccdaxp.dll
    <CCCMD> ntaxp/wcdaxp.sym        <OWRELROOT>/axpnt/wccdaxp.sym
    <CCCMD> ntaxp/wcaxp01.int       <OWRELROOT>/axpnt/wccaxp01.int
    <CCCMD> qnx386/wcaxp.exe        <OWRELROOT>/qnx/binq/wccaxp
    <CCCMD> qnx386/wcaxp.sym        <OWRELROOT>/qnx/sym/wccaxp.sym
    <CCCMD> dos386/wcaxp01.int      <OWRELROOT>/qnx/binq/wccaxp01.int
    <CCCMD> linux386/wcaxp.exe      <OWRELROOT>/binl/wccaxp
    <CCCMD> linux386/wcaxp.sym      <OWRELROOT>/binl/wccaxp.sym
    <CCCMD> linux386/wcaxp01.int    <OWRELROOT>/binl/wccaxp01.int

[ BLOCK . . ]
#============
cdsay <PROJDIR>
