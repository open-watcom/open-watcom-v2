# WFC Builder Control file
# ========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d buildlib <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================

# intel compilers and link utilities
  [ IFDEF (os_dos "") <2*> ]
   # 16-bit DOS Libraries
    <CPCMD> <DEVDIR>/f77/f77lib/msdos.086/ml/flibl.lib      <RELROOT>/lib286/dos/
    <CPCMD> <DEVDIR>/f77/f77lib/msdos.086/mm/flibm.lib      <RELROOT>/lib286/dos/
    <CPCMD> <DEVDIR>/f77/f77lib/msdos.087/ml/flib7l.lib     <RELROOT>/lib286/dos/
    <CPCMD> <DEVDIR>/f77/f77lib/msdos.087/mm/flib7m.lib     <RELROOT>/lib286/dos/
   # 32-bit DOS Libraries
    <CPCMD> <DEVDIR>/f77/f77lib/msdos.386/ms_r/flib.lib     <RELROOT>/lib386/dos/flib.lib
    <CPCMD> <DEVDIR>/f77/f77lib/msdos.386/ms_s/flibs.lib    <RELROOT>/lib386/dos/flibs.lib
    <CPCMD> <DEVDIR>/f77/f77lib/msdos.387/ms_r/flib7.lib    <RELROOT>/lib386/dos/flib7.lib
    <CPCMD> <DEVDIR>/f77/f77lib/msdos.387/ms_s/flib7s.lib   <RELROOT>/lib386/dos/flib7s.lib

  [ IFDEF (os_win "") <2*> ]
   # 16-bit Windows Libraries
    <CPCMD> <DEVDIR>/f77/f77lib/windows.086/ml/flibl.lib    <RELROOT>/lib286/win/flibl.lib
    <CPCMD> <DEVDIR>/f77/f77lib/windows.086/mm/flibm.lib    <RELROOT>/lib286/win/flibm.lib
    <CPCMD> <DEVDIR>/f77/f77lib/windows.087/ml/flib7l.lib   <RELROOT>/lib286/win/flib7l.lib
    <CPCMD> <DEVDIR>/f77/f77lib/windows.087/mm/flib7m.lib   <RELROOT>/lib286/win/flib7m.lib
   # 32-bit Windows Libraries
    <CPCMD> <DEVDIR>/f77/f77lib/windows.386/mf_r/flib.lib   <RELROOT>/lib386/win/flib.lib
    <CPCMD> <DEVDIR>/f77/f77lib/windows.386/mf_s/flibs.lib  <RELROOT>/lib386/win/flibs.lib
    <CPCMD> <DEVDIR>/f77/f77lib/windows.387/mf_r/flib7.lib  <RELROOT>/lib386/win/flib7.lib
    <CPCMD> <DEVDIR>/f77/f77lib/windows.387/mf_s/flib7s.lib <RELROOT>/lib386/win/flib7s.lib

  [ IFDEF (os_os2 "") <2*> ]
   # 16-bit OS/2 Libraries
    <CPCMD> <DEVDIR>/f77/f77lib/os2.286/ml/flibl.lib        <RELROOT>/lib286/os2/flibl.lib
    <CPCMD> <DEVDIR>/f77/f77lib/os2.286/mm/flibm.lib        <RELROOT>/lib286/os2/flibm.lib
    <CPCMD> <DEVDIR>/f77/f77lib/os2.287/ml/flib7l.lib       <RELROOT>/lib286/os2/flib7l.lib
    <CPCMD> <DEVDIR>/f77/f77lib/os2.287/mm/flib7m.lib       <RELROOT>/lib286/os2/flib7m.lib
   # 32-bit OS/2 Libraries
    <CPCMD> <DEVDIR>/f77/f77lib/os2.386/mf_r/flib.lib       <RELROOT>/lib386/os2/flib.lib
    <CPCMD> <DEVDIR>/f77/f77lib/os2.386/mf_s/flibs.lib      <RELROOT>/lib386/os2/flibs.lib
    <CPCMD> <DEVDIR>/f77/f77lib/os2.387/mf_r/flib7.lib      <RELROOT>/lib386/os2/flib7.lib
    <CPCMD> <DEVDIR>/f77/f77lib/os2.387/mf_s/flib7s.lib     <RELROOT>/lib386/os2/flib7s.lib

  [ IFDEF (os_nt "") <2*> ]
   # NT Libraries
    <CPCMD> <DEVDIR>/f77/f77lib/winnt.386/mf_r/flib.lib     <RELROOT>/lib386/nt/flib.lib
    <CPCMD> <DEVDIR>/f77/f77lib/winnt.386/mf_s/flibs.lib    <RELROOT>/lib386/nt/flibs.lib
    <CPCMD> <DEVDIR>/f77/f77lib/winnt.387/mf_r/flib7.lib    <RELROOT>/lib386/nt/flib7.lib
    <CPCMD> <DEVDIR>/f77/f77lib/winnt.387/mf_s/flib7s.lib   <RELROOT>/lib386/nt/flib7s.lib

  [ IFDEF (os_nov "") <2*> ]
   # 32-bit NETWARE Libraries
    <CPCMD> <DEVDIR>/f77/f77lib/netware.386/ms_s/flibs.lib  <RELROOT>/lib386/netware/
    <CPCMD> <DEVDIR>/f77/f77lib/netware.387/ms_s/flib7s.lib <RELROOT>/lib386/netware/

  [ IFDEF (os_linux "") <2*> ]
   # LINUX Libraries
    <CPCMD> <DEVDIR>/f77/f77lib/linux.386/mf_r/flib.lib     <RELROOT>/lib386/linux/flib.lib
    <CPCMD> <DEVDIR>/f77/f77lib/linux.386/mf_s/flibs.lib    <RELROOT>/lib386/linux/flibs.lib
    <CPCMD> <DEVDIR>/f77/f77lib/linux.387/mf_r/flib7.lib    <RELROOT>/lib386/linux/flib7.lib
    <CPCMD> <DEVDIR>/f77/f77lib/linux.387/mf_s/flib7s.lib   <RELROOT>/lib386/linux/flib7s.lib

# axp compilers and link utilities (NT)
  [ IFDEF (cpu_axp) <2*> ]
   # NT Libraries
    <CPCMD> <DEVDIR>/f77/f77lib/winnt.axp/_s/flib.lib       <RELROOT>/libaxp/nt/

[ BLOCK <1> clean ]
#==================
    pmake -d buildlib <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
