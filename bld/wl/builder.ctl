# wlink Builder Control file
# ==========================

set PROJNAME=wlink

set BINTOOL=0

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay "<PROJDIR>"

[ BLOCK <BINTOOL> build ]
#========================
    cdsay "<PROJDIR>"
    <CPCMD> <OWOBJDIR>/bwlink.exe     "<OWROOT>/build/<OWOBJDIR>/bwlink<CMDEXT>"
    <CCCMD> <OWOBJDIR>/bwlinkd<DYEXT> "<OWROOT>/build/<OWOBJDIR>/bwlinkd<DYEXT>"

[ BLOCK <BINTOOL> clean ]
#========================
    echo rm -f "<OWROOT>/build/<OWOBJDIR>/bwlink<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwlink<CMDEXT>"
    rm -f "<OWROOT>/build/<OWOBJDIR>/bwlinkd<DYEXT>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos386/wlink.exe                "<OWRELROOT>/binw/"
    <CCCMD> dos386/wlink.sym                "<OWRELROOT>/binw/"
    <CCCMD> ms2wlink/dosi86/ms2wlink.exe    "<OWRELROOT>/binw/"
    <CCCMD> fcenable/dosi86/fcenable.exe    "<OWRELROOT>/binw/"
    <CCCMD> exe2bin/dosi86/exe2bin.exe      "<OWRELROOT>/binw/"
    <CCCMD> lnk/dos386/wlink.lnk            "<OWRELROOT>/binw/"
    <CCCMD> ovlldr/dosi86/wovl.lib          "<OWRELROOT>/lib286/dos/"
#    <CCCMD> ovlldr/dosi86/wmovl.lib         "<OWRELROOT>/lib286/dos/"

    <CCCMD> os2386.dll/wlink.exe            "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wlink.sym            "<OWRELROOT>/binp/"
    <CCCMD> os2386.dll/wlinkd.dll           "<OWRELROOT>/binp/dll/"
    <CCCMD> os2386.dll/wlinkd.sym           "<OWRELROOT>/binp/dll/"
    <CCCMD> ms2wlink/os2i86/ms2wlink.exe    "<OWRELROOT>/binp/"
    <CCCMD> fcenable/os2i86/fcenable.exe    "<OWRELROOT>/binp/"
    <CCCMD> exe2bin/os2386/exe2bin.exe      "<OWRELROOT>/binp/"
    <CCCMD> lnk/os2386/wlink.lnk            "<OWRELROOT>/binp/"

    <CCCMD> nt386.dll/wlink.exe             "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wlink.sym             "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wlinkd.dll            "<OWRELROOT>/binnt/"
    <CCCMD> nt386.dll/wlinkd.sym            "<OWRELROOT>/binnt/"
    <CCCMD> ms2wlink/nt386/ms2wlink.exe     "<OWRELROOT>/binnt/"
    <CCCMD> fcenable/nt386/fcenable.exe     "<OWRELROOT>/binnt/"
    <CCCMD> exe2bin/nt386/exe2bin.exe       "<OWRELROOT>/binnt/"
    <CCCMD> lnk/nt386/wlink.lnk             "<OWRELROOT>/binnt/"

    <CCCMD> linux386/wlink.exe              "<OWRELROOT>/binl/wlink"
    <CCCMD> linux386/wlink.sym              "<OWRELROOT>/binl/"
    <CCCMD> ms2wlink/linux386/ms2wlink.exe  "<OWRELROOT>/binl/ms2wlink"
    <CCCMD> fcenable/linux386/fcenable.exe  "<OWRELROOT>/binl/fcenable"
    <CCCMD> exe2bin/linux386/exe2bin.exe    "<OWRELROOT>/binl/exe2bin"
    <CCCMD> lnk/linux386/wlink.lnk          "<OWRELROOT>/binl/"

    <CCCMD> ntaxp.dll/wlink.exe             "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wlink.sym             "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wlinkd.dll            "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.dll/wlinkd.sym            "<OWRELROOT>/axpnt/"
    <CCCMD> exe2bin/ntaxp/exe2bin.exe       "<OWRELROOT>/axpnt/"
    <CCCMD> lnk/ntaxp/wlink.lnk             "<OWRELROOT>/axpnt/"

    <CCCMD> qnx386/wlink.exe                "<OWRELROOT>/qnx/binq/wlink"
    <CCCMD> qnx386/wlink.sym                "<OWRELROOT>/qnx/sym/"
    <CCCMD> lnk/qnx/wlink.lnk               "<OWRELROOT>/qnx/etc/"

    <CCCMD> bsdx64/wlink.exe                "<OWRELROOT>/binb64/wlink"
    <CCCMD> ms2wlink/bsdx64/ms2wlink.exe    "<OWRELROOT>/binb64/ms2wlink"
#    <CCCMD> fcenable/bsdx64/fcenable.exe    "<OWRELROOT>/binb64/fcenable"
    <CCCMD> exe2bin/bsdx64/exe2bin.exe      "<OWRELROOT>/binb64/exe2bin"
    <CCCMD> lnk/bsdx64/wlink.lnk            "<OWRELROOT>/binb64/"

    <CCCMD> ntx64.dll/wlink.exe             "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.dll/wlinkd.dll            "<OWRELROOT>/binnt64/"
    <CCCMD> ms2wlink/ntx64/ms2wlink.exe     "<OWRELROOT>/binnt64/"
    <CCCMD> fcenable/ntx64/fcenable.exe     "<OWRELROOT>/binnt64/"
    <CCCMD> exe2bin/ntx64/exe2bin.exe       "<OWRELROOT>/binnt64/"
    <CCCMD> lnk/ntx64/wlink.lnk             "<OWRELROOT>/binnt64/"

    <CCCMD> linuxx64/wlink.exe              "<OWRELROOT>/binl64/wlink"
    <CCCMD> linuxx64/wlink.sym              "<OWRELROOT>/binl64/"
    <CCCMD> ms2wlink/linuxx64/ms2wlink.exe  "<OWRELROOT>/binl64/ms2wlink"
    <CCCMD> fcenable/linuxx64/fcenable.exe  "<OWRELROOT>/binl64/fcenable"
    <CCCMD> exe2bin/linuxx64/exe2bin.exe    "<OWRELROOT>/binl64/exe2bin"
    <CCCMD> lnk/linuxx64/wlink.lnk          "<OWRELROOT>/binl64/"

    <CCCMD> linuxarm/wlink.exe              "<OWRELROOT>/arml/wlink"
#    <CCCMD> ms2wlink/linuxarm/ms2wlink.exe  "<OWRELROOT>/arml/ms2wlink"
#    <CCCMD> fcenable/linuxarm/fcenable.exe  "<OWRELROOT>/arml/fcenable"
#    <CCCMD> exe2bin/linuxarm/exe2bin.exe    "<OWRELROOT>/arml/exe2bin"
    <CCCMD> lnk/linuxarm/wlink.lnk          "<OWRELROOT>/arml/"
    <CCCMD> linuxa64/wlink.exe              "<OWRELROOT>/arml64/wlink"
#    <CCCMD> ms2wlink/linuxa64/ms2wlink.exe  "<OWRELROOT>/arml64/ms2wlink"
#    <CCCMD> fcenable/linuxa64/fcenable.exe  "<OWRELROOT>/arml64/fcenable"
#    <CCCMD> exe2bin/linuxa64/exe2bin.exe    "<OWRELROOT>/arml64/exe2bin"
    <CCCMD> lnk/linuxa64/wlink.lnk          "<OWRELROOT>/arml64/"

    <CCCMD> rdos386/wlink.exe               "<OWRELROOT>/rdos/"
    <CCCMD> rdos386/wlink.sym               "<OWRELROOT>/rdos/"
    <CCCMD> exe2bin/rdos386/exe2bin.exe     "<OWRELROOT>/rdos/"
    <CCCMD> lnk/rdos386/wlink.lnk           "<OWRELROOT>/rdos/"

    <CCCMD> osxx64/wlink.exe                "<OWRELROOT>/bino64/wlink"
    <CCCMD> ms2wlink/osxx64/ms2wlink.exe    "<OWRELROOT>/bino64/ms2wlink"
#    <CCCMD> fcenable/osxx64/fcenable.exe    "<OWRELROOT>/bino64/fcenable"
    <CCCMD> exe2bin/osxx64/exe2bin.exe      "<OWRELROOT>/bino64/exe2bin"
    <CCCMD> lnk/osxx64/wlink.lnk            "<OWRELROOT>/bino64/"
    <CCCMD> osxarm/wlink.exe                "<OWRELROOT>/armo/wlink"
    <CCCMD> ms2wlink/osxarm/ms2wlink.exe    "<OWRELROOT>/armo/ms2wlink"
#    <CCCMD> fcenable/osxarm/fcenable.exe    "<OWRELROOT>/armo/fcenable"
    <CCCMD> exe2bin/osxarm/exe2bin.exe      "<OWRELROOT>/armo/exe2bin"
    <CCCMD> lnk/osxarm/wlink.lnk            "<OWRELROOT>/armo/"
    <CCCMD> osxa64/wlink.exe                "<OWRELROOT>/armo64/wlink"
    <CCCMD> ms2wlink/osxa64/ms2wlink.exe    "<OWRELROOT>/armo64/ms2wlink"
#    <CCCMD> fcenable/osxa64/fcenable.exe    "<OWRELROOT>/armo64/fcenable"
    <CCCMD> exe2bin/osxa64/exe2bin.exe      "<OWRELROOT>/armo64/exe2bin"
    <CCCMD> lnk/osxa64/wlink.lnk            "<OWRELROOT>/armo64/"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
