# MATHLIB Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=mathlib

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> library/msdos.286/mc/mathc.lib          <OWRELROOT>/lib286/mathc.lib
    <CCCMD> library/msdos.286/mh/mathh.lib          <OWRELROOT>/lib286/mathh.lib
    <CCCMD> library/msdos.286/ml/mathl.lib          <OWRELROOT>/lib286/mathl.lib
    <CCCMD> library/msdos.286/mm/mathm.lib          <OWRELROOT>/lib286/mathm.lib
    <CCCMD> library/msdos.286/ms/maths.lib          <OWRELROOT>/lib286/maths.lib
    <CCCMD> library/msdos.287/mc/math87c.lib        <OWRELROOT>/lib286/math87c.lib
    <CCCMD> library/msdos.287/mh/math87h.lib        <OWRELROOT>/lib286/math87h.lib
    <CCCMD> library/msdos.287/ml/math87l.lib        <OWRELROOT>/lib286/math87l.lib
    <CCCMD> library/msdos.287/mm/math87m.lib        <OWRELROOT>/lib286/math87m.lib
    <CCCMD> library/msdos.287/ms/math87s.lib        <OWRELROOT>/lib286/math87s.lib

    <CCCMD> library/windows.286/mc/mathc.lib        <OWRELROOT>/lib286/win/mathc.lib
    <CCCMD> library/windows.286/ml/mathl.lib        <OWRELROOT>/lib286/win/mathl.lib
    <CCCMD> library/windows.287/mc/math87c.lib      <OWRELROOT>/lib286/win/math87c.lib
    <CCCMD> library/windows.287/ml/math87l.lib      <OWRELROOT>/lib286/win/math87l.lib

    <CCCMD> library/msdos.386/ms_r/math3r.lib       <OWRELROOT>/lib386/dos/math3r.lib
    <CCCMD> library/msdos.386/ms_rd/math3r.lib      <OWRELROOT>/lib386/dos/math3rd.lib
    <CCCMD> library/msdos.386/ms_s/math3s.lib       <OWRELROOT>/lib386/dos/math3s.lib
    <CCCMD> library/msdos.387/ms_r/math387r.lib     <OWRELROOT>/lib386/dos/math387r.lib
    <CCCMD> library/msdos.387/ms_rd/math387r.lib    <OWRELROOT>/lib386/dos/mth387rd.lib
    <CCCMD> library/msdos.387/ms_s/math387s.lib     <OWRELROOT>/lib386/dos/math387s.lib

    <CCCMD> library/msdos.386/ms_s/math3s.lib       <OWRELROOT>/lib386/netware/math3s.lib
    <CCCMD> library/msdos.387/ms_s/math387s.lib     <OWRELROOT>/lib386/netware/math387s.lib

    <CCCMD> library/winnt.axp/_s/mathaxp.lib        <OWRELROOT>/libaxp/nt/math.lib

    <CCCMD> library/msdos.386/mf_r/math3r.lib       <OWRELROOT>/lib386/math3r.lib
    <CCCMD> library/msdos.386/mf_rd/math3r.lib      <OWRELROOT>/lib386/math3rd.lib
    <CCCMD> library/msdos.386/mf_s/math3s.lib       <OWRELROOT>/lib386/math3s.lib
    <CCCMD> library/msdos.387/mf_r/math387r.lib     <OWRELROOT>/lib386/math387r.lib
    <CCCMD> library/msdos.387/mf_rd/math387r.lib    <OWRELROOT>/lib386/mth387rd.lib
    <CCCMD> library/msdos.387/mf_s/math387s.lib     <OWRELROOT>/lib386/math387s.lib

    <CCCMD> rtdll/os2.386/mf_r/mt*.lib              <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.386/mf_rd/mt*.lib             <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.386/mf_s/mt*.lib              <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.387/mf_r/mt*.lib              <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.387/mf_rd/mt*.lib             <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.387/mf_s/mt*.lib              <OWRELROOT>/lib386/os2/

    <CCCMD> rtdll/os2.386/mf_r/mt*.dll              <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_r/mt*.sym              <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_rd/mt*.dll             <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_s/mt*.dll              <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_s/mt*.sym              <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.387/mf_r/mt*.dll              <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.387/mf_r/mt*.sym              <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.387/mf_rd/mt*.dll             <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.387/mf_s/mt*.dll              <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.387/mf_s/mt*.sym              <OWRELROOT>/binp/dll/

    <CCCMD> rtdll/winnt.386/mf_r/mt*.lib            <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_rd/mt*.lib           <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_rp/mt*.lib           <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_s/mt*.lib            <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.387/mf_r/mt*.lib            <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.387/mf_rd/mt*.lib           <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.387/mf_rp/mt*.lib           <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.387/mf_s/mt*.lib            <OWRELROOT>/lib386/nt/

    <CCCMD> rtdll/winnt.386/mf_r/mt*.dll            <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_r/mt*.sym            <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_rd/mt*.dll           <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_rp/mt*.dll           <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_s/mt*.dll            <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_s/mt*.sym            <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.387/mf_r/mt*.dll            <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.387/mf_r/mt*.sym            <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.387/mf_rd/mt*.dll           <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.387/mf_rp/mt*.dll           <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.387/mf_s/mt*.dll            <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.387/mf_s/mt*.sym            <OWRELROOT>/binnt/

[ BLOCK <1> tests ]
#==================
    cdsay qa
    pmake -d mathqa -h tests

[ BLOCK . . ]
#============
cdsay <PROJDIR>
