# WPROF Builder Control file
# ==========================

set PROJNAME=wprof

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#========================
    <CCCMD> dos386/wprof.exe     <OWRELROOT>/binw/
    <CCCMD> wini86/wprof.exe     <OWRELROOT>/binw/wprofw.exe
    <CCCMD> os2386pm/wprof.exe   <OWRELROOT>/binp/
    <CCCMD> os2386/wprof.exe     <OWRELROOT>/binp/wprofc.exe
    <CCCMD> nt386/wprof.exe      <OWRELROOT>/binnt/
    <CCCMD> nt386c/wprof.exe     <OWRELROOT>/binnt/wprofc.exe
    <CCCMD> ntaxp/wprof.exe      <OWRELROOT>/axpnt/
    <CCCMD> linux386/wprof.exe   <OWRELROOT>/binl/wprof
    <CCCMD> linux386/wprof.sym   <OWRELROOT>/binl/
    <CCCMD> qnx386/wprof.qnx     <OWRELROOT>/qnx/binq/wprof
    <CCCMD> qnx386/wprof.sym     <OWRELROOT>/qnx/sym/

    <CCCMD> ntx64/wprof.exe      <OWRELROOT>/binnt64/
    <CCCMD> ntx64c/wprof.exe     <OWRELROOT>/binnt64/wprofc.exe
    <CCCMD> linuxx64/wprof.exe   <OWRELROOT>/binl64/wprof

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
