# Wlink Builder Control file
# ==========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
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
    <CPCMD> <PROJDIR>/dos386/wl.exe                     <RELROOT>/binw/wlink.exe
    <CPCMD> <PROJDIR>/dos386/wl.sym                     <RELROOT>/binw/wlink.sym
    <CPCMD> <PROJDIR>/ms2wlink/dosi86/ms2wlink.exe      <RELROOT>/binw/ms2wlink.exe
    <CPCMD> <PROJDIR>/fcenable/dosi86/fcenable.exe      <RELROOT>/binw/fcenable.exe
    <CPCMD> <PROJDIR>/exe2bin/dosi86/exe2bin.exe        <RELROOT>/binw/exe2bin.exe
    <CPCMD> <PROJDIR>/wlsystem.lnk                      <RELROOT>/binw/wlsystem.lnk
    <CPCMD> <PROJDIR>/wlbin.lnk                         <RELROOT>/binw/wlink.lnk
    <CPCMD> <PROJDIR>/ovlldr/dosi86/wovl.lib            <RELROOT>/lib286/dos/wovl.lib
#    <CPCMD> <PROJDIR>/ovlldr/dosi86/wmovl.lib           <RELROOT>/lib286/dos/wmovl.lib

  [ IFDEF (os_zdos) <2*> ]
    <CPCMD> <PROJDIR>/zdos386/wl.exe                    <RELROOT>/binz/wlink.exe
    <CPCMD> <PROJDIR>/zdos386/wl.sym                    <RELROOT>/binz/sym/wlink.sym
    <CPCMD> <PROJDIR>/wlbinz.lnk                        <RELROOT>/binz/wlink.lnk

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <PROJDIR>/os2386.dll/wl.exe                 <RELROOT>/binp/wlink.exe
    <CPCMD> <PROJDIR>/os2386.dll/wl.sym                 <RELROOT>/binp/wlink.sym
    <CPCMD> <PROJDIR>/os2386.dll/wld.dll                <RELROOT>/binp/dll/wlinkd.dll
    <CPCMD> <PROJDIR>/os2386.dll/wld.sym                <RELROOT>/binp/dll/wlinkd.sym
    <CPCMD> <PROJDIR>/ms2wlink/os2i86/ms2wlink.exe      <RELROOT>/binp/ms2wlink.exe
    <CPCMD> <PROJDIR>/fcenable/os2i86/fcenable.exe      <RELROOT>/binp/fcenable.exe
    <CPCMD> <PROJDIR>/exe2bin/os2386/exe2bin.exe        <RELROOT>/binp/exe2bin.exe
    <CPCMD> <PROJDIR>/wlsystem.lnk                      <RELROOT>/binw/wlsystem.lnk
    <CPCMD> <PROJDIR>/wlbinp.lnk                        <RELROOT>/binp/wlink.lnk

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <PROJDIR>/nt386.dll/wl.exe                  <RELROOT>/binnt/wlink.exe
    <CPCMD> <PROJDIR>/nt386.dll/wl.sym                  <RELROOT>/binnt/wlink.sym
    <CPCMD> <PROJDIR>/nt386.dll/wld.dll                 <RELROOT>/binnt/wlinkd.dll
    <CPCMD> <PROJDIR>/nt386.dll/wld.sym                 <RELROOT>/binnt/wlinkd.sym
    <CPCMD> <PROJDIR>/nt386.rtd/wl.exe                  <RELROOT>/binnt/rtdll/wlink.exe
    <CPCMD> <PROJDIR>/nt386.rtd/wld.dll                 <RELROOT>/binnt/rtdll/wlinkd.dll
    <CPCMD> <PROJDIR>/ms2wlink/nt386/ms2wlink.exe       <RELROOT>/binnt/ms2wlink.exe
    <CPCMD> <PROJDIR>/fcenable/nt386/fcenable.exe       <RELROOT>/binnt/fcenable.exe
    <CPCMD> <PROJDIR>/exe2bin/nt386/exe2bin.exe         <RELROOT>/binnt/exe2bin.exe
    <CPCMD> <PROJDIR>/wlsystem.lnk                      <RELROOT>/binw/wlsystem.lnk
    <CPCMD> <PROJDIR>/wlbinnt.lnk                       <RELROOT>/binnt/wlink.lnk

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> <PROJDIR>/linux386/wl.exe                   <RELROOT>/binl/wlink
    <CPCMD> <PROJDIR>/linux386/wl.sym                   <RELROOT>/binl/wlink.sym
    <CPCMD> <PROJDIR>/ms2wlink/linux386/ms2wlink.exe    <RELROOT>/binl/ms2wlink
    <CPCMD> <PROJDIR>/fcenable/linux386/fcenable.exe    <RELROOT>/binl/fcenable
    <CPCMD> <PROJDIR>/exe2bin/linux386/exe2bin.exe      <RELROOT>/binl/exe2bin
    <CPCMD> <PROJDIR>/wlsystem.lnk                      <RELROOT>/binl/wlsystem.lnk
    <CPCMD> <PROJDIR>/wlbinl.lnk                        <RELROOT>/binl/wlink.lnk

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <PROJDIR>/ntaxp.dll/wl.exe                  <RELROOT>/axpnt/wlink.exe
    <CPCMD> <PROJDIR>/ntaxp.dll/wld.dll                 <RELROOT>/axpnt/wlinkd.dll
    <CPCMD> <PROJDIR>/exe2bin/ntaxp/exe2bin.exe         <RELROOT>/axpnt/exe2bin.exe
    <CPCMD> <PROJDIR>/wlsystem.lnk                      <RELROOT>/binw/wlsystem.lnk
    <CPCMD> <PROJDIR>/wlaxpnt.lnk                       <RELROOT>/axpnt/wlink.lnk

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> <PROJDIR>/qnx386/wl.exe                     <RELROOT>/qnx/binq/wlink
    <CPCMD> <PROJDIR>/qnx386/wl.sym                     <RELROOT>/qnx/sym/wlink.sym
    <CPCMD> <PROJDIR>/qnx.lnk                           <RELROOT>/qnx/etc/wlink.lnk

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
