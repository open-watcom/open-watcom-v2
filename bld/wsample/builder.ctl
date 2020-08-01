# WSAMPLE Builder Control file
# ============================

set PROJNAME=wsample

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/deftool.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#========================
    <CCCMD> dosi86/wsample.exe      <OWRELROOT>/binw/
    <CCCMD> dosi86/wsample.sym      <OWRELROOT>/binw/
    <CCCMD> rsii86/wsamprsi.exe     <OWRELROOT>/binw/
    <CCCMD> rsii86/wsamprsi.sym     <OWRELROOT>/binw/
    <CCCMD> pls386/wsamppls.exp     <OWRELROOT>/binw/
    <CCCMD> pls386/wsamppls.sym     <OWRELROOT>/binw/
    <CCCMD> wini86/wsamplew.exe     <OWRELROOT>/binw/
    <CCCMD> wini86/wsamplew.sym     <OWRELROOT>/binw/
#    <CCCMD> os2i86/wsampos2.exe     <OWRELROOT>/binp/
#    <CCCMD> os2i86/wsampos2.sym     <OWRELROOT>/binp/
    <CCCMD> os2386/wsample.exe      <OWRELROOT>/binp/
    <CCCMD> os2386/wsample.sym      <OWRELROOT>/binp/
    <CCCMD> nt386/wsample.exe       <OWRELROOT>/binnt/
    <CCCMD> nt386/wsample.sym       <OWRELROOT>/binnt/
    <CCCMD> linux386/wsample.exe    <OWRELROOT>/binl/wsample
    <CCCMD> linux386/wsample.sym    <OWRELROOT>/binl/
    <CCCMD> ntaxp/wsample.exe       <OWRELROOT>/axpnt/
    <CCCMD> ntaxp/wsample.sym       <OWRELROOT>/axpnt/
    <CCCMD> nov386/wsample.nlm      <OWRELROOT>/nlm/
    <CCCMD> nov386/wsample.sym      <OWRELROOT>/nlm/

    <CCCMD> ntx64/wsample.exe       <OWRELROOT>/binnt64/
    <CCCMD> ntx64/wsample.sym       <OWRELROOT>/binnt64/
    <CCCMD> linuxx64/wsample.exe    <OWRELROOT>/binl64/wsample
    <CCCMD> linuxx64/wsample.sym    <OWRELROOT>/binl64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
