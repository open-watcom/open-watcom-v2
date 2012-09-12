# WRESEDIT Builder Control file
# =============================

set PROJDIR=<CWD>
set PROJNAME=wresedit

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_win "") <2*> ]
    <CPCMD> waccel/wini86/wacc.dll     <OWRELROOT>/binw/wacc.dll
    <CPCMD> waccel/wini86/wacc.sym     <OWRELROOT>/binw/wacc.sym
    <CPCMD> wmenu/wini86/wmenu.dll     <OWRELROOT>/binw/wmenu.dll
    <CPCMD> wmenu/wini86/wmenu.sym     <OWRELROOT>/binw/wmenu.sym
    <CPCMD> wstring/wini86/wstring.dll <OWRELROOT>/binw/wstring.dll
    <CPCMD> wstring/wini86/wstring.sym <OWRELROOT>/binw/wstring.sym

  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> waccel/nt386/wacc.dll      <OWRELROOT>/binnt/wacc.dll
    <CPCMD> waccel/nt386/wacc.sym     <OWRELROOT>/binnt/wacc.sym
    <CPCMD> wmenu/nt386/wmenu.dll      <OWRELROOT>/binnt/wmenu.dll
    <CPCMD> wmenu/nt386/wmenu.sym      <OWRELROOT>/binnt/wmenu.sym
    <CPCMD> wstring/nt386/wstring.dll  <OWRELROOT>/binnt/wstring.dll
    <CPCMD> wstring/nt386/wstring.sym  <OWRELROOT>/binnt/wstring.sym

  [ IFDEF (cpu_axp) <2*> ]
    <CPCMD> waccel/ntaxp/wacc.dll      <OWRELROOT>/axpnt/wacc.dll
    <CPCMD> waccel/ntaxp/wacc.sym      <OWRELROOT>/axpnt/wacc.sym
    <CPCMD> wmenu/ntaxp/wmenu.dll      <OWRELROOT>/axpnt/wmenu.dll
    <CPCMD> wmenu/ntaxp/wmenu.sym      <OWRELROOT>/axpnt/wmenu.sym
    <CPCMD> wstring/ntaxp/wstring.dll  <OWRELROOT>/axpnt/wstring.dll
    <CPCMD> wstring/ntaxp/wstring.sym  <OWRELROOT>/axpnt/wstring.sym

[ BLOCK . . ]
#============
cdsay <PROJDIR>
