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
    <CCCMD> cl/nt386/cl386.exe         <OWRELROOT>/binnt/cl386.exe
    <CCCMD> cvtres/nt386/cvtres.exe    <OWRELROOT>/binnt/cvtres.exe
    <CCCMD> lib/nt386/lib.exe          <OWRELROOT>/binnt/lib.exe
    <CCCMD> lib/nt386/lib386.exe       <OWRELROOT>/binnt/lib386.exe
    <CCCMD> link/nt386/link.exe        <OWRELROOT>/binnt/link.exe
    <CCCMD> link/nt386/link386.exe     <OWRELROOT>/binnt/link386.exe
    <CCCMD> nmake/nt386/nmake.exe      <OWRELROOT>/binnt/nmake.exe
    <CCCMD> rc/nt386/rc.exe            <OWRELROOT>/binnt/rc.exe

#    <CCCMD> asaxp/nt386/asaxp.exe      <OWRELROOT>/binnt/asaxp.exe
#    <CCCMD> asaxp/ntaxp/asaxp.exe      <OWRELROOT>/axpnt/asaxp.exe
#    <CCCMD> cl/nt386/claxp.exe         <OWRELROOT>/binnt/claxp.exe
#    <CCCMD> cl/ntaxp/clx.exe           <OWRELROOT>/axpnt/cl.exe
#    <CCCMD> cl/ntaxp/cl386.exe         <OWRELROOT>/axpnt/cl386.exe
#    <CCCMD> cl/ntaxp/claxp.exe         <OWRELROOT>/axpnt/claxp.exe
#    <CCCMD> cvtres/ntaxp/cvtres.exe    <OWRELROOT>/axpnt/cvtres.exe
#    <CCCMD> lib/nt386/libaxp.exe       <OWRELROOT>/binnt/libaxp.exe
#    <CCCMD> lib/ntaxp/lib.exe          <OWRELROOT>/axpnt/lib.exe
#    <CCCMD> lib/ntaxp/lib386.exe       <OWRELROOT>/axpnt/lib386.exe
#    <CCCMD> lib/ntaxp/libaxp.exe       <OWRELROOT>/axpnt/libaxp.exe
#    <CCCMD> link/nt386/linkaxp.exe     <OWRELROOT>/binnt/linkaxp.exe
#    <CCCMD> link/ntaxp/link.exe        <OWRELROOT>/axpnt/link.exe
#    <CCCMD> link/ntaxp/link386.exe     <OWRELROOT>/axpnt/link386.exe
#    <CCCMD> link/ntaxp/linkaxp.exe     <OWRELROOT>/axpnt/linkaxp.exe
#    <CCCMD> nmake/ntaxp/nmake.exe      <OWRELROOT>/axpnt/nmake.exe
#    <CCCMD> rc/ntaxp/rc.exe            <OWRELROOT>/axpnt/rc.exe

    <CCCMD> cl/ntx64/clx.exe           <OWRELROOT>/binnt64/
    <CCCMD> cl/ntx64/cl386.exe         <OWRELROOT>/binnt64/
    <CCCMD> cvtres/ntx64/cvtres.exe    <OWRELROOT>/binnt64/
    <CCCMD> lib/ntx64/lib.exe          <OWRELROOT>/binnt64/
    <CCCMD> lib/ntx64/lib386.exe       <OWRELROOT>/binnt64/
    <CCCMD> link/ntx64/link.exe        <OWRELROOT>/binnt64/
    <CCCMD> link/ntx64/link386.exe     <OWRELROOT>/binnt64/
    <CCCMD> nmake/ntx64/nmake.exe      <OWRELROOT>/binnt64/
    <CCCMD> rc/ntx64/rc.exe            <OWRELROOT>/binnt64/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
