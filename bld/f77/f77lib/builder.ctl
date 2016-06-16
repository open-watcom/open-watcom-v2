# WFLIB Builder Control file
# ==========================

set PROJNAME=wflib

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================

# intel compilers and link utilities
   # 16-bit DOS Libraries
    <CCCMD> msdos.086/ml/flibl.lib      <OWRELROOT>/lib286/dos/
    <CCCMD> msdos.086/mm/flibm.lib      <OWRELROOT>/lib286/dos/
    <CCCMD> msdos.087/ml/flib7l.lib     <OWRELROOT>/lib286/dos/
    <CCCMD> msdos.087/mm/flib7m.lib     <OWRELROOT>/lib286/dos/
   # 32-bit DOS Libraries
    <CCCMD> msdos.386/ms_r/flib.lib     <OWRELROOT>/lib386/dos/flib.lib
    <CCCMD> msdos.386/ms_s/flibs.lib    <OWRELROOT>/lib386/dos/flibs.lib
    <CCCMD> msdos.387/ms_r/flib7.lib    <OWRELROOT>/lib386/dos/flib7.lib
    <CCCMD> msdos.387/ms_s/flib7s.lib   <OWRELROOT>/lib386/dos/flib7s.lib

   # 16-bit Windows Libraries
    <CCCMD> windows.086/ml/flibl.lib    <OWRELROOT>/lib286/win/flibl.lib
    <CCCMD> windows.086/mm/flibm.lib    <OWRELROOT>/lib286/win/flibm.lib
    <CCCMD> windows.087/ml/flib7l.lib   <OWRELROOT>/lib286/win/flib7l.lib
    <CCCMD> windows.087/mm/flib7m.lib   <OWRELROOT>/lib286/win/flib7m.lib
   # 32-bit Windows Libraries
    <CCCMD> windows.386/mf_r/flib.lib   <OWRELROOT>/lib386/win/flib.lib
    <CCCMD> windows.386/mf_s/flibs.lib  <OWRELROOT>/lib386/win/flibs.lib
    <CCCMD> windows.387/mf_r/flib7.lib  <OWRELROOT>/lib386/win/flib7.lib
    <CCCMD> windows.387/mf_s/flib7s.lib <OWRELROOT>/lib386/win/flib7s.lib

   # 16-bit OS/2 Libraries
    <CCCMD> os2.286/ml/flibl.lib        <OWRELROOT>/lib286/os2/flibl.lib
    <CCCMD> os2.286/mm/flibm.lib        <OWRELROOT>/lib286/os2/flibm.lib
    <CCCMD> os2.287/ml/flib7l.lib       <OWRELROOT>/lib286/os2/flib7l.lib
    <CCCMD> os2.287/mm/flib7m.lib       <OWRELROOT>/lib286/os2/flib7m.lib
   # 32-bit OS/2 Libraries
    <CCCMD> os2.386/mf_r/flib.lib       <OWRELROOT>/lib386/os2/flib.lib
    <CCCMD> os2.386/mf_s/flibs.lib      <OWRELROOT>/lib386/os2/flibs.lib
    <CCCMD> os2.387/mf_r/flib7.lib      <OWRELROOT>/lib386/os2/flib7.lib
    <CCCMD> os2.387/mf_s/flib7s.lib     <OWRELROOT>/lib386/os2/flib7s.lib

   # NT Libraries
    <CCCMD> winnt.386/mf_r/flib.lib     <OWRELROOT>/lib386/nt/flib.lib
    <CCCMD> winnt.386/mf_s/flibs.lib    <OWRELROOT>/lib386/nt/flibs.lib
    <CCCMD> winnt.387/mf_r/flib7.lib    <OWRELROOT>/lib386/nt/flib7.lib
    <CCCMD> winnt.387/mf_s/flib7s.lib   <OWRELROOT>/lib386/nt/flib7s.lib

   # 32-bit NETWARE Libraries
    <CCCMD> netware.386/ms_s/flibs.lib  <OWRELROOT>/lib386/netware/
    <CCCMD> netware.387/ms_s/flib7s.lib <OWRELROOT>/lib386/netware/

   # LINUX Libraries
    <CCCMD> linux.386/mf_r/flib.lib     <OWRELROOT>/lib386/linux/flib.lib
    <CCCMD> linux.386/mf_s/flibs.lib    <OWRELROOT>/lib386/linux/flibs.lib
    <CCCMD> linux.387/mf_r/flib7.lib    <OWRELROOT>/lib386/linux/flib7.lib
    <CCCMD> linux.387/mf_s/flib7s.lib   <OWRELROOT>/lib386/linux/flib7s.lib

# axp compilers and link utilities (NT)
   # NT Libraries
    <CCCMD> winnt.axp/_s/flib.lib       <OWRELROOT>/libaxp/nt/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
