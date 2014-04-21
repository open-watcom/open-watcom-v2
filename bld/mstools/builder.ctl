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
#    <CCCMD> asaxp/generic/nt386/asaxp.exe   <OWRELROOT>/binnt/
    <CCCMD> cl/generic/nt386/clx.exe        <OWRELROOT>/binnt/cl.exe
    <CCCMD> cl/386/nt386/cl386.exe          <OWRELROOT>/binnt/
#    <CCCMD> cl/axp/nt386/claxp.exe          <OWRELROOT>/binnt/
    <CCCMD> cvtres/generic/nt386/cvtres.exe <OWRELROOT>/binnt/
    <CCCMD> lib/generic/nt386/libx.exe      <OWRELROOT>/binnt/lib.exe
    <CCCMD> lib/386/nt386/lib386.exe        <OWRELROOT>/binnt/
#    <CCCMD> lib/axp/nt386/libaxp.exe        <OWRELROOT>/binnt/
    <CCCMD> link/generic/nt386/linkx.exe    <OWRELROOT>/binnt/link.exe
    <CCCMD> link/386/nt386/link386.exe      <OWRELROOT>/binnt/
#    <CCCMD> link/axp/nt386/linkaxp.exe      <OWRELROOT>/binnt/
    <CCCMD> nmake/generic/nt386/nmake.exe   <OWRELROOT>/binnt/
    <CCCMD> rc/generic/nt386/rcx.exe        <OWRELROOT>/binnt/rc.exe

#    <CCCMD> asaxp/generic/ntaxp/asaxp.exe   <OWRELROOT>/axpnt/
#    <CCCMD> cl/generic/ntaxp/clx.exe        <OWRELROOT>/axpnt/cl.exe
#    <CCCMD> cl/386/ntaxp/cl386.exe          <OWRELROOT>/axpnt/
#    <CCCMD> cl/axp/ntaxp/claxp.exe          <OWRELROOT>/axpnt/
#    <CCCMD> cvtres/generic/ntaxp/cvtres.exe <OWRELROOT>/axpnt/
#    <CCCMD> lib/generic/ntaxp/libx.exe      <OWRELROOT>/axpnt/lib.exe
#    <CCCMD> lib/386/ntaxp/lib386.exe        <OWRELROOT>/axpnt/
#    <CCCMD> lib/axp/ntaxp/libaxp.exe        <OWRELROOT>/axpnt/
#    <CCCMD> link/generic/ntaxp/linkx.exe    <OWRELROOT>/axpnt/link.exe
#    <CCCMD> link/386/ntaxp/link386.exe      <OWRELROOT>/axpnt/
#    <CCCMD> link/axp/ntaxp/linkaxp.exe      <OWRELROOT>/axpnt/
#    <CCCMD> nmake/generic/ntaxp/nmake.exe   <OWRELROOT>/axpnt/
#    <CCCMD> rc/generic/ntaxp/rcx.exe        <OWRELROOT>/axpnt/rc.exe

#    <CCCMD> asaxp/generic/ntx64/asaxp.exe   <OWRELROOT>/binnt64/
    <CCCMD> cl/generic/ntx64/clx.exe        <OWRELROOT>/binnt64/cl.exe
    <CCCMD> cl/386/ntx64/cl386.exe          <OWRELROOT>/binnt64/
#    <CCCMD> cl/axp/ntx64/claxp.exe          <OWRELROOT>/binnt64/
    <CCCMD> cvtres/generic/ntx64/cvtres.exe <OWRELROOT>/binnt64/
    <CCCMD> lib/generic/ntx64/libx.exe      <OWRELROOT>/binnt64/lib.exe
    <CCCMD> lib/386/ntx64/lib386.exe        <OWRELROOT>/binnt64/
#    <CCCMD> lib/axp/ntx64/libaxp.exe        <OWRELROOT>/binnt64/
    <CCCMD> link/generic/ntx64/linkx.exe    <OWRELROOT>/binnt64/link.exe
    <CCCMD> link/386/ntx64/link386.exe      <OWRELROOT>/binnt64/
#    <CCCMD> link/axp/ntx64/linkaxp.exe      <OWRELROOT>/binnt64/
    <CCCMD> nmake/generic/ntx64/nmake.exe   <OWRELROOT>/binnt64/
    <CCCMD> rc/generic/ntx64/rcx.exe        <OWRELROOT>/binnt64/rc.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
