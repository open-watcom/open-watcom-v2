# WSAMPLE Builder Control file
# ============================

set PROJNAME=wsample

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#========================
    <CCCMD> dosi86/wsample.exe      <OWRELROOT>/binw/
    <CCCMD> rsii86/wsamprsi.exe     <OWRELROOT>/binw/
    <CCCMD> pls386/wsamppls.exp     <OWRELROOT>/binw/
    <CCCMD> wini86/wsamplew.exe     <OWRELROOT>/binw/
#    <CCCMD> os2i86/wsampos2.exe     <OWRELROOT>/binp/
    <CCCMD> os2386/wsmpos22.exe     <OWRELROOT>/binp/wsample.exe
    <CCCMD> nt386/wsmpnt.exe        <OWRELROOT>/binnt/wsample.exe
    <CCCMD> linux386/wsample.exe    <OWRELROOT>/binl/wsample
    <CCCMD> ntaxp/wsmpaxp.exe       <OWRELROOT>/axpnt/wsample.exe
    <CCCMD> nlm/wsample.nlm         <OWRELROOT>/nlm/

    <CCCMD> ntx64/wsmpnt.exe        <OWRELROOT>/binnt64/wsample.exe
    <CCCMD> linuxx64/wsample.exe    <OWRELROOT>/binl64/wsample

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
