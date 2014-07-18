# C++ runtime library Builder Control file
# ========================================

set PROJDIR=<CWD>
set PROJNAME=cpplib

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> library/generic.086/ms/plibs.lib        <OWRELROOT>/lib286/
    <CCCMD> library/generic.086/ms/plbxs.lib        <OWRELROOT>/lib286/
    <CCCMD> library/generic.086/mc/plibc.lib        <OWRELROOT>/lib286/
    <CCCMD> library/generic.086/mc/plbxc.lib        <OWRELROOT>/lib286/
    <CCCMD> library/generic.086/mm/plibm.lib        <OWRELROOT>/lib286/
    <CCCMD> library/generic.086/mm/plbxm.lib        <OWRELROOT>/lib286/
    <CCCMD> library/generic.086/ml/plibl.lib        <OWRELROOT>/lib286/
    <CCCMD> library/generic.086/ml/plbxl.lib        <OWRELROOT>/lib286/
    <CCCMD> library/generic.086/mh/plibh.lib        <OWRELROOT>/lib286/
    <CCCMD> library/generic.086/mh/plbxh.lib        <OWRELROOT>/lib286/
    <CCCMD> complex/generic.086/ms/cplxs.lib        <OWRELROOT>/lib286/
    <CCCMD> complex/generic.086/mc/cplxc.lib        <OWRELROOT>/lib286/
    <CCCMD> complex/generic.086/mm/cplxm.lib        <OWRELROOT>/lib286/
    <CCCMD> complex/generic.086/ml/cplxl.lib        <OWRELROOT>/lib286/
    <CCCMD> complex/generic.086/mh/cplxh.lib        <OWRELROOT>/lib286/
    <CCCMD> complex/generic.087/ms/cplx7s.lib       <OWRELROOT>/lib286/
    <CCCMD> complex/generic.087/mc/cplx7c.lib       <OWRELROOT>/lib286/
    <CCCMD> complex/generic.087/mm/cplx7m.lib       <OWRELROOT>/lib286/
    <CCCMD> complex/generic.087/ml/cplx7l.lib       <OWRELROOT>/lib286/
    <CCCMD> complex/generic.087/mh/cplx7h.lib       <OWRELROOT>/lib286/

    <CCCMD> library/windows.086/ms/plibs.lib        <OWRELROOT>/lib286/win/
    <CCCMD> library/windows.086/ms/plbxs.lib        <OWRELROOT>/lib286/win/
    <CCCMD> library/windows.086/mc/plibc.lib        <OWRELROOT>/lib286/win/
    <CCCMD> library/windows.086/mc/plbxc.lib        <OWRELROOT>/lib286/win/
    <CCCMD> library/windows.086/mm/plibm.lib        <OWRELROOT>/lib286/win/
    <CCCMD> library/windows.086/mm/plbxm.lib        <OWRELROOT>/lib286/win/
    <CCCMD> library/windows.086/ml/plibl.lib        <OWRELROOT>/lib286/win/
    <CCCMD> library/windows.086/ml/plbxl.lib        <OWRELROOT>/lib286/win/
    <CCCMD> complex/windows.086/ms/cplxs.lib        <OWRELROOT>/lib286/win/
    <CCCMD> complex/windows.086/mc/cplxc.lib        <OWRELROOT>/lib286/win/
    <CCCMD> complex/windows.086/mm/cplxm.lib        <OWRELROOT>/lib286/win/
    <CCCMD> complex/windows.086/ml/cplxl.lib        <OWRELROOT>/lib286/win/
    <CCCMD> complex/windows.087/ms/cplx7s.lib       <OWRELROOT>/lib286/win/
    <CCCMD> complex/windows.087/mc/cplx7c.lib       <OWRELROOT>/lib286/win/
    <CCCMD> complex/windows.087/mm/cplx7m.lib       <OWRELROOT>/lib286/win/
    <CCCMD> complex/windows.087/ml/cplx7l.lib       <OWRELROOT>/lib286/win/

    <CCCMD> library/os2.286/ml_mt/plibmtl.lib       <OWRELROOT>/lib286/os2/
    <CCCMD> library/os2.286/ml_mt/plbxmtl.lib       <OWRELROOT>/lib286/os2/

    <CCCMD> library/generic.386/mf_r/plib3r.lib     <OWRELROOT>/lib386/
    <CCCMD> library/generic.386/mf_r/plbx3r.lib     <OWRELROOT>/lib386/
    <CCCMD> library/generic.386/mf_s/plib3s.lib     <OWRELROOT>/lib386/
    <CCCMD> library/generic.386/mf_s/plbx3s.lib     <OWRELROOT>/lib386/
    <CCCMD> library/generic.386/mf_rmt/plibmt3r.lib <OWRELROOT>/lib386/
    <CCCMD> library/generic.386/mf_rmt/plbxmt3r.lib <OWRELROOT>/lib386/
    <CCCMD> library/generic.386/mf_smt/plibmt3s.lib <OWRELROOT>/lib386/
    <CCCMD> library/generic.386/mf_smt/plbxmt3s.lib <OWRELROOT>/lib386/
    <CCCMD> complex/generic.386/mf_r/cplx3r.lib     <OWRELROOT>/lib386/
    <CCCMD> complex/generic.386/mf_s/cplx3s.lib     <OWRELROOT>/lib386/
    <CCCMD> complex/generic.387/mf_r/cplx73r.lib    <OWRELROOT>/lib386/
    <CCCMD> complex/generic.387/mf_s/cplx73s.lib    <OWRELROOT>/lib386/

    <CCCMD> library/generic.386/ms_r/plib3r.lib     <OWRELROOT>/lib386/dos/
    <CCCMD> library/generic.386/ms_r/plbx3r.lib     <OWRELROOT>/lib386/dos/
    <CCCMD> library/generic.386/ms_s/plib3s.lib     <OWRELROOT>/lib386/dos/
    <CCCMD> library/generic.386/ms_s/plbx3s.lib     <OWRELROOT>/lib386/dos/
    <CCCMD> library/generic.386/ms_rmt/plibmt3r.lib <OWRELROOT>/lib386/dos/
    <CCCMD> library/generic.386/ms_rmt/plbxmt3r.lib <OWRELROOT>/lib386/dos/
    <CCCMD> library/generic.386/ms_smt/plibmt3s.lib <OWRELROOT>/lib386/dos/
    <CCCMD> library/generic.386/ms_smt/plbxmt3s.lib <OWRELROOT>/lib386/dos/
    <CCCMD> complex/generic.386/ms_r/cplx3r.lib     <OWRELROOT>/lib386/dos/
    <CCCMD> complex/generic.386/ms_s/cplx3s.lib     <OWRELROOT>/lib386/dos/
    <CCCMD> complex/generic.387/ms_r/cplx73r.lib    <OWRELROOT>/lib386/dos/
    <CCCMD> complex/generic.387/ms_s/cplx73s.lib    <OWRELROOT>/lib386/dos/

    <CCCMD> library/winnt.386/mf_r/plib3r.lib       <OWRELROOT>/lib386/nt/
    <CCCMD> library/winnt.386/mf_r/plbx3r.lib       <OWRELROOT>/lib386/nt/
    <CCCMD> library/winnt.386/mf_s/plib3s.lib       <OWRELROOT>/lib386/nt/
    <CCCMD> library/winnt.386/mf_s/plbx3s.lib       <OWRELROOT>/lib386/nt/
    <CCCMD> library/winnt.386/mf_rmt/plibmt3r.lib   <OWRELROOT>/lib386/nt/
    <CCCMD> library/winnt.386/mf_rmt/plbxmt3r.lib   <OWRELROOT>/lib386/nt/
    <CCCMD> library/winnt.386/mf_smt/plibmt3s.lib   <OWRELROOT>/lib386/nt/
    <CCCMD> library/winnt.386/mf_smt/plbxmt3s.lib   <OWRELROOT>/lib386/nt/

    <CCCMD> rtdll/winnt.386/mf_r/plb*.lib           <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_s/plb*.lib           <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_rd/plb*.lib          <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_rp/plb*.lib          <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_r/plb*.dll           <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_r/plb*.sym           <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_s/plb*.dll           <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_s/plb*.sym           <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_rd/plb*.dll          <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_rd/plb*.sym          <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_rp/plb*.dll          <OWRELROOT>/binnt/

    <CCCMD> library/os2.386/mf_r/plib3r.lib         <OWRELROOT>/lib386/os2/
    <CCCMD> library/os2.386/mf_r/plbx3r.lib         <OWRELROOT>/lib386/os2/
    <CCCMD> library/os2.386/mf_s/plib3s.lib         <OWRELROOT>/lib386/os2/
    <CCCMD> library/os2.386/mf_s/plbx3s.lib         <OWRELROOT>/lib386/os2/
    <CCCMD> library/os2.386/mf_rmt/plibmt3r.lib     <OWRELROOT>/lib386/os2/
    <CCCMD> library/os2.386/mf_rmt/plbxmt3r.lib     <OWRELROOT>/lib386/os2/
    <CCCMD> library/os2.386/mf_smt/plibmt3s.lib     <OWRELROOT>/lib386/os2/
    <CCCMD> library/os2.386/mf_smt/plbxmt3s.lib     <OWRELROOT>/lib386/os2/

    <CCCMD> rtdll/os2.386/mf_r/plb*.lib             <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.386/mf_s/plb*.lib             <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.386/mf_rd/plb*.lib            <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.386/mf_r/plb*.dll             <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_r/plb*.sym             <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_s/plb*.dll             <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_s/plb*.sym             <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_rd/plb*.dll            <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_rd/plb*.sym            <OWRELROOT>/binp/dll/

    <CCCMD> library/generic.386/ms_s/plib3s.lib     <OWRELROOT>/lib386/netware/
    <CCCMD> library/generic.386/ms_s/plbx3s.lib     <OWRELROOT>/lib386/netware/
    <CCCMD> library/generic.386/ms_smt/plibmt3s.lib <OWRELROOT>/lib386/netware/
    <CCCMD> library/generic.386/ms_smt/plbxmt3s.lib <OWRELROOT>/lib386/netware/
    <CCCMD> complex/netware.386/ms_s/cplx3s.lib     <OWRELROOT>/lib386/netware/
    <CCCMD> complex/netware.387/ms_s/cplx73s.lib    <OWRELROOT>/lib386/netware/

    <CCCMD> library/qnx.386/ms_rmt/plibmt3r.lib     <OWRELROOT>/lib386/qnx/
    <CCCMD> library/qnx.386/ms_rmt/plbxmt3r.lib     <OWRELROOT>/lib386/qnx/
    <CCCMD> library/qnx.386/ms_smt/plibmt3s.lib     <OWRELROOT>/lib386/qnx/
    <CCCMD> library/qnx.386/ms_smt/plbxmt3s.lib     <OWRELROOT>/lib386/qnx/

    <CCCMD> library/winnt.axp/_s/plib.lib           <OWRELROOT>/libaxp/nt/plib.lib
    <CCCMD> library/winnt.axp/_s/plbx.lib           <OWRELROOT>/libaxp/nt/plbx.lib
    <CCCMD> complex/winnt.axp/_s/cplx.lib           <OWRELROOT>/libaxp/cplx.lib
    <CCCMD> library/winnt.axp/_smt/plibmt.lib       <OWRELROOT>/libaxp/nt/plibmt.lib
    <CCCMD> library/winnt.axp/_smt/plbxmt.lib       <OWRELROOT>/libaxp/nt/plbxmt.lib

[ BLOCK . . ]
#============
cdsay <PROJDIR>
