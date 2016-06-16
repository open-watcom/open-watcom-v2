# DWARF DIP Builder Control file
# ==============================

set PROJNAME=dwarf

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ BLOCK .<USE_FILENAME_VERSION>. .1. ]
#=====================================
    set PROJNAME=dwarf<OWBLDVER>

[ BLOCK .<WATCOMBOOT>. .1. ]
#===========================
    [ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK .<WATCOMBOOT>. .. ]
#==========================
    [ INCLUDE <OWROOT>/build/defdylib.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/<PROJNAME>.dip      <OWRELROOT>/binw/
#    <CCCMD> dos386/<PROJNAME>.sym      <OWRELROOT>/binw/<PROJNAME>.dsy

    <CCCMD> wini86/<PROJNAME>.dll      <OWRELROOT>/binw/
#    <CCCMD> wini86/<PROJNAME>.sym      <OWRELROOT>/binw/

#    <CCCMD> os2i86/<PROJNAME>.dll      <OWRELROOT>/binp/dll/
#    <CCCMD> os2i86/<PROJNAME>.sym      <OWRELROOT>/binp/dll/
    <CCCMD> os2386/<PROJNAME>.d32      <OWRELROOT>/binp/
    <CCCMD> os2386/<PROJNAME>.sym      <OWRELROOT>/binp/

    <CCCMD> nt386/<PROJNAME>.dll       <OWRELROOT>/binnt/
    <CCCMD> nt386/<PROJNAME>.sym       <OWRELROOT>/binnt/

    <CCCMD> linux386/<PROJNAME>.dip    <OWRELROOT>/binl/
    <CCCMD> linux386/<PROJNAME>.sym    <OWRELROOT>/binl/

    <CCCMD> ntaxp/<PROJNAME>.dll       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/<PROJNAME>.sym       <OWRELROOT>/axpnt/

    <CCCMD> qnx386/<PROJNAME>.dip      <OWRELROOT>/qnx/watcom/wd/
    <CCCMD> qnx386/<PROJNAME>.sym      <OWRELROOT>/qnx/sym/

    <CCCMD> rdos386/<PROJNAME>.dll     <OWRELROOT>/rdos/
    <CCCMD> rdos386/<PROJNAME>.sym     <OWRELROOT>/rdos/

    <CCCMD> linuxx64/<PROJNAME>.so     <OWRELROOT>/binl64/
    <CCCMD> ntx64/<PROJNAME>.dll       <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
