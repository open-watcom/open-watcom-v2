:cmt.****************************************************************
:cmt. This file contains symbol definitions for the
:cmt. WATCOM Student Workbench documentation
:cmt.************************************************************


:cmt. ***************************************************************
:cmt. Names of things pertaining to the workbench
:cmt.    & wbench        - name of workbench
:cmt.    & wb            - shortened name of workbench
:cmt.    & uidoc         - name of User Interface Manual
:cmt.    & wbtop         - name of workbench screen - the bench top
:cmt.    - names of all software packages and manuals
:cmt. ***************************************************************

:set symbol='wbench'    value='WATCOM Information Workbench'.
:set symbol='wbdocp'    value='&wbench.: '.
:set symbol='wb'        value='InfoBench'.
:set symbol='wbtop'     value='Benchtop'.
:set symbol='shell'     value='Waterloo Application Shell'.
:set symbol='ss'        value='WATCOM Spreadsheet'.
:set symbol='acc'       value='WATCOM Accounting System'.
:set symbol='watfile'   value='WATFILE'.
:set symbol='wgml'      value='WATCOM GML'.
:set symbol='wbg'       value='WATCOM Business Graphics Subsystem'.
:set symbol='c'         value='WATCOM C 7.0'.
:set symbol='express'   value='WATCOM Express C'.
:set symbol='wp'        value='Waterloo Word Processor'.
:set symbol='janet'     value='Waterloo JANET'.
:set symbol='pacedb'    value='PACEBase'.
:set symbol='isql'      value='Interactive SQL'.
:set symbol='acme'      value='Application Creation Made Easy'.
:set symbol='wed'       value="WATCOM InfoBench Editor".
:set symbol='ed'        value="InfoBench Editor".
:set symbol='chcomp'    value="Chart Composer".

:cmt.Manual Names

.dm q begin
.ct :Q.
.dm q end

.dm eq begin
.ct :eQ.
.dm eq end

:set symbol='uidoc'     value=';.q;.ct &wbdocp.Getting Started;.eq;.ct '.
:set symbol='shelldoc'
     value='&wbdocp.Waterloo Application Shell Reference Guide'.
:set symbol='sstut'     value='&wbdocp.Spreadsheet Tutorial'
:set symbol='ssref'     value='&wbdocp.Spreadsheet Reference Manual'
:set symbol='accdoc'    value='&wbdocp.Accounting System'.
:set symbol='wftut'     value='&wbdocp.WATFILE Tutorial'.
:set symbol='wfref'     value='WATFILE Tutorial and Reference'.
:set symbol='gmldoc'    value='&wbdocp.GML'.
:set symbol='wpdoc'     value="&wbdocp.Waterloo Word Processor User's Guide".
:set symbol='janetdoc'  value="&wbdocp.Waterloo JANET Network User's Guide".
:set symbol='sqldoc'
     value='&wbdocp.PACEBase SQL Database System Tutorial and Reference'.
:set symbol='acmedoc'
     value='&wbdocp.Application Creation Made Easy Tutorial and Reference'.
:set symbol='wbgdoc' value='&wbdocp.Business Graphics Tutorial and Reference'.
:set symbol='weddoc' value='&wbdocp.Editor and Configuration'.
:set symbol='utldoc' value='&wbdocp.Support Utilities'.
:set symbol='dosdoc' value='DOS technical manuals'.


:cmt. ***************************************************************
:cmt. Highlighting
:cmt.   - various macros for highlighting text within a document
:cmt.   - each macro corresponds to a kind of text
:cmt.     1. keyboard keys              & key.a& ekey.
:cmt.     2. program keywords           & keyword.select& ekeyword.
:cmt.     3. terms                      & term.mouse& eterm.
:cmt.     4. program objects            & name.C1& ename.
:cmt.     5. values                     & value.25& evalue.
:cmt.     5. special symbols            & sym.a& esym.     (alpha)
:cmt. ***************************************************************

.dm sf1 begin
.ct :SF font=1.
.dm sf1 end

.dm sf2 begin
.ct :SF font=2.
.dm sf2 end

.dm sf4 begin
.ct :SF font=4.
.dm sf4 end

.dm sf7 begin
.ct :SF font=7.
.dm sf7 end

.dm esf begin
.ct :eSF.
.dm esf end

.dm evalue begin
.ct ":eSF.
.dm evalue end

:cmt. keyboard keys
:cmt.   eg. Enter, Esc, F1, a, etc.
:cmt.   - see also key macros later on in this file
    :set symbol='key'           value=';.sf2;.ct '.
    :set symbol='ekey'          value=';.esf;.ct '.

:cmt. program keywords
:cmt.   eg. combine, select, etc.
    :set symbol='keyword'       value=';.sf4;.ct '.
    :set symbol='ekeyword'      value=';.esf;.ct '.

:cmt. new terms
:cmt.   eg.
    :set symbol='term'          value=';.sf1;.ct '.
    :set symbol='eterm'         value=';.esf;.ct '.

:cmt. names of program objects
:cmt.  eg. spread sheet cell names such as A1, file names etc.
    :set symbol='name'          value=';.sf2;.ct '.
    :set symbol='ename'         value=';.esf;.ct '.

:cmt. values
:cmt.  eg. spread sheet cell values, watfile field values, numbers, etc.
    :set symbol='value'         value=';.sf4;.ct "'.
    :set symbol='evalue'        value=';.evalue;.ct '.

:cmt. special symbol font
:cmt.  eg. arrow keys, greek letters, etc.
    :set symbol='sym'           value=';.sf7;.ct '.
    :set symbol='esym'          value=';.esf;.ct '.


:cmt. ***************************************************************
:cmt. Special symbol definitions
:cmt.   - includes copyright symbol, etc
:cmt. ***************************************************************

:set symbol='copyr'     value=';.sf7;.ct \C;.esf;.ct '.


:cmt. ***************************************************************
:cmt. Symbols for mouse actions and the like
:cmt.   - these were distilled from Don Cowans script macros
:cmt.     for the Watfile manual
:cmt. ***************************************************************

:cmt. SYMBOLS FOR KEYBOARD AND SCREEN
:cmt. POINTING DEVICE
    :set symbol='ptr' value='mouse'.
:cmt. POINTER OR CURSOR
    :set symbol='pt' value='pointer'.
:cmt. MOVEMENT OF BUTTON ON POINTING DEVICE
    :set symbol='op' value='click'.
:cmt. BUTTON ON POINTING DEVICE
    :set symbol='bt' value='button'.
:cmt. MOTION OF MOVING POINTER OR CURSOR WITH POINTING DEVICE
    :set symbol='dr' value='drag'.
    :set symbol='udr' value='Drag'.
    :set symbol='drn' value='&dr.ging'.
:cmt. LIST OF ITEMS AT TOP OF SCREEN
    :set symbol='mb' value='menu bar'.
:cmt. MENU CREATED BY ACTIVATING ITEM ON MENU BAR
    :set symbol='mn' value='pull-down menu'.
:cmt. BOX CREATED BY ACTIVATING ITEM ON PULL-DOWN MENU
    :set symbol='dbx' value='dialogue box'.
:cmt. RECTANGULAR AREA IN DIALOGUE BOX
    :set symbol='bx' value='box'.

:cmt. ***************************************************************
:cmt. Keyboard key macros
:cmt.   - various macros for keyboard keys
:cmt.   - includes arrow keys, function keys, enter, escape,
:cmt.     pgup, etc
:cmt. ***************************************************************

:set symbol='crsup'     value=';.sf7;.ct \U;.esf;.ct '.
:set symbol='crsdn'     value=';.sf7;.ct \D;.esf;.ct '.
:set symbol='crslt'     value=';.sf7;.ct \L;.esf;.ct '.
:set symbol='crsrt'     value=';.sf7;.ct \R;.esf;.ct '.
:set symbol='bksp'      value=';.sf7;.ct \L;.esf;.ct '.

:set symbol='enter'     value=';.sf2;.ct Enter;.esf;.ct '.
:set symbol='tab'       value=';.sf2;.ct Tab;.esf;.ct '.
:set symbol='backtab'   value=';.sf2;.ct Backtab;.esf;.ct '.
:set symbol='rubout'    value=';.sf2;.ct Backspace;.esf;.ct '.
:set symbol='backsp'    value=';.sf2;.ct Backspace;.esf;.ct '.
:set symbol='esc'       value=';.sf2;.ct Esc;.esf;.ct '.
:set symbol='alt'       value=';.sf2;.ct Alt;.esf;.ct '.
:set symbol='ins'       value=';.sf2;.ct Ins;.esf;.ct '.
:set symbol='del'       value=';.sf2;.ct Del;.esf;.ct '.
:set symbol='home'      value=';.sf2;.ct Home;.esf;.ct '.
:set symbol='ctrl'      value=';.sf2;.ct Ctrl;.esf;.ct '.
:set symbol='end'       value=';.sf2;.ct End;.esf;.ct '.
:set symbol='pgup'      value=';.sf2;.ct PgUp;.esf;.ct '.
:set symbol='pgdn'      value=';.sf2;.ct PgDn;.esf;.ct '.
:set symbol='break'     value=';.sf2;.ct Break;.esf;.ct '.
:set symbol='space'     value=';.sf2;.ct Space;.esf;.ct '.
:set symbol='spacebar'  value=';.sf2;.ct SpaceBar;.esf;.ct '.

:set symbol='f1'        value=';.sf2;.ct F1;.esf;.ct '.
:set symbol='f2'        value=';.sf2;.ct F2;.esf;.ct '.
:set symbol='f3'        value=';.sf2;.ct F3;.esf;.ct '.
:set symbol='f4'        value=';.sf2;.ct F4;.esf;.ct '.
:set symbol='f5'        value=';.sf2;.ct F5;.esf;.ct '.
:set symbol='f6'        value=';.sf2;.ct F6;.esf;.ct '.
:set symbol='f7'        value=';.sf2;.ct F7;.esf;.ct '.
:set symbol='f8'        value=';.sf2;.ct F8;.esf;.ct '.
:set symbol='f9'        value=';.sf2;.ct F9;.esf;.ct '.
:set symbol='f10'       value=';.sf2;.ct F10;.esf;.ct '.
:set symbol='f11'       value=';.sf2;.ct F11;.esf;.ct '.
:set symbol='f12'       value=';.sf2;.ct F12;.esf;.ct '.

:cmt. ***************************************************************
:cmt. Generic enter and escape symbols
:cmt. ***************************************************************

:set symbol='g_enter'     value=';.sf2;.ct Enter;.esf;.ct '.
:set symbol='g_escape'    value=';.sf2;.ct Escape;.esf;.ct '.

:cmt. ***************************************************************
:cmt. Tag overrides
:cmt. ***************************************************************

.dm gml$toc begin
.pa odd
:TOC.
.dm gml$toc end

.dm gml$figl begin
.pa odd
:FIGLIST.
.dm gml$figl end

.dm gml$appx begin
:H0.Appendices
:APPENDIX.
.dm gml$appx end

.if &$macpass. eq 'ugh' .do begin
.gt TOC delete
.gt FIGLIST delete
.gt APPENDIX delete
.do end

.gt TOC add gml$toc texterror tagnext
.gt FIGLIST add gml$figl texterror tagnext
.gt APPENDIX add gml$appx texterror tagnext

:CMT. the following symbol set is too get around a macro defining problem
:SET symbol='$macpass' value='ugh'.
