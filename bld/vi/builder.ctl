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

#    <CCCMD> obj286/vi.exe            <OWRELROOT>/binw/vi286.exe
    <CCCMD> dos386/vi.exe             <OWRELROOT>/binw/vi.exe
    <CCCMD> dos386/vi.sym             <OWRELROOT>/binw/vi.sym
    <CCCMD> ctags/dos386/ctags.exe    <OWRELROOT>/binw/ctags.exe
    <CCCMD> bind/dosi86/edbind.exe    <OWRELROOT>/binw/edbind.exe
    <CCCMD> wini86/vi.exe             <OWRELROOT>/binw/viw.exe
    <CCCMD> wini86/vi.sym             <OWRELROOT>/binw/viw.sym
    <CCCMD> win386/vi.exe             <OWRELROOT>/binw/viwp.exe
    <CCCMD> win386/vi.sym             <OWRELROOT>/binw/viwp.sym
    <CCCMD> nt386/vi.exe              <OWRELROOT>/binnt/vi.exe
    <CCCMD> nt386/vi.sym              <OWRELROOT>/binnt/vi.sym
    <CCCMD> nt386.win/vi.exe          <OWRELROOT>/binnt/viw.exe
    <CCCMD> nt386.win/vi.sym          <OWRELROOT>/binnt/viw.sym
    <CCCMD> ctags/nt386/ctags.exe     <OWRELROOT>/binnt/ctags.exe
    <CCCMD> bind/nt386/edbind.exe     <OWRELROOT>/binnt/edbind.exe
    <CCCMD> os2386/vi.exe             <OWRELROOT>/binp/vi.exe
    <CCCMD> os2386/vi.sym             <OWRELROOT>/binp/vi.sym
    <CCCMD> ctags/os2386/ctags.exe    <OWRELROOT>/binp/ctags.exe
    <CCCMD> bind/os2386/edbind.exe    <OWRELROOT>/binp/edbind.exe
    <CCCMD> linux386/vi.exe           <OWRELROOT>/binl/vi
    <CCCMD> linux386/vi.sym           <OWRELROOT>/binl/vi.sym
    <CCCMD> ctags/linux386/ctags.exe  <OWRELROOT>/binl/ctags
    <CCCMD> bind/linux386/edbind.exe  <OWRELROOT>/binl/edbind

    <CCCMD> ntaxp/vi.exe              <OWRELROOT>/axpnt/vi.exe
    <CCCMD> ntaxp/vi.sym              <OWRELROOT>/axpnt/vi.sym
    <CCCMD> ntaxp.win/vi.exe          <OWRELROOT>/axpnt/viw.exe
    <CCCMD> ntaxp.win/vi.sym          <OWRELROOT>/axpnt/viw.sym
    <CCCMD> ctags/ntaxp/ctags.exe     <OWRELROOT>/axpnt/ctags.exe
    <CCCMD> bind/ntaxp/edbind.exe     <OWRELROOT>/axpnt/edbind.exe

    <CCCMD> qnxi86/vi.exe             <OWRELROOT>/qnx/binq/vi

    <CCCMD> ntx64/vi.exe              <OWRELROOT>/binnt64/
    <CCCMD> ntx64.win/vi.exe          <OWRELROOT>/binnt64/viw.exe

[ BLOCK . . ]
#============
cdsay <PROJDIR>
