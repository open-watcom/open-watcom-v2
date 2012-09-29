# WSAMPLE Builder Control file
# ============================

set PROJDIR=<CWD>
set PROJNAME=wsample

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#========================
    <CCCMD> dosi86/wsample.exe      <OWRELROOT>/binw/wsample.exe
    <CCCMD> rsii86/wsamprsi.exe     <OWRELROOT>/binw/wsamprsi.exe
    <CCCMD> pls386/wsamppls.exp     <OWRELROOT>/binw/wsamppls.exp
    <CCCMD> wini86/wsamplew.exe     <OWRELROOT>/binw/wsamplew.exe
#    <CCCMD> os2i86/wsampos2.exe     <OWRELROOT>/binp/wsampos2.exe
    <CCCMD> os2386/wsmpos22.exe     <OWRELROOT>/binp/wsample.exe
    <CCCMD> nt386/wsmpnt.exe        <OWRELROOT>/binnt/wsample.exe
    <CCCMD> linux386/wsample.exe    <OWRELROOT>/binl/wsample
    <CCCMD> ntaxp/wsmpaxp.exe       <OWRELROOT>/axpnt/wsample.exe
    <CCCMD> nlm/wsample.nlm         <OWRELROOT>/nlm/wsample.nlm

[ BLOCK . . ]
#============
cdsay <PROJDIR>
