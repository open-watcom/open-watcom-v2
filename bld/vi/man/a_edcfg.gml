.*
.* (c) Copyright 1992 by WATCOM International Corp.
.*
.* All rights reserved. No part of this publication may be reproduced or
.* used in any form or by any means - graphic, electronic, or mechanical,
.* including photocopying, recording, taping or information storage and
.* retrieval systems - without written permission of WATCOM Publications
.* Limited.
.*
.* Date		By		Reason
.* ----		--		------
.* 18-aug-92	Craig Eisler	initial draft
.*
:CHAPTER.Inside ED.CFG
This appendix shows the configuration script that is provided with &edname.
:PERIOD.
Each section of the script is explained.
:P.
:SEXMP.
:SELINE.#
:SELINE.# color names
:SELINE.#
:SELINE.assign %(black) = 0
:SELINE.assign %(blue) = 1
:SELINE.assign %(green) = 2
:SELINE.assign %(pale_blue) = 3
:SELINE.assign %(red) = 4
:SELINE.assign %(pale_green) = 5
:SELINE.assign %(pink) = 6
:SELINE.assign %(white) = 7
:SELINE.if %(OS) == dos
:SELINE.assign %(Brw) = 8
:SELINE.else
:SELINE.assign %(Brw) = 15
:SELINE.endif
:SELINE.assign %(orange) = 9
:SELINE.assign %(tan) = 10
:SELINE.assign %(bright_cyan) = 11
:SELINE.assign %(gold) = 12
:SELINE.assign %(light_magenta) = 13
:SELINE.assign %(yellow) = 14
:SELINE.assign %(text_white) = 15
:SELINE.
:SELINE.assign %(Rdhook) = rdme._vi
:SELINE.assign %(Wrhook) = wrme._vi
:SELINE.assign %(Modhook) = rcs._vi
:SELINE.assign %(MCselhook) = mcsel._vi
:SELINE.assign %(MLselhook) = mlsel._vi
:SELINE.load rdme._vi
:SELINE.load wrme._vi
:SELINE.load rcs._vi
:SELINE.
:SELINE.#
:SELINE.# aliases defined here
:SELINE.#
:SELINE.#alias wq x
:SELINE.alias untile tile 1 1
:SELINE.abbrev wh while(
:SELINE.abbrev fo for(
:SELINE.
:SELINE.#
:SELINE.# menus
:SELINE.#
:SELINE.menu windowgadget
:SELINE.    menuitem &Maximize maximize
:SELINE.    menuitem M&inimize minimize
:SELINE.    menuitem ""
:SELINE.    menuitem "&Save & close" keyadd ZZ
:SELINE.    menuitem "Close &no save" quit!
:SELINE.    menuitem &Close quit
:SELINE.endmenu
:SELINE.menu &Control
:SELINE.    menuitem "&Tile windows" tile
:SELINE.    menuitem "&Cascade windows" cascade
:SELINE.    menuitem "&Reset windows" tile 1 1
:SELINE.    menuitem ""
:SELINE.    menuitem "Set &booleans" set
:SELINE.    menuitem "Set &other" set 2
:SELINE.    menuitem ""
:SELINE.    menuitem "&Enter command" keyadd :
:SELINE.    menuitem &System shell
:SELINE.    menuitem E&xit quitall
:SELINE.endmenu
:SELINE.menu &File
:SELINE.    menuitem "&File list" file
:SELINE.    menuitem "&Next file" next
:SELINE.    menuitem "&Edit new file" edit
:SELINE.    menuitem "&Read file" read
:SELINE.    menuitem "&Write current" write
:SELINE.    menuitem "&Save and exit" keyadd ZZ
:SELINE.endmenu
:SELINE.menu &Edit
:SELINE.    menuitem "&Start region" keyadd 
:SELINE.    menuitem "&Delete region" keyadd dr
:SELINE.    menuitem "&Copy (yank) region" keyadd yr
:SELINE.    menuitem "&Paste (put)" put
:SELINE.    menuitem ""
:SELINE.    menuitem "&Insert Text" keyadd i
:SELINE.    menuitem "&Overstrike Text" keyadd R
:SELINE.    menuitem ""
:SELINE.    menuitem &Undo undo
:SELINE.    menuitem &Redo undo!
:SELINE.endmenu
:SELINE.menu &Position
:SELINE.    menuitem "&Start of file" 1
:SELINE.    menuitem "&End of file" $
:SELINE.    menuitem "Line &number" so lnum._vi
:SELINE.    menuitem ""
:SELINE.    menuitem "S&tart of line" keyadd 0
:SELINE.    menuitem "En&d of line" keyadd $
:SELINE.    menuitem ""
:SELINE.    menuitem "Search &forwards" keyadd /
:SELINE.    menuitem "Search &backwards" keyadd ?
:SELINE.    menuitem "&Last search" keyadd n
:SELINE.    menuitem "&Reverse last search" keyadd N
:SELINE.endmenu
:SELINE.menu &Help
:SELINE.    menuitem "&Command Line" help com
:SELINE.    menuitem "&Key Strokes" help keys
:SELINE.    menuitem "&Regular Expressions" help reg
:SELINE.    menuitem "&Scripts" help scr
:SELINE.    menuitem "Starting &Up" help start
:SELINE.endmenu
:SELINE.
:SELINE.#
:SELINE.# parameters
:SELINE.#
:SELINE.if %(OS) == unix 
:SELINE.    set historyfile = /tmp/vi.his
:SELINE.    set tmpdir = /tmp
:SELINE.    set nowritecrlf
:SELINE.else
:SELINE.    set historyfile = c:\vi.his
:SELINE.    set tmpdir = d:\tmp
:SELINE.endif
:SELINE.
:SELINE.set autosaveinterval = 30
:SELINE.
:SELINE.#set word = __09AZaz
:SELINE.#set wordalt = \\..__09AZaz
:SELINE.
:SELINE.set maxswapk = 8192
:SELINE.#set maxemsk = 1024
:SELINE.#set maxxmsk = 1024
:SELINE.if %(OS386) == 1
:SELINE.    set stackk=40
:SELINE.else
:SELINE.    set stackk=12
:SELINE.endif
:SELINE.set maxlinelen = 512
:SELINE.
:SELINE.set maxclhistory = 20, maxfindhistory = 10
:SELINE.if %(OS) != nt
:SELINE.    set insertcursortype= 50, commandcursortype= 93
:SELINE.else
:SELINE.    set insertcursortype= 70, commandcursortype= 100
:SELINE.endif
:SELINE.
:SELINE.set realtabs
:SELINE.
:SELINE.set magic
:SELINE.set magicstring =()
:SELINE.
:SELINE.set shiftwidth=4, tabamount=8, hardtab=8
:SELINE.
:SELINE.set currentstatuscolumn = %(SW)-25
:SELINE.
:SELINE.#
:SELINE.# flags
:SELINE.#
:SELINE.set readentirefile, statusinfo
:SELINE.set escapemessage,cmode
:SELINE.set verbose, beepflag, autoindent, extendedmemory
:SELINE.set caseignore, searchwrap, currentstatus
:SELINE.
:SELINE.#
:SELINE.# stuff to ignore when using command completion
:SELINE.#
:SELINE.#set fignore = com, fignore= obj, fignore= exe
:SELINE.
:SELINE.#
:SELINE.# some mouse/windows stuff
:SELINE.#
:SELINE.#set doubleclickticks = 7
:SELINE.#set mousespeed = 4
:SELINE.
:SELINE.set maxwindowtilex = 3, maxwindowtiley = 2
:SELINE.set inactivewindowcolor %(white)
:SELINE.
:SELINE.#
:SELINE.# extra things to match with '%'
:SELINE.#
:SELINE.match /\/\*/\*\//
:SELINE.match /# *if/# *endif/
:SELINE.
:SELINE.#
:SELINE.# key mappings
:SELINE.#
:SELINE.map SHIFT_F1 \x\e:\hso proc._vi\n
:SELINE.map SHIFT_F5 \<CTRL_F1>
:SELINE.map SHIFT_F6 \<CTRL_F2>
:SELINE.map SHIFT_F7 \<CTRL_F3>
:SELINE.map SHIFT_F8 \<CTRL_F4>
:SELINE.map SHIFT_F9 \<CTRL_F5>
:SELINE.map SHIFT_F10 \<CTRL_F6>
:SELINE.map SHIFT_F11 \<CTRL_F7>
:SELINE.map SHIFT_F12 \<CTRL_F8>
:SELINE.map F3 \x:\hso err._vi\n
:SELINE.map F6 \x:so qall._vi\n
:SELINE.#map F7 \eyypj:.,.s/./*/g\n0r/$r/o
:SELINE.map V \x:\hf\n
:SELINE.map F7 \x:\hf\n
:SELINE.map F9 \x:\hso chkout._vi\n
:SELINE.map F10 \x:\hso forceout._vi\n
:SELINE.map ALT_F9 \x:\hso unlock._vi\n
:SELINE.map CTRL_T \x:\hda\n
:SELINE.map CTRL_W \x:fgrep \<CTRL_W>\n
:SELINE.map K      \x:\hnext\n
:SELINE.
:SELINE.map! F1 \e\<F1>g
:SELINE.map! F2 \e\<F2>g
:SELINE.map! F7 \e\x:\hso fin._vi\n
:SELINE.map! SHIFT_F1 \x\e:so proc._vi\n
:SELINE.map! CTRL_W \edwi
:SELINE.map! CTRL_X \ecw
:SELINE.
:SELINE.#
:SELINE.# line numbering setup
:SELINE.#
:SELINE.#set linenumbers
:SELINE.#set linenumsonright
:SELINE.#set linenumwinwidth = 6
:SELINE.#linenumberwindow
:SELINE.#    border 1 %(Brw) %(pale_blue)
:SELINE.#    text %(blue) %(pale_blue)
:SELINE.#endwindow
:SELINE.
:SELINE.#
:SELINE.# window definitions follow
:SELINE.#
:SELINE.set clockx = %(SW)-9, clocky = 0
:SELINE.set spinx= %(SW)-15, spiny = 0
:SELINE.
:SELINE.#
:SELINE.# COLOR CONFIGURATION
:SELINE.#
:SELINE.if config == 100
:SELINE.
:SELINE.#    set maxtilecolors = 4
:SELINE.#    set tilecolor 0 %(text_white) %(blue)
:SELINE.#    set tilecolor 1 %(text_white) %(pink)
:SELINE.#    set tilecolor 2 %(text_white) %(pale_green)
:SELINE.#    set tilecolor 3 %(text_white) %(black)
:SELINE.    
:SELINE.    setcolor %(blue) 0 0 29
:SELINE.    setcolor %(text_white) 52 52 52
:SELINE.    setcolor %(Brw) 63 63 63
:SELINE.    setcolor %(bright_cyan) 0 62 57
:SELINE.    setcolor %(pink) 58 0 33
:SELINE.    #setcolor %(tan) 48 40 23
:SELINE.    #setcolor %(gold) 63 42 0
:SELINE.    setcolor %(pale_blue) 0 36 48
:SELINE.    setcolor %(pale_green) 25 40 25
:SELINE.    #setcolor %(orange) 35 00 00
:SELINE.    setcolor %(yellow) 63 63 0
:SELINE.     
:SELINE.    set resizecolor = %(yellow)
:SELINE.    set movecolor = %(pink)
:SELINE.    
:SELINE.    editwindow
:SELINE.       border 1 %(bright_cyan) %(blue)
:SELINE.       dimension 0 1 %(SW)-1 %(SH)-3
:SELINE.       text %(text_white) %(blue)
:SELINE.if %(OS) == dos
:SELINE.        hilight %(pink) %(blue)
:SELINE.endif
:SELINE.    endwindow
:SELINE.    
:SELINE.    extrainfowindow
:SELINE.        border 1 %(Brw) %(pale_green)
:SELINE.        text %(Brw) %(pale_green)
:SELINE.    endwindow
:SELINE.    
:SELINE.    filecwindow
:SELINE.       border 1 %(yellow) %(white)
:SELINE.       dimension 4 8 %(SW)-5 %(SH)-8
:SELINE.       text %(black) %(white)
:SELINE.    endwindow
:SELINE.    
:SELINE.    countwindow
:SELINE.        dimension 28 %(SH)-5 43 %(SH)-3
:SELINE.        border 1 %(yellow) %(red)
:SELINE.        text %(Brw) %(red)
:SELINE.    endwindow
:SELINE.    
:SELINE.    commandwindow
:SELINE.if %(OS) == dos
:SELINE.       text %(yellow) %(pale_blue)
:SELINE.       border 1 %(Brw) %(pale_blue)
:SELINE.endif
:SELINE.       dimension 2 %(SH)-7 %(SW)-3 %(SH)-5
:SELINE.    endwindow
:SELINE.    
:SELINE.    dirwindow
:SELINE.        dimension 15 2 %(SW)-14 %(SH)-7
:SELINE.        border 1 %(Brw) %(pink)
:SELINE.        text %(yellow) %(pink)
:SELINE.        hilight %(Brw) %(pale_blue)
:SELINE.    endwindow
:SELINE.    
:SELINE.    setwindow
:SELINE.        border 1 %(Brw) %(pale_blue)
:SELINE.        dimension 12 2 43 %(SH)-4
:SELINE.        text %(yellow) %(pale_blue)
:SELINE.    endwindow
:SELINE.    
:SELINE.    setvalwindow
:SELINE.        border 1 %(Brw) %(pink)
:SELINE.        text %(yellow) %(pink)
:SELINE.        dimension 46 6 73 9
:SELINE.    endwindow
:SELINE.    
:SELINE.    filewindow
:SELINE.        border 1 %(Brw) %(red)
:SELINE.        text %(Brw) %(red)
:SELINE.        hilight %(black) %(yellow)
:SELINE.        dimension 26 2 %(SW)-2 %(SH)-7
:SELINE.    endwindow
:SELINE.    
:SELINE.    statuswindow
:SELINE.       border -1
:SELINE.       dimension 0 %(SH)-2 7 %(SH)-1
:SELINE.       text %(black) %(white)
:SELINE.    endwindow
:SELINE.    
:SELINE.    messagewindow
:SELINE.        border -1
:SELINE.        dimension 8 %(SH)-2 %(SW)-1 %(SH)-1
:SELINE.        text %(text_white) %(pale_blue)
:SELINE.        hilight %(blue) %(pale_blue)
:SELINE.    endwindow
:SELINE.    
:SELINE.    menuwindow
:SELINE.        border 1 %(Brw) %(pink)
:SELINE.        text %(yellow) %(pink)
:SELINE.        hilight %(Brw) %(pale_blue)
:SELINE.    endwindow
:SELINE.    
:SELINE.    menubarwindow
:SELINE.if %(OS) == dos
:SELINE.        text %(Brw) %(white)
:SELINE.        hilight %(yellow) %(white)
:SELINE.endif
:SELINE.#        dimension 0 0 %(SW)-1 0
:SELINE.    endwindow
:SELINE.    
:SELINE.#
:SELINE.# MONOCOLOR CONFIGURATION  
:SELINE.#
:SELINE.elseif config == 1
:SELINE.
:SELINE.    editwindow
:SELINE.       border 1 %(Brw) %(black)
:SELINE.       dimension 0 1 %(SW)-1 %(SH)-3
:SELINE.       text %(white) %(black)
:SELINE.        hilight %(Brw) %(black)
:SELINE.    endwindow
:SELINE.    
:SELINE.    setwindow
:SELINE.        border 1 %(white) %(black)
:SELINE.        dimension 12 2 43 %(SH)-4
:SELINE.        text %(white) %(black)
:SELINE.    endwindow
:SELINE.    
:SELINE.    commandwindow
:SELINE.       text %(white) %(black)
:SELINE.       border 1 %(white) %(black)
:SELINE.       dimension 2 %(SH)-7 %(SW)-3 %(SH)-5
:SELINE.    endwindow
:SELINE.    
:SELINE.    statuswindow
:SELINE.       border -1
:SELINE.       dimension 0 %(SH)-2 7 %(SH)-1
:SELINE.       text %(white) %(black)
:SELINE.    endwindow
:SELINE.    
:SELINE.    messagewindow
:SELINE.        border -1
:SELINE.        dimension 9 %(SH)-2 %(SW)-1 %(SH)-1
:SELINE.        text %(white) %(black)
:SELINE.        hilight %(Brw) %(black)
:SELINE.    endwindow
:SELINE.    
:SELINE.    filecwindow
:SELINE.       border 1 %(white) %(black)
:SELINE.       dimension 4 8 %(SW)-5 %(SH)-8
:SELINE.       text %(text_white) %(black)
:SELINE.        hilight %(Brw) %(black)
:SELINE.    endwindow
:SELINE.
:SELINE.#
:SELINE.# BLACK AND WHITE CONFGURATION
:SELINE.# 
:SELINE.else
:SELINE.
:SELINE.endif
:eSEXMP.

:SEREF.
:eSEREF.
