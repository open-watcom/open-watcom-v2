# plib Builder Control file
# =========================

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
    <CPCMD> generic.086/ms/plibs.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/ms/plbxs.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/mc/plibc.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/mc/plbxc.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/mm/plibm.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/mm/plbxm.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/ml/plibl.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/ml/plbxl.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/mh/plibh.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/mh/plbxh.lib             <RELROOT>/lib286/
    <CPCMD> complex/generic.086/ms/cplxs.lib     <RELROOT>/lib286/
    <CPCMD> complex/generic.086/mc/cplxc.lib     <RELROOT>/lib286/
    <CPCMD> complex/generic.086/mm/cplxm.lib     <RELROOT>/lib286/
    <CPCMD> complex/generic.086/ml/cplxl.lib     <RELROOT>/lib286/
    <CPCMD> complex/generic.086/mh/cplxh.lib     <RELROOT>/lib286/
    <CPCMD> complex/generic.087/ms/cplx7s.lib    <RELROOT>/lib286/
    <CPCMD> complex/generic.087/mc/cplx7c.lib    <RELROOT>/lib286/
    <CPCMD> complex/generic.087/mm/cplx7m.lib    <RELROOT>/lib286/
    <CPCMD> complex/generic.087/ml/cplx7l.lib    <RELROOT>/lib286/
    <CPCMD> complex/generic.087/mh/cplx7h.lib    <RELROOT>/lib286/

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> windows.086/ms/plibs.lib             <RELROOT>/lib286/win/
    <CPCMD> windows.086/ms/plbxs.lib             <RELROOT>/lib286/win/
    <CPCMD> windows.086/mc/plibc.lib             <RELROOT>/lib286/win/
    <CPCMD> windows.086/mc/plbxc.lib             <RELROOT>/lib286/win/
    <CPCMD> windows.086/mm/plibm.lib             <RELROOT>/lib286/win/
    <CPCMD> windows.086/mm/plbxm.lib             <RELROOT>/lib286/win/
    <CPCMD> windows.086/ml/plibl.lib             <RELROOT>/lib286/win/
    <CPCMD> windows.086/ml/plbxl.lib             <RELROOT>/lib286/win/
    <CPCMD> complex/windows.086/ms/cplxs.lib     <RELROOT>/lib286/win/
    <CPCMD> complex/windows.086/mc/cplxc.lib     <RELROOT>/lib286/win/
    <CPCMD> complex/windows.086/mm/cplxm.lib     <RELROOT>/lib286/win/
    <CPCMD> complex/windows.086/ml/cplxl.lib     <RELROOT>/lib286/win/
    <CPCMD> complex/windows.087/ms/cplx7s.lib    <RELROOT>/lib286/win/
    <CPCMD> complex/windows.087/mc/cplx7c.lib    <RELROOT>/lib286/win/
    <CPCMD> complex/windows.087/mm/cplx7m.lib    <RELROOT>/lib286/win/
    <CPCMD> complex/windows.087/ml/cplx7l.lib    <RELROOT>/lib286/win/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2.286/ml_mt/plibmtl.lib            <RELROOT>/lib286/os2/
    <CPCMD> os2.286/ml_mt/plbxmtl.lib            <RELROOT>/lib286/os2/

  [ IFDEF (os_os2) <2*> ]
    <CPCMD> complex/generic.087/mm/cplx7m.lib    <RELROOT>/lib286/
    <CPCMD> generic.086/mc/plbxc.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/mc/plibc.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/ml/plbxl.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/ml/plibl.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/mm/plbxm.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/mm/plibm.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/ms/plbxs.lib             <RELROOT>/lib286/
    <CPCMD> generic.086/ms/plibs.lib             <RELROOT>/lib286/

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (os_osi os_dos os_linux os_nov "") <2*> ]
    <CPCMD> generic.386/mf_r/plib3r.lib          <RELROOT>/lib386/
    <CPCMD> generic.386/mf_r/plbx3r.lib          <RELROOT>/lib386/
    <CPCMD> generic.386/mf_s/plib3s.lib          <RELROOT>/lib386/
    <CPCMD> generic.386/mf_s/plbx3s.lib          <RELROOT>/lib386/
    <CPCMD> complex/generic.386/mf_r/cplx3r.lib  <RELROOT>/lib386/
    <CPCMD> complex/generic.386/mf_s/cplx3s.lib  <RELROOT>/lib386/

  [ IFDEF (os_osi "") <2*> ]
    <CPCMD> generic.386/mf_rmt/plibmt3r.lib      <RELROOT>/lib386/
    <CPCMD> generic.386/mf_rmt/plbxmt3r.lib      <RELROOT>/lib386/
    <CPCMD> generic.386/mf_smt/plibmt3s.lib      <RELROOT>/lib386/
    <CPCMD> generic.386/mf_smt/plbxmt3s.lib      <RELROOT>/lib386/
    <CPCMD> complex/generic.387/mf_r/cplx73r.lib <RELROOT>/lib386/
    <CPCMD> complex/generic.387/mf_s/cplx73s.lib <RELROOT>/lib386/

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> winnt.386/mf_r/plbx3r.lib            <RELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_r/plib3r.lib            <RELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_s/plbx3s.lib            <RELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_s/plib3s.lib            <RELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_rmt/plbxmt3r.lib        <RELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_rmt/plibmt3r.lib        <RELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_smt/plbxmt3s.lib        <RELROOT>/lib386/nt/
    <CPCMD> winnt.386/mf_smt/plibmt3s.lib        <RELROOT>/lib386/nt/

    <CPCMD> rtdll/winnt.386/mf_r/plb*.lib       <RELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_r/plbrdll.lib    <RELROOT>/lib386/nt/plbrdllx.lib
    <CPCMD> rtdll/winnt.386/mf_r/plb*.dll       <RELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_r/plb*.sym       <RELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_rd/plb*.lib      <RELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_rd/plbrdlld.lib  <RELROOT>/lib386/nt/plbrdlxd.lib
    <CPCMD> rtdll/winnt.386/mf_rd/plb*.dll      <RELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_rp/plb*.lib      <RELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_rp/plb*.dll      <RELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_s/plb*.lib       <RELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_s/plbsdll.lib    <RELROOT>/lib386/nt/plbsdllx.lib
    <CPCMD> rtdll/winnt.386/mf_s/plb*.dll       <RELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_s/plb*.sym       <RELROOT>/binnt/

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2.386/mf_r/plbx3r.lib              <RELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_r/plib3r.lib              <RELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_s/plbx3s.lib              <RELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_s/plib3s.lib              <RELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_rmt/plbxmt3r.lib          <RELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_rmt/plibmt3r.lib          <RELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_smt/plbxmt3s.lib          <RELROOT>/lib386/os2/
    <CPCMD> os2.386/mf_smt/plibmt3s.lib          <RELROOT>/lib386/os2/

    <CPCMD> rtdll/os2.386/mf_r/plb*.lib         <RELROOT>/lib386/os2/
    <CPCMD> rtdll/os2.386/mf_r/plbrdll.lib      <RELROOT>/lib386/os2/plbrdllx.lib
    <CPCMD> rtdll/os2.386/mf_r/plb*.dll         <RELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_r/plb*.sym         <RELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_rd/plb*.lib        <RELROOT>/lib386/os2/
    <CPCMD> rtdll/os2.386/mf_rd/plbrdlld.lib    <RELROOT>/lib386/os2/plbrdlxd.lib
    <CPCMD> rtdll/os2.386/mf_rd/plb*.dll        <RELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_s/plb*.lib         <RELROOT>/lib386/os2/
    <CPCMD> rtdll/os2.386/mf_s/plbsdll.lib      <RELROOT>/lib386/os2/plbsdllx.lib
    <CPCMD> rtdll/os2.386/mf_s/plb*.dll         <RELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_s/plb*.sym         <RELROOT>/binp/dll/

  [ IFDEF (os_nov "") <2*> ]
    <CPCMD> complex/netware.386/ms_s/cplx3s.lib  <RELROOT>/lib386/netware/
    <CPCMD> complex/netware.387/ms_s/cplx73s.lib <RELROOT>/lib386/netware/

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> q3rmt/plbxmt3r.lib                   <RELROOT>/lib386/qnx/
    <CPCMD> q3rmt/plibmt3r.lib                   <RELROOT>/lib386/qnx/
    <CPCMD> q3smt/plbxmt3s.lib                   <RELROOT>/lib386/qnx/
    <CPCMD> q3smt/plibmt3s.lib                   <RELROOT>/lib386/qnx/

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> winnt.axp/_s/plib.lib                <RELROOT>/libaxp/nt/plib.lib
    <CPCMD> winnt.axp/_s/plbx.lib                <RELROOT>/libaxp/nt/plbx.lib
    <CPCMD> winnt.axp/_smt/plibmt.lib            <RELROOT>/libaxp/nt/plibmt.lib
    <CPCMD> winnt.axp/_smt/plbxmt.lib            <RELROOT>/libaxp/nt/plbxmt.lib
    <CPCMD> complex/winnt.axp/_s/cplx.lib        <RELROOT>/libaxp/cplx.lib

[ BLOCK <1> clean ]
#==================
    [ INCLUDE clean.ctl ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
