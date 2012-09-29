# WRESEDIT Builder Control file
# =============================

set PROJDIR=<CWD>
set PROJNAME=wresedit

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> waccel/wini86/wacc.dll     <OWRELROOT>/binw/wacc.dll
    <CCCMD> waccel/wini86/wacc.sym     <OWRELROOT>/binw/wacc.sym
    <CCCMD> wmenu/wini86/wmenu.dll     <OWRELROOT>/binw/wmenu.dll
    <CCCMD> wmenu/wini86/wmenu.sym     <OWRELROOT>/binw/wmenu.sym
    <CCCMD> wstring/wini86/wstring.dll <OWRELROOT>/binw/wstring.dll
    <CCCMD> wstring/wini86/wstring.sym <OWRELROOT>/binw/wstring.sym
    <CCCMD> waccel/nt386/wacc.dll      <OWRELROOT>/binnt/wacc.dll
    <CCCMD> waccel/nt386/wacc.sym      <OWRELROOT>/binnt/wacc.sym
    <CCCMD> wmenu/nt386/wmenu.dll      <OWRELROOT>/binnt/wmenu.dll
    <CCCMD> wmenu/nt386/wmenu.sym      <OWRELROOT>/binnt/wmenu.sym
    <CCCMD> wstring/nt386/wstring.dll  <OWRELROOT>/binnt/wstring.dll
    <CCCMD> wstring/nt386/wstring.sym  <OWRELROOT>/binnt/wstring.sym
    <CCCMD> waccel/ntaxp/wacc.dll      <OWRELROOT>/axpnt/wacc.dll
    <CCCMD> waccel/ntaxp/wacc.sym      <OWRELROOT>/axpnt/wacc.sym
    <CCCMD> wmenu/ntaxp/wmenu.dll      <OWRELROOT>/axpnt/wmenu.dll
    <CCCMD> wmenu/ntaxp/wmenu.sym      <OWRELROOT>/axpnt/wmenu.sym
    <CCCMD> wstring/ntaxp/wstring.dll  <OWRELROOT>/axpnt/wstring.dll
    <CCCMD> wstring/ntaxp/wstring.sym  <OWRELROOT>/axpnt/wstring.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
