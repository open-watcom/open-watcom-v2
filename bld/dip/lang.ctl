# DIP Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
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
        <CPCMD> <DEVDIR>/dip/dwarf/dos386/dwarf.dip         <RELROOT>/binw/
#        <CPCMD> <DEVDIR>/dip/dwarf/dos386/dwarf.sym         <RELROOT>/binw/dwarf.dsy
        <CPCMD> <DEVDIR>/dip/codeview/dos386/codeview.dip   <RELROOT>/binw/
#        <CPCMD> <DEVDIR>/dip/codeview/dos386/codeview.sym   <RELROOT>/binw/codeview.dsy
        <CPCMD> <DEVDIR>/dip/export/dos386/export.dip       <RELROOT>/binw/
#        <CPCMD> <DEVDIR>/dip/export/dos386/export.sym       <RELROOT>/binw/export.dsy
        <CPCMD> <DEVDIR>/dip/watcom/dos386/watcom.dip       <RELROOT>/binw/
#        <CPCMD> <DEVDIR>/dip/watcom/dos386/watcom.sym       <RELROOT>/binw/watcom.dsy
        <CPCMD> <DEVDIR>/dip/mapsym/dos386/mapsym.dip       <RELROOT>/binw/
#        <CPCMD> <DEVDIR>/dip/mapsym/dos386/mapsym.sym       <RELROOT>/binw/watcom.dsy

  [ IFDEF (os_win "") <2*> ]
        <CPCMD> <DEVDIR>/dip/dwarf/wini86/dwarf.dll         <RELROOT>/binw/
#        <CPCMD> <DEVDIR>/dip/dwarf/wini86/dwarf.sym         <RELROOT>/binw/
        <CPCMD> <DEVDIR>/dip/codeview/wini86/codeview.dll   <RELROOT>/binw/
#        <CPCMD> <DEVDIR>/dip/codeview/wini86/codeview.sym   <RELROOT>/binw/
        <CPCMD> <DEVDIR>/dip/export/wini86/export.dll       <RELROOT>/binw/
#        <CPCMD> <DEVDIR>/dip/export/wini86/export.sym       <RELROOT>/binw/
        <CPCMD> <DEVDIR>/dip/watcom/wini86/watcom.dll       <RELROOT>/binw/
#        <CPCMD> <DEVDIR>/dip/watcom/wini86/watcom.sym       <RELROOT>/binw/
        <CPCMD> <DEVDIR>/dip/mapsym/wini86/mapsym.dll       <RELROOT>/binw/
#        <CPCMD> <DEVDIR>/dip/mapsym/wini86/mapsym.sym       <RELROOT>/binw/

  [ IFDEF (os_os2 "") <2*> ]
#        <CPCMD> <DEVDIR>/dip/dwarf/os2i86/dwarf.dll         <RELROOT>/binp/dll/
#        <CPCMD> <DEVDIR>/dip/dwarf/os2i86/dwarf.sym         <RELROOT>/binp/dll/
#        <CPCMD> <DEVDIR>/dip/codeview/os2i86/codeview.dll   <RELROOT>/binp/dll/
#        <CPCMD> <DEVDIR>/dip/codeview/os2i86/codeview.sym   <RELROOT>/binp/dll/
#        <CPCMD> <DEVDIR>/dip/export/os2i86/export.dll       <RELROOT>/binp/dll/
#        <CPCMD> <DEVDIR>/dip/export/os2i86/export.sym       <RELROOT>/binp/dll/
#        <CPCMD> <DEVDIR>/dip/watcom/os2i86/watcom.dll       <RELROOT>/binp/dll/
#        <CPCMD> <DEVDIR>/dip/watcom/os2i86/watcom.sym       <RELROOT>/binp/dll/
        <CPCMD> <DEVDIR>/dip/dwarf/os2386/dwarf.d32         <RELROOT>/binp/
        <CPCMD> <DEVDIR>/dip/dwarf/os2386/dwarf.sym         <RELROOT>/binp/
        <CPCMD> <DEVDIR>/dip/codeview/os2386/codeview.d32   <RELROOT>/binp/
        <CPCMD> <DEVDIR>/dip/codeview/os2386/codeview.sym   <RELROOT>/binp/
        <CPCMD> <DEVDIR>/dip/export/os2386/export.d32       <RELROOT>/binp/
        <CPCMD> <DEVDIR>/dip/export/os2386/export.sym       <RELROOT>/binp/
        <CPCMD> <DEVDIR>/dip/watcom/os2386/watcom.d32       <RELROOT>/binp/
        <CPCMD> <DEVDIR>/dip/watcom/os2386/watcom.sym       <RELROOT>/binp/
        <CPCMD> <DEVDIR>/dip/mapsym/os2386/mapsym.d32       <RELROOT>/binp/
        <CPCMD> <DEVDIR>/dip/mapsym/os2386/mapsym.sym       <RELROOT>/binp/
        <CPCMD> <DEVDIR>/dip/hllcv/os2386/hllcv.d32         <RELROOT>/binp/
        <CPCMD> <DEVDIR>/dip/hllcv/os2386/hllcv.sym         <RELROOT>/binp/

  [ IFDEF (os_nt "") <2*> ]
        <CPCMD> <DEVDIR>/dip/dwarf/nt386/dwarf.dll          <RELROOT>/binnt/
        <CPCMD> <DEVDIR>/dip/dwarf/nt386/dwarf.sym          <RELROOT>/binnt/
        <CPCMD> <DEVDIR>/dip/codeview/nt386/codeview.dll    <RELROOT>/binnt/
        <CPCMD> <DEVDIR>/dip/codeview/nt386/codeview.sym    <RELROOT>/binnt/
        <CPCMD> <DEVDIR>/dip/export/nt386/export.dll        <RELROOT>/binnt/
        <CPCMD> <DEVDIR>/dip/export/nt386/export.sym        <RELROOT>/binnt/
        <CPCMD> <DEVDIR>/dip/watcom/nt386/watcom.dll        <RELROOT>/binnt/
        <CPCMD> <DEVDIR>/dip/watcom/nt386/watcom.sym        <RELROOT>/binnt/
        <CPCMD> <DEVDIR>/dip/mapsym/nt386/mapsym.dll        <RELROOT>/binnt/
        <CPCMD> <DEVDIR>/dip/mapsym/nt386/mapsym.sym        <RELROOT>/binnt/

  [ IFDEF (os_linux "") <2*> ]
        <CPCMD> <DEVDIR>/dip/dwarf/linux386/dwarf.dip       <RELROOT>/binl/
        <CPCMD> <DEVDIR>/dip/dwarf/linux386/dwarf.sym       <RELROOT>/binl/
        <CPCMD> <DEVDIR>/dip/watcom/linux386/watcom.dip     <RELROOT>/binl/
        <CPCMD> <DEVDIR>/dip/watcom/linux386/watcom.sym     <RELROOT>/binl/
        <CPCMD> <DEVDIR>/dip/codeview/linux386/codeview.dip <RELROOT>/binl/
        <CPCMD> <DEVDIR>/dip/codeview/linux386/codeview.sym <RELROOT>/binl/
        <CPCMD> <DEVDIR>/dip/export/linux386/export.dip     <RELROOT>/binl/
        <CPCMD> <DEVDIR>/dip/export/linux386/export.sym     <RELROOT>/binl/
        <CPCMD> <DEVDIR>/dip/mapsym/linux386/mapsym.dip     <RELROOT>/binl/
        <CPCMD> <DEVDIR>/dip/mapsym/linux386/mapsym.sym     <RELROOT>/binl/

  [ IFDEF cpu_axp <2*> ]
        <CPCMD> <DEVDIR>/dip/dwarf/ntaxp/dwarf.dll          <RELROOT>/axpnt/
        <CPCMD> <DEVDIR>/dip/dwarf/ntaxp/dwarf.sym          <RELROOT>/axpnt/
        <CPCMD> <DEVDIR>/dip/codeview/ntaxp/codeview.dll    <RELROOT>/axpnt/
        <CPCMD> <DEVDIR>/dip/codeview/ntaxp/codeview.sym    <RELROOT>/axpnt/
        <CPCMD> <DEVDIR>/dip/export/ntaxp/export.dll        <RELROOT>/axpnt/
        <CPCMD> <DEVDIR>/dip/export/ntaxp/export.sym        <RELROOT>/axpnt/
        <CPCMD> <DEVDIR>/dip/watcom/ntaxp/watcom.dll        <RELROOT>/axpnt/
        <CPCMD> <DEVDIR>/dip/watcom/ntaxp/watcom.sym        <RELROOT>/axpnt/
        <CPCMD> <DEVDIR>/dip/mapsym/ntaxp/mapsym.dll        <RELROOT>/axpnt/
        <CPCMD> <DEVDIR>/dip/mapsym/ntaxp/mapsym.sym        <RELROOT>/axpnt/

  [ IFDEF os_qnx <2*> ]
#        <CPCMD> <DEVDIR>/dip/dwarf/qnx386/dwarf.dip         <RELROOT>/qnx/watcom/wd/
#        <CPCMD> <DEVDIR>/dip/dwarf/qnx386/dwarf.sym         <RELROOT>/qnx/sym/
#        <CPCMD> <DEVDIR>/dip/codeview/qnx386/codeview.dip   <RELROOT>/qnx/watcom/wd/
#        <CPCMD> <DEVDIR>/dip/codeview/qnx386/codeview.sym   <RELROOT>/qnx/sym/
#        <CPCMD> <DEVDIR>/dip/export/qnx386/export.dip       <RELROOT>/qnx/watcom/wd/
#        <CPCMD> <DEVDIR>/dip/export/qnx386/export.sym       <RELROOT>/qnx/sym/
#        <CPCMD> <DEVDIR>/dip/watcom/qnx386/watcom.dip       <RELROOT>/qnx/watcom/wd/
#        <CPCMD> <DEVDIR>/dip/watcom/qnx386/watcom.sym       <RELROOT>/qnx/sym/

  [ IFDEF (os_rdos "") <2*> ]
        <CPCMD> <DEVDIR>/dip/dwarf/rdos386/dwarf.dll        <RELROOT>/rdos/
        <CPCMD> <DEVDIR>/dip/dwarf/rdos386/dwarf.sym        <RELROOT>/rdos/
        <CPCMD> <DEVDIR>/dip/watcom/rdos386/watcom.dll      <RELROOT>/rdos/
        <CPCMD> <DEVDIR>/dip/watcom/rdos386/watcom.sym      <RELROOT>/rdos/
        <CPCMD> <DEVDIR>/dip/mapsym/rdos386/mapsym.dll      <RELROOT>/rdos/
        <CPCMD> <DEVDIR>/dip/mapsym/rdos386/mapsym.sym      <RELROOT>/rdos/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
