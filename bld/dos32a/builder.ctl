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
    <CCCMD> dos32a/dosi86/dos32a.exe    "<OWRELROOT>/binw/"
    <CCCMD> dos32a/dosi86/stub32a.exe   "<OWRELROOT>/binw/"
    <CCCMD> dos32a/dosi86/stub32c.exe   "<OWRELROOT>/binw/"

    <CCCMD> sb/dos386/sb.exe            "<OWRELROOT>/binw/"
    <CCCMD> sc/dos386/sc.exe            "<OWRELROOT>/binw/"
    <CCCMD> sd/dos386/sd.exe            "<OWRELROOT>/binw/"
    <CCCMD> ss/dos386/ss.exe            "<OWRELROOT>/binw/"

    <CCCMD> license.d32                 "<OWRELROOT>/binw/"
    <CCCMD> readme.d32                  "<OWRELROOT>/binw/"

    <CCCMD> sver/dosi86/sver.exe        "<OWRELROOT>/binw/"
    <CCCMD> sver/os2386/sver.exe        "<OWRELROOT>/binp/"
    <CCCMD> sver/nt386/sver.exe         "<OWRELROOT>/binnt/"
    <CCCMD> sver/linux386/sver.exe      "<OWRELROOT>/binl/sver"
    <CCCMD> sver/ntaxp/sver.exe         "<OWRELROOT>/axpnt/"

    <CCCMD> sver/bsdx64/sver.exe        "<OWRELROOT>/binb64/sver"
    <CCCMD> sver/bsda64/sver.exe        "<OWRELROOT>/armb64/sver"
    <CCCMD> sver/ntx64/sver.exe         "<OWRELROOT>/binnt64/"
    <CCCMD> sver/linuxx64/sver.exe      "<OWRELROOT>/binl64/sver"
    <CCCMD> sver/linuxarm/sver.exe      "<OWRELROOT>/arml/sver"
    <CCCMD> sver/linuxa64/sver.exe      "<OWRELROOT>/arml64/sver"
    <CCCMD> sver/osxx64/sver.exe        "<OWRELROOT>/bino64/sver"
    <CCCMD> sver/osxarm/sver.exe        "<OWRELROOT>/armo/sver"
    <CCCMD> sver/osxa64/sver.exe        "<OWRELROOT>/armo64/sver"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
