.dm sf1 begin
:SF font=1.&*.
.dm sf1 end
.*
.dm sf2 begin
:SF font=2.&*.
.dm sf2 end
.*
.dm sf4 begin
:SF font=4.&*.
.dm sf4 end
.*
.dm sf7 begin
:SF font=7.&*.
.dm sf7 end
.*
.dm esf begin
:eSF.&*.
.dm esf end
.*
:set symbol='us'        value=';.sf1 '.
:set symbol='eus'       value=';.esf '.
:set symbol='bd'        value=';.sf2 '.
:set symbol='ebd'       value=';.esf '.
:set symbol='mn'        value=';.sf4 '.
:set symbol='emn'       value=';.esf '.
.*
:set symbol="copyr"     value=";.ct .sf7 ~C;.esf ".
:set symbol="tm"        value=";.ct .sf7 ~T;.esf ".
:set symbol="rt"        value=";.ct .sf7 ~r;.esf ".
:set symbol="pi"        value=";.ct .sf7 ~P;.esf ".
:set symbol="pm"        value=";.ct .sf7 ~p;.esf ".
:set symbol="sysper"    value="~."
:set symbol="mdash"     value="~-".
:set symbol="period"    value=".".
:set symbol="minus"     value="-".
:set symbol="dagger"    value="~D".
:set symbol="lt"        value="<".
:set symbol="gt"        value=">".
:set symbol="lc"        value="<".
:set symbol="rc"        value=">".
:set symbol="lbrkt"     value="[".
:set symbol="rbrkt"     value="]".
:set symbol="slash"     value="\".
:set symbol="caret"     value="^".
:set symbol="arrow"     value="<-".
:set symbol="tilde"     value="~~".
.****************************************************************
.* Keyboard key macros
.*  - various macros for keyboard keys
.*  - includes arrow keys, function keys, enter, escape,
.*    pgup, etc
.****************************************************************
:set symbol="crsup"     value=";.ct .sf7 ~U;.esf ".
:set symbol="crsdn"     value=";.ct .sf7 ~D;.esf ".
:set symbol="crslt"     value=";.ct .sf7 ~L;.esf ".
:set symbol="crsrt"     value=";.ct .sf7 ~R;.esf ".
:set symbol="bksp"      value=";.sf7 ~L;.esf ".
:set symbol="minus"     value=";.ct .sf7 -;.esf ".
:set symbol="semi"      value=";".
:set symbol='enter'     value=';.ct .sf2 Enter;.esf '.
:set symbol='tab'       value=';.ct .sf2 Tab;.esf '.
:set symbol='insert'    value=';.ct .sf2 Insert;.esf '.
:set symbol='delete'    value=';.ct .sf2 Delete;.esf '.
:set symbol='pageup'    value=';.ct .sf2 Page Up;.esf '.
:set symbol='pagedn'    value=';.ct .sf2 Page Down;.esf '.
:set symbol='shift'     value=';.ct .sf2 Shift;.esf '.
:set symbol='numlock'   value=';.ct .sf2 Num Lock;.esf '.
:set symbol='esc'       value=';.ct .sf2 Esc;.esf '.
:set symbol='alt'       value=';.ct .sf2 Alt;.esf '.
:set symbol='ctrl'      value=';.ct .sf2 Ctrl;.esf '.
:set symbol='spacebar'  value=';.ct .sf2 SpaceBar;.esf '.
.*
:set symbol='f1'        value=';.ct .sf2 F1;.esf '.
:set symbol='f2'        value=';.ct .sf2 F2;.esf '.
:set symbol='f3'        value=';.ct .sf2 F3;.esf '.
:set symbol='f4'        value=';.ct .sf2 F4;.esf '.
:set symbol='f5'        value=';.ct .sf2 F5;.esf '.
:set symbol='f6'        value=';.ct .sf2 F6;.esf '.
:set symbol='f7'        value=';.ct .sf2 F7;.esf '.
:set symbol='f8'        value=';.ct .sf2 F8;.esf '.
:set symbol='f9'        value=';.ct .sf2 F9;.esf '.
:set symbol='f10'       value=';.ct .sf2 F10;.esf '.
:set symbol='f11'       value=';.ct .sf2 F11;.esf '.
:set symbol='f12'       value=';.ct .sf2 F12;.esf '.
.****************************************************************
.* Generic enter and escape symbols
.****************************************************************
:set symbol='g_enter'     value=';.sf2 Enter;.esf '.
:set symbol='g_escape'    value=';.sf2 Escape;.esf '.
