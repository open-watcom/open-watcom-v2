.****************************************************************************
.*
.*                            Open Watcom Project
.*
.*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
.*
.*  ========================================================================
.*
.*    This file contains Original Code and/or Modifications of Original
.*    Code as defined in and that are subject to the Sybase Open Watcom
.*    Public License version 1.0 (the 'License'). You may not use this file
.*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
.*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
.*    provided with the Original Code and Modifications, and is also
.*    available at www.sybase.com/developer/opensource.
.*
.*    The Original Code and all software distributed under the License are
.*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
.*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
.*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
.*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
.*    NON-INFRINGEMENT. Please see the License for the specific language
.*    governing rights and limitations under the License.
.*
.*  ========================================================================
.*
.* Description:  Root file of VI documentation.
.*
.* Date         By              Reason
.* ----         --              ------
.* 03-aug-92    Craig Eisler    initial draft
.* 02-oct-05    L. Haynes       reformatted for hlp, figures
.*
.****************************************************************************/
.chap *refid=winmenu 'Windows and Menus'
:cmt. .if &e'&dohelp eq 0 .do begin
:cmt. .   .section 'Introduction'
:cmt. .do end
.np
This chapter describes the &cmdline
commands devoted to configuring the
&edname.'s windows and menus.  All windows are fully configurable:
dimension, colors, existence of borders.  The menus are fully configurable:
all menu topics and menu items are user settable.
.np
A window is configured first by specifying a window.  Once the
a window is specified, a number of properties may be set.  These properties
are described in the following section.
.* ******************************************************************
.section 'Window Properties'
.* ******************************************************************
.np
Some of the following &cmdline
commands accept colors as parameters.
These colors may be numbers in the range 0 through 15.  As well,
&edvi has symbolic names for these colors, they are:
:UL compact.
:LI.black
:LI.blue
:LI.green
:LI.cyan
:LI.red
:LI.magenta
:LI.brown
:LI.white
:LI.dark_gray
:LI.light_blue
:LI.light_green
:LI.light_cyan
:LI.light_red
:LI.light_magenta
:LI.yellow
:LI.bright_white
:eUL
.*
.fnlist begin 'Window Properties'
.*
.begfunc BORDER
.syntx * BORDER &lt.hasbord&gt. &lt.fg_clr&gt. &lt.bg_clr&gt.
.begdescr
This command specifies the type of border and its colors.
.np
&parm1 describes the type of border: if &parm1 is set to -1,
there is no border, if &parm1 is set to 1, there is a border.
.np
&parm2 is used to specify the foreground color of the border (0-15).
It is ignored for &parm1 of -1.
.np
&parm3 is used to specify the background color of the border (0-15).
It is ignored for &parm1 of -1.
.enddescr
.xmplsect begin
.begxmpl border -1
The currently selected window has no border.
.endxmpl
.begxmpl border 1 yellow black
The currently selected window has a yellow border with a black background.
.endxmpl
.xmplsect end
.endfunc

.begfunc DIMENSION
.syntx * DIMENSION &lt.x1&gt. &lt.y1&gt. &lt.x2&gt. &lt.y2&gt.
.begdescr
(&parm1, &parm2) specifies the coordinates of the upper left-hand corner
of the window, and (&parm3, &parm4) specifies the coordinates of the
lower right-hand corner of the window.
.np
&edname editor automatically senses the number of lines and columns 
available.  The global variables %(SW) (screen width) and %(SH)
(screen height) are always set.  These are useful for coding
dimensions that are relative to the size of the screen.
All parameters may be coded as expressions.
.np
All coordinates are 0-based.  The top left corner of the
screen is (0,0).  The bottom right corner of the screen is
(%(SW)-1,%(SH)-1).
.enddescr
.xmplsect begin
.begxmpl dimension 0 1 %(SW)-1 %(SH)-3
Makes the currently selected window the full width of the screen.
Its y dimensions are from the second line of the screen and
to the third last line of the screen.
.endxmpl
.begxmpl dimension %(SW)-10 5 %(SW)-1 10
Makes the currently selected window's x dimension start at the tenth
column from the right of the screen and end at the rightmost column of the
screen. Its y dimensions range from the fifth line to the tenth line of
the screen.
.endxmpl
.xmplsect end
.endfunc

.begfunc ENDWINDOW
.syntx * ENDWindow
.begdescr
Ends entry of properties for the currently selected window.  The currently
selected window is then redrawn with the new properties, if it
was previously visible.
.enddescr
.endfunc

.begfunc HILIGHT
.syntx * HILIGHT &lt.fg_clr&gt. &lt.bg_clr&gt.
.begdescr
&parm1 sets the foreground color and &parm2 sets the background color
of highlighted text in the currently selected window.
.enddescr
.xmplsect begin
.begxmpl hilight bright_white black
Sets the highlighted color of the currently selected window to be bright white
text with a black background.
.endxmpl
.xmplsect end
.endfunc

.begfunc TEXT
.syntx TEXT &lt.fg_clr&gt. &lt.bg_clr&gt.
.begdescr
&parm1 sets the foreground color and &parm2 sets the background color
of text in the currently selected window.
.enddescr
.xmplsect begin
.begxmpl text white black
Sets the text color of the currently selected window to be white
text with a black background.
.endxmpl
.xmplsect end
.endfunc
.*
.fnlist end
.* ******************************************************************
.section 'Window Types'
.* ******************************************************************
.np
This section describes all possible windows that may be selected.  These
&cmdline commands select the window to start setting properties.
See the previous section for properties that may be set.
.*
.fnlist begin 'Window Types'
.*
.begfunc COMMANDWINDOW
.syntx * COMMANDWindow
.begdescr
This is the window that is displayed whenever &edname is prompting
for a &cmdline,
a search string, a filter command or any other command or data.
.enddescr
.endfunc

.begfunc COUNTWINDOW
.syntx * COUNTWindow
.begdescr
Window that opens when repeat counts are entered.  This window is
disabled if
.keyref repeatinfo
is not set.
.enddescr
.endfunc

.begfunc DEFAULTWINDOW
.syntx * DEFAULTWindow
.begdescr
When the
.keyword defaultwindow
is selected, all windows get the properties that are set.
Note that this is best defined first to provide the default behaviour,
and all other windows that you want to be different from the
default can be specified after.
.enddescr
.endfunc

.begfunc DIRWINDOW
.syntx * DIRWindow
.begdescr
This is the window that you select a file from whenever no file
is specified from the &cmdline
commands
.keyref edit
or
.keyref read
:period.
.enddescr
.endfunc

.begfunc EDITWINDOW
.syntx * EDITWindow
.begdescr
This is the window that files are edited from.
.enddescr
.endfunc

.begfunc EXTRAINFOWINDOW
.syntx * EXTRAINFOWindow
.begdescr
This window contains extra information about possible things that
may be done.
It is displayed when selecting results from the &cmdline commands
.keyref fgrep
:cont.,
.keyref egrep
:cont.,
and
.keyref files
:period.
.enddescr
.endfunc

.begfunc FILEWINDOW
.syntx * FILEWindow
.begdescr
The window in which list of files currently being edited is displayed.
This is displayed when the &cmdline command
.keyref files
is executed.
.enddescr
.endfunc

.begfunc FILECWINDOW
.syntx * FILECWindow
.begdescr
This window contains list of possible file choices when
file completion cannot match one specific file.  It is displayed
whenever the
.keyword TAB
key is pressed in a command window.
.enddescr
.endfunc

.begfunc LINENUMBERWINDOW
.syntx * LINENUMBERWindow
.begdescr
This is the window that line numbers are displayed in.
The dimension of this window is ignored, the absolute position of the window
is decided by the position of the edit window that it is associated with. 
:cmt. However, the width of this window may be set using the
:cmt. .keyref linenumwidth
:cmt. setting.
.np
This window is disabled if
.keyref linenumbers
is not set.
.enddescr
.endfunc

.begfunc MENUWINDOW
.syntx * MENUWindow
.begdescr
This sets the properties of the windows that open whenever
a menu is activated.  Any
.keyref dimension
given with this window is ignored; the position of each
menu is variable.
.enddescr
.endfunc

.begfunc MENUBARWINDOW
.syntx * MENUBARWindow
.begdescr
This window is the menu bar.
This is where all menus set up with the &cmdline
.keyref menu
are displayed.
.enddescr
.endfunc

.begfunc MESSAGEWINDOW
.syntx * MESSAGEWindow
.begdescr
The window in which all all editor feedback is reported. 
The
.keyref hilight
color is the color that errors are reported in.
.np
This window needs to have two lines in order to view
&edname feedback;  however, very few messages use two lines,
so you can get by with a message window that is only one line high.
.enddescr
.endfunc

.begfunc SETWINDOW
.syntx * SETWindow
.begdescr
The window in which &edname settings are displayed.
This window is displayed when the &cmdline
.keyref set
command is entered without parameters or with
.param 2
as its only parameter.
.enddescr
.endfunc

.begfunc SETVALWINDOW
.syntx * SETVALWindow
.begdescr
The window in which the new value of an editor setting is entered.
This window is displayed if a value change is requested
after entering the &cmdline
.keyref set
with
.param 2
as the parameter.
.enddescr
.endfunc

.begfunc STATUSWINDOW
.syntx * STATUSWindow
.begdescr
This is the window where the current line and column are reported.
The current line number is displayed in the first line of the window,
and the current column is displayed in the second line of the window.
This window is disabled if
.keyref statusinfo
is not set.
.enddescr
.endfunc
.*
.fnlist end
.* ******************************************************************
.section 'Sample Window Settings'
.* ******************************************************************
.np
The following examples are sample settings of the different types of
windows. These commands may be issued from the &cmdline
one at at time, or may be executed from a &edname script.
Typically, these commands will be found in the &edname
.keyword configuration script
:period.

.np
.keyword Commandwindow
:cont.:
.millust begin
commandwindow
    dimension 2 %(SH)-7 %(SW)-3 %(SH)-5
    text %(white) %(blud) 0
    border 1 7 1
    hilight %(highwhite) %(cyan) 1
endwindow
.millust end
    
.np
.keyword Countwindow
:cont.:
.millust begin
countwindow
    dimension 28 %(SH)-5 43 %(SH)-3
    border 1 7 1
    text %(white) %(blue) 0
    hilight %(highwhite) %(cyan) 1
endwindow
.millust end
    
.np
.keyword Dirwindow
:cont.:
.millust begin
dirwindow
    dimension 15 2 %(SW)-12 %(SH)-7
    border 1 7 1
    text %(white) %(blue) 0
    hilight %(highwhite) %(cyan) 1
endwindow
.millust end

.np
.keyword Editwindow
:cont.:
.millust begin
editwindow
    dimension 0 1 %(SW)-1 %(SH)-2
    border 1 %(white) %(black)
    text %(white) %(black) 0
    hilight %(yellow) %(blue) 0
    whitespace %(white) %(black) 0
    selection %(yellow) %(blue) 0
    eoftext %(white) %(black) 0
    keyword %(highwhite) %(black) 0
    octal %(cyan) %(black) 0
    hex %(cyan) %(black) 0
    integer %(cyan) %(black) 0
    char %(cyan) %(black) 0
    preprocessor %(yellow) %(black) 0
    symbol %(white) %(black) 0
    invalidtext %(yellow) %(black) 0
    identifier %(white) %(black) 0
    jumplabel %(cyan) %(black) 0
    comment %(lightcyan) %(black) 0
    float %(cyan) %(black) 0
    string %(cyan) %(black) 0
endwindow
.millust end

.np
.keyword Extrainfowindow
:cont.:
.millust begin
extrainfowindow
    dimension 0 1 %(SW)-1 %(SH)-3
    border 1 7 1
    text %(white) %(blue) 0
    hilight %(purple) %(cyan) 1
endwindow
.millust end
    
.keyword Filecwindow
:cont.:
.millust begin
filecwindow
    dimension 4 7 %(SW)-5 %(SH)-9
    border 1 7 1
    text %(white) %(blue) 0
    hilight %(highwhite) %(cyan) 1
endwindow
.millust end

.np
.keyword Filewindow
:cont.:
.millust begin
filewindow
    dimension 26 2 %(SW)-2 %(SH)-7
    border 1 7 1
    text %(white) %(blue) 0
    hilight %(highwhite) %(cyan) 1
endwindow
.millust end

.np
.keyword Messagewindow
:cont.:
.millust begin
messagewindow
    dimension 18 %(SH)-1 %(SW)-1 %(SH)-1
    border -1
    text %(white) %(blue) 0
    hilight %(yellow) %(blue) 1
endwindow
.millust end

.np
.keyword Menuwindow
:cont.:
.millust begin
menuwindow
    dimension 0 1 %(SW)-1 %(SH)-3
    border 1 14 1
    text %(highwhite) %(blue) 0
    hilight %(yellow) %(blue) 1
endwindow
.millust end
    
.np
.keyword Menubarwindow
:cont.:
.millust begin
menubarwindow
    dimension 0 0 %(SW)-1 0
    border -1
    text %(highwhite) %(blue) 0
    hilight %(yellow) %(blue) 1
endwindow
.millust end

.keyword Setvalwindow
:cont.:
.millust begin
setvalwindow
    dimension 46 6 %(SW)-7 9
    border 1 7 1
    text %(white) %(blue) 0
    hilight %(highwhite) %(cyan) 1
endwindow
.millust end
    
.np
.keyword Setwindow
:cont.:
.millust begin
setwindow
    dimension 12 2 43 %(SH)-4
    border 1 7 1
    text %(white) %(blue) 0
    hilight %(highwhite) %(cyan) 1
endwindow
.millust end
    
.np
.keyword Statuswindow
:cont.:
.millust begin
statuswindow
    dimension 0 %(SH)-1 17 %(SH)-1
    border -1
    text %(white) %(blue) 0
    hilight %(yellow) %(blue) 1
endwindow
.millust end
.* ******************************************************************
.section *refid=menucmd 'Menu Commands'
.* ******************************************************************
.np
&edvi menus are set dynamically.  Any menu will
automatically be added to the menu bar when created, and removed when
destroyed.  There are some reserved menus:
:DL.
:DT.windowgadget
:DD.This menu is the one that appears when the upper left hand
corner of an edit window is clicked.
:DT.float&lt.0-3&gt.
:DD.These are floating (popup) menus.  They are made to appear when
the script command
.keyref floatmenu
is used.
There are 4 floating menus,
.param float0
through
.param float3
:period.
:eDL.
.np
Menus will attempt to open up where they are told to; however, if the menu
cannot fit, then it will try to open above the position that it was told
to, to the left of the position that it was told to, and above and to the
left of the position that it was told to.
.np
The following sections describe each of the &cmdline
commands for controlling menus.
.*
.fnlist begin 'Menu Commands'
.*
.begfunc ADDMENUITEM
.syntx * ADDMENUItem &lt.menuname&gt. &lt.itemname&gt. &lt.cmd&gt.
.begdescr
Adds a new item to a previously created menu &parm1.
:period.
The item &parm2 is added to the menu.
.np
This command is similar to the &cmdline command
.keyref menuitem
:cont., only the
.keyref menuitem
command is used when first defining a menu.
.np
&parm1 may be a quoted string, if the parameter
contains spaces. If &parm1 is specified as "", then
a solid bar is displayed in the menu.
.np
A character in &parm2 preceded with an '&' will be the hot key
for activating the menu, e.g. &Control would have 'C' as
the hot key.
.np
The command &parm3 may be any &edname &cmdline
command, and is run whenever the item is selected.
.enddescr
.alsosee begin
.seethis menuitem
.alsosee end
.endfunc

.begfunc DELETEMENU
.syntx * DELETEMENU &lt.menuname&gt.
.begdescr
Destroys menu with name &parm1.
:period.
.enddescr
.endfunc

.begfunc DELETEMENUITEM
.syntx * DELETEMENUItem &lt.menuname&gt. &lt.index&gt.
.begdescr
Deletes item number &parm2 from menu &parm1.
:period.
The item number &parm2
is 0 based.
.np
If &parm2 is specified as -1, then the last item
is removed from the menu.
.enddescr
.endfunc

.begfunc ENDMENU
.syntx * ENDMENU
.begdescr
Finishes the creation of a new menu.
.enddescr
.alsosee begin
.seethis menu
.alsosee end
.endfunc

.begfunc MENU
.syntx * MENU &lt.menuname&gt.
.begdescr
Starts the creation of a new menu &parm1.
:period.
If a menu exists with the name already, it is destroyed and re-created.
A character preceded with an ampersand ('&') will be the hot key
for activating the menu, e.g. &Control would have 'C' as
the hot key.
.np
&parm1 may be a reserved name: windowgadget, float0,
float1, float2, or float3
.enddescr
.alsosee begin
.seethis endmenu
.seethis menuitem
.alsosee end
.endfunc

.begfunc MENUITEM
.syntx * MENUITEM &lt.itemname&gt. &lt.cmd&gt.
.begdescr
Adds the item &parm1 to the last menu started with the
.keyref menu
command.   Menu items may be added until the &cmdline command
.keyref endmenu
has been issued.
.np
&parm1 may be a quoted string, if the parameter
contains spaces. If &parm1 is specified as "", then
a solid bar is displayed in the menu.
.np
A character in &parm1 preceded
with an ampersand ('&') will be the hot key for activating the menu
item, e.g. &Exit would have 'E' as the hot key.
.np
The command &parm2 may be any &edname &cmdline
command, and is run whenever the item is selected.
.enddescr
.alsosee begin
.seethis addmenuitem
.seethis endmenu
.seethis menu
.alsosee end
.endfunc
.*
.fnlist end
.* ******************************************************************
.section 'Sample Menus'
.* ******************************************************************
.np
The following examples are sample menu setups.
These commands may be issued from the &cmdline
one at at time, or may be contained in a
.keyword script
and executed in the script.
Typically, these commands will be found in the &edname
.keyword configuration script
:period.
.np
This configures the menu that is popped up whenever the top left-hand
corner of an edit window is clicked with the mouse.
.millust begin
menu windowgadget
    menuitem "&$AMP.Maximize" maximize
    menuitem "M&$AMP.inimize" minimize
    menuitem ""
    menuitem "&$AMP.Open another view" open
    menuitem ""
    menuitem "&$AMP.Save" write
    menuitem "&$AMP.Save &$AMP. close" keyadd ZZ
    menuitem "Close &$AMP.no save" quit!
    menuitem "&$AMP.Close" quit
endmenu
.millust end
.np
This configures one of the floating (popup) menus.  This menu could
be displayed whenever a word is selected with the mouse.
.millust begin
menu float0
    menuitem "&$AMP.Open" edit %1
    menuitem ""
    menuitem "&$AMP.Change" keyadd cr
    menuitem "&$AMP.Delete" keyadd dr
    menuitem "&$AMP.Yank" keyadd yr
    menuitem ""
    menuitem "&$AMP.Fgrep" fgrep "%1"
    menuitem "&$AMP.Tag" tag %1
endmenu
.millust end

.np
This configures a menu bar menu item called File.
.millust begin
menu &$AMP.File
    menuitem "&$AMP.Open new file ..." edit
    menuitem "&$AMP.Next file" next
    menuitem "&$AMP.Read file ..." read
    menuitem "&$AMP.File list ..." file
    menuitem ""
    menuitem "&$AMP.Save current file" write
    menuitem "Save &$AMP.current file & close" wq
    menuitem ""
    menuitem "En&$AMP.ter command ..." keyadd \&lt.CTRL_K&gt.
    menuitem "S&$AMP.ystem" shell
    menuitem ""
    menuitem "E&$AMP.xit" exitall
endmenu
.millust end

.np
This configures a menu bar menu item called Edit.
.millust begin
menu &$AMP.Edit
    menuitem "&$AMP.Delete region" keyadd \&lt.SHIFT_DEL&gt.
    menuitem "&$AMP.Copy (yank) region" keyadd yr
    menuitem "&$AMP.Paste (put)" put
    menuitem ""
    menuitem "&$AMP.Insert Text" keyadd i
    menuitem "&$AMP.Overstrike Text" keyadd R
    menuitem ""
    menuitem "&$AMP.Undo" undo
    menuitem "&$AMP.Redo" undo!
endmenu
.millust end

.np
This configures a menu bar menu item called Position.
.millust begin
menu &$AMP.Position
    menuitem "&$AMP.Start of file" 1
    menuitem "&$AMP.End of file" $
    menuitem "Line &$AMP.number" so lnum._vi
    menuitem ""
    menuitem "S&$AMP.tart of line" keyadd 0
    menuitem "En&$AMP.d of line" keyadd $
    menuitem ""
    menuitem "Search &$AMP.forwards" keyadd /
    menuitem "Search &$AMP.backwards" keyadd ?
    menuitem "&$AMP.Last search" keyadd n
    menuitem "&$AMP.Reverse last search" keyadd N
endmenu
.millust end

.np
This configures a menu bar menu item called Window.
.millust begin
menu &$AMP.Window
    menuitem "&$AMP.Tile windows" tile
    menuitem "&$AMP.Cascade windows" cascade
    menuitem "&$AMP.Reset windows" tile 1 1
    menuwindowlist
endmenu
.millust end

.np
This configures a menu bar menu item called Options.
.millust begin
menu &$AMP.Options
    menuitem "&$AMP.Settings ..." set
endmenu
.millust end

.np
This configures a menu bar menu item called Help.
.millust begin
menu &$AMP.Help
    menuitem "&$AMP.Command Line" help com
    menuitem "&$AMP.Key Strokes" help keys
    menuitem "&$AMP.Regular Expressions" help reg
    menuitem "&$AMP.Scripts" help scr
    menuitem "S&$AMP.ettings" help set
    menuitem "Starting &$AMP.Up" help start
endmenu
.millust end

