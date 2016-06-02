# ce Builder Control file
# =======================

set PROJNAME=cmdedit

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CPCMD> cmdedit.txt         <OWRELROOT>/binw/cmdedit.txt

    <CCCMD> dosi86/cmdedit.exe  <OWRELROOT>/binw/cmdedit.exe

    <CCCMD> os2i86/os2edit.exe  <OWRELROOT>/binp/os2edit.exe
    <CCCMD> os2i86/os22edit.exe <OWRELROOT>/binp/os22edit.exe
    <CCCMD> os2i86/os2edit.dll  <OWRELROOT>/binp/dll/os2edit.dll

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
