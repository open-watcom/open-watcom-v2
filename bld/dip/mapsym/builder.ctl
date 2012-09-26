# MAPSYM DIP Builder Control file
# ===============================

set PROJDIR=<CWD>
set PROJNAME=mapsym

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
    <CCCMD> <PROJDIR>/dos386/mapsym.dip         <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/dos386/mapsym.sym         <OWRELROOT>/binw/mapsym.dsy

    <CCCMD> <PROJDIR>/wini86/mapsym.dll         <OWRELROOT>/binw/
#    <CCCMD> <PROJDIR>/wini86/mapsym.sym         <OWRELROOT>/binw/

#    <CCCMD> <PROJDIR>/os2i86/mapsym.dll         <OWRELROOT>/binp/dll/
#    <CCCMD> <PROJDIR>/os2i86/mapsym.sym         <OWRELROOT>/binp/dll/
    <CCCMD> <PROJDIR>/os2386/mapsym.d32         <OWRELROOT>/binp/
    <CCCMD> <PROJDIR>/os2386/mapsym.sym         <OWRELROOT>/binp/

    <CCCMD> <PROJDIR>/nt386/mapsym.dll          <OWRELROOT>/binnt/
    <CCCMD> <PROJDIR>/nt386/mapsym.sym          <OWRELROOT>/binnt/

    <CCCMD> <PROJDIR>/linux386/mapsym.dip       <OWRELROOT>/binl/
    <CCCMD> <PROJDIR>/linux386/mapsym.sym       <OWRELROOT>/binl/

    <CCCMD> <PROJDIR>/ntaxp/mapsym.dll          <OWRELROOT>/axpnt/
    <CCCMD> <PROJDIR>/ntaxp/mapsym.sym          <OWRELROOT>/axpnt/

    <CCCMD> <PROJDIR>/qnx386/mapsym.dip         <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> <PROJDIR>/qnx386/mapsym.sym         <OWRELROOT>/qnx/sym/

    <CCCMD> <PROJDIR>/rdos386/mapsym.dll        <OWRELROOT>/rdos/
    <CCCMD> <PROJDIR>/rdos386/mapsym.sym        <OWRELROOT>/rdos/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
