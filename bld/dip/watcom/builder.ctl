# WATCOM DIP Builder Control file
# ===============================

set PROJDIR=<CWD>
set PROJNAME=watcom

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
    <CCCMD> dos386/watcom.dip     <OWRELROOT>/binw/
#    <CCCMD> dos386/watcom.sym     <OWRELROOT>/binw/watcom.dsy

    <CCCMD> wini86/watcom.dll     <OWRELROOT>/binw/
#    <CCCMD> wini86/watcom.sym     <OWRELROOT>/binw/

#    <CCCMD> os2i86/watcom.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/watcom.sym     <OWRELROOT>/binp/dll/
    <CCCMD> os2386/watcom.d32     <OWRELROOT>/binp/
    <CCCMD> os2386/watcom.sym     <OWRELROOT>/binp/

    <CCCMD> nt386/watcom.dll      <OWRELROOT>/binnt/
    <CCCMD> nt386/watcom.sym      <OWRELROOT>/binnt/

    <CCCMD> linux386/watcom.dip   <OWRELROOT>/binl/
    <CCCMD> linux386/watcom.sym   <OWRELROOT>/binl/

    <CCCMD> ntaxp/watcom.dll      <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/watcom.sym      <OWRELROOT>/axpnt/

    <CCCMD> qnx386/watcom.dip     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/watcom.sym     <OWRELROOT>/qnx/sym/

    <CCCMD> rdos386/watcom.dll    <OWRELROOT>/rdos/
    <CCCMD> rdos386/watcom.sym    <OWRELROOT>/rdos/

    <CCCMD> linuxx64/watcom.so    <OWRELROOT>/binl64/
    <CCCMD> ntx64/watcom.dll      <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
