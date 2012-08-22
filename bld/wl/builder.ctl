# Wlink Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================

  [ IFDEF (os_dos "") <2*> ]
    <CPCMD> dos386/wl.exe                     <OWRELROOT>/binw/wlink.exe
    <CPCMD> dos386/wl.sym                     <OWRELROOT>/binw/wlink.sym
    <CPCMD> ms2wlink/dosi86/ms2wlink.exe      <OWRELROOT>/binw/ms2wlink.exe
    <CPCMD> fcenable/dosi86/fcenable.exe      <OWRELROOT>/binw/fcenable.exe
    <CPCMD> exe2bin/dosi86/exe2bin.exe        <OWRELROOT>/binw/exe2bin.exe
    <CPCMD> wlsystem.lnk                      <OWRELROOT>/binw/wlsystem.lnk
    <CPCMD> wlbin.lnk                         <OWRELROOT>/binw/wlink.lnk
    <CPCMD> ovlldr/dosi86/wovl.lib            <OWRELROOT>/lib286/dos/wovl.lib
#    <CPCMD> ovlldr/dosi86/wmovl.lib           <OWRELROOT>/lib286/dos/wmovl.lib

  [ IFDEF (os_zdos) <2*> ]
    <CPCMD> zdos386/wl.exe                    <OWRELROOT>/binz/wlink.exe
    <CPCMD> zdos386/wl.sym                    <OWRELROOT>/binz/sym/wlink.sym
    <CPCMD> wlbinz.lnk                        <OWRELROOT>/binz/wlink.lnk

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386.dll/wl.exe                 <OWRELROOT>/binp/wlink.exe
    <CPCMD> os2386.dll/wl.sym                 <OWRELROOT>/binp/wlink.sym
    <CPCMD> os2386.dll/wld.dll                <OWRELROOT>/binp/dll/wlinkd.dll
    <CPCMD> os2386.dll/wld.sym                <OWRELROOT>/binp/dll/wlinkd.sym
    <CPCMD> ms2wlink/os2i86/ms2wlink.exe      <OWRELROOT>/binp/ms2wlink.exe
    <CPCMD> fcenable/os2i86/fcenable.exe      <OWRELROOT>/binp/fcenable.exe
    <CPCMD> exe2bin/os2386/exe2bin.exe        <OWRELROOT>/binp/exe2bin.exe
    <CPCMD> wlsystem.lnk                      <OWRELROOT>/binw/wlsystem.lnk
    <CPCMD> wlbinp.lnk                        <OWRELROOT>/binp/wlink.lnk

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386.dll/wl.exe                  <OWRELROOT>/binnt/wlink.exe
    <CPCMD> nt386.dll/wl.sym                  <OWRELROOT>/binnt/wlink.sym
    <CPCMD> nt386.dll/wld.dll                 <OWRELROOT>/binnt/wlinkd.dll
    <CPCMD> nt386.dll/wld.sym                 <OWRELROOT>/binnt/wlinkd.sym
    <CPCMD> nt386.rtd/wl.exe                  <OWRELROOT>/binnt/rtdll/wlink.exe
    <CPCMD> nt386.rtd/wld.dll                 <OWRELROOT>/binnt/rtdll/wlinkd.dll
    <CPCMD> ms2wlink/nt386/ms2wlink.exe       <OWRELROOT>/binnt/ms2wlink.exe
    <CPCMD> fcenable/nt386/fcenable.exe       <OWRELROOT>/binnt/fcenable.exe
    <CPCMD> exe2bin/nt386/exe2bin.exe         <OWRELROOT>/binnt/exe2bin.exe
    <CPCMD> wlsystem.lnk                      <OWRELROOT>/binw/wlsystem.lnk
    <CPCMD> wlbinnt.lnk                       <OWRELROOT>/binnt/wlink.lnk

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/wl.exe                   <OWRELROOT>/binl/wlink
    <CPCMD> linux386/wl.sym                   <OWRELROOT>/binl/wlink.sym
    <CPCMD> ms2wlink/linux386/ms2wlink.exe    <OWRELROOT>/binl/ms2wlink
    <CPCMD> fcenable/linux386/fcenable.exe    <OWRELROOT>/binl/fcenable
    <CPCMD> exe2bin/linux386/exe2bin.exe      <OWRELROOT>/binl/exe2bin
    <CPCMD> wlsystem.lnk                      <OWRELROOT>/binl/wlsystem.lnk
    <CPCMD> wlbinl.lnk                        <OWRELROOT>/binl/wlink.lnk

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp.dll/wl.exe                  <OWRELROOT>/axpnt/wlink.exe
    <CPCMD> ntaxp.dll/wld.dll                 <OWRELROOT>/axpnt/wlinkd.dll
    <CPCMD> exe2bin/ntaxp/exe2bin.exe         <OWRELROOT>/axpnt/exe2bin.exe
    <CPCMD> wlsystem.lnk                      <OWRELROOT>/binw/wlsystem.lnk
    <CPCMD> wlaxpnt.lnk                       <OWRELROOT>/axpnt/wlink.lnk

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnx386/wl.exe                     <OWRELROOT>/qnx/binq/wlink
    <CPCMD> qnx386/wl.sym                     <OWRELROOT>/qnx/sym/wlink.sym
    <CPCMD> qnx.lnk                           <OWRELROOT>/qnx/etc/wlink.lnk

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
