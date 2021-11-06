# MSTOOLS Builder Control file
# ============================

set PROJNAME=mstools

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel acprel ]
#===================================
#    <CCCMD> asaxp/generic/nt386/<OWOBJDIR>/asaxp.exe    <OWRELROOT>/binnt/
    <CCCMD> cl/generic/nt386/<OWOBJDIR>/cl.exe          <OWRELROOT>/binnt/
    <CCCMD> cl/386/nt386/<OWOBJDIR>/cl386.exe           <OWRELROOT>/binnt/
#    <CCCMD> cl/axp/nt386/<OWOBJDIR>/claxp.exe           <OWRELROOT>/binnt/
    <CCCMD> cvtres/generic/nt386/<OWOBJDIR>/cvtres.exe  <OWRELROOT>/binnt/
    <CCCMD> lib/generic/nt386/<OWOBJDIR>/lib.exe        <OWRELROOT>/binnt/
    <CCCMD> lib/386/nt386/<OWOBJDIR>/lib386.exe         <OWRELROOT>/binnt/
#    <CCCMD> lib/axp/nt386/<OWOBJDIR>/libaxp.exe         <OWRELROOT>/binnt/
    <CCCMD> link/generic/nt386/<OWOBJDIR>/link.exe      <OWRELROOT>/binnt/
    <CCCMD> link/386/nt386/<OWOBJDIR>/link386.exe       <OWRELROOT>/binnt/
#    <CCCMD> link/axp/nt386/<OWOBJDIR>/linkaxp.exe       <OWRELROOT>/binnt/
    <CCCMD> nmake/generic/nt386/<OWOBJDIR>/nmake.exe    <OWRELROOT>/binnt/
    <CCCMD> rc/generic/nt386/<OWOBJDIR>/rc.exe          <OWRELROOT>/binnt/

#    <CCCMD> asaxp/generic/ntaxp/<OWOBJDIR>/asaxp.exe    <OWRELROOT>/axpnt/
#    <CCCMD> cl/generic/ntaxp/<OWOBJDIR>/cl.exe          <OWRELROOT>/axpnt/
#    <CCCMD> cl/386/ntaxp/<OWOBJDIR>/cl386.exe           <OWRELROOT>/axpnt/
#    <CCCMD> cl/axp/ntaxp/<OWOBJDIR>/claxp.exe           <OWRELROOT>/axpnt/
#    <CCCMD> cvtres/generic/ntaxp/<OWOBJDIR>/cvtres.exe  <OWRELROOT>/axpnt/
#    <CCCMD> lib/generic/ntaxp/<OWOBJDIR>/lib.exe        <OWRELROOT>/axpnt/
#    <CCCMD> lib/386/ntaxp/<OWOBJDIR>/lib386.exe         <OWRELROOT>/axpnt/
#    <CCCMD> lib/axp/ntaxp/<OWOBJDIR>/libaxp.exe         <OWRELROOT>/axpnt/
#    <CCCMD> link/generic/ntaxp/<OWOBJDIR>/link.exe      <OWRELROOT>/axpnt/
#    <CCCMD> link/386/ntaxp/<OWOBJDIR>/link386.exe       <OWRELROOT>/axpnt/
#    <CCCMD> link/axp/ntaxp/<OWOBJDIR>/linkaxp.exe       <OWRELROOT>/axpnt/
#    <CCCMD> nmake/generic/ntaxp/<OWOBJDIR>/nmake.exe    <OWRELROOT>/axpnt/
#    <CCCMD> rc/generic/ntaxp/<OWOBJDIR>/rc.exe          <OWRELROOT>/axpnt/

#    <CCCMD> asaxp/generic/ntx64/<OWOBJDIR>/asaxp.exe    <OWRELROOT>/binnt64/
    <CCCMD> cl/generic/ntx64/<OWOBJDIR>/cl.exe          <OWRELROOT>/binnt64/
    <CCCMD> cl/386/ntx64/<OWOBJDIR>/cl386.exe           <OWRELROOT>/binnt64/
#    <CCCMD> cl/axp/ntx64/<OWOBJDIR>/claxp.exe           <OWRELROOT>/binnt64/
    <CCCMD> cvtres/generic/ntx64/<OWOBJDIR>/cvtres.exe  <OWRELROOT>/binnt64/
    <CCCMD> lib/generic/ntx64/<OWOBJDIR>/lib.exe        <OWRELROOT>/binnt64/
    <CCCMD> lib/386/ntx64/<OWOBJDIR>/lib386.exe         <OWRELROOT>/binnt64/
#    <CCCMD> lib/axp/ntx64/<OWOBJDIR>/libaxp.exe         <OWRELROOT>/binnt64/
    <CCCMD> link/generic/ntx64/<OWOBJDIR>/link.exe      <OWRELROOT>/binnt64/
    <CCCMD> link/386/ntx64/<OWOBJDIR>/link386.exe       <OWRELROOT>/binnt64/
#    <CCCMD> link/axp/ntx64/<OWOBJDIR>/linkaxp.exe       <OWRELROOT>/binnt64/
    <CCCMD> nmake/generic/ntx64/<OWOBJDIR>/nmake.exe    <OWRELROOT>/binnt64/
    <CCCMD> rc/generic/ntx64/<OWOBJDIR>/rc.exe          <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
