# setupgui Builder Control file
# =============================
 
set PROJNAME=setupgui

set PROJDIR=<CWD>
 
[ INCLUDE "<OWROOT>/build/prolog.ctl" ]
 
[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay "<PROJDIR>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/setup.exe    "<OWRELROOT>/setup/dos/"
    <CCCMD> dos386/setup.sym    "<OWRELROOT>/setup/dos/"
    <CCCMD> os2386/setup.exe    "<OWRELROOT>/setup/os2/"
    <CCCMD> os2386/setup.sym    "<OWRELROOT>/setup/os2/"
    <CCCMD> nt386/setup.exe     "<OWRELROOT>/setup/nt32/"
    <CCCMD> nt386/setup.sym     "<OWRELROOT>/setup/nt32/"
    <CCCMD> linux386/setup.exe  "<OWRELROOT>/setup/linux32/"
    <CCCMD> linux386/setup.sym  "<OWRELROOT>/setup/linux32/"
    <CCCMD> ntx64/setup.exe     "<OWRELROOT>/setup/nt64/"
    <CCCMD> ntx64/setup.sym     "<OWRELROOT>/setup/nt64/"
    <CCCMD> linuxx64/setup.exe  "<OWRELROOT>/setup/linux64/"
    <CCCMD> linuxx64/setup.sym  "<OWRELROOT>/setup/linux64/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
