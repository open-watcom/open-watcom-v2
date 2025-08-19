# CLIB Builder Control file
# =========================

set PROJNAME=clib

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay "<PROJDIR>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> library/msdos.086/mc/clibc.lib                  "<OWRELROOT>/lib286/dos/"
    <CCCMD> library/msdos.086/mh/clibh.lib                  "<OWRELROOT>/lib286/dos/"
    <CCCMD> library/msdos.086/ml/clibl.lib                  "<OWRELROOT>/lib286/dos/"
    <CCCMD> library/msdos.086/ml_ov/clibl.lib               "<OWRELROOT>/lib286/dos/clibol.lib"
    <CCCMD> library/msdos.086/mm/clibm.lib                  "<OWRELROOT>/lib286/dos/"
    <CCCMD> library/msdos.086/mm_ov/clibm.lib               "<OWRELROOT>/lib286/dos/clibom.lib"
    <CCCMD> library/msdos.086/ms/clibs.lib                  "<OWRELROOT>/lib286/dos/"
    <CCCMD> doslfn/library/msdos.086/mc/doslfnc.lib         "<OWRELROOT>/lib286/dos/"
    <CCCMD> doslfn/library/msdos.086/mh/doslfnh.lib         "<OWRELROOT>/lib286/dos/"
    <CCCMD> doslfn/library/msdos.086/ml/doslfnl.lib         "<OWRELROOT>/lib286/dos/"
    <CCCMD> doslfn/library/msdos.086/ml_ov/doslfnl.lib      "<OWRELROOT>/lib286/dos/doslfnol.lib"
    <CCCMD> doslfn/library/msdos.086/mm/doslfnm.lib         "<OWRELROOT>/lib286/dos/"
    <CCCMD> doslfn/library/msdos.086/mm_ov/doslfnm.lib      "<OWRELROOT>/lib286/dos/doslfnom.lib"
    <CCCMD> doslfn/library/msdos.086/ms/doslfns.lib         "<OWRELROOT>/lib286/dos/"
    <CCCMD> pc98/library/msdos.086/mc/pc98c.lib             "<OWRELROOT>/lib286/dos/"
    <CCCMD> pc98/library/msdos.086/mh/pc98h.lib             "<OWRELROOT>/lib286/dos/"
    <CCCMD> pc98/library/msdos.086/ml/pc98l.lib             "<OWRELROOT>/lib286/dos/"
    <CCCMD> pc98/library/msdos.086/ml_ov/pc98l.lib          "<OWRELROOT>/lib286/dos/pc98ol.lib"
    <CCCMD> pc98/library/msdos.086/mm/pc98m.lib             "<OWRELROOT>/lib286/dos/"
    <CCCMD> pc98/library/msdos.086/mm_ov/pc98m.lib          "<OWRELROOT>/lib286/dos/pc98om.lib"
    <CCCMD> pc98/library/msdos.086/ms/pc98s.lib             "<OWRELROOT>/lib286/dos/"
    <CCCMD> handleio/library/msdos.086/ms/binmode.obj       "<OWRELROOT>/lib286/dos/"
    <CCCMD> startup/library/msdos.086/ml/d16msels.obj       "<OWRELROOT>/lib286/dos/"
    <CCCMD> startup/library/msdos.086/ml/dos16m.obj         "<OWRELROOT>/lib286/dos/"
    <CCCMD> startup/library/msdos.086/ms/commode.obj        "<OWRELROOT>/lib286/dos/"
    <CCCMD> startup/library/msdos.086/ms/cstrtiny.obj       "<OWRELROOT>/lib286/dos/cstart_t.obj"

    <CCCMD> library/msdos.386/ms_r/clib3r.lib               "<OWRELROOT>/lib386/dos/"
    <CCCMD> library/msdos.386/ms_s/clib3s.lib               "<OWRELROOT>/lib386/dos/"
    <CCCMD> doslfn/library/msdos.386/ms_r/doslfn3r.lib      "<OWRELROOT>/lib386/dos/"
    <CCCMD> doslfn/library/msdos.386/ms_s/doslfn3s.lib      "<OWRELROOT>/lib386/dos/"
    <CCCMD> pc98/library/msdos.386/ms_r/pc983r.lib          "<OWRELROOT>/lib386/dos/"
    <CCCMD> pc98/library/msdos.386/ms_s/pc983s.lib          "<OWRELROOT>/lib386/dos/"
    <CCCMD> handleio/library/msdos.386/ms_r/binmode.obj     "<OWRELROOT>/lib386/dos/"
    <CCCMD> startup/library/msdos.386/ms_r/commode.obj      "<OWRELROOT>/lib386/dos/"
    <CCCMD> startup/library/msdos.386/ms_r/cstrtx32.obj     "<OWRELROOT>/lib386/dos/cstrtx3r.obj"
    <CCCMD> startup/library/msdos.386/ms_r/dllstart.obj     "<OWRELROOT>/lib386/dos/dllstrtr.obj"
    <CCCMD> startup/library/msdos.386/ms_s/cstrtx32.obj     "<OWRELROOT>/lib386/dos/cstrtx3s.obj"
    <CCCMD> startup/library/msdos.386/ms_s/adsstart.obj     "<OWRELROOT>/lib386/dos/"
    <CCCMD> startup/library/msdos.386/ms_s/adiestrt.obj     "<OWRELROOT>/lib386/dos/"
    <CCCMD> startup/library/msdos.386/ms_s/adifstrt.obj     "<OWRELROOT>/lib386/dos/"
    <CCCMD> startup/library/msdos.386/ms_s/dllstart.obj     "<OWRELROOT>/lib386/dos/dllstrts.obj"

    <CCCMD> library/msdos.286/mc/clibc.lib                  "<OWRELROOT>/lib286/os2/dospmc.lib"
    <CCCMD> library/msdos.286/mh/clibh.lib                  "<OWRELROOT>/lib286/os2/dospmh.lib"
    <CCCMD> library/msdos.286/ml/clibl.lib                  "<OWRELROOT>/lib286/os2/dospml.lib"
    <CCCMD> library/msdos.286/mm/clibm.lib                  "<OWRELROOT>/lib286/os2/dospmm.lib"
    <CCCMD> library/msdos.286/ms/clibs.lib                  "<OWRELROOT>/lib286/os2/dospms.lib"

    <CCCMD> library/os2.286/mc/clibc.lib                    "<OWRELROOT>/lib286/os2/"
    <CCCMD> library/os2.286/mh/clibh.lib                    "<OWRELROOT>/lib286/os2/"
    <CCCMD> library/os2.286/ml/clibl.lib                    "<OWRELROOT>/lib286/os2/"
    <CCCMD> library/os2.286/ml_dll/clibdll.lib              "<OWRELROOT>/lib286/os2/"
    <CCCMD> library/os2.286/ml_mt/clibmtl.lib               "<OWRELROOT>/lib286/os2/"
    <CCCMD> library/os2.286/mm/clibm.lib                    "<OWRELROOT>/lib286/os2/"
    <CCCMD> library/os2.286/ms/clibs.lib                    "<OWRELROOT>/lib286/os2/"
    <CCCMD> handleio/library/os2.286/ms/binmode.obj         "<OWRELROOT>/lib286/os2/"
    <CCCMD> startup/library/os2.286/ms/commode.obj          "<OWRELROOT>/lib286/os2/"

    <CCCMD> library/os2.386/mf_r/clib3r.lib                 "<OWRELROOT>/lib386/os2/"
    <CCCMD> library/os2.386/mf_s/clib3s.lib                 "<OWRELROOT>/lib386/os2/"
    <CCCMD> library/os2.386/mf_rd/clib3r.lib                "<OWRELROOT>/lib386/os2/clib3rd.lib"
    <CCCMD> handleio/library/os2.386/mf_r/binmode.obj       "<OWRELROOT>/lib386/os2/"
    <CCCMD> startup/library/os2.386/mf_r/commode.obj        "<OWRELROOT>/lib386/os2/"

    # run-time DLL version
    <CCCMD> rtdll/os2.386/mf_r/clb*.lib                     "<OWRELROOT>/lib386/os2/"
    <CCCMD> rtdll/os2.386/mf_r/clb*.dll                     "<OWRELROOT>/binp/dll/"
    <CCCMD> rtdll/os2.386/mf_r/clb*.sym                     "<OWRELROOT>/binp/dll/"
#    <CCCMD> rtdll/os2.386/mf_rd/clb*.lib                    "<OWRELROOT>/lib386/os2/"
#    <CCCMD> rtdll/os2.386/mf_rd/clb*.dll                    "<OWRELROOT>/binp/dll/"
    <CCCMD> rtdll/os2.386/mf_s/clb*.lib                     "<OWRELROOT>/lib386/os2/"
    <CCCMD> rtdll/os2.386/mf_s/clb*.dll                     "<OWRELROOT>/binp/dll/"
    <CCCMD> rtdll/os2.386/mf_s/clb*.sym                     "<OWRELROOT>/binp/dll/"

    <CCCMD> library/windows.086/mc/clibc.lib                "<OWRELROOT>/lib286/win/"
    <CCCMD> library/windows.086/ml/clibl.lib                "<OWRELROOT>/lib286/win/"
    <CCCMD> library/windows.086/mm/clibm.lib                "<OWRELROOT>/lib286/win/"
    <CCCMD> library/windows.086/ms/clibs.lib                "<OWRELROOT>/lib286/win/"
    <CCCMD> pc98/library/windows.086/mc/pc98c.lib           "<OWRELROOT>/lib286/win/"
    <CCCMD> pc98/library/windows.086/ml/pc98l.lib           "<OWRELROOT>/lib286/win/"
    <CCCMD> pc98/library/windows.086/mm/pc98m.lib           "<OWRELROOT>/lib286/win/"
    <CCCMD> pc98/library/windows.086/ms/pc98s.lib           "<OWRELROOT>/lib286/win/"
    <CCCMD> win10/library/windows.086/mc/win10c.lib         "<OWRELROOT>/lib286/win/"
    <CCCMD> win10/library/windows.086/ml/win10l.lib         "<OWRELROOT>/lib286/win/"
    <CCCMD> win10/library/windows.086/mm/win10m.lib         "<OWRELROOT>/lib286/win/"
    <CCCMD> win10/library/windows.086/ms/win10s.lib         "<OWRELROOT>/lib286/win/"
    <CCCMD> handleio/library/windows.086/ms/binmode.obj     "<OWRELROOT>/lib286/win/"
    <CCCMD> startup/library/windows.086/ml/libentry.obj     "<OWRELROOT>/lib286/win/"
    <CCCMD> startup/library/windows.086/ms/commode.obj      "<OWRELROOT>/lib286/win/"
    <CCCMD> win10/library/windows.086/ml/libent10.obj       "<OWRELROOT>/lib286/win/"
    <CCCMD> win10/library/windows.086/ml/win1stub.obj       "<OWRELROOT>/lib286/win/"

    <CCCMD> library/windows.386/mf_r/clib3r.lib             "<OWRELROOT>/lib386/win/"
    <CCCMD> library/windows.386/mf_s/clib3s.lib             "<OWRELROOT>/lib386/win/"
    <CCCMD> pc98/library/windows.386/mf_r/pc983r.lib        "<OWRELROOT>/lib386/win/"
    <CCCMD> pc98/library/windows.386/mf_s/pc983s.lib        "<OWRELROOT>/lib386/win/"
    <CCCMD> handleio/library/windows.386/mf_r/binmode.obj   "<OWRELROOT>/lib386/win/"
    <CCCMD> startup/library/windows.386/mf_r/commode.obj    "<OWRELROOT>/lib386/win/"

    <CCCMD> library/winnt.386/mf_r/clib3r.lib               "<OWRELROOT>/lib386/nt/"
    <CCCMD> library/winnt.386/mf_s/clib3s.lib               "<OWRELROOT>/lib386/nt/"
    <CCCMD> library/winnt.386/mf_rd/clib3r.lib              "<OWRELROOT>/lib386/nt/clib3rd.lib"

    <CCCMD> handleio/library/winnt.386/mf_r/binmode.obj     "<OWRELROOT>/lib386/nt/"
    <CCCMD> startup/library/winnt.386/mf_r/commode.obj      "<OWRELROOT>/lib386/nt/"

    # run-time DLL version
    <CCCMD> rtdll/winnt.386/mf_r/clb*.lib                   "<OWRELROOT>/lib386/nt/"
    <CCCMD> rtdll/winnt.386/mf_r/clb*.dll                   "<OWRELROOT>/binnt/"
    <CCCMD> rtdll/winnt.386/mf_r/clb*.sym                   "<OWRELROOT>/binnt/"
#    <CCCMD> rtdll/winnt.386/mf_rd/clb*.lib                  "<OWRELROOT>/lib386/nt/"
#    <CCCMD> rtdll/winnt.386/mf_rd/clb*.dll                  "<OWRELROOT>/binnt/"
    <CCCMD> rtdll/winnt.386/mf_rp/clb*.lib                  "<OWRELROOT>/lib386/nt/"
    <CCCMD> rtdll/winnt.386/mf_rp/clb*.dll                  "<OWRELROOT>/binnt/"
    <CCCMD> rtdll/winnt.386/mf_s/clb*.lib                   "<OWRELROOT>/lib386/nt/"
    <CCCMD> rtdll/winnt.386/mf_s/clb*.dll                   "<OWRELROOT>/binnt/"
    <CCCMD> rtdll/winnt.386/mf_s/clb*.sym                   "<OWRELROOT>/binnt/"

    <CCCMD> library/linux.386/mf_r/clib3r.lib               "<OWRELROOT>/lib386/linux/"
    <CCCMD> library/linux.386/mf_s/clib3s.lib               "<OWRELROOT>/lib386/linux/"
    <CCCMD> library/linux.386/mf_rd/clib3r.lib              "<OWRELROOT>/lib386/linux/clib3rd.lib"

    <CCCMD> startup/library/linux.386/mf_r/commode.obj      "<OWRELROOT>/lib386/linux/"

    <CCCMD> library/nw_libc.386/ms_s/clib3s.lib             "<OWRELROOT>/lib386/netware/libc3s.lib"
    <CCCMD> library/nw_libcl.386/ms_s/clib3s.lib            "<OWRELROOT>/lib386/netware/libc3sl.lib"
    <CCCMD> library/nw_clib.386/ms_s/clib3s.lib             "<OWRELROOT>/lib386/netware/"
    <CCCMD> library/nw_clibl.386/ms_s/clib3s.lib            "<OWRELROOT>/lib386/netware/clib3sl.lib"
    <CCCMD> library/nw_libc.386/ms_sd/clib3s.lib            "<OWRELROOT>/lib386/netware/libc3sd.lib"
    <CCCMD> library/nw_libcl.386/ms_sd/clib3s.lib           "<OWRELROOT>/lib386/netware/libc3sld.lib"
    <CCCMD> library/nw_clib.386/ms_sd/clib3s.lib            "<OWRELROOT>/lib386/netware/clib3sd.lib"
    <CCCMD> library/nw_clibl.386/ms_sd/clib3s.lib           "<OWRELROOT>/lib386/netware/clib3sld.lib"
#
# Note binmode applies to both LIBC and CLIB libraries but only needs to be built once from the
# fat CLIB source code.
#
    <CCCMD> handleio/library/nw_clib.386/ms_s/binmode.obj   "<OWRELROOT>/lib386/netware/"
    <CCCMD> handleio/library/nw_clib.386/ms_sd/binmode.obj  "<OWRELROOT>/lib386/netware/binmoded.obj"

    <CCCMD> library/qnx.286/mc/clibc.lib                    "<OWRELROOT>/lib286/qnx/"
    <CCCMD> library/qnx.286/mh/clibh.lib                    "<OWRELROOT>/lib286/qnx/"
    <CCCMD> library/qnx.286/ml/clibl.lib                    "<OWRELROOT>/lib286/qnx/"
    <CCCMD> library/qnx.286/ml_sh/clibl.lib                 "<OWRELROOT>/lib286/qnx/clibshar.lib"
    <CCCMD> library/qnx.286/mm/clibm.lib                    "<OWRELROOT>/lib286/qnx/"
    <CCCMD> library/qnx.286/ms/clibs.lib                    "<OWRELROOT>/lib286/qnx/"
    <CCCMD> startup/library/qnx.286/ms/commode.obj          "<OWRELROOT>/lib286/qnx/"

    <CCCMD> library/qnx.386/ms_r/clib3r.lib                 "<OWRELROOT>/lib386/qnx/"
    <CCCMD> library/qnx.386/ms_s/clib3s.lib                 "<OWRELROOT>/lib386/qnx/"
    <CCCMD> startup/library/qnx.386/ms_r/commode.obj        "<OWRELROOT>/lib386/qnx/"

    <CCCMD> library/rdos.386/mf_r/clib3r.lib                "<OWRELROOT>/lib386/rdos/"
    <CCCMD> library/rdos.386/mf_s/clib3s.lib                "<OWRELROOT>/lib386/rdos/"
    <CCCMD> library/rdosdev.386/mc_r/clib3r.lib             "<OWRELROOT>/lib386/rdosdev/"

    <CCCMD> handleio/library/rdos.386/mf_r/binmode.obj      "<OWRELROOT>/lib386/rdos/"
    <CCCMD> startup/library/rdos.386/mf_r/commode.obj       "<OWRELROOT>/lib386/rdos/"
    <CCCMD> startup/library/rdos.386/mf_r/resstub.obj       "<OWRELROOT>/lib386/rdos/"

# Libraries not built by default

    <CCCMD> library/winnt.axp/_s/clib.lib                   "<OWRELROOT>/libaxp/nt/"
    <CCCMD> handleio/library/winnt.axp/_s/binmode.obj       "<OWRELROOT>/libaxp/nt/"
    <CCCMD> startup/library/winnt.axp/_s/commode.obj        "<OWRELROOT>/libaxp/nt/"

    <CCCMD> library/linux.mps/_s/clib.lib                   "<OWRELROOT>/libmps/linux/"
    <CCCMD> library/winnt.mps/_s/clib.lib                   "<OWRELROOT>/libmps/nt/"
    <CCCMD> handleio/library/winnt.mps/_s/binmode.obj       "<OWRELROOT>/libmps/nt/"
    <CCCMD> startup/library/winnt.mps/_s/commode.obj        "<OWRELROOT>/libmps/nt/"

[ BLOCK <1> clibqa ]
#===================
    cdsay qa
    pmake -d clibqa -h

[ BLOCK <1> tests ]
#==================
    cdsay qa
    pmake -d clibqa -h tests

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
