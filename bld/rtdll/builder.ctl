# Builder Control file
# ==========================

set PROJDIR=<CWD>
set PROJNAME=rtdll

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> wrtlib/os2.386/mf_r/wr*.lib         <OWRELROOT>/lib386/os2/
    <CCCMD> wrtlib/os2.386/mf_r/wr*.dll         <OWRELROOT>/binp/dll/
    <CCCMD> wrtlib/os2.386/mf_r/wr*.sym         <OWRELROOT>/binp/dll/
    <CCCMD> wrtlib/os2.386/mf_s/wr*.lib         <OWRELROOT>/lib386/os2/
    <CCCMD> wrtlib/os2.386/mf_s/wr*.dll         <OWRELROOT>/binp/dll/
    <CCCMD> wrtlib/os2.386/mf_s/wr*.sym         <OWRELROOT>/binp/dll/
    <CCCMD> wrtlib/os2.387/mf_r/wr*.lib         <OWRELROOT>/lib386/os2/
    <CCCMD> wrtlib/os2.387/mf_r/wr*.dll         <OWRELROOT>/binp/dll/
    <CCCMD> wrtlib/os2.387/mf_r/wr*.sym         <OWRELROOT>/binp/dll/
    <CCCMD> wrtlib/os2.387/mf_s/wr*.lib         <OWRELROOT>/lib386/os2/
    <CCCMD> wrtlib/os2.387/mf_s/wr*.dll         <OWRELROOT>/binp/dll/
    <CCCMD> wrtlib/os2.387/mf_s/wr*.sym         <OWRELROOT>/binp/dll/

    <CCCMD> wrtlib/winnt.386/mf_r/wr*.lib       <OWRELROOT>/lib386/nt/
    <CCCMD> wrtlib/winnt.386/mf_r/wr*.dll       <OWRELROOT>/binnt/
    <CCCMD> wrtlib/winnt.386/mf_r/wr*.sym       <OWRELROOT>/binnt/
    <CCCMD> wrtlib/winnt.386/mf_s/wr*.lib       <OWRELROOT>/lib386/nt/
    <CCCMD> wrtlib/winnt.386/mf_s/wr*.dll       <OWRELROOT>/binnt/
    <CCCMD> wrtlib/winnt.386/mf_s/wr*.sym       <OWRELROOT>/binnt/
    <CCCMD> wrtlib/winnt.387/mf_r/wr*.lib       <OWRELROOT>/lib386/nt/
    <CCCMD> wrtlib/winnt.387/mf_r/wr*.dll       <OWRELROOT>/binnt/
    <CCCMD> wrtlib/winnt.387/mf_r/wr*.sym       <OWRELROOT>/binnt/
    <CCCMD> wrtlib/winnt.387/mf_s/wr*.lib       <OWRELROOT>/lib386/nt/
    <CCCMD> wrtlib/winnt.387/mf_s/wr*.dll       <OWRELROOT>/binnt/
    <CCCMD> wrtlib/winnt.387/mf_s/wr*.sym       <OWRELROOT>/binnt/

[ BLOCK . . ]
#============
cdsay <PROJDIR>
