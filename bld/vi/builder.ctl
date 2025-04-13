# VI Builder Control file
# =======================

set PROJNAME=vi

set PROJDIR=<CWD>

[ INCLUDE "<OWROOT>/build/prolog.ctl" ]

[ INCLUDE "<OWROOT>/build/defrule.ctl" ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay "<PROJDIR>"

[ BLOCK <BLDRULE> rel cprel ]
#============================
# Common stuff
    <CPCMD> dat/v*.cmd                   "<OWRELROOT>/binp/"
    <CPCMD> dat/v*.bat                   "<OWRELROOT>/binw/"
    <CPCMD> dat/*.vi                     "<OWRELROOT>/eddat/"
    <CPCMD> dat/*.cfg                    "<OWRELROOT>/eddat/"
    <CPCMD> dat/*.dat                    "<OWRELROOT>/eddat/"
    <CPCMD> dat/*.ini                    "<OWRELROOT>/eddat/"
    <CPCMD> dat/bindvi.*                 "<OWRELROOT>/eddat/"
    <CPCMD> doc/*.hlp                    "<OWRELROOT>/eddat/"

#    <CCCMD> obj286/<OWOBJDIR>/vi.exe    "<OWRELROOT>/binw/vi286.exe"
    <CCCMD> dos386/<OWOBJDIR>/vi.exe     "<OWRELROOT>/binw/"
    <CCCMD> dos386/vi.sym                "<OWRELROOT>/binw/"
    <CCCMD> ctags/dos386/ctags.exe       "<OWRELROOT>/binw/"
    <CCCMD> bind/dosi86/edbind.exe       "<OWRELROOT>/binw/"
    <CCCMD> wini86/<OWOBJDIR>/viw.exe    "<OWRELROOT>/binw/"
    <CCCMD> wini86/viw.sym               "<OWRELROOT>/binw/"
    <CCCMD> win386/<OWOBJDIR>/viw32.exe  "<OWRELROOT>/binw/"
    <CCCMD> win386/viw32.sym             "<OWRELROOT>/binw/"
    <CCCMD> nt386/<OWOBJDIR>/vi.exe      "<OWRELROOT>/binnt/"
    <CCCMD> nt386/vi.sym                 "<OWRELROOT>/binnt/"
    <CCCMD> nt386.win/<OWOBJDIR>/viw.exe "<OWRELROOT>/binnt/"
    <CCCMD> nt386.win/viw.sym            "<OWRELROOT>/binnt/"
    <CCCMD> ctags/nt386/ctags.exe        "<OWRELROOT>/binnt/"
    <CCCMD> bind/nt386/edbind.exe        "<OWRELROOT>/binnt/"
    <CCCMD> os2386/<OWOBJDIR>/vi.exe     "<OWRELROOT>/binp/"
    <CCCMD> os2386/vi.sym                "<OWRELROOT>/binp/"
    <CCCMD> ctags/os2386/ctags.exe       "<OWRELROOT>/binp/"
    <CCCMD> bind/os2386/edbind.exe       "<OWRELROOT>/binp/"
    <CCCMD> linux386/<OWOBJDIR>/vi.exe   "<OWRELROOT>/binl/vi"
    <CCCMD> linux386/vi.sym              "<OWRELROOT>/binl/"
    <CCCMD> ctags/linux386/ctags.exe     "<OWRELROOT>/binl/ctags"
    <CCCMD> bind/linux386/edbind.exe     "<OWRELROOT>/binl/edbind"

    <CCCMD> ntaxp/<OWOBJDIR>/vi.exe      "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp/vi.sym                 "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.win/<OWOBJDIR>/viw.exe "<OWRELROOT>/axpnt/"
    <CCCMD> ntaxp.win/viw.sym            "<OWRELROOT>/axpnt/"
    <CCCMD> ctags/ntaxp/ctags.exe        "<OWRELROOT>/axpnt/"
    <CCCMD> bind/ntaxp/edbind.exe        "<OWRELROOT>/axpnt/"

    <CCCMD> qnxi86/<OWOBJDIR>/vi.exe     "<OWRELROOT>/qnx/binq/vi"

    <CCCMD> bsdx64/<OWOBJDIR>/vi.exe     "<OWRELROOT>/binb64/vi"
    <CCCMD> ctags/bsdx64/ctags.exe       "<OWRELROOT>/binb64/ctags"
    <CCCMD> bind/bsdx64/edbind.exe       "<OWRELROOT>/binb64/edbind"
    <CCCMD> ntx64/<OWOBJDIR>/vi.exe      "<OWRELROOT>/binnt64/"
    <CCCMD> ntx64.win/<OWOBJDIR>/viw.exe "<OWRELROOT>/binnt64/"
    <CCCMD> ctags/ntx64/ctags.exe        "<OWRELROOT>/binnt64/"
    <CCCMD> bind/ntx64/edbind.exe        "<OWRELROOT>/binnt64/"
    <CCCMD> linuxx64/<OWOBJDIR>/vi.exe   "<OWRELROOT>/binl64/vi"
    <CCCMD> linuxx64/vi.sym              "<OWRELROOT>/binl64/"
    <CCCMD> ctags/linuxx64/ctags.exe     "<OWRELROOT>/binl64/ctags"
    <CCCMD> bind/linuxx64/edbind.exe     "<OWRELROOT>/binl64/edbind"
    <CCCMD> linuxarm/<OWOBJDIR>/vi.exe   "<OWRELROOT>/arml/vi"
    <CCCMD> linuxa64/<OWOBJDIR>/vi.exe   "<OWRELROOT>/arml64/vi"
    <CCCMD> ctags/linuxarm/ctags.exe     "<OWRELROOT>/arml/ctags"
    <CCCMD> bind/linuxarm/edbind.exe     "<OWRELROOT>/arml/edbind"
    <CCCMD> ctags/linuxa64/ctags.exe     "<OWRELROOT>/arml64/ctags"
    <CCCMD> bind/linuxa64/edbind.exe     "<OWRELROOT>/arml64/edbind"
    <CCCMD> osxx64/<OWOBJDIR>/vi.exe     "<OWRELROOT>/bino64/vi"
    <CCCMD> ctags/osxx64/ctags.exe       "<OWRELROOT>/bino64/ctags"
    <CCCMD> bind/osxx64/edbind.exe       "<OWRELROOT>/bino64/edbind"
    <CCCMD> osxarm/<OWOBJDIR>/vi.exe     "<OWRELROOT>/armo/vi"
    <CCCMD> osxa64/<OWOBJDIR>/vi.exe     "<OWRELROOT>/armo64/vi"
    <CCCMD> ctags/osxarm/ctags.exe       "<OWRELROOT>/armo/ctags"
    <CCCMD> bind/osxarm/edbind.exe       "<OWRELROOT>/armo/edbind"
    <CCCMD> ctags/osxa64/ctags.exe       "<OWRELROOT>/armo64/ctags"
    <CCCMD> bind/osxa64/edbind.exe       "<OWRELROOT>/armo64/edbind"

[ BLOCK . . ]

[ INCLUDE "<OWROOT>/build/epilog.ctl" ]
