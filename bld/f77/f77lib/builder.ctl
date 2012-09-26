# WFLIB Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=wflib

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================

# intel compilers and link utilities
  [ IFDEF (os_dos "") <2*> ]
   # 16-bit DOS Libraries
    <CPCMD> msdos.086/ml/flibl.lib      <OWRELROOT>/lib286/dos/
    <CPCMD> msdos.086/mm/flibm.lib      <OWRELROOT>/lib286/dos/
    <CPCMD> msdos.087/ml/flib7l.lib     <OWRELROOT>/lib286/dos/
    <CPCMD> msdos.087/mm/flib7m.lib     <OWRELROOT>/lib286/dos/
   # 32-bit DOS Libraries
    <CPCMD> msdos.386/ms_r/flib.lib     <OWRELROOT>/lib386/dos/flib.lib
    <CPCMD> msdos.386/ms_s/flibs.lib    <OWRELROOT>/lib386/dos/flibs.lib
    <CPCMD> msdos.387/ms_r/flib7.lib    <OWRELROOT>/lib386/dos/flib7.lib
    <CPCMD> msdos.387/ms_s/flib7s.lib   <OWRELROOT>/lib386/dos/flib7s.lib

  [ IFDEF (os_win "") <2*> ]
   # 16-bit Windows Libraries
    <CPCMD> windows.086/ml/flibl.lib    <OWRELROOT>/lib286/win/flibl.lib
    <CPCMD> windows.086/mm/flibm.lib    <OWRELROOT>/lib286/win/flibm.lib
    <CPCMD> windows.087/ml/flib7l.lib   <OWRELROOT>/lib286/win/flib7l.lib
    <CPCMD> windows.087/mm/flib7m.lib   <OWRELROOT>/lib286/win/flib7m.lib
   # 32-bit Windows Libraries
    <CPCMD> windows.386/mf_r/flib.lib   <OWRELROOT>/lib386/win/flib.lib
    <CPCMD> windows.386/mf_s/flibs.lib  <OWRELROOT>/lib386/win/flibs.lib
    <CPCMD> windows.387/mf_r/flib7.lib  <OWRELROOT>/lib386/win/flib7.lib
    <CPCMD> windows.387/mf_s/flib7s.lib <OWRELROOT>/lib386/win/flib7s.lib

  [ IFDEF (os_os2 "") <2*> ]
   # 16-bit OS/2 Libraries
    <CPCMD> os2.286/ml/flibl.lib        <OWRELROOT>/lib286/os2/flibl.lib
    <CPCMD> os2.286/mm/flibm.lib        <OWRELROOT>/lib286/os2/flibm.lib
    <CPCMD> os2.287/ml/flib7l.lib       <OWRELROOT>/lib286/os2/flib7l.lib
    <CPCMD> os2.287/mm/flib7m.lib       <OWRELROOT>/lib286/os2/flib7m.lib
   # 32-bit OS/2 Libraries
    <CPCMD> os2.386/mf_r/flib.lib       <OWRELROOT>/lib386/os2/flib.lib
    <CPCMD> os2.386/mf_s/flibs.lib      <OWRELROOT>/lib386/os2/flibs.lib
    <CPCMD> os2.387/mf_r/flib7.lib      <OWRELROOT>/lib386/os2/flib7.lib
    <CPCMD> os2.387/mf_s/flib7s.lib     <OWRELROOT>/lib386/os2/flib7s.lib

  [ IFDEF (os_nt "") <2*> ]
   # NT Libraries
    <CPCMD> winnt.386/mf_r/flib.lib     <OWRELROOT>/lib386/nt/flib.lib
    <CPCMD> winnt.386/mf_s/flibs.lib    <OWRELROOT>/lib386/nt/flibs.lib
    <CPCMD> winnt.387/mf_r/flib7.lib    <OWRELROOT>/lib386/nt/flib7.lib
    <CPCMD> winnt.387/mf_s/flib7s.lib   <OWRELROOT>/lib386/nt/flib7s.lib

  [ IFDEF (os_nov "") <2*> ]
   # 32-bit NETWARE Libraries
    <CPCMD> netware.386/ms_s/flibs.lib  <OWRELROOT>/lib386/netware/
    <CPCMD> netware.387/ms_s/flib7s.lib <OWRELROOT>/lib386/netware/

  [ IFDEF (os_linux "") <2*> ]
   # LINUX Libraries
    <CPCMD> linux.386/mf_r/flib.lib     <OWRELROOT>/lib386/linux/flib.lib
    <CPCMD> linux.386/mf_s/flibs.lib    <OWRELROOT>/lib386/linux/flibs.lib
    <CPCMD> linux.387/mf_r/flib7.lib    <OWRELROOT>/lib386/linux/flib7.lib
    <CPCMD> linux.387/mf_s/flib7s.lib   <OWRELROOT>/lib386/linux/flib7s.lib

# axp compilers and link utilities (NT)
  [ IFDEF (cpu_axp) <2*> ]
   # NT Libraries
    <CPCMD> winnt.axp/_s/flib.lib       <OWRELROOT>/libaxp/nt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
