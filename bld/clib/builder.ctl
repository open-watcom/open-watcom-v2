# CLIB Builder Control file
# =========================

set PROJDIR=<CWD>
set PROJNAME=clib

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> library/msdos.086/mc/clibc.lib                  <OWRELROOT>/lib286/dos/clibc.lib
    <CCCMD> library/msdos.086/mh/clibh.lib                  <OWRELROOT>/lib286/dos/clibh.lib
    <CCCMD> library/msdos.086/ml/clibl.lib                  <OWRELROOT>/lib286/dos/clibl.lib
    <CCCMD> library/msdos.086/ml_ov/clibl.lib               <OWRELROOT>/lib286/dos/clibol.lib
    <CCCMD> library/msdos.086/mm/clibm.lib                  <OWRELROOT>/lib286/dos/clibm.lib
    <CCCMD> library/msdos.086/mm_ov/clibm.lib               <OWRELROOT>/lib286/dos/clibom.lib
    <CCCMD> library/msdos.086/ms/clibs.lib                  <OWRELROOT>/lib286/dos/clibs.lib
    <CCCMD> doslfn/library/msdos.086/mc/doslfnc.lib         <OWRELROOT>/lib286/dos/doslfnc.lib
    <CCCMD> doslfn/library/msdos.086/mh/doslfnh.lib         <OWRELROOT>/lib286/dos/doslfnh.lib
    <CCCMD> doslfn/library/msdos.086/ml/doslfnl.lib         <OWRELROOT>/lib286/dos/doslfnl.lib
    <CCCMD> doslfn/library/msdos.086/ml_ov/doslfnl.lib      <OWRELROOT>/lib286/dos/doslfnol.lib
    <CCCMD> doslfn/library/msdos.086/mm/doslfnm.lib         <OWRELROOT>/lib286/dos/doslfnm.lib
    <CCCMD> doslfn/library/msdos.086/mm_ov/doslfnm.lib      <OWRELROOT>/lib286/dos/doslfnom.lib
    <CCCMD> doslfn/library/msdos.086/ms/doslfns.lib         <OWRELROOT>/lib286/dos/doslfns.lib
    <CCCMD> handleio/library/msdos.086/ms/binmode.obj       <OWRELROOT>/lib286/dos/binmode.obj
    <CCCMD> startup/library/msdos.086/ml/d16msels.obj       <OWRELROOT>/lib286/dos/d16msels.obj
    <CCCMD> startup/library/msdos.086/ml/dos16m.obj         <OWRELROOT>/lib286/dos/dos16m.obj
    <CCCMD> startup/library/msdos.086/ms/commode.obj        <OWRELROOT>/lib286/dos/commode.obj
    <CCCMD> startup/library/msdos.086/ms/cstrtiny.obj       <OWRELROOT>/lib286/dos/cstart_t.obj

    <CCCMD> library/msdos.386/ms_r/clib3r.lib               <OWRELROOT>/lib386/dos/clib3r.lib
    <CCCMD> library/msdos.386/ms_s/clib3s.lib               <OWRELROOT>/lib386/dos/clib3s.lib
    <CCCMD> doslfn/library/msdos.386/ms_r/doslfn3r.lib      <OWRELROOT>/lib386/dos/doslfn3r.lib
    <CCCMD> doslfn/library/msdos.386/ms_s/doslfn3s.lib      <OWRELROOT>/lib386/dos/doslfn3s.lib
    <CCCMD> handleio/library/msdos.386/ms_r/binmode.obj     <OWRELROOT>/lib386/dos/binmode.obj
    <CCCMD> startup/library/msdos.386/ms_r/commode.obj      <OWRELROOT>/lib386/dos/commode.obj
    <CCCMD> startup/library/msdos.386/ms_r/cstrtx32.obj     <OWRELROOT>/lib386/dos/cstrtx3r.obj
    <CCCMD> startup/library/msdos.386/ms_r/dllstart.obj     <OWRELROOT>/lib386/dos/dllstrtr.obj
    <CCCMD> startup/library/msdos.386/ms_s/cstrtx32.obj     <OWRELROOT>/lib386/dos/cstrtx3s.obj
    <CCCMD> startup/library/msdos.386/ms_s/adsstart.obj     <OWRELROOT>/lib386/dos/adsstart.obj
    <CCCMD> startup/library/msdos.386/ms_s/adiestrt.obj     <OWRELROOT>/lib386/dos/adiestrt.obj
    <CCCMD> startup/library/msdos.386/ms_s/adifstrt.obj     <OWRELROOT>/lib386/dos/adifstrt.obj
    <CCCMD> startup/library/msdos.386/ms_s/dllstart.obj     <OWRELROOT>/lib386/dos/dllstrts.obj

    <CCCMD> library/msdos.286/mc/clibc.lib                  <OWRELROOT>/lib286/os2/dospmc.lib
    <CCCMD> library/msdos.286/mh/clibh.lib                  <OWRELROOT>/lib286/os2/dospmh.lib
    <CCCMD> library/msdos.286/ml/clibl.lib                  <OWRELROOT>/lib286/os2/dospml.lib
    <CCCMD> library/msdos.286/mm/clibm.lib                  <OWRELROOT>/lib286/os2/dospmm.lib
    <CCCMD> library/msdos.286/ms/clibs.lib                  <OWRELROOT>/lib286/os2/dospms.lib

    <CCCMD> library/os2.286/mc/clibc.lib                    <OWRELROOT>/lib286/os2/clibc.lib
    <CCCMD> library/os2.286/mh/clibh.lib                    <OWRELROOT>/lib286/os2/clibh.lib
    <CCCMD> library/os2.286/ml/clibl.lib                    <OWRELROOT>/lib286/os2/clibl.lib
    <CCCMD> library/os2.286/ml_dll/clibdll.lib              <OWRELROOT>/lib286/os2/clibdll.lib
    <CCCMD> library/os2.286/ml_mt/clibmtl.lib               <OWRELROOT>/lib286/os2/clibmtl.lib
    <CCCMD> library/os2.286/mm/clibm.lib                    <OWRELROOT>/lib286/os2/clibm.lib
    <CCCMD> library/os2.286/ms/clibs.lib                    <OWRELROOT>/lib286/os2/clibs.lib
    <CCCMD> handleio/library/os2.286/ms/binmode.obj         <OWRELROOT>/lib286/os2/binmode.obj
    <CCCMD> startup/library/os2.286/ms/commode.obj          <OWRELROOT>/lib286/os2/commode.obj

    <CCCMD> library/os2.386/mf_r/clib3r.lib                 <OWRELROOT>/lib386/os2/clib3r.lib
    <CCCMD> library/os2.386/mf_s/clib3s.lib                 <OWRELROOT>/lib386/os2/clib3s.lib
    <CCCMD> library/os2.386/mf_rd/clib3r.lib                <OWRELROOT>/lib386/os2/clib3rd.lib
    <CCCMD> handleio/library/os2.386/mf_r/binmode.obj       <OWRELROOT>/lib386/os2/binmode.obj
    <CCCMD> startup/library/os2.386/mf_r/commode.obj        <OWRELROOT>/lib386/os2/commode.obj

    # run-time DLL version
    <CCCMD> rtdll/os2.386/mf_r/clb*.lib                     <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.386/mf_r/clb*.dll                     <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_r/clb*.sym                     <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_rd/clb*.lib                    <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.386/mf_rd/clb*.dll                    <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_s/clb*.lib                     <OWRELROOT>/lib386/os2/
    <CCCMD> rtdll/os2.386/mf_s/clb*.dll                     <OWRELROOT>/binp/dll/
    <CCCMD> rtdll/os2.386/mf_s/clb*.sym                     <OWRELROOT>/binp/dll/

    <CCCMD> library/windows.086/mc/clibc.lib                <OWRELROOT>/lib286/win/clibc.lib
    <CCCMD> library/windows.086/ml/clibl.lib                <OWRELROOT>/lib286/win/clibl.lib
    <CCCMD> library/windows.086/mm/clibm.lib                <OWRELROOT>/lib286/win/clibm.lib
    <CCCMD> library/windows.086/ms/clibs.lib                <OWRELROOT>/lib286/win/clibs.lib
    <CCCMD> handleio/library/windows.086/ms/binmode.obj     <OWRELROOT>/lib286/win/binmode.obj
    <CCCMD> startup/library/windows.086/ml/libentry.obj     <OWRELROOT>/lib286/win/libentry.obj
    <CCCMD> startup/library/windows.086/ms/commode.obj      <OWRELROOT>/lib286/win/commode.obj

    <CCCMD> library/windows.386/mf_r/clib3r.lib             <OWRELROOT>/lib386/win/clib3r.lib
    <CCCMD> library/windows.386/mf_s/clib3s.lib             <OWRELROOT>/lib386/win/clib3s.lib
    <CCCMD> handleio/library/windows.386/mf_r/binmode.obj   <OWRELROOT>/lib386/win/binmode.obj
    <CCCMD> startup/library/windows.386/mf_r/commode.obj    <OWRELROOT>/lib386/win/commode.obj

    <CCCMD> library/winnt.386/mf_r/clib3r.lib               <OWRELROOT>/lib386/nt/clib3r.lib
    <CCCMD> library/winnt.386/mf_s/clib3s.lib               <OWRELROOT>/lib386/nt/clib3s.lib
    <CCCMD> library/winnt.386/mf_rd/clib3r.lib              <OWRELROOT>/lib386/nt/clib3rd.lib

    <CCCMD> handleio/library/winnt.386/mf_r/binmode.obj     <OWRELROOT>/lib386/nt/binmode.obj
    <CCCMD> startup/library/winnt.386/mf_r/commode.obj      <OWRELROOT>/lib386/nt/commode.obj

    # run-time DLL version
    <CCCMD> rtdll/winnt.386/mf_r/clb*.lib                   <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_r/clb*.dll                   <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_r/clb*.sym                   <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_rd/clb*.lib                  <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_rd/clb*.dll                  <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_rp/clb*.lib                  <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_rp/clb*.dll                  <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_s/clb*.lib                   <OWRELROOT>/lib386/nt/
    <CCCMD> rtdll/winnt.386/mf_s/clb*.dll                   <OWRELROOT>/binnt/
    <CCCMD> rtdll/winnt.386/mf_s/clb*.sym                   <OWRELROOT>/binnt/

    <CCCMD> library/win32s.386/mf_r/clib3r.lib              <OWRELROOT>/lib386/win32s/clib3r.lib
    <CCCMD> library/win32s.386/mf_s/clib3s.lib              <OWRELROOT>/lib386/win32s/clib3s.lib
    <CCCMD> library/win32s.386/mf_rd/clib3r.lib             <OWRELROOT>/lib386/win32s/clib3rd.lib

    <CCCMD> handleio/library/win32s.386/mf_r/binmode.obj    <OWRELROOT>/lib386/win32s/binmode.obj
    <CCCMD> startup/library/win32s.386/mf_r/commode.obj     <OWRELROOT>/lib386/win32s/commode.obj

    # run-time DLL version
    <CCCMD> rtdll/win32s.386/mf_r/clb*.lib                  <OWRELROOT>/lib386/win32s/
    <CCCMD> rtdll/win32s.386/mf_r/clb*.dll                  <OWRELROOT>/binw32s/
    <CCCMD> rtdll/win32s.386/mf_r/clb*.sym                  <OWRELROOT>/binw32s/
    <CCCMD> rtdll/win32s.386/mf_rd/clb*.lib                 <OWRELROOT>/lib386/win32s/
    <CCCMD> rtdll/win32s.386/mf_rd/clb*.dll                 <OWRELROOT>/binw32s/
    <CCCMD> rtdll/win32s.386/mf_rp/clb*.lib                 <OWRELROOT>/lib386/win32s/
    <CCCMD> rtdll/win32s.386/mf_rp/clb*.dll                 <OWRELROOT>/binw32s/
    <CCCMD> rtdll/win32s.386/mf_s/clb*.lib                  <OWRELROOT>/lib386/win32s/
    <CCCMD> rtdll/win32s.386/mf_s/clb*.dll                  <OWRELROOT>/binw32s/
    <CCCMD> rtdll/win32s.386/mf_s/clb*.sym                  <OWRELROOT>/binw32s/

    <CCCMD> library/linux.386/mf_r/clib3r.lib               <OWRELROOT>/lib386/linux/clib3r.lib
    <CCCMD> library/linux.386/mf_s/clib3s.lib               <OWRELROOT>/lib386/linux/clib3s.lib
    <CCCMD> library/linux.386/mf_rd/clib3r.lib              <OWRELROOT>/lib386/linux/clib3rd.lib

    <CCCMD> startup/library/linux.386/mf_r/commode.obj      <OWRELROOT>/lib386/linux/commode.obj

    <CCCMD> library/nw_libc.386/ms_s/clib3s.lib             <OWRELROOT>/lib386/netware/libc3s.lib
    <CCCMD> library/nw_libcl.386/ms_s/clib3s.lib            <OWRELROOT>/lib386/netware/libc3sl.lib
    <CCCMD> library/nw_clib.386/ms_s/clib3s.lib             <OWRELROOT>/lib386/netware/clib3s.lib
    <CCCMD> library/nw_clibl.386/ms_s/clib3s.lib            <OWRELROOT>/lib386/netware/clib3sl.lib
    <CCCMD> library/nw_libc.386/ms_sd/clib3s.lib            <OWRELROOT>/lib386/netware/libc3sd.lib
    <CCCMD> library/nw_libcl.386/ms_sd/clib3s.lib           <OWRELROOT>/lib386/netware/libc3sld.lib
    <CCCMD> library/nw_clib.386/ms_sd/clib3s.lib            <OWRELROOT>/lib386/netware/clib3sd.lib
    <CCCMD> library/nw_clibl.386/ms_sd/clib3s.lib           <OWRELROOT>/lib386/netware/clib3sld.lib
#
# Note binmode applies to both LIBC and CLIB libraries but only needs to be built once from the
# fat CLIB source code.
#
    <CCCMD> handleio/library/nw_clib.386/ms_s/binmode.obj   <OWRELROOT>/lib386/netware/binmode.obj
    <CCCMD> handleio/library/nw_clib.386/ms_sd/binmode.obj  <OWRELROOT>/lib386/netware/binmoded.obj

    <CCCMD> library/qnx.286/mc/clibc.lib                    <OWRELROOT>/lib286/qnx/clibc.lib
    <CCCMD> library/qnx.286/mh/clibh.lib                    <OWRELROOT>/lib286/qnx/clibh.lib
    <CCCMD> library/qnx.286/ml/clibl.lib                    <OWRELROOT>/lib286/qnx/clibl.lib
    <CCCMD> library/qnx.286/ml_sh/clibl.lib                 <OWRELROOT>/lib286/qnx/clibshar.lib
    <CCCMD> library/qnx.286/mm/clibm.lib                    <OWRELROOT>/lib286/qnx/clibm.lib
    <CCCMD> library/qnx.286/ms/clibs.lib                    <OWRELROOT>/lib286/qnx/clibs.lib
    <CCCMD> startup/library/qnx.286/ms/commode.obj          <OWRELROOT>/lib286/qnx/commode.obj

    <CCCMD> library/qnx.386/ms_r/clib3r.lib                 <OWRELROOT>/lib386/qnx/clib3r.lib
    <CCCMD> library/qnx.386/ms_s/clib3s.lib                 <OWRELROOT>/lib386/qnx/clib3s.lib
    <CCCMD> startup/library/qnx.386/ms_r/commode.obj        <OWRELROOT>/lib386/qnx/commode.obj

    <CCCMD> library/rdos.386/mf_r/clib3r.lib                <OWRELROOT>/lib386/rdos/clib3r.lib
    <CCCMD> library/rdos.386/mf_s/clib3s.lib                <OWRELROOT>/lib386/rdos/clib3s.lib
    <CCCMD> library/rdosdev.386/mc_r/clib3r.lib             <OWRELROOT>/lib386/rdosdev/clib3r.lib

    <CCCMD> handleio/library/rdos.386/mf_r/binmode.obj      <OWRELROOT>/lib386/rdos/binmode.obj
    <CCCMD> startup/library/rdos.386/mf_r/commode.obj       <OWRELROOT>/lib386/rdos/commode.obj
    <CCCMD> startup/library/rdos.386/mf_r/resstub.obj       <OWRELROOT>/lib386/rdos/resstub.obj

# Libraries not built by default

    <CCCMD> library/winnt.axp/_s/clib.lib                   <OWRELROOT>/libaxp/nt/clib.lib
    <CCCMD> handleio/library/winnt.axp/_s/binmode.obj       <OWRELROOT>/libaxp/nt/binmode.obj
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

[ BLOCK . . ]
#==================
cdsay <PROJDIR>
