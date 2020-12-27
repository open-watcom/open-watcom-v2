# C++ runtime library Builder Control file
# ========================================

set PROJNAME=cpplib

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
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

# generic - flat (used by WIN386, RDOS, winnt, os2 )
    <CCCMD> complex/generic.386/mf_r/cplx*.lib      <OWRELROOT>/lib386/
    <CCCMD> complex/generic.386/mf_s/cplx*.lib      <OWRELROOT>/lib386/
    <CCCMD> complex/generic.387/mf_r/cplx*.lib      <OWRELROOT>/lib386/
    <CCCMD> complex/generic.387/mf_s/cplx*.lib      <OWRELROOT>/lib386/
# generic - flat (used by WIN386, RDOS, linux )
    <CCCMD> library/generic.386/mf_r/pl*.lib        <OWRELROOT>/lib386/
    <CCCMD> library/generic.386/mf_s/pl*.lib        <OWRELROOT>/lib386/
# generic - small (used by DOS)
    <CCCMD> library/generic.386/ms_r/pl*.lib        <OWRELROOT>/lib386/dos/
    <CCCMD> library/generic.386/ms_s/pl*.lib        <OWRELROOT>/lib386/dos/
    <CCCMD> complex/generic.386/ms_r/cplx*.lib      <OWRELROOT>/lib386/dos/
    <CCCMD> complex/generic.386/ms_s/cplx*.lib      <OWRELROOT>/lib386/dos/
    <CCCMD> complex/generic.387/ms_r/cplx*.lib      <OWRELROOT>/lib386/dos/
    <CCCMD> complex/generic.387/ms_s/cplx*.lib      <OWRELROOT>/lib386/dos/
#generic - small (used by Netware)
    <CCCMD> complex/netware.386/ms_s/cplx*.lib      <OWRELROOT>/lib386/netware/
    <CCCMD> complex/netware.387/ms_s/cplx*.lib      <OWRELROOT>/lib386/netware/
    <CCCMD> library/generic.386/ms_s/pl*.lib        <OWRELROOT>/lib386/netware/
    <CCCMD> library/netware.386/ms_smt/pl*.lib      <OWRELROOT>/lib386/netware/

    <CCCMD> complex/linux.386/mf_r/cplx*.lib        <OWRELROOT>/lib386/linux/
    <CCCMD> complex/linux.386/mf_s/cplx*.lib        <OWRELROOT>/lib386/linux/
    <CCCMD> complex/linux.387/mf_r/cplx*.lib        <OWRELROOT>/lib386/linux/
    <CCCMD> complex/linux.387/mf_s/cplx*.lib        <OWRELROOT>/lib386/linux/
    <CCCMD> library/linux.386/mf_r/pl*.lib          <OWRELROOT>/lib386/linux/
    <CCCMD> library/linux.386/mf_s/pl*.lib          <OWRELROOT>/lib386/linux/
    <CCCMD> library/linux.386/mf_rmt/pl*.lib        <OWRELROOT>/lib386/linux/
    <CCCMD> library/linux.386/mf_smt/pl*.lib        <OWRELROOT>/lib386/linux/

    <CCCMD> library/rdos.386/mf_rmt/pl*.lib         <OWRELROOT>/lib386/rdos/
    <CCCMD> library/rdos.386/mf_smt/pl*.lib         <OWRELROOT>/lib386/rdos/

    <CCCMD> library/winnt.386/mf_r/pl*.lib          <OWRELROOT>/lib386/nt/
    <CCCMD> library/winnt.386/mf_s/pl*.lib          <OWRELROOT>/lib386/nt/
    <CCCMD> library/winnt.386/mf_rmt/pl*.lib        <OWRELROOT>/lib386/nt/
    <CCCMD> library/winnt.386/mf_smt/pl*.lib        <OWRELROOT>/lib386/nt/

    <CCCMD> rtdll/winnt.386/mf_r/plb*.lib           <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_s/plb*.lib           <OWRELROOT>/lib386/nt/
#    <CCCMD> rtdll/winnt.386/mf_rd/plb*.lib          <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_rp/plb*.lib          <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_r/plb*.dll           <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_r/plb*.sym           <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_s/plb*.dll           <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_s/plb*.sym           <OWRELROOT>/binnt/
#    <CCCMD> rtdll/winnt.386/mf_rd/plb*.dll          <OWRELROOT>/binnt/
#    <CCCMD> rtdll/winnt.386/mf_rd/plb*.sym          <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_rp/plb*.dll          <OWRELROOT>/binnt/

    <CCCMD> library/os2.386/mf_r/pl*.lib            <OWRELROOT>/lib386/os2/
    <CCCMD> library/os2.386/mf_s/pl*.lib            <OWRELROOT>/lib386/os2/
    <CCCMD> library/os2.386/mf_rmt/pl*.lib          <OWRELROOT>/lib386/os2/
    <CCCMD> library/os2.386/mf_smt/pl*.lib          <OWRELROOT>/lib386/os2/

    <CCCMD> rtdll/os2.386/mf_r/plb*.lib             <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.386/mf_s/plb*.lib             <OWRELROOT>/lib386/os2/
#    <CCCMD> rtdll/os2.386/mf_rd/plb*.lib            <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.386/mf_r/plb*.dll             <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_r/plb*.sym             <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_s/plb*.dll             <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_s/plb*.sym             <OWRELROOT>/binp/dll/
#    <CCCMD> rtdll/os2.386/mf_rd/plb*.dll            <OWRELROOT>/binp/dll/
#    <CCCMD> rtdll/os2.386/mf_rd/plb*.sym            <OWRELROOT>/binp/dll/

    <CCCMD> library/qnx.386/ms_rmt/pl*.lib          <OWRELROOT>/lib386/qnx/
    <CCCMD> library/qnx.386/ms_smt/pl*.lib          <OWRELROOT>/lib386/qnx/

    <CCCMD> library/winnt.axp/_s/pl*.lib            <OWRELROOT>/libaxp/nt/plib.lib
    <CCCMD> complex/winnt.axp/_s/cplx.lib           <OWRELROOT>/libaxp/cplx.lib
    <CCCMD> library/winnt.axp/_smt/pl*.lib          <OWRELROOT>/libaxp/nt/plibmt.lib

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
