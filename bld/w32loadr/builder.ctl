# w32ldr Builder Control file
# ===========================

set PROJNAME=w32ldr

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <BINTOOL> build ]
#========================
    cdsay <PROJDIR>
    <CPCMD> <OWOBJDIR>/w32bind.exe <OWBINDIR>/w32bind<CMDEXT>

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f <OWBINDIR>/w32bind<CMDEXT>
    rm -f <OWBINDIR>/w32bind<CMDEXT>

[ BLOCK <1> rel cprel ]
#======================
   <CCCMD> dos386/cwsrun.exe   <OWRELROOT>/binw/w32run.exe
#   <CCCMD> dos386/x32run.exe   <OWRELROOT>/binw/x32run.exe
#   <CCCMD> dos386/x32run.exe   <OWRELROOT>/binw/x32run.exe
#   <CCCMD> dos386/d4grun.exe   <OWRELROOT>/binw/d4grun.exe
#   <CCCMD> dos386/tntrun.exe   <OWRELROOT>/binw/tntrun.exe

#   <CCCMD> os2386/w32bind.exe  ../build/binp/w32bind.exe
#   <CCCMD> os2386/os2ldr.exe   ../build/binp/os2ldr.exe

    <CCCMD> nt386/w32run.exe   <OWRELROOT>/binnt/w32run.exe
#    these are now real NT exe's - don't copy the stub
#    <CCCMD> nt/*.exe           <OWRELROOT>/binnt/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
