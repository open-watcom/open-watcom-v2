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
    <CCCMD> <PROJDIR>/dos386/dwarf.dip      <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/dos386/dwarf.sym      <OWRELROOT>/binw/dwarf.dsy

    <CCCMD> <PROJDIR>/wini86/dwarf.dll      <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/wini86/dwarf.sym      <OWRELROOT>/binw/

#    <CCCMD> <PROJDIR>/os2i86/dwarf.dll      <OWRELROOT>/binp/dll/
#    <CCCMD> <PROJDIR>/os2i86/dwarf.sym      <OWRELROOT>/binp/dll/
    <CCCMD> <PROJDIR>/os2386/dwarf.d32      <OWRELROOT>/binp/
    <CCCMD> <PROJDIR>/os2386/dwarf.sym      <OWRELROOT>/binp/

    <CCCMD> <PROJDIR>/nt386/dwarf.dll       <OWRELROOT>/binnt/
    <CCCMD> <PROJDIR>/nt386/dwarf.sym       <OWRELROOT>/binnt/

    <CCCMD> <PROJDIR>/linux386/dwarf.dip    <OWRELROOT>/binl/
    <CCCMD> <PROJDIR>/linux386/dwarf.sym    <OWRELROOT>/binl/

    <CCCMD> <PROJDIR>/ntaxp/dwarf.dll       <OWRELROOT>/axpnt/
    <CCCMD> <PROJDIR>/ntaxp/dwarf.sym       <OWRELROOT>/axpnt/

    <CCCMD> <PROJDIR>/qnx386/dwarf.dip      <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> <PROJDIR>/qnx386/dwarf.sym      <OWRELROOT>/qnx/sym/

    <CCCMD> <PROJDIR>/rdos386/dwarf.dll     <OWRELROOT>/rdos/
    <CCCMD> <PROJDIR>/rdos386/dwarf.sym     <OWRELROOT>/rdos/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
