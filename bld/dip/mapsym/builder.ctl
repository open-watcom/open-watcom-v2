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
    <CCCMD> dos386/mapsym.dip         <OWRELROOT>/binw/
#    <CCCMD> dos386/mapsym.sym         <OWRELROOT>/binw/mapsym.dsy

    <CCCMD> wini86/mapsym.dll         <OWRELROOT>/binw/
#    <CCCMD> wini86/mapsym.sym         <OWRELROOT>/binw/

#    <CCCMD> os2i86/mapsym.dll         <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/mapsym.sym         <OWRELROOT>/binp/dll/
    <CCCMD> os2386/mapsym.d32         <OWRELROOT>/binp/
    <CCCMD> os2386/mapsym.sym         <OWRELROOT>/binp/

    <CCCMD> nt386/mapsym.dll          <OWRELROOT>/binnt/
    <CCCMD> nt386/mapsym.sym          <OWRELROOT>/binnt/

    <CCCMD> linux386/mapsym.dip       <OWRELROOT>/binl/
    <CCCMD> linux386/mapsym.sym       <OWRELROOT>/binl/

    <CCCMD> ntaxp/mapsym.dll          <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/mapsym.sym          <OWRELROOT>/axpnt/

    <CCCMD> qnx386/mapsym.dip         <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/mapsym.sym         <OWRELROOT>/qnx/sym/

    <CCCMD> rdos386/mapsym.dll        <OWRELROOT>/rdos/
    <CCCMD> rdos386/mapsym.sym        <OWRELROOT>/rdos/

    <CCCMD> ntx64/mapsym.dll          <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
