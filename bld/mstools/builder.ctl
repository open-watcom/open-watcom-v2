# MSTOOLS Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=mstools

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel acprel ]
#==============================
    <CCCMD> cl/nt386/clx.exe           <OWRELROOT>/binnt/cl.exe
    <CCCMD> cl/nt386/cl386.exe         <OWRELROOT>/binnt/
    <CCCMD> cvtres/nt386/cvtres.exe    <OWRELROOT>/binnt/
    <CCCMD> lib/nt386/libx.exe         <OWRELROOT>/binnt/lib.exe
    <CCCMD> lib/nt386/lib386.exe       <OWRELROOT>/binnt/
    <CCCMD> link/nt386/linkx.exe       <OWRELROOT>/binnt/link.exe
    <CCCMD> link/nt386/link386.exe     <OWRELROOT>/binnt/
    <CCCMD> nmake/nt386/nmake.exe      <OWRELROOT>/binnt/
    <CCCMD> rc/nt386/rcx.exe           <OWRELROOT>/binnt/rc.exe

#    <CCCMD> asaxp/nt386/asaxp.exe      <OWRELROOT>/binnt/
#    <CCCMD> asaxp/ntaxp/asaxp.exe      <OWRELROOT>/axpnt/
#    <CCCMD> cl/nt386/claxp.exe         <OWRELROOT>/binnt/
#    <CCCMD> cl/ntaxp/clx.exe           <OWRELROOT>/axpnt/cl.exe
#    <CCCMD> cl/ntaxp/cl386.exe         <OWRELROOT>/axpnt/
#    <CCCMD> cl/ntaxp/claxp.exe         <OWRELROOT>/axpnt/
#    <CCCMD> cvtres/ntaxp/cvtres.exe    <OWRELROOT>/axpnt/
#    <CCCMD> lib/nt386/libaxp.exe       <OWRELROOT>/binnt/
#    <CCCMD> lib/ntaxp/libx.exe         <OWRELROOT>/axpnt/lib.exe
#    <CCCMD> lib/ntaxp/lib386.exe       <OWRELROOT>/axpnt/
#    <CCCMD> lib/ntaxp/libaxp.exe       <OWRELROOT>/axpnt/
#    <CCCMD> link/nt386/linkaxp.exe     <OWRELROOT>/binnt/
#    <CCCMD> link/ntaxp/linkx.exe       <OWRELROOT>/axpnt/link.exe
#    <CCCMD> link/ntaxp/link386.exe     <OWRELROOT>/axpnt/
#    <CCCMD> link/ntaxp/linkaxp.exe     <OWRELROOT>/axpnt/
#    <CCCMD> nmake/ntaxp/nmake.exe      <OWRELROOT>/axpnt/
#    <CCCMD> rc/ntaxp/rcx.exe           <OWRELROOT>/axpnt/rc.exe

    <CCCMD> cl/ntx64/clx.exe           <OWRELROOT>/binnt64/cl.exe
    <CCCMD> cl/ntx64/cl386.exe         <OWRELROOT>/binnt64/
    <CCCMD> cvtres/ntx64/cvtres.exe    <OWRELROOT>/binnt64/
    <CCCMD> lib/ntx64/libx.exe         <OWRELROOT>/binnt64/lib.exe
    <CCCMD> lib/ntx64/lib386.exe       <OWRELROOT>/binnt64/
    <CCCMD> link/ntx64/linkx.exe       <OWRELROOT>/binnt64/link.exe
    <CCCMD> link/ntx64/link386.exe     <OWRELROOT>/binnt64/
    <CCCMD> nmake/ntx64/nmake.exe      <OWRELROOT>/binnt64/
    <CCCMD> rc/ntx64/rcx.exe           <OWRELROOT>/binnt64/rc.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
