.dm sf1 begin
.ct :SF font=1.
.dm sf1 end
.*
.dm sf2 begin
.ct :SF font=2.
.dm sf2 end
.*
.dm sf4 begin
.ct :SF font=4.
.dm sf4 end
.*
.dm sf7 begin
.ct :SF font=7.
.dm sf7 end
.*
.dm esf begin
.ct :eSF.
.dm esf end
.*
:set symbol='us'	value=';.sf1;.ct '.
:set symbol='eus'	value=';.esf;.ct '.
:set symbol='bd'	value=';.sf2;.ct '.
:set symbol='ebd'	value=';.esf;.ct '.
:set symbol='mn'	value=';.sf4;.ct '.
:set symbol='emn'	value=';.esf;.ct '.
.*
:set symbol="copyr"	value=";.sf7;.ct ~C;.esf;.ct ".
:set symbol="tm"	value=";.sf7;.ct ~T;.esf;.ct ".
:set symbol="rt"	value=";.sf7;.ct ~r;.esf;.ct ".
:set symbol="pi"	value=";.sf7;.ct ~P;.esf;.ct ".
:set symbol="pm"	value=";.sf7;.ct ~p;.esf;.ct ".
:set symbol="period"	value=".".
:set symbol="minus"	value="-".
:set symbol="dagger"	value="~D".
:set symbol="lbrkt"	value="[".
:set symbol="rbrkt"	value="]".
:set symbol="slash"	value="\".
:set symbol="caret"	value="^".
:set symbol="arrow"	value="<-".
.****************************************************************
.* Keyboard key macros
.*  - various macros for keyboard keys
.*  - includes arrow keys, function keys, enter, escape,
.*    pgup, etc
.****************************************************************
:set symbol="crsup"	value=";.sf7;.ct ~U;.esf;.ct ".
:set symbol="crsdn"	value=";.sf7;.ct ~D;.esf;.ct ".
:set symbol="crslt"	value=";.sf7;.ct ~L;.esf;.ct ".
:set symbol="crsrt"	value=";.sf7;.ct ~R;.esf;.ct ".
:set symbol="bksp"	value=";.sf7;.ct ~L;.esf;.ct ".
:set symbol="minus"	value=";.sf7;.ct -;.esf;.ct ".
:set symbol="semi"	value=";".
:set symbol='enter'	value=';.sf2;.ct Enter;.esf;.ct '.
:set symbol='tab'	value=';.sf2;.ct Tab;.esf;.ct '.
:set symbol="insert"	value=";.sf2;.ct Insert;.esf;.ct ".
:set symbol="delete"	value=";.sf2;.ct Delete;.esf;.ct ".
:set symbol="pageup"	value=";.sf2;.ct Page Up;.esf;.ct ".
:set symbol="pagedn"	value=";.sf2;.ct Page Down;.esf;.ct ".
:set symbol="shift"	value=";.sf2;.ct Shift;.esf;.ct ".
:set symbol="numlock"	value=";.sf2;.ct Num Lock;.esf;.ct ".
:set symbol='esc'	value=';.sf2;.ct Esc;.esf;.ct '.
:set symbol='alt'	value=';.sf2;.ct Alt;.esf;.ct '.
:set symbol='ctrl'	value=';.sf2;.ct Ctrl;.esf;.ct '.
:set symbol='spacebar'	value=';.sf2;.ct SpaceBar;.esf;.ct '.
.*
:set symbol='f1'	value=';.sf2;.ct F1;.esf;.ct '.
:set symbol='f2'	value=';.sf2;.ct F2;.esf;.ct '.
:set symbol='f3'	value=';.sf2;.ct F3;.esf;.ct '.
:set symbol='f4'	value=';.sf2;.ct F4;.esf;.ct '.
:set symbol='f5'	value=';.sf2;.ct F5;.esf;.ct '.
:set symbol='f6'	value=';.sf2;.ct F6;.esf;.ct '.
:set symbol='f7'	value=';.sf2;.ct F7;.esf;.ct '.
:set symbol='f8'	value=';.sf2;.ct F8;.esf;.ct '.
:set symbol='f9'	value=';.sf2;.ct F9;.esf;.ct '.
:set symbol='f10'	value=';.sf2;.ct F10;.esf;.ct '.
:set symbol='f11'	value=';.sf2;.ct F11;.esf;.ct '.
:set symbol='f12'	value=';.sf2;.ct F12;.esf;.ct '.
.****************************************************************
.* Generic enter and escape symbols
.****************************************************************
:set symbol='g_enter'	  value=';.sf2;.ct Enter;.esf;.ct '.
:set symbol='g_escape'	  value=';.sf2;.ct Escape;.esf;.ct '.
