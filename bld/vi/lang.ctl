# VI Builder Control file
# =======================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
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

    <CPCMD> <PROJDIR>/dat/v*.cmd                <RELROOT>/binp/
    <CPCMD> <PROJDIR>/dat/v*.bat                <RELROOT>/binw/
    <CPCMD> <PROJDIR>/dat/*.vi                  <RELROOT>/eddat/
    <CPCMD> <PROJDIR>/dat/*.cfg                 <RELROOT>/eddat/
    <CPCMD> <PROJDIR>/dat/*.dat                 <RELROOT>/eddat/
    <CPCMD> <PROJDIR>/dat/*.ini                 <RELROOT>/eddat/
    <CPCMD> <PROJDIR>/dat/bindvi.*              <RELROOT>/eddat/
    <CPCMD> <PROJDIR>/doc/*.hlp                 <RELROOT>/eddat/

  [ IFDEF (os_dos "") <2*> ]
#    <CPCMD> <PROJDIR>/obj286/vi.exe            <RELROOT>/binw/vi286.exe
    <CPCMD> <PROJDIR>/dos386/vi.exe             <RELROOT>/binw/vi.exe
    <CPCMD> <PROJDIR>/dos386/vi.sym             <RELROOT>/binw/vi.sym
    <CPCMD> <PROJDIR>/ctags/dos386/ctags.exe    <RELROOT>/binw/ctags.exe
    <CPCMD> <PROJDIR>/bind/dosi86/edbind.exe    <RELROOT>/binw/edbind.exe

  [ IFDEF (os_win "") <2*> ]
    <CPCMD> <PROJDIR>/wini86/vi.exe             <RELROOT>/binw/viw.exe
    <CPCMD> <PROJDIR>/wini86/vi.sym             <RELROOT>/binw/viw.sym
    <CPCMD> <PROJDIR>/win386/vi.exe             <RELROOT>/binw/viwp.exe
    <CPCMD> <PROJDIR>/win386/vi.sym             <RELROOT>/binw/viwp.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> <PROJDIR>/nt386/vi.exe              <RELROOT>/binnt/vi.exe
    <CPCMD> <PROJDIR>/nt386/vi.sym              <RELROOT>/binnt/vi.sym
    <CPCMD> <PROJDIR>/nt386.win/vi.exe          <RELROOT>/binnt/viw.exe
    <CPCMD> <PROJDIR>/nt386.win/vi.sym          <RELROOT>/binnt/viw.sym
    <CPCMD> <PROJDIR>/ctags/nt386/ctags.exe     <RELROOT>/binnt/ctags.exe
    <CPCMD> <PROJDIR>/bind/nt386/edbind.exe     <RELROOT>/binnt/edbind.exe

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> <PROJDIR>/ntaxp/vi.exe              <RELROOT>/axpnt/vi.exe
    <CPCMD> <PROJDIR>/ntaxp/vi.sym              <RELROOT>/axpnt/vi.sym
    <CPCMD> <PROJDIR>/ntaxp.win/vi.exe          <RELROOT>/axpnt/viw.exe
    <CPCMD> <PROJDIR>/ntaxp.win/vi.sym          <RELROOT>/axpnt/viw.sym
    <CPCMD> <PROJDIR>/ctags/ntaxp/ctags.exe     <RELROOT>/axpnt/ctags.exe
    <CPCMD> <PROJDIR>/bind/ntaxp/edbind.exe     <RELROOT>/axpnt/edbind.exe

  [ IFDEF (os_os2 "") <2*> ]
    <CPCMD> <PROJDIR>/os2386/vi.exe             <RELROOT>/binp/vi.exe
    <CPCMD> <PROJDIR>/os2386/vi.sym             <RELROOT>/binp/vi.sym
    <CPCMD> <PROJDIR>/ctags/os2386/ctags.exe    <RELROOT>/binp/ctags.exe
    <CPCMD> <PROJDIR>/bind/os2386/edbind.exe    <RELROOT>/binp/edbind.exe

  [ IFDEF (os_qnx) <2*> ]
    <CPCMD> <PROJDIR>/qnxi86/vi.exe             <RELROOT>/qnx/binq/vi

  [ IFDEF (os_linux "") <2*> ]
    <CPCMD> <PROJDIR>/linux386/vi.exe           <RELROOT>/binl/vi
    <CPCMD> <PROJDIR>/linux386/vi.sym           <RELROOT>/binl/vi.sym
    <CPCMD> <PROJDIR>/ctags/linux386/ctags.exe  <RELROOT>/binl/ctags
    <CPCMD> <PROJDIR>/bind/linux386/edbind.exe  <RELROOT>/binl/edbind

[ BLOCK <1> clean ]
#==================
    pmake -d all <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>
