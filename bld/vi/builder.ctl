# VI Builder Control file
# =======================

set PROJDIR=<CWD>
set PROJNAME=vi

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
# Common stuff

    <CPCMD> dat/v*.cmd                <OWRELROOT>/binp/
    <CPCMD> dat/v*.bat                <OWRELROOT>/binw/
    <CPCMD> dat/*.vi                  <OWRELROOT>/eddat/
    <CPCMD> dat/*.cfg                 <OWRELROOT>/eddat/
    <CPCMD> dat/*.dat                 <OWRELROOT>/eddat/
    <CPCMD> dat/*.ini                 <OWRELROOT>/eddat/
    <CPCMD> dat/bindvi.*              <OWRELROOT>/eddat/
    <CPCMD> doc/*.hlp                 <OWRELROOT>/eddat/

  [ IFDEF (os_dos "") <2*> ]
#    <CPCMD> obj286/vi.exe            <OWRELROOT>/binw/vi286.exe
    <CPCMD> dos386/vi.exe             <OWRELROOT>/binw/vi.exe
    <CPCMD> dos386/vi.sym             <OWRELROOT>/binw/vi.sym
    <CPCMD> ctags/dos386/ctags.exe    <OWRELROOT>/binw/ctags.exe
    <CPCMD> bind/dosi86/edbind.exe    <OWRELROOT>/binw/edbind.exe

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/vi.exe             <OWRELROOT>/binw/viw.exe
    <CPCMD> wini86/vi.sym             <OWRELROOT>/binw/viw.sym
    <CPCMD> win386/vi.exe             <OWRELROOT>/binw/viwp.exe
    <CPCMD> win386/vi.sym             <OWRELROOT>/binw/viwp.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/vi.exe              <OWRELROOT>/binnt/vi.exe
    <CPCMD> nt386/vi.sym              <OWRELROOT>/binnt/vi.sym
    <CPCMD> nt386.win/vi.exe          <OWRELROOT>/binnt/viw.exe
    <CPCMD> nt386.win/vi.sym          <OWRELROOT>/binnt/viw.sym
    <CPCMD> ctags/nt386/ctags.exe     <OWRELROOT>/binnt/ctags.exe
    <CPCMD> bind/nt386/edbind.exe     <OWRELROOT>/binnt/edbind.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/vi.exe              <OWRELROOT>/axpnt/vi.exe
    <CPCMD> ntaxp/vi.sym              <OWRELROOT>/axpnt/vi.sym
    <CPCMD> ntaxp.win/vi.exe          <OWRELROOT>/axpnt/viw.exe
    <CPCMD> ntaxp.win/vi.sym          <OWRELROOT>/axpnt/viw.sym
    <CPCMD> ctags/ntaxp/ctags.exe     <OWRELROOT>/axpnt/ctags.exe
    <CPCMD> bind/ntaxp/edbind.exe     <OWRELROOT>/axpnt/edbind.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/vi.exe             <OWRELROOT>/binp/vi.exe
    <CPCMD> os2386/vi.sym             <OWRELROOT>/binp/vi.sym
    <CPCMD> ctags/os2386/ctags.exe    <OWRELROOT>/binp/ctags.exe
    <CPCMD> bind/os2386/edbind.exe    <OWRELROOT>/binp/edbind.exe

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnxi86/vi.exe             <OWRELROOT>/qnx/binq/vi

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/vi.exe           <OWRELROOT>/binl/vi
    <CPCMD> linux386/vi.sym           <OWRELROOT>/binl/vi.sym
    <CPCMD> ctags/linux386/ctags.exe  <OWRELROOT>/binl/ctags
    <CPCMD> bind/linux386/edbind.exe  <OWRELROOT>/binl/edbind

[ BLOCK . . ]
#============
cdsay <PROJDIR>
