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
 [ IFDEF (os_nt "") <2*> ]
    <CPCMD> cl/nt386/cl.exe            <OWRELROOT>/binnt/cl.exe
    <CPCMD> cl/nt386/cl386.exe         <OWRELROOT>/binnt/cl386.exe
    <CPCMD> cvtres/nt386/cvtres.exe    <OWRELROOT>/binnt/cvtres.exe
    <CPCMD> lib/nt386/lib.exe          <OWRELROOT>/binnt/lib.exe
    <CPCMD> lib/nt386/lib386.exe       <OWRELROOT>/binnt/lib386.exe
    <CPCMD> link/nt386/link.exe        <OWRELROOT>/binnt/link.exe
    <CPCMD> link/nt386/link386.exe     <OWRELROOT>/binnt/link386.exe
    <CPCMD> nmake/nt386/nmake.exe      <OWRELROOT>/binnt/nmake.exe
    <CPCMD> rc/nt386/rc.exe            <OWRELROOT>/binnt/rc.exe
 [ ENDIF ]
#    <CPCMD> asaxp/nt386/asaxp.exe     <OWRELROOT>/binnt/asaxp.exe
#    <CPCMD> asaxp/ntaxp/asaxp.exe     <OWRELROOT>/axpnt/asaxp.exe
#    <CPCMD> cl/nt386/claxp.exe        <OWRELROOT>/binnt/claxp.exe
#    <CPCMD> cl/ntaxp/cl.exe           <OWRELROOT>/axpnt/cl.exe
#    <CPCMD> cl/ntaxp/cl386.exe        <OWRELROOT>/axpnt/cl386.exe
#    <CPCMD> cl/ntaxp/claxp.exe        <OWRELROOT>/axpnt/claxp.exe
#    <CPCMD> cvtres/ntaxp/cvtres.exe   <OWRELROOT>/axpnt/cvtres.exe
#    <CPCMD> lib/nt386/libaxp.exe      <OWRELROOT>/binnt/libaxp.exe
#    <CPCMD> lib/ntaxp/lib.exe         <OWRELROOT>/axpnt/lib.exe
#    <CPCMD> lib/ntaxp/lib386.exe      <OWRELROOT>/axpnt/lib386.exe
#    <CPCMD> lib/ntaxp/libaxp.exe      <OWRELROOT>/axpnt/libaxp.exe
#    <CPCMD> link/nt386/linkaxp.exe    <OWRELROOT>/binnt/linkaxp.exe
#    <CPCMD> link/ntaxp/link.exe       <OWRELROOT>/axpnt/link.exe
#    <CPCMD> link/ntaxp/link386.exe    <OWRELROOT>/axpnt/link386.exe
#    <CPCMD> link/ntaxp/linkaxp.exe    <OWRELROOT>/axpnt/linkaxp.exe
#    <CPCMD> nmake/ntaxp/nmake.exe     <OWRELROOT>/axpnt/nmake.exe
#    <CPCMD> rc/ntaxp/rc.exe           <OWRELROOT>/axpnt/rc.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
