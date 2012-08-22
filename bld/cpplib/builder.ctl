# C++ runtime library Builder Control file
# ========================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

#
# Make sure we have the reference compilers
#
[ BLOCK <1> build rel2 ]
#=======================
    [ INCLUDE prereq.ctl ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (os_osi "") <2*> ]
    <CPCMD> generic.086/ms/plibs.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/ms/plbxs.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/mc/plibc.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/mc/plbxc.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/mm/plibm.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/mm/plbxm.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/ml/plibl.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/ml/plbxl.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/mh/plibh.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/mh/plbxh.lib             <OWRELROOT>/lib286/
    <CPCMD> complex/generic.086/ms/cplxs.lib     <OWRELROOT>/lib286/
    <CPCMD> complex/generic.086/mc/cplxc.lib     <OWRELROOT>/lib286/
    <CPCMD> complex/generic.086/mm/cplxm.lib     <OWRELROOT>/lib286/
    <CPCMD> complex/generic.086/ml/cplxl.lib     <OWRELROOT>/lib286/
    <CPCMD> complex/generic.086/mh/cplxh.lib     <OWRELROOT>/lib286/
    <CPCMD> complex/generic.087/ms/cplx7s.lib    <OWRELROOT>/lib286/
    <CPCMD> complex/generic.087/mc/cplx7c.lib    <OWRELROOT>/lib286/
    <CPCMD> complex/generic.087/mm/cplx7m.lib    <OWRELROOT>/lib286/
    <CPCMD> complex/generic.087/ml/cplx7l.lib    <OWRELROOT>/lib286/
    <CPCMD> complex/generic.087/mh/cplx7h.lib    <OWRELROOT>/lib286/

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> windows.086/ms/plibs.lib             <OWRELROOT>/lib286/win/
    <CPCMD> windows.086/ms/plbxs.lib             <OWRELROOT>/lib286/win/
    <CPCMD> windows.086/mc/plibc.lib             <OWRELROOT>/lib286/win/
    <CPCMD> windows.086/mc/plbxc.lib             <OWRELROOT>/lib286/win/
    <CPCMD> windows.086/mm/plibm.lib             <OWRELROOT>/lib286/win/
    <CPCMD> windows.086/mm/plbxm.lib             <OWRELROOT>/lib286/win/
    <CPCMD> windows.086/ml/plibl.lib             <OWRELROOT>/lib286/win/
    <CPCMD> windows.086/ml/plbxl.lib             <OWRELROOT>/lib286/win/
    <CPCMD> complex/windows.086/ms/cplxs.lib     <OWRELROOT>/lib286/win/
    <CPCMD> complex/windows.086/mc/cplxc.lib     <OWRELROOT>/lib286/win/
    <CPCMD> complex/windows.086/mm/cplxm.lib     <OWRELROOT>/lib286/win/
    <CPCMD> complex/windows.086/ml/cplxl.lib     <OWRELROOT>/lib286/win/
    <CPCMD> complex/windows.087/ms/cplx7s.lib    <OWRELROOT>/lib286/win/
    <CPCMD> complex/windows.087/mc/cplx7c.lib    <OWRELROOT>/lib286/win/
    <CPCMD> complex/windows.087/mm/cplx7m.lib    <OWRELROOT>/lib286/win/
    <CPCMD> complex/windows.087/ml/cplx7l.lib    <OWRELROOT>/lib286/win/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2.286/ml_mt/plibmtl.lib            <OWRELROOT>/lib286/os2/
    <CPCMD> os2.286/ml_mt/plbxmtl.lib            <OWRELROOT>/lib286/os2/

  [ IFDEF (os_os2) <2*> ]
    <CPCMD> complex/generic.087/mm/cplx7m.lib    <OWRELROOT>/lib286/
    <CPCMD> generic.086/mc/plbxc.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/mc/plibc.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/ml/plbxl.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/ml/plibl.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/mm/plbxm.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/mm/plibm.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/ms/plbxs.lib             <OWRELROOT>/lib286/
    <CPCMD> generic.086/ms/plibs.lib             <OWRELROOT>/lib286/

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (os_osi os_dos os_linux os_nov "") <2*> ]
    <CPCMD> generic.386/mf_r/plib3r.lib          <OWRELROOT>/lib386/
    <CPCMD> generic.386/mf_r/plbx3r.lib          <OWRELROOT>/lib386/
    <CPCMD> generic.386/mf_s/plib3s.lib          <OWRELROOT>/lib386/
    <CPCMD> generic.386/mf_s/plbx3s.lib          <OWRELROOT>/lib386/
    <CPCMD> complex/generic.386/mf_r/cplx3r.lib  <OWRELROOT>/lib386/
    <CPCMD> complex/generic.386/mf_s/cplx3s.lib  <OWRELROOT>/lib386/

  [ IFDEF (os_osi "") <2*> ]
    <CPCMD> generic.386/mf_rmt/plibmt3r.lib      <OWRELROOT>/lib386/
    <CPCMD> generic.386/mf_rmt/plbxmt3r.lib      <OWRELROOT>/lib386/
    <CPCMD> generic.386/mf_smt/plibmt3s.lib      <OWRELROOT>/lib386/
    <CPCMD> generic.386/mf_smt/plbxmt3s.lib      <OWRELROOT>/lib386/
    <CPCMD> complex/generic.387/mf_r/cplx73r.lib <OWRELROOT>/lib386/
    <CPCMD> complex/generic.387/mf_s/cplx73s.lib <OWRELROOT>/lib386/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> winnt.386/mf_r/plbx3r.lib            <OWRELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_r/plib3r.lib            <OWRELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_s/plbx3s.lib            <OWRELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_s/plib3s.lib            <OWRELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_rmt/plbxmt3r.lib        <OWRELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_rmt/plibmt3r.lib        <OWRELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_smt/plbxmt3s.lib        <OWRELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_smt/plibmt3s.lib        <OWRELROOT>/lib386/nt/

    <CPCMD> rtdll/winnt.386/mf_r/plb*.lib       <OWRELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_r/plbrdll.lib    <OWRELROOT>/lib386/nt/plbrdllx.lib
    <CPCMD> rtdll/winnt.386/mf_r/plb*.dll       <OWRELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_r/plb*.sym       <OWRELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_rd/plb*.lib      <OWRELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_rd/plbrdlld.lib  <OWRELROOT>/lib386/nt/plbrdlxd.lib
    <CPCMD> rtdll/winnt.386/mf_rd/plb*.dll      <OWRELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_rp/plb*.lib      <OWRELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_rp/plb*.dll      <OWRELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_s/plb*.lib       <OWRELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_s/plbsdll.lib    <OWRELROOT>/lib386/nt/plbsdllx.lib
    <CPCMD> rtdll/winnt.386/mf_s/plb*.dll       <OWRELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_s/plb*.sym       <OWRELROOT>/binnt/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2.386/mf_r/plbx3r.lib              <OWRELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_r/plib3r.lib              <OWRELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_s/plbx3s.lib              <OWRELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_s/plib3s.lib              <OWRELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_rmt/plbxmt3r.lib          <OWRELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_rmt/plibmt3r.lib          <OWRELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_smt/plbxmt3s.lib          <OWRELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_smt/plibmt3s.lib          <OWRELROOT>/lib386/os2/

    <CPCMD> rtdll/os2.386/mf_r/plb*.lib         <OWRELROOT>/lib386/os2/
    <CPCMD> rtdll/os2.386/mf_r/plbrdll.lib      <OWRELROOT>/lib386/os2/plbrdllx.lib
    <CPCMD> rtdll/os2.386/mf_r/plb*.dll         <OWRELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_r/plb*.sym         <OWRELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_rd/plb*.lib        <OWRELROOT>/lib386/os2/
    <CPCMD> rtdll/os2.386/mf_rd/plbrdlld.lib    <OWRELROOT>/lib386/os2/plbrdlxd.lib
    <CPCMD> rtdll/os2.386/mf_rd/plb*.dll        <OWRELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_s/plb*.lib         <OWRELROOT>/lib386/os2/
    <CPCMD> rtdll/os2.386/mf_s/plbsdll.lib      <OWRELROOT>/lib386/os2/plbsdllx.lib
    <CPCMD> rtdll/os2.386/mf_s/plb*.dll         <OWRELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_s/plb*.sym         <OWRELROOT>/binp/dll/

  [ IFDEF (os_nov "") <2*> ]
    <CPCMD> complex/netware.386/ms_s/cplx3s.lib  <OWRELROOT>/lib386/netware/
    <CPCMD> complex/netware.387/ms_s/cplx73s.lib <OWRELROOT>/lib386/netware/

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> q3rmt/plbxmt3r.lib                   <OWRELROOT>/lib386/qnx/
    <CPCMD> q3rmt/plibmt3r.lib                   <OWRELROOT>/lib386/qnx/
    <CPCMD> q3smt/plbxmt3s.lib                   <OWRELROOT>/lib386/qnx/
    <CPCMD> q3smt/plibmt3s.lib                   <OWRELROOT>/lib386/qnx/

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> winnt.axp/_s/plib.lib                <OWRELROOT>/libaxp/nt/plib.lib
    <CPCMD> winnt.axp/_s/plbx.lib                <OWRELROOT>/libaxp/nt/plbx.lib
    <CPCMD> winnt.axp/_smt/plibmt.lib            <OWRELROOT>/libaxp/nt/plibmt.lib
    <CPCMD> winnt.axp/_smt/plbxmt.lib            <OWRELROOT>/libaxp/nt/plbxmt.lib
    <CPCMD> complex/winnt.axp/_s/cplx.lib        <OWRELROOT>/libaxp/cplx.lib

[ BLOCK <1> clean ]
#==================
    [ INCLUDE clean.ctl ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
