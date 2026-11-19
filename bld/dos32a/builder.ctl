# DOS/32A DOS Extender Builder Control file
# =========================================

set PROJNAME=dos32a

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay "<PROJDIR>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/*.exe            "<OWRELROOT>/binw/"
    <CCCMD> dos386/*.stb            "<OWRELROOT>/binw/"

    <CCCMD> dos386/dos32a.exe       "<OWRELROOT>/binnt/"
    <CCCMD> dos386/dos32a.stb       "<OWRELROOT>/binnt/"
    <CCCMD> dos386/stub32a.exe      "<OWRELROOT>/binnt/"
    <CCCMD> dos386/stub32c.exe      "<OWRELROOT>/binnt/"

    <CCCMD> dos386/dos32a.exe       "<OWRELROOT>/binnt64/"
    <CCCMD> dos386/dos32a.stb       "<OWRELROOT>/binnt64/"
    <CCCMD> dos386/stub32a.exe      "<OWRELROOT>/binnt64/"
    <CCCMD> dos386/stub32c.exe      "<OWRELROOT>/binnt64/"

    <CCCMD> dos386/dos32a.exe       "<OWRELROOT>/binl/"
    <CCCMD> dos386/dos32a.stb       "<OWRELROOT>/binl/"
    <CCCMD> dos386/stub32a.exe      "<OWRELROOT>/binl/"
    <CCCMD> dos386/stub32c.exe      "<OWRELROOT>/binl/"

    <CCCMD> dos386/dos32a.exe       "<OWRELROOT>/binl64/"
    <CCCMD> dos386/dos32a.stb       "<OWRELROOT>/binl64/"
    <CCCMD> dos386/stub32a.exe      "<OWRELROOT>/binl64/"
    <CCCMD> dos386/stub32c.exe      "<OWRELROOT>/binl64/"

    <CCCMD> dos386/dos32a.exe       "<OWRELROOT>/arml64/"
    <CCCMD> dos386/dos32a.stb       "<OWRELROOT>/arml64/"
    <CCCMD> dos386/stub32a.exe      "<OWRELROOT>/arml64/"
    <CCCMD> dos386/stub32c.exe      "<OWRELROOT>/arml64/"

    <CCCMD> dos386/dos32a.exe       "<OWRELROOT>/bino64/"
    <CCCMD> dos386/dos32a.stb       "<OWRELROOT>/bino64/"
    <CCCMD> dos386/stub32a.exe      "<OWRELROOT>/bino64/"
    <CCCMD> dos386/stub32c.exe      "<OWRELROOT>/bino64/"

    <CCCMD> dos386/dos32a.exe       "<OWRELROOT>/armo64/"
    <CCCMD> dos386/dos32a.stb       "<OWRELROOT>/armo64/"
    <CCCMD> dos386/stub32a.exe      "<OWRELROOT>/armo64/"
    <CCCMD> dos386/stub32c.exe      "<OWRELROOT>/armo64/"

    <CCCMD> "<OWROOT>/contrib/extender/dos32a/h32/d32a.h" "<OWRELROOT>/h/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
