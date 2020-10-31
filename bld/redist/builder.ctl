# Redistributable binary files control file
# =========================================

set PROJNAME=redist

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

cdsay .

[ BLOCK <BLDRULE> boot ]
#=================
    <CPCMD> dos4gw/dos4gw.exe <OWBINDIR>/<OWOBJDIR>/dos4gw.exe

[ BLOCK <BLDRULE> bootclean ]
#============================
    echo rm -f <OWBINDIR>/<OWOBJDIR>/dos4gw.exe
    rm -f <OWBINDIR>/<OWOBJDIR>/dos4gw.exe

[ BLOCK <BLDRULE> build rel ]
#============================

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CPCMD> dos4gw/*.exe            <OWRELROOT>/binw/
    <CPCMD> dos4gw/*.doc            <OWRELROOT>/binw/
    <CPCMD> dos32a/*                <OWRELROOT>/binw/
    <CPCMD> pmodew/*                <OWRELROOT>/binw/

[ BLOCK <BLDRULE> clean ]
#========================

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
