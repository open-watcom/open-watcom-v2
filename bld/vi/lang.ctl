# VI Builder Control file
# =======================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    [ INCLUDE prereq.ctl ]
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h bind=1

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
# Common stuff

    <CPCMD> dat/v*.cmd                <RELROOT>/binp/
    <CPCMD> dat/v*.bat                <RELROOT>/binw/
    <CPCMD> dat/*.vi                  <RELROOT>/eddat/
    <CPCMD> dat/*.cfg                 <RELROOT>/eddat/
    <CPCMD> dat/*.dat                 <RELROOT>/eddat/
    <CPCMD> dat/*.ini                 <RELROOT>/eddat/
    <CPCMD> dat/bindvi.*              <RELROOT>/eddat/
    <CPCMD> doc/*.hlp                 <RELROOT>/eddat/

  [ IFDEF (os_dos "") <2*> ]
#    <CPCMD> obj286/vi.exe            <RELROOT>/binw/vi286.exe
    <CPCMD> dos386/vi.exe             <RELROOT>/binw/vi.exe
    <CPCMD> dos386/vi.sym             <RELROOT>/binw/vi.sym
    <CPCMD> ctags/dos386/ctags.exe    <RELROOT>/binw/ctags.exe
    <CPCMD> bind/dosi86/edbind.exe    <RELROOT>/binw/edbind.exe

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> wini86/vi.exe             <RELROOT>/binw/viw.exe
    <CPCMD> wini86/vi.sym             <RELROOT>/binw/viw.sym
    <CPCMD> win386/vi.exe             <RELROOT>/binw/viwp.exe
    <CPCMD> win386/vi.sym             <RELROOT>/binw/viwp.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt386/vi.exe              <RELROOT>/binnt/vi.exe
    <CPCMD> nt386/vi.sym              <RELROOT>/binnt/vi.sym
    <CPCMD> nt386.win/vi.exe          <RELROOT>/binnt/viw.exe
    <CPCMD> nt386.win/vi.sym          <RELROOT>/binnt/viw.sym
    <CPCMD> ctags/nt386/ctags.exe     <RELROOT>/binnt/ctags.exe
    <CPCMD> bind/nt386/edbind.exe     <RELROOT>/binnt/edbind.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> ntaxp/vi.exe              <RELROOT>/axpnt/vi.exe
    <CPCMD> ntaxp/vi.sym              <RELROOT>/axpnt/vi.sym
    <CPCMD> ntaxp.win/vi.exe          <RELROOT>/axpnt/viw.exe
    <CPCMD> ntaxp.win/vi.sym          <RELROOT>/axpnt/viw.sym
    <CPCMD> ctags/ntaxp/ctags.exe     <RELROOT>/axpnt/ctags.exe
    <CPCMD> bind/ntaxp/edbind.exe     <RELROOT>/axpnt/edbind.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> os2386/vi.exe             <RELROOT>/binp/vi.exe
    <CPCMD> os2386/vi.sym             <RELROOT>/binp/vi.sym
    <CPCMD> ctags/os2386/ctags.exe    <RELROOT>/binp/ctags.exe
    <CPCMD> bind/os2386/edbind.exe    <RELROOT>/binp/edbind.exe

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> qnxi86/vi.exe             <RELROOT>/qnx/binq/vi

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> linux386/vi.exe           <RELROOT>/binl/vi
    <CPCMD> linux386/vi.sym           <RELROOT>/binl/vi.sym
    <CPCMD> ctags/linux386/ctags.exe  <RELROOT>/binl/ctags
    <CPCMD> bind/linux386/edbind.exe  <RELROOT>/binl/edbind

[ BLOCK <1> clean ]
#==================
    pmake -d all <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
