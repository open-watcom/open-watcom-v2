# DIP Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
  [ IFDEF (os_dos "") <2*> ]
        <CPCMD> dwarf/dos386/dwarf.dip         <OWRELROOT>/binw/
#        <CPCMD> dwarf/dos386/dwarf.sym         <OWRELROOT>/binw/dwarf.dsy
        <CPCMD> codeview/dos386/codeview.dip   <OWRELROOT>/binw/
#        <CPCMD> codeview/dos386/codeview.sym   <OWRELROOT>/binw/codeview.dsy
        <CPCMD> export/dos386/export.dip       <OWRELROOT>/binw/
#        <CPCMD> export/dos386/export.sym       <OWRELROOT>/binw/export.dsy
        <CPCMD> watcom/dos386/watcom.dip       <OWRELROOT>/binw/
#        <CPCMD> watcom/dos386/watcom.sym       <OWRELROOT>/binw/watcom.dsy
        <CPCMD> mapsym/dos386/mapsym.dip       <OWRELROOT>/binw/
#        <CPCMD> mapsym/dos386/mapsym.sym       <OWRELROOT>/binw/watcom.dsy

  [ IFDEF (os_win "") <2*> ]
        <CPCMD> dwarf/wini86/dwarf.dll         <OWRELROOT>/binw/
#        <CPCMD> dwarf/wini86/dwarf.sym         <OWRELROOT>/binw/
        <CPCMD> codeview/wini86/codeview.dll   <OWRELROOT>/binw/
#        <CPCMD> codeview/wini86/codeview.sym   <OWRELROOT>/binw/
        <CPCMD> export/wini86/export.dll       <OWRELROOT>/binw/
#        <CPCMD> export/wini86/export.sym       <OWRELROOT>/binw/
        <CPCMD> watcom/wini86/watcom.dll       <OWRELROOT>/binw/
#        <CPCMD> watcom/wini86/watcom.sym       <OWRELROOT>/binw/
        <CPCMD> mapsym/wini86/mapsym.dll       <OWRELROOT>/binw/
#        <CPCMD> mapsym/wini86/mapsym.sym       <OWRELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
#        <CPCMD> dwarf/os2i86/dwarf.dll         <OWRELROOT>/binp/dll/
#        <CPCMD> dwarf/os2i86/dwarf.sym         <OWRELROOT>/binp/dll/
#        <CPCMD> codeview/os2i86/codeview.dll   <OWRELROOT>/binp/dll/
#        <CPCMD> codeview/os2i86/codeview.sym   <OWRELROOT>/binp/dll/
#        <CPCMD> export/os2i86/export.dll       <OWRELROOT>/binp/dll/
#        <CPCMD> export/os2i86/export.sym       <OWRELROOT>/binp/dll/
#        <CPCMD> watcom/os2i86/watcom.dll       <OWRELROOT>/binp/dll/
#        <CPCMD> watcom/os2i86/watcom.sym       <OWRELROOT>/binp/dll/
        <CPCMD> dwarf/os2386/dwarf.d32         <OWRELROOT>/binp/
        <CPCMD> dwarf/os2386/dwarf.sym         <OWRELROOT>/binp/
        <CPCMD> codeview/os2386/codeview.d32   <OWRELROOT>/binp/
        <CPCMD> codeview/os2386/codeview.sym   <OWRELROOT>/binp/
        <CPCMD> export/os2386/export.d32       <OWRELROOT>/binp/
        <CPCMD> export/os2386/export.sym       <OWRELROOT>/binp/
        <CPCMD> watcom/os2386/watcom.d32       <OWRELROOT>/binp/
        <CPCMD> watcom/os2386/watcom.sym       <OWRELROOT>/binp/
        <CPCMD> mapsym/os2386/mapsym.d32       <OWRELROOT>/binp/
        <CPCMD> mapsym/os2386/mapsym.sym       <OWRELROOT>/binp/
        <CPCMD> hllcv/os2386/hllcv.d32         <OWRELROOT>/binp/
        <CPCMD> hllcv/os2386/hllcv.sym         <OWRELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
        <CPCMD> dwarf/nt386/dwarf.dll          <OWRELROOT>/binnt/
        <CPCMD> dwarf/nt386/dwarf.sym          <OWRELROOT>/binnt/
        <CPCMD> codeview/nt386/codeview.dll    <OWRELROOT>/binnt/
        <CPCMD> codeview/nt386/codeview.sym    <OWRELROOT>/binnt/
        <CPCMD> export/nt386/export.dll        <OWRELROOT>/binnt/
        <CPCMD> export/nt386/export.sym        <OWRELROOT>/binnt/
        <CPCMD> watcom/nt386/watcom.dll        <OWRELROOT>/binnt/
        <CPCMD> watcom/nt386/watcom.sym        <OWRELROOT>/binnt/
        <CPCMD> mapsym/nt386/mapsym.dll        <OWRELROOT>/binnt/
        <CPCMD> mapsym/nt386/mapsym.sym        <OWRELROOT>/binnt/

  [ IFDEF (os_linux "") <2*> ]
        <CPCMD> dwarf/linux386/dwarf.dip       <OWRELROOT>/binl/
        <CPCMD> dwarf/linux386/dwarf.sym       <OWRELROOT>/binl/
        <CPCMD> watcom/linux386/watcom.dip     <OWRELROOT>/binl/
        <CPCMD> watcom/linux386/watcom.sym     <OWRELROOT>/binl/
        <CPCMD> codeview/linux386/codeview.dip <OWRELROOT>/binl/
        <CPCMD> codeview/linux386/codeview.sym <OWRELROOT>/binl/
        <CPCMD> export/linux386/export.dip     <OWRELROOT>/binl/
        <CPCMD> export/linux386/export.sym     <OWRELROOT>/binl/
        <CPCMD> mapsym/linux386/mapsym.dip     <OWRELROOT>/binl/
        <CPCMD> mapsym/linux386/mapsym.sym     <OWRELROOT>/binl/

  [ IFDEF cpu_axp <2*> ]
        <CPCMD> dwarf/ntaxp/dwarf.dll          <OWRELROOT>/axpnt/
        <CPCMD> dwarf/ntaxp/dwarf.sym          <OWRELROOT>/axpnt/
        <CPCMD> codeview/ntaxp/codeview.dll    <OWRELROOT>/axpnt/
        <CPCMD> codeview/ntaxp/codeview.sym    <OWRELROOT>/axpnt/
        <CPCMD> export/ntaxp/export.dll        <OWRELROOT>/axpnt/
        <CPCMD> export/ntaxp/export.sym        <OWRELROOT>/axpnt/
        <CPCMD> watcom/ntaxp/watcom.dll        <OWRELROOT>/axpnt/
        <CPCMD> watcom/ntaxp/watcom.sym        <OWRELROOT>/axpnt/
        <CPCMD> mapsym/ntaxp/mapsym.dll        <OWRELROOT>/axpnt/
        <CPCMD> mapsym/ntaxp/mapsym.sym        <OWRELROOT>/axpnt/

  [ IFDEF os_qnx <2*> ]
#        <CPCMD> dwarf/qnx386/dwarf.dip         <OWRELROOT>/qnx/watcom/wd/
#        <CPCMD> dwarf/qnx386/dwarf.sym         <OWRELROOT>/qnx/sym/
#        <CPCMD> codeview/qnx386/codeview.dip   <OWRELROOT>/qnx/watcom/wd/
#        <CPCMD> codeview/qnx386/codeview.sym   <OWRELROOT>/qnx/sym/
#        <CPCMD> export/qnx386/export.dip       <OWRELROOT>/qnx/watcom/wd/
#        <CPCMD> export/qnx386/export.sym       <OWRELROOT>/qnx/sym/
#        <CPCMD> watcom/qnx386/watcom.dip       <OWRELROOT>/qnx/watcom/wd/
#        <CPCMD> watcom/qnx386/watcom.sym       <OWRELROOT>/qnx/sym/

  [ IFDEF (os_rdos "") <2*> ]
        <CPCMD> dwarf/rdos386/dwarf.dll        <OWRELROOT>/rdos/
        <CPCMD> dwarf/rdos386/dwarf.sym        <OWRELROOT>/rdos/
        <CPCMD> watcom/rdos386/watcom.dll      <OWRELROOT>/rdos/
        <CPCMD> watcom/rdos386/watcom.sym      <OWRELROOT>/rdos/
        <CPCMD> mapsym/rdos386/mapsym.dll      <OWRELROOT>/rdos/
        <CPCMD> mapsym/rdos386/mapsym.sym      <OWRELROOT>/rdos/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
