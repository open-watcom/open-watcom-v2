# CLIB Builder Control file
# =========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h
    cdsay <PROJDIR>

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> library/msdos.086/mc/clibc.lib                  <OWRELROOT>/lib286/dos/clibc.lib
    <CPCMD> library/msdos.086/mh/clibh.lib                  <OWRELROOT>/lib286/dos/clibh.lib
    <CPCMD> library/msdos.086/ml/clibl.lib                  <OWRELROOT>/lib286/dos/clibl.lib
    <CPCMD> library/msdos.086/ml_ov/clibl.lib               <OWRELROOT>/lib286/dos/clibol.lib
    <CPCMD> library/msdos.086/mm/clibm.lib                  <OWRELROOT>/lib286/dos/clibm.lib
    <CPCMD> library/msdos.086/mm_ov/clibm.lib               <OWRELROOT>/lib286/dos/clibom.lib
    <CPCMD> library/msdos.086/ms/clibs.lib                  <OWRELROOT>/lib286/dos/clibs.lib
    <CPCMD> doslfn/library/msdos.086/mc/doslfnc.lib         <OWRELROOT>/lib286/dos/doslfnc.lib
    <CPCMD> doslfn/library/msdos.086/mh/doslfnh.lib         <OWRELROOT>/lib286/dos/doslfnh.lib
    <CPCMD> doslfn/library/msdos.086/ml/doslfnl.lib         <OWRELROOT>/lib286/dos/doslfnl.lib
    <CPCMD> doslfn/library/msdos.086/ml_ov/doslfnl.lib      <OWRELROOT>/lib286/dos/doslfnol.lib
    <CPCMD> doslfn/library/msdos.086/mm/doslfnm.lib         <OWRELROOT>/lib286/dos/doslfnm.lib
    <CPCMD> doslfn/library/msdos.086/mm_ov/doslfnm.lib      <OWRELROOT>/lib286/dos/doslfnom.lib
    <CPCMD> doslfn/library/msdos.086/ms/doslfns.lib         <OWRELROOT>/lib286/dos/doslfns.lib
    <CPCMD> startup/library/msdos.086/ml/d16msels.obj       <OWRELROOT>/lib286/dos/d16msels.obj
    <CPCMD> startup/library/msdos.086/ml/dos16m.obj         <OWRELROOT>/lib286/dos/dos16m.obj
    <CPCMD> startup/library/msdos.086/ms/binmode.obj        <OWRELROOT>/lib286/dos/binmode.obj
    <CPCMD> startup/library/msdos.086/ms/commode.obj        <OWRELROOT>/lib286/dos/commode.obj
    <CPCMD> startup/library/msdos.086/ms/cstrtiny.obj       <OWRELROOT>/lib286/dos/cstart_t.obj

    <CPCMD> library/msdos.386/ms_r/clib3r.lib               <OWRELROOT>/lib386/dos/clib3r.lib
    <CPCMD> library/msdos.386/ms_s/clib3s.lib               <OWRELROOT>/lib386/dos/clib3s.lib
    <CPCMD> doslfn/library/msdos.386/ms_r/doslfn3r.lib      <OWRELROOT>/lib386/dos/doslfn3r.lib
    <CPCMD> doslfn/library/msdos.386/ms_s/doslfn3s.lib      <OWRELROOT>/lib386/dos/doslfn3s.lib
    <CPCMD> startup/library/msdos.386/ms_r/binmode.obj      <OWRELROOT>/lib386/dos/binmode.obj
    <CPCMD> startup/library/msdos.386/ms_r/commode.obj      <OWRELROOT>/lib386/dos/commode.obj
    <CPCMD> startup/library/msdos.386/ms_r/cstrtx32.obj     <OWRELROOT>/lib386/dos/cstrtx3r.obj
    <CPCMD> startup/library/msdos.386/ms_r/dllstart.obj     <OWRELROOT>/lib386/dos/dllstrtr.obj
    <CPCMD> startup/library/msdos.386/ms_s/cstrtx32.obj     <OWRELROOT>/lib386/dos/cstrtx3s.obj
    <CPCMD> startup/library/msdos.386/ms_s/adsstart.obj     <OWRELROOT>/lib386/dos/adsstart.obj
    <CPCMD> startup/library/msdos.386/ms_s/adiestrt.obj     <OWRELROOT>/lib386/dos/adiestrt.obj
    <CPCMD> startup/library/msdos.386/ms_s/adifstrt.obj     <OWRELROOT>/lib386/dos/adifstrt.obj
    <CPCMD> startup/library/msdos.386/ms_s/dllstart.obj     <OWRELROOT>/lib386/dos/dllstrts.obj

  [ IFDEF (os_dos os_os2 "") <2*> ]
    <CPCMD> library/msdos.286/mc/clibc.lib                  <OWRELROOT>/lib286/os2/dospmc.lib
    <CPCMD> library/msdos.286/mh/clibh.lib                  <OWRELROOT>/lib286/os2/dospmh.lib
    <CPCMD> library/msdos.286/ml/clibl.lib                  <OWRELROOT>/lib286/os2/dospml.lib
    <CPCMD> library/msdos.286/mm/clibm.lib                  <OWRELROOT>/lib286/os2/dospmm.lib
    <CPCMD> library/msdos.286/ms/clibs.lib                  <OWRELROOT>/lib286/os2/dospms.lib

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> library/os2.286/mc/clibc.lib                    <OWRELROOT>/lib286/os2/clibc.lib
    <CPCMD> library/os2.286/mh/clibh.lib                    <OWRELROOT>/lib286/os2/clibh.lib
    <CPCMD> library/os2.286/ml/clibl.lib                    <OWRELROOT>/lib286/os2/clibl.lib
    <CPCMD> library/os2.286/ml_dll/clibl.lib                <OWRELROOT>/lib286/os2/clibdll.lib
    <CPCMD> library/os2.286/ml_mt/clibl.lib                 <OWRELROOT>/lib286/os2/clibmtl.lib
    <CPCMD> library/os2.286/mm/clibm.lib                    <OWRELROOT>/lib286/os2/clibm.lib
    <CPCMD> library/os2.286/ms/clibs.lib                    <OWRELROOT>/lib286/os2/clibs.lib
    <CPCMD> startup/library/os2.286/ms/binmode.obj          <OWRELROOT>/lib286/os2/binmode.obj
    <CPCMD> startup/library/os2.286/ms/commode.obj          <OWRELROOT>/lib286/os2/commode.obj

    <CPCMD> library/os2.386/mf_r/clib3r.lib                 <OWRELROOT>/lib386/os2/clib3r.lib
    <CPCMD> library/os2.386/mf_s/clib3s.lib                 <OWRELROOT>/lib386/os2/clib3s.lib
    <CPCMD> library/os2.386/mf_rd/clib3r.lib                <OWRELROOT>/lib386/os2/clib3rd.lib
    <CPCMD> startup/library/os2.386/mf_r/binmode.obj        <OWRELROOT>/lib386/os2/binmode.obj
    <CPCMD> startup/library/os2.386/mf_r/commode.obj        <OWRELROOT>/lib386/os2/commode.obj

    # run-time DLL version
    <CPCMD> rtdll/os2.386/mf_r/clb*.lib                     <OWRELROOT>/lib386/os2/
    <CPCMD> rtdll/os2.386/mf_r/clb*.dll                     <OWRELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_r/clb*.sym                     <OWRELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_rd/clb*.lib                    <OWRELROOT>/lib386/os2/
    <CPCMD> rtdll/os2.386/mf_rd/clb*.dll                    <OWRELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_s/clb*.lib                     <OWRELROOT>/lib386/os2/
    <CPCMD> rtdll/os2.386/mf_s/clb*.dll                     <OWRELROOT>/binp/dll/
    <CPCMD> rtdll/os2.386/mf_s/clb*.sym                     <OWRELROOT>/binp/dll/

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> library/windows.086/mc/clibc.lib                <OWRELROOT>/lib286/win/clibc.lib
    <CPCMD> library/windows.086/ml/clibl.lib                <OWRELROOT>/lib286/win/clibl.lib
    <CPCMD> library/windows.086/mm/clibm.lib                <OWRELROOT>/lib286/win/clibm.lib
    <CPCMD> library/windows.086/ms/clibs.lib                <OWRELROOT>/lib286/win/clibs.lib
    <CPCMD> startup/library/windows.086/ml/libentry.obj     <OWRELROOT>/lib286/win/libentry.obj
    <CPCMD> startup/library/windows.086/ms/binmode.obj      <OWRELROOT>/lib286/win/binmode.obj
    <CPCMD> startup/library/windows.086/ms/commode.obj      <OWRELROOT>/lib286/win/commode.obj

    <CPCMD> library/windows.386/mf_r/clib3r.lib             <OWRELROOT>/lib386/win/clib3r.lib
    <CPCMD> library/windows.386/mf_s/clib3s.lib             <OWRELROOT>/lib386/win/clib3s.lib
    <CPCMD> startup/library/windows.386/mf_r/binmode.obj    <OWRELROOT>/lib386/win/binmode.obj
    <CPCMD> startup/library/windows.386/mf_r/commode.obj    <OWRELROOT>/lib386/win/commode.obj

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> library/winnt.386/mf_r/clib3r.lib               <OWRELROOT>/lib386/nt/clib3r.lib
    <CPCMD> library/winnt.386/mf_s/clib3s.lib               <OWRELROOT>/lib386/nt/clib3s.lib
    <CPCMD> library/winnt.386/mf_rd/clib3r.lib              <OWRELROOT>/lib386/nt/clib3rd.lib

    <CPCMD> startup/library/winnt.386/mf_r/binmode.obj      <OWRELROOT>/lib386/nt/binmode.obj
    <CPCMD> startup/library/winnt.386/mf_r/commode.obj      <OWRELROOT>/lib386/nt/commode.obj

    # run-time DLL version
    <CPCMD> rtdll/winnt.386/mf_r/clb*.lib                   <OWRELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_r/clb*.dll                   <OWRELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_r/clb*.sym                   <OWRELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_rd/clb*.lib                  <OWRELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_rd/clb*.dll                  <OWRELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_rp/clb*.lib                  <OWRELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_rp/clb*.dll                  <OWRELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_s/clb*.lib                   <OWRELROOT>/lib386/nt/
    <CPCMD> rtdll/winnt.386/mf_s/clb*.dll                   <OWRELROOT>/binnt/
    <CPCMD> rtdll/winnt.386/mf_s/clb*.sym                   <OWRELROOT>/binnt/

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> library/linux.386/mf_r/clib3r.lib               <OWRELROOT>/lib386/linux/clib3r.lib
    <CPCMD> library/linux.386/mf_s/clib3s.lib               <OWRELROOT>/lib386/linux/clib3s.lib
    <CPCMD> library/linux.386/mf_rd/clib3r.lib              <OWRELROOT>/lib386/linux/clib3rd.lib

    <CPCMD> startup/library/linux.386/mf_r/commode.obj      <OWRELROOT>/lib386/linux/commode.obj

  [ IFDEF (os_nov "") <2*> ]
    <CPCMD> library/nw_libc.386/ms_s/clib3s.lib             <OWRELROOT>/lib386/netware/libc3s.lib
    <CPCMD> library/nw_libcl.386/ms_s/clib3s.lib            <OWRELROOT>/lib386/netware/libc3sl.lib
    <CPCMD> library/nw_clib.386/ms_s/clib3s.lib             <OWRELROOT>/lib386/netware/clib3s.lib
    <CPCMD> library/nw_clibl.386/ms_s/clib3s.lib            <OWRELROOT>/lib386/netware/clib3sl.lib
    <CPCMD> library/nw_libc.386/ms_sd/clib3s.lib            <OWRELROOT>/lib386/netware/libc3sd.lib
    <CPCMD> library/nw_libcl.386/ms_sd/clib3s.lib           <OWRELROOT>/lib386/netware/libc3sld.lib
    <CPCMD> library/nw_clib.386/ms_sd/clib3s.lib            <OWRELROOT>/lib386/netware/clib3sd.lib
    <CPCMD> library/nw_clibl.386/ms_sd/clib3s.lib           <OWRELROOT>/lib386/netware/clib3sld.lib
#
# Note binmode applies to both LIBC and CLIB libraries but only needs to be built once from the
# fat CLIB source code.
#
    <CPCMD> startup/library/nw_clib.386/ms_s/binmode.obj    <OWRELROOT>/lib386/netware/binmode.obj
    <CPCMD> startup/library/nw_clib.386/ms_sd/binmode.obj   <OWRELROOT>/lib386/netware/binmoded.obj

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> library/qnx.286/mc/clibc.lib                    <OWRELROOT>/lib286/qnx/clibc.lib
    <CPCMD> library/qnx.286/mh/clibh.lib                    <OWRELROOT>/lib286/qnx/clibh.lib
    <CPCMD> library/qnx.286/ml/clibl.lib                    <OWRELROOT>/lib286/qnx/clibl.lib
    <CPCMD> library/qnx.286/ml_sh/clibl.lib                 <OWRELROOT>/lib286/qnx/clibshar.lib
    <CPCMD> library/qnx.286/mm/clibm.lib                    <OWRELROOT>/lib286/qnx/clibm.lib
    <CPCMD> library/qnx.286/ms/clibs.lib                    <OWRELROOT>/lib286/qnx/clibs.lib
    <CPCMD> startup/library/qnx.286/ms/commode.obj          <OWRELROOT>/lib286/qnx/commode.obj

    <CPCMD> library/qnx.386/ms_r/clib3r.lib                 <OWRELROOT>/lib386/qnx/clib3r.lib
    <CPCMD> library/qnx.386/ms_s/clib3s.lib                 <OWRELROOT>/lib386/qnx/clib3s.lib
    <CPCMD> startup/library/qnx.386/ms_r/commode.obj        <OWRELROOT>/lib386/qnx/commode.obj

  [ IFDEF (os_rdos "") <2*> ]
    <CPCMD> library/rdos.386/mf_r/clib3r.lib                <OWRELROOT>/lib386/rdos/clib3r.lib
    <CPCMD> library/rdos.386/mf_s/clib3s.lib                <OWRELROOT>/lib386/rdos/clib3s.lib
    <CPCMD> library/rdosdev.386/mc_r/clib3r.lib             <OWRELROOT>/lib386/rdosdev/clib3r.lib

    <CPCMD> startup/library/rdos.386/mf_r/binmode.obj       <OWRELROOT>/lib386/rdos/binmode.obj
    <CPCMD> startup/library/rdos.386/mf_r/commode.obj       <OWRELROOT>/lib386/rdos/commode.obj
    <CPCMD> startup/library/rdos.386/mf_r/resstub.obj       <OWRELROOT>/lib386/rdos/resstub.obj
  [ ENDIF]


# Libraries not built by default

    <CCCMD> library/winnt.axp/_s/clib.lib                   <OWRELROOT>/libaxp/nt/clib.lib
    <CCCMD> startup/library/winnt.axp/_s/binmode.obj        <OWRELROOT>/libaxp/nt/binmode.obj
    <CCCMD> startup/library/winnt.axp/_s/commode.obj        <OWRELROOT>/libaxp/nt/commode.obj

    <CCCMD> library/osi.386/ms_r/clib3r.lib                 <OWRELROOT>/lib386/osi/clib3r.lib

    <CCCMD> library/linux.mps/_s/clib.lib                   <OWRELROOT>/libmps/linux/clib.lib

[ BLOCK <1> clibqa ]
#===================
    cdsay qa
    pmake -d clibqa -h

[ BLOCK <1> tests ]
#==================
    cdsay qa
    pmake -d clibqa -h tests

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#==================

cdsay <PROJDIR>
