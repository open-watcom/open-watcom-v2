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

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> <PROJDIR>/dos386/watcom.dip     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/dos386/watcom.sym     <OWRELROOT>/binw/watcom.dsy

    <CCCMD> <PROJDIR>/wini86/watcom.dll     <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/wini86/watcom.sym     <OWRELROOT>/binw/

#    <CCCMD> <PROJDIR>/os2i86/watcom.dll     <OWRELROOT>/binp/dll/
#    <CCCMD> <PROJDIR>/os2i86/watcom.sym     <OWRELROOT>/binp/dll/
    <CCCMD> <PROJDIR>/os2386/watcom.d32     <OWRELROOT>/binp/
    <CCCMD> <PROJDIR>/os2386/watcom.sym     <OWRELROOT>/binp/

    <CCCMD> <PROJDIR>/nt386/watcom.dll      <OWRELROOT>/binnt/
    <CCCMD> <PROJDIR>/nt386/watcom.sym      <OWRELROOT>/binnt/

    <CCCMD> <PROJDIR>/linux386/watcom.dip   <OWRELROOT>/binl/
    <CCCMD> <PROJDIR>/linux386/watcom.sym   <OWRELROOT>/binl/

    <CCCMD> <PROJDIR>/ntaxp/watcom.dll      <OWRELROOT>/axpnt/
    <CCCMD> <PROJDIR>/ntaxp/watcom.sym      <OWRELROOT>/axpnt/

    <CCCMD> <PROJDIR>/qnx386/watcom.dip     <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> <PROJDIR>/qnx386/watcom.sym     <OWRELROOT>/qnx/sym/

    <CCCMD> <PROJDIR>/rdos386/watcom.dll    <OWRELROOT>/rdos/
    <CCCMD> <PROJDIR>/rdos386/watcom.sym    <OWRELROOT>/rdos/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
