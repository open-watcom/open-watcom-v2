# WRESEDIT Builder Control file
# =============================

set PROJNAME=wresedit

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> waccel/wini86/wacc.dll     <OWRELROOT>/binw/
    <CCCMD> waccel/wini86/wacc.sym     <OWRELROOT>/binw/
    <CCCMD> wmenu/wini86/wmenu.dll     <OWRELROOT>/binw/
    <CCCMD> wmenu/wini86/wmenu.sym     <OWRELROOT>/binw/
    <CCCMD> wstring/wini86/wstring.dll <OWRELROOT>/binw/
    <CCCMD> wstring/wini86/wstring.sym <OWRELROOT>/binw/
    <CCCMD> waccel/nt386/wacc.dll      <OWRELROOT>/binnt/
    <CCCMD> waccel/nt386/wacc.sym      <OWRELROOT>/binnt/
    <CCCMD> wmenu/nt386/wmenu.dll      <OWRELROOT>/binnt/
    <CCCMD> wmenu/nt386/wmenu.sym      <OWRELROOT>/binnt/
    <CCCMD> wstring/nt386/wstring.dll  <OWRELROOT>/binnt/
    <CCCMD> wstring/nt386/wstring.sym  <OWRELROOT>/binnt/
    <CCCMD> waccel/ntaxp/wacc.dll      <OWRELROOT>/axpnt/
    <CCCMD> waccel/ntaxp/wacc.sym      <OWRELROOT>/axpnt/
    <CCCMD> wmenu/ntaxp/wmenu.dll      <OWRELROOT>/axpnt/
    <CCCMD> wmenu/ntaxp/wmenu.sym      <OWRELROOT>/axpnt/
    <CCCMD> wstring/ntaxp/wstring.dll  <OWRELROOT>/axpnt/
    <CCCMD> wstring/ntaxp/wstring.sym  <OWRELROOT>/axpnt/

    <CCCMD> waccel/ntx64/wacc.dll      <OWRELROOT>/binnt64/
    <CCCMD> wmenu/ntx64/wmenu.dll      <OWRELROOT>/binnt64/
    <CCCMD> wstring/ntx64/wstring.dll  <OWRELROOT>/binnt64/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
