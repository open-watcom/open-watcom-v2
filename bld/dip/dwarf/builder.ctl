# DWARF DIP Builder Control file
# ==============================

set PROJDIR=<CWD>
set PROJNAME=dwarf

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ BLOCK .<WATCOMBOOT>. .1. ]
    [ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK .<WATCOMBOOT>. .. ]
    [ INCLUDE <OWROOT>/build/defdylib.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/dwarf.dip      <OWRELROOT>/binw/
#    <CCCMD> dos386/dwarf.sym      <OWRELROOT>/binw/dwarf.dsy

    <CCCMD> wini86/dwarf.dll      <OWRELROOT>/binw/
#    <CCCMD> wini86/dwarf.sym      <OWRELROOT>/binw/

#    <CCCMD> os2i86/dwarf.dll      <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/dwarf.sym      <OWRELROOT>/binp/dll/
    <CCCMD> os2386/dwarf.d32      <OWRELROOT>/binp/
    <CCCMD> os2386/dwarf.sym      <OWRELROOT>/binp/

    <CCCMD> nt386/dwarf.dll       <OWRELROOT>/binnt/
    <CCCMD> nt386/dwarf.sym       <OWRELROOT>/binnt/

    <CCCMD> linux386/dwarf.dip    <OWRELROOT>/binl/
    <CCCMD> linux386/dwarf.sym    <OWRELROOT>/binl/

    <CCCMD> ntaxp/dwarf.dll       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/dwarf.sym       <OWRELROOT>/axpnt/

    <CCCMD> qnx386/dwarf.dip      <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/dwarf.sym      <OWRELROOT>/qnx/sym/

    <CCCMD> rdos386/dwarf.dll     <OWRELROOT>/rdos/
    <CCCMD> rdos386/dwarf.sym     <OWRELROOT>/rdos/

    <CCCMD> linuxx64/dwarf.so     <OWRELROOT>/binl64/
    <CCCMD> ntx64/dwarf.dll       <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
