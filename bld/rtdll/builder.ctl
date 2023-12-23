# Builder Control file
# ==========================

set PROJNAME=rtdll

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
#    cdsay "<PROJDIR>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> wrtlib/os2.386/mf_r/wr*.lib         "<OWRELROOT>/lib386/os2/"
    <CCCMD> wrtlib/os2.386/mf_r/wr*.dll         "<OWRELROOT>/binp/dll/"
    <CCCMD> wrtlib/os2.386/mf_r/wr*.sym         "<OWRELROOT>/binp/dll/"
    <CCCMD> wrtlib/os2.386/mf_s/wr*.lib         "<OWRELROOT>/lib386/os2/"
    <CCCMD> wrtlib/os2.386/mf_s/wr*.dll         "<OWRELROOT>/binp/dll/"
    <CCCMD> wrtlib/os2.386/mf_s/wr*.sym         "<OWRELROOT>/binp/dll/"
    <CCCMD> wrtlib/os2.387/mf_r/wr*.lib         "<OWRELROOT>/lib386/os2/"
    <CCCMD> wrtlib/os2.387/mf_r/wr*.dll         "<OWRELROOT>/binp/dll/"
    <CCCMD> wrtlib/os2.387/mf_r/wr*.sym         "<OWRELROOT>/binp/dll/"
    <CCCMD> wrtlib/os2.387/mf_s/wr*.lib         "<OWRELROOT>/lib386/os2/"
    <CCCMD> wrtlib/os2.387/mf_s/wr*.dll         "<OWRELROOT>/binp/dll/"
    <CCCMD> wrtlib/os2.387/mf_s/wr*.sym         "<OWRELROOT>/binp/dll/"

    <CCCMD> wrtlib/winnt.386/mf_r/wr*.lib       "<OWRELROOT>/lib386/nt/"
    <CCCMD> wrtlib/winnt.386/mf_r/wr*.dll       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.386/mf_r/wr*.sym       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.386/mf_s/wr*.lib       "<OWRELROOT>/lib386/nt/"
    <CCCMD> wrtlib/winnt.386/mf_s/wr*.dll       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.386/mf_s/wr*.sym       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.387/mf_r/wr*.lib       "<OWRELROOT>/lib386/nt/"
    <CCCMD> wrtlib/winnt.387/mf_r/wr*.dll       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.387/mf_r/wr*.sym       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.387/mf_s/wr*.lib       "<OWRELROOT>/lib386/nt/"
    <CCCMD> wrtlib/winnt.387/mf_s/wr*.dll       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.387/mf_s/wr*.sym       "<OWRELROOT>/binnt/"

    <CCCMD> wrtlib/winnt.mps/_s/wr*.lib       "<OWRELROOT>/libmps/nt/"
    <CCCMD> wrtlib/winnt.mps/_s/wr*.dll       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.mps/_s/wr*.sym       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.axp/_s/wr*.lib       "<OWRELROOT>/libaxp/nt/"
    <CCCMD> wrtlib/winnt.axp/_s/wr*.dll       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.axp/_s/wr*.sym       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.ppc/_s/wr*.lib       "<OWRELROOT>/libppc/nt/"
    <CCCMD> wrtlib/winnt.ppc/_s/wr*.dll       "<OWRELROOT>/binnt/"
    <CCCMD> wrtlib/winnt.ppc/_s/wr*.sym       "<OWRELROOT>/binnt/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
