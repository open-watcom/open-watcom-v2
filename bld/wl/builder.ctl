# wlink Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=wlink

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> boot ]
#=================
    <CPCMD> <OWOBJDIR>/wl.exe <OWBINDIR>/bwlink<CMDEXT>

[ BLOCK <1> bootclean ]
#======================
    echo rm -f <OWBINDIR>/bwlink<CMDEXT>
    rm -f <OWBINDIR>/bwlink<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> dos386/wl.exe                     <OWRELROOT>/binw/wlink.exe
    <CCCMD> dos386/wl.sym                     <OWRELROOT>/binw/wlink.sym
    <CCCMD> ms2wlink/dosi86/ms2wlink.exe      <OWRELROOT>/binw/ms2wlink.exe
    <CCCMD> fcenable/dosi86/fcenable.exe      <OWRELROOT>/binw/fcenable.exe
    <CCCMD> exe2bin/dosi86/exe2bin.exe        <OWRELROOT>/binw/exe2bin.exe
    <CCCMD> wlsystem.lnk                      <OWRELROOT>/binw/wlsystem.lnk
    <CCCMD> wlbin.lnk                         <OWRELROOT>/binw/wlink.lnk
    <CCCMD> ovlldr/dosi86/wovl.lib            <OWRELROOT>/lib286/dos/wovl.lib
#    <CCCMD> ovlldr/dosi86/wmovl.lib           <OWRELROOT>/lib286/dos/wmovl.lib

    <CCCMD> os2386/wl.exe                     <OWRELROOT>/binp/wlink.exe
    <CCCMD> os2386/wl.sym                     <OWRELROOT>/binp/wlink.sym
    <CCCMD> os2386/wld.dll                    <OWRELROOT>/binp/dll/wlinkd.dll
    <CCCMD> os2386/wld.sym                    <OWRELROOT>/binp/dll/wlinkd.sym
    <CCCMD> ms2wlink/os2i86/ms2wlink.exe      <OWRELROOT>/binp/ms2wlink.exe
    <CCCMD> fcenable/os2i86/fcenable.exe      <OWRELROOT>/binp/fcenable.exe
    <CCCMD> exe2bin/os2386/exe2bin.exe        <OWRELROOT>/binp/exe2bin.exe
    <CCCMD> wlbinp.lnk                        <OWRELROOT>/binp/wlink.lnk

    <CCCMD> nt386/wl.exe                      <OWRELROOT>/binnt/wlink.exe
    <CCCMD> nt386/wl.sym                      <OWRELROOT>/binnt/wlink.sym
    <CCCMD> nt386/wld.dll                     <OWRELROOT>/binnt/wlinkd.dll
    <CCCMD> nt386/wld.sym                     <OWRELROOT>/binnt/wlinkd.sym
    <CCCMD> ms2wlink/nt386/ms2wlink.exe       <OWRELROOT>/binnt/ms2wlink.exe
    <CCCMD> fcenable/nt386/fcenable.exe       <OWRELROOT>/binnt/fcenable.exe
    <CCCMD> exe2bin/nt386/exe2bin.exe         <OWRELROOT>/binnt/exe2bin.exe
    <CCCMD> wlbinnt.lnk                       <OWRELROOT>/binnt/wlink.lnk

    <CCCMD> linux386/wl.exe                   <OWRELROOT>/binl/wlink
    <CCCMD> linux386/wl.sym                   <OWRELROOT>/binl/wlink.sym
    <CCCMD> ms2wlink/linux386/ms2wlink.exe    <OWRELROOT>/binl/ms2wlink
    <CCCMD> fcenable/linux386/fcenable.exe    <OWRELROOT>/binl/fcenable
    <CCCMD> exe2bin/linux386/exe2bin.exe      <OWRELROOT>/binl/exe2bin
    <CCCMD> linux.lnk                         <OWRELROOT>/binl/wlink.lnk

    <CCCMD> ntaxp/wl.exe                      <OWRELROOT>/axpnt/wlink.exe
    <CCCMD> ntaxp/wl.sym                      <OWRELROOT>/axpnt/wlink.sym
    <CCCMD> ntaxp/wld.dll                     <OWRELROOT>/axpnt/wlinkd.dll
    <CCCMD> ntaxp/wld.sym                     <OWRELROOT>/axpnt/wlinkd.sym
    <CCCMD> exe2bin/ntaxp/exe2bin.exe         <OWRELROOT>/axpnt/exe2bin.exe
    <CCCMD> wlaxpnt.lnk                       <OWRELROOT>/axpnt/wlink.lnk

    <CCCMD> qnx386/wl.exe                     <OWRELROOT>/qnx/binq/wlink
    <CCCMD> qnx386/wl.sym                     <OWRELROOT>/qnx/sym/wlink.sym
    <CCCMD> qnx.lnk                           <OWRELROOT>/qnx/etc/wlink.lnk

[ BLOCK . . ]
#============
cdsay <PROJDIR>
