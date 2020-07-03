# WRES Builder Control file
# =========================

set PROJNAME=wres

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/deflib.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
# WIN 32-bit version
    <CCCMD> win386/mf_r/wres.lib   <OWRELROOT>/lib386/win/wresf.lib
    <CCCMD> win386/mf_s/wres.lib   <OWRELROOT>/lib386/win/wresfs.lib
# DOS 32-bit version
    <CCCMD> dos386/ms_r/wres.lib    <OWRELROOT>/lib386/dos/wresf.lib
    <CCCMD> dos386/ms_s/wres.lib    <OWRELROOT>/lib386/dos/wresfs.lib
# NT 32-bit version
    <CCCMD> nt386/mf_r/wres.lib     <OWRELROOT>/lib386/nt/wresf.lib
    <CCCMD> nt386/mf_s/wres.lib     <OWRELROOT>/lib386/nt/wresfs.lib
# OS2 32-bit version
    <CCCMD> os2386/mf_r/wres.lib    <OWRELROOT>/lib386/os2/wresf.lib
    <CCCMD> os2386/mf_s/wres.lib    <OWRELROOT>/lib386/os2/wresfs.lib
# AXP version
    <CCCMD> ntaxp/_s/wres.lib       <OWRELROOT>/libaxp/wresaxp.lib
# LINUX version
    <CCCMD> linux386/mf_r/wres.lib  <OWRELROOT>/lib386/linux/wresf.lib
    <CCCMD> linux386/mf_s/wres.lib  <OWRELROOT>/lib386/linux/wresfs.lib
# QNX version
    <CCCMD> qnx386/ms_r/wres.lib    <OWRELROOT>/lib386/qnx/wresf.lib
    <CCCMD> qnx386/ms_s/wres.lib    <OWRELROOT>/lib386/qnx/wresfs.lib
# Netware version
    <CCCMD> nov386/ms_r/wres.lib    <OWRELROOT>/lib386/netware/wresf.lib
    <CCCMD> nov386/ms_s/wres.lib    <OWRELROOT>/lib386/netware/wresfs.lib

# DOS 16-bit version
    <CCCMD> dosi86/mm/wres.lib      <OWRELROOT>/lib286/wresm.lib
    <CCCMD> dosi86/ml/wres.lib      <OWRELROOT>/lib286/wresl.lib
# OS2 16-bit version
    <CCCMD> os2i86/ml/wres.lib      <OWRELROOT>/lib286/os2/wresl.lib
# WIN 16-bit version
    <CCCMD> wini86/ml/wres.lib      <OWRELROOT>/lib286/win/wresl.lib

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
