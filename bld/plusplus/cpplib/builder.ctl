# C++ runtime Builder Control file
# ================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

#
# Make sure we have the reference compilers
#
# NB: This will probably go away. But at the moment the libs needs to build
# in both V1 and V2 branch, so for V2 we just grab the compilers and copy
# them in place of the reference compilers (since that's what they effectively
# are anyway).
#
[ BLOCK <1> build rel2 ]
#=======================
    <CPCMD> <OWBINDIR>/wpp386<CMDEXT> ../bin/rpp386<CMDEXT>
    <CPCMD> <OWBINDIR>/wpp<CMDEXT>    ../bin/rppi86<CMDEXT>
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

#[ BLOCK <1> rel2 ]
#=================
#    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
    <CCCMD> generic.086/ms/plibs.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/ms/plbxs.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/mc/plibc.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/mc/plbxc.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/mm/plibm.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/mm/plbxm.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/ml/plibl.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/ml/plbxl.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/mh/plibh.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/mh/plbxh.lib             <RELROOT>/lib286/
    <CCCMD> complex/generic.086/ms/cplxs.lib     <RELROOT>/lib286/
    <CCCMD> complex/generic.086/mc/cplxc.lib     <RELROOT>/lib286/
    <CCCMD> complex/generic.086/mm/cplxm.lib     <RELROOT>/lib286/
    <CCCMD> complex/generic.086/ml/cplxl.lib     <RELROOT>/lib286/
    <CCCMD> complex/generic.086/mh/cplxh.lib     <RELROOT>/lib286/
    <CCCMD> complex/generic.087/ms/cplx7s.lib    <RELROOT>/lib286/
    <CCCMD> complex/generic.087/mc/cplx7c.lib    <RELROOT>/lib286/
    <CCCMD> complex/generic.087/mm/cplx7m.lib    <RELROOT>/lib286/
    <CCCMD> complex/generic.087/ml/cplx7l.lib    <RELROOT>/lib286/
    <CCCMD> complex/generic.087/mh/cplx7h.lib    <RELROOT>/lib286/

    <CCCMD> windows.086/ms/plibs.lib             <RELROOT>/lib286/win/
    <CCCMD> windows.086/ms/plbxs.lib             <RELROOT>/lib286/win/
    <CCCMD> windows.086/mc/plibc.lib             <RELROOT>/lib286/win/
    <CCCMD> windows.086/mc/plbxc.lib             <RELROOT>/lib286/win/
    <CCCMD> windows.086/mm/plibm.lib             <RELROOT>/lib286/win/
    <CCCMD> windows.086/mm/plbxm.lib             <RELROOT>/lib286/win/
    <CCCMD> windows.086/ml/plibl.lib             <RELROOT>/lib286/win/
    <CCCMD> windows.086/ml/plbxl.lib             <RELROOT>/lib286/win/
    <CCCMD> complex/windows.086/ms/cplxs.lib     <RELROOT>/lib286/win/
    <CCCMD> complex/windows.086/mc/cplxc.lib     <RELROOT>/lib286/win/
    <CCCMD> complex/windows.086/mm/cplxm.lib     <RELROOT>/lib286/win/
    <CCCMD> complex/windows.086/ml/cplxl.lib     <RELROOT>/lib286/win/
    <CCCMD> complex/windows.087/ms/cplx7s.lib    <RELROOT>/lib286/win/
    <CCCMD> complex/windows.087/mc/cplx7c.lib    <RELROOT>/lib286/win/
    <CCCMD> complex/windows.087/mm/cplx7m.lib    <RELROOT>/lib286/win/
    <CCCMD> complex/windows.087/ml/cplx7l.lib    <RELROOT>/lib286/win/

    <CCCMD> os2.286/ml_mt/plibmtl.lib            <RELROOT>/lib286/os2/
    <CCCMD> os2.286/ml_mt/plbxmtl.lib            <RELROOT>/lib286/os2/

    <CCCMD> complex/generic.087/mm/cplx7m.lib    <RELROOT>/lib286/
    <CCCMD> generic.086/mc/plbxc.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/mc/plibc.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/ml/plbxl.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/ml/plibl.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/mm/plbxm.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/mm/plibm.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/ms/plbxs.lib             <RELROOT>/lib286/
    <CCCMD> generic.086/ms/plibs.lib             <RELROOT>/lib286/

    <CCCMD> generic.386/mf_r/plib3r.lib          <RELROOT>/lib386/
    <CCCMD> generic.386/mf_r/plbx3r.lib          <RELROOT>/lib386/
    <CCCMD> generic.386/mf_s/plib3s.lib          <RELROOT>/lib386/
    <CCCMD> generic.386/mf_s/plbx3s.lib          <RELROOT>/lib386/
    <CCCMD> complex/generic.386/mf_r/cplx3r.lib  <RELROOT>/lib386/
    <CCCMD> complex/generic.386/mf_s/cplx3s.lib  <RELROOT>/lib386/

    <CCCMD> generic.386/mf_rmt/plibmt3r.lib      <RELROOT>/lib386/
    <CCCMD> generic.386/mf_rmt/plbxmt3r.lib      <RELROOT>/lib386/
    <CCCMD> generic.386/mf_smt/plibmt3s.lib      <RELROOT>/lib386/
    <CCCMD> generic.386/mf_smt/plbxmt3s.lib      <RELROOT>/lib386/
    <CCCMD> complex/generic.387/mf_r/cplx73r.lib <RELROOT>/lib386/
    <CCCMD> complex/generic.387/mf_s/cplx73s.lib <RELROOT>/lib386/

    <CCCMD> winnt.386/mf_r/plbx3r.lib            <RELROOT>/lib386/nt/
    <CCCMD> winnt.386/mf_r/plib3r.lib            <RELROOT>/lib386/nt/
    <CCCMD> winnt.386/mf_s/plbx3s.lib            <RELROOT>/lib386/nt/
    <CCCMD> winnt.386/mf_s/plib3s.lib            <RELROOT>/lib386/nt/
    <CCCMD> winnt.386/mf_rmt/plbxmt3r.lib        <RELROOT>/lib386/nt/
    <CCCMD> winnt.386/mf_rmt/plibmt3r.lib        <RELROOT>/lib386/nt/
    <CCCMD> winnt.386/mf_smt/plbxmt3s.lib        <RELROOT>/lib386/nt/
    <CCCMD> winnt.386/mf_smt/plibmt3s.lib        <RELROOT>/lib386/nt/

    <CCCMD> os2.386/mf_r/plbx3r.lib              <RELROOT>/lib386/os2/
    <CCCMD> os2.386/mf_r/plib3r.lib              <RELROOT>/lib386/os2/
    <CCCMD> os2.386/mf_s/plbx3s.lib              <RELROOT>/lib386/os2/
    <CCCMD> os2.386/mf_s/plib3s.lib              <RELROOT>/lib386/os2/
    <CCCMD> os2.386/mf_rmt/plbxmt3r.lib          <RELROOT>/lib386/os2/
    <CCCMD> os2.386/mf_rmt/plibmt3r.lib          <RELROOT>/lib386/os2/
    <CCCMD> os2.386/mf_smt/plbxmt3s.lib          <RELROOT>/lib386/os2/
    <CCCMD> os2.386/mf_smt/plibmt3s.lib          <RELROOT>/lib386/os2/

    <CCCMD> complex/netware.386/ms_s/cplx3s.lib  <RELROOT>/lib386/netware/
    <CCCMD> complex/netware.387/ms_s/cplx73s.lib <RELROOT>/lib386/netware/

    <CCCMD> q3rmt/plbxmt3r.lib                   <RELROOT>/lib386/qnx/
    <CCCMD> q3rmt/plibmt3r.lib                   <RELROOT>/lib386/qnx/
    <CCCMD> q3smt/plbxmt3s.lib                   <RELROOT>/lib386/qnx/
    <CCCMD> q3smt/plibmt3s.lib                   <RELROOT>/lib386/qnx/

    <CCCMD> winnt.axp/_s/plib.lib                <RELROOT>/libaxp/nt/plib.lib
    <CCCMD> winnt.axp/_s/plbx.lib                <RELROOT>/libaxp/nt/plbx.lib
    <CCCMD> winnt.axp/_smt/plibmt.lib            <RELROOT>/libaxp/nt/plibmt.lib
    <CCCMD> winnt.axp/_smt/plbxmt.lib            <RELROOT>/libaxp/nt/plbxmt.lib
    <CCCMD> complex/winnt.axp/_s/cplx.lib        <RELROOT>/libaxp/cplx.lib

[ BLOCK <1> clean ]
#==================
    [ INCLUDE clean.ctl ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
