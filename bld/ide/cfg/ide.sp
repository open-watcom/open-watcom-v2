rem  System abbreviations used here are five characters 'smpth' where
rem  's' is the target system
rem     d DOS
rem     o OS/2
rem     l Netware NLM
rem     n Windows NT
rem     q QNX
rem     r RDOS
rem     w Windows 3.x
rem     x Linux
rem  'm' modifies the system specified in 's'
rem     dc DOS .com file
rem     df Flashtek, register based calling conventions
rem     ds Flashtek, stack based calling conventions
rem     dp Pharlap
rem     dt Pharlap tnt
rem     dr Tenberry DOS/4G
rem     dw CauseWay DOS32
rem     dx CauseWay DLL, register based calling conventions
rem     dy CauseWay DLL, stack based calling conventions
rem     dm PMODE/W
rem     da DOS32/A le format
rem     db DOS32/A lx format
rem     nc Windows NT character mode executable
rem     nw Windows NT windowed executable
rem     nm 32-bit MFC static linkage
rem     na 32-bit MFC DLL linkage
rem     no 32-bit MFC Unicode DLL linkage
rem     nu 32-bit MFC Unicode static linkage
rem     np Windows NT PowerBuilder DLL
rem     oc OS/2 PM Compatible
rem     op OS/2 PM
rem     of OS/2 Full screen
rem     ov OS/2 Virtual device
rem     rp RDOS, PE executable
rem     rn RDOS, NE executable (not yet supported)
rem     rd RDOS, DOS executable (not yet supported)
rem     rk RDOS, .rdv kernel-mode device driver
rem     rb RDOS, .bin binary file
rem     rm RDOS, multiboot stub
rem     wm Windows 3.x MFC 2.X application, static linkage
rem     wa Windows 3.x MFC 2.X application, DLL linkage
rem     wp Windows 3.x PowerBuilder DLL
rem     ws Windows 3.x application (not Visual Programmer)
rem     qn qnx neutrino (ELF)
rem     q4 qnx 4.0
rem     qo qnx
rem  'p' is the processor
rem     6 for 16 bit x86
rem     2 for 32 bit x86
rem     A for Alpha ( 32 bit )
rem     M for MIPS
rem     P for PPC
rem  't' is the type of target
rem     e executable
rem     d dll
rem     s library
rem     h help file
rem     i inf file
rem  'h' is the host system
rem     o OS/2 PM
rem     s Win-OS/2
rem     n Windows NT
rem     9 Windows 95
rem     w Windows 3.x
rem     j Japanese Windows 3.x on an IBM
rem     8 Japanese Windows 3.x on a Nec98
rem     a Dec Alpha (Windows NT)
rem     d DOS
rem     x Linux
rem If you add new hosts, also change bld\ide\lib\mconfig.c


Version 5

IncludeFile override.cfg

Tool WMAKE "Make Utility"
  SwitchText a,      "Make all targets", .
  SwitchText h,      "Don't print header", .
  SwitchText e,      "Erase bad targets", .
  SwitchText i,      "Ignore bad targets", .
  SwitchText c,      "Don't check for files made", .
  SwitchText m,      "Don't use MAKEINIT file", .
  SwitchText o,      "Use circular path", .
  SwitchText p,      "Print info", .
  SwitchText r,      "Ignore built-in defns", .
  SwitchText t,      "Only touch files", .
  SwitchText z,      "Do not erase files", .
  SwitchText d,      "Echo progress of work", .
  SwitchText k,      "Continue after an error", .
  SwitchText n,      "Print without executing", .
  SwitchText q,      "Query mode", .
  SwitchText s,      "Silent mode", .
  SwitchText l,      "Append output to log:", .
  SwitchText others, "Other options:", .
  Family Make
    CSwitch 0, ?????, "", "wmake -f $(%_makefile)", ON
    CSwitch 0, ?????, a, -a, OFF
    CSwitch 0, ?????, h, -h, ON
    CSwitch 0, ?????, e, -e, ON
    CSwitch 0, ?????, i, -i, OFF
    CSwitch 0, ?????, c, -c, OFF
    CSwitch 0, ?????, m, -m, OFF
    CSwitch 0, ?????, o, -o, OFF
    CSwitch 0, ?????, p, -p, OFF
    CSwitch 0, ?????, r, -r, OFF
    CSwitch 0, ?????, t, -t, OFF
    CSwitch 0, ?????, z, -z, OFF
    CSwitch 1, ?????, d, -d, OFF
    CSwitch 1, ?????, k, -k, OFF
    CSwitch 1, ?????, n, -n, OFF
    CSwitch 1, ?????, q, -q, OFF
    CSwitch 1, ?????, s, -s, OFF
    VSwitch 1, ?????, l, -l, " ", ONE, OFF, "$*.lst"
    VSwitch 1, ?????, others,,, ONE, REQ, ""

Tool RUN "User Application"
  SwitchText fullscr, "Fullscreen", .
  SwitchText args,    "Application parameters:", .
  Family Application
    CSwitch 0, x???d, "", "!Error Cannot run Linux target under DOS$", ON
    CSwitch 0, x???o, "", "!Error Cannot run Linux target under OS/2$", ON
    CSwitch 0, x???l, "", "!Error Cannot run Linux target under Netware$", ON
    CSwitch 0, x???n, "", "!Error Cannot run Linux target under Win32$", ON
    CSwitch 0, x???q, "", "!Error Cannot run Linux target under QNX$", ON
    CSwitch 0, x???w, "", "!Error Cannot run Linux target under Win16$", ON
rem make fullscreen an option where both windowed & fullscreen work
    CSwitch 0, oc?eo, fullscr, "!Fullscreen", OFF
    CSwitch 0, of?eo, fullscr, "!Fullscreen", ON
    CSwitch 0, ov?eo, fullscr, "!Fullscreen", OFF
    CSwitch 0, d??eo, fullscr, "!Fullscreen", ON
    CSwitch 0, dr2e?, "", $(%watcom)\binw\dos4gw, ON
    CSwitch 0, dw2e?, "", $(%watcom)\binw\cwstub, ON
rem Pharlap run386 must be in your path
    CSwitch 0, dp2e?, "", run386, ON
rem Pharlap TNT run386 must be in your path
    CSwitch 0, dt2e?, "", tnt, ON
    CSwitch 0, w?2eo, "", "!Fullscreen", ON
    CSwitch 0, ???eo, "",, ON
    CSwitch 0, ?????, "", $@, ON
    VSwitch 0, ?????, args,,, ONE, REQ, ""

Tool CDEBUG "Common Debugging Setup"
  SwitchText winmode,  "Windowed", .
  SwitchText charmode, "Character mode", .
  SwitchText pmmode,   "PM", .
  SwitchText i,        "Configuration file:", .
  SwitchText noi,      "Don't invoke configuration", .
  SwitchText symfile,  "Use symbol file", .
  SwitchText nosy,     "Don't process symbolic info", .
  SwitchText nof,      "Ignore any floating point hardware", . , "Ignore any floating point hdwr"
  SwitchText noch,     "Don't do character remapping", . , "Don't do char remapping"
  SwitchText di,       "Dip files:", .
  SwitchText dy,       "Dynamic memory:", .
  SwitchText li,       "Lines of character mode screen:", . , "Lines of debugger screen:", "Lines of char mode screen:"
  SwitchText co,       "Columns of character mode screen:", . , "Columns of debugger screen:", "Columns of char mode screen:"
  SwitchText defadapt, "Default adapter", . , "Default Adapter"
  SwitchText m,        "Monochrome adapter", . , "Monochrome Adapter"
  SwitchText c,        "Color adapter", . , "Color Adapter"
  SwitchText e,        "43-line EGA adapter", . , "43 line EGA Adapter"
  SwitchText v,        "50-line VGA adapter", . , "50 line VGA Adapter"
  SwitchText args,     "Application parameters:", .
  SwitchText cmd_name, " Name", .

Tool WDEBUG "Local Debugger"
  IncludeTool CDEBUG
  SwitchText tr,       "Trap file:", .
  SwitchText nom,      "Ignore any attached mouse", .
  SwitchText nog,      "Don't use graphical mouse", .
  SwitchText ch,       "Free guarantee:", .
  SwitchText o,        "Overwrite protocol", .
  SwitchText p,        "Page protocol", .
  SwitchText s,        "Swap protocol", .
  SwitchText f,        "Fast-swap protocol", .
  SwitchText t,        "Two protocol", .
  SwitchText cmd_run,  " Run", .
  Family "Debugging Session"
    CSwitch 0, n???o, "", "!Error Cannot locally debug a Windows NT target in OS/2$", ON
    CSwitch 0, w?2?o, "", "!Error Cannot locally debug a 32bit Windows 3.x program in OS/2$", ON
    CSwitch 0, df2e?, "", "!Error Cannot locally debug a FlashTek executable$", ON
    CSwitch 0, ds2e?, "", "!Error Cannot locally debug a FlashTek executable$", ON
    CSwitch 0, x???d, "", "!Error Cannot locally debug a Linux target under DOS$", ON
    CSwitch 0, x???o, "", "!Error Cannot locally debug a Linux target under OS/2$", ON
    CSwitch 0, x???l, "", "!Error Cannot locally debug a Linux target under Netware$", ON
    CSwitch 0, x???n, "", "!Error Cannot locally debug a Linux target under Win32$", ON
    CSwitch 0, x???q, "", "!Error Cannot locally debug a Linux target under QNX$", ON
    CSwitch 0, x???w, "", "!Error Cannot locally debug a Linux target under Win16$", ON
    CSwitch 0, r????, "", "!Error Cannot locally debug a RDOS target$", ON
    CSwitch 0, dr2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, dw2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\cw.trp $&\n", ON
    CSwitch 0, dp2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch 0, dt2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch 0, da2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, db2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, dm2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, d?6en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\std.trp $&\n", ON
    CSwitch 0, dr2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, dw2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\cw.trp $&\n", ON
    CSwitch 0, dp2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch 0, dt2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch 0, da2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, db2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, dm2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, dr2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, dw2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\cw.trp $&\n", ON
    CSwitch 0, dp2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch 0, dt2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch 0, da2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, db2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, dm2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch 0, d?6eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\std.trp $&\n", ON
:segment !C_FOR_PB
    RGroup "Debugger"
        RSwitch 0, w???o, winmode, "!Fullscreen $(%watcom)\\binw\\wdw", ON
        RSwitch 0, w???n, winmode, $(%watcom)\binnt\wdw, ON
        RSwitch 0, w???a, winmode, $(%watcom)\axpnt\wdw, ON
        RSwitch 0, w???w, winmode, $(%watcom)\binw\wdw, ON
        RSwitch 0, w???j, winmode, $(%watcom)\binw\wdw, ON
        RSwitch 0, w???8, winmode, $(%watcom)\binw\wdw, ON
        RSwitch 0, w???s, winmode, $(%watcom)\binw\wdw, ON
        RSwitch 0, n????, winmode, $(%watcom)\binnt\wdw, ON
        RSwitch 0, w???n, charmode, "!Fullscreen $(%watcom)\\binnt\\wd.exe", OFF
        RSwitch 0, w???a, charmode, "!Fullscreen $(%watcom)\\axpnt\\wd.exe", OFF
        RSwitch 0, w???8, charmode, "!Fullscreen $(%watcom)\\n98bw\\wdc.exe", OFF
        RSwitch 0, w???w, charmode, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, w???j, charmode, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, w???o, charmode, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, w???s, charmode, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, n????, charmode, $(%watcom)\binnt\wd, OFF
        RSwitch 0, o????, pmmode, $(%watcom)\binp\wdw, ON
        RSwitch 0, o????, charmode, "!Fullscreen $(%watcom)\\binp\\wd", OFF
:endsegment
    CSwitch 0, dr2ew, cmd_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, dw2ew, cmd_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, dp2ew, cmd_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, dt2ew, cmd_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, da2ew, cmd_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, db2ew, cmd_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, dm2ew, cmd_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, dr2ej, cmd_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, dw2ej, cmd_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, dp2ej, cmd_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, dt2ej, cmd_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, da2ej, cmd_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, db2ej, cmd_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, dm2ej, cmd_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, dr2e8, cmd_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, dw2e8, cmd_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, dp2e8, cmd_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, dt2e8, cmd_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, da2e8, cmd_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, db2e8, cmd_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, dm2e8, cmd_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, d???o, cmd_run, "$(%watcom)\\binp\\wd", ON
    CSwitch 0, d???n, cmd_run, "$(%watcom)\\binnt\\wdw", ON
    CSwitch 0, d???a, cmd_run, "$(%watcom)\\axpnt\\wdw", ON
rem binw\wd is used because wdw doesn't load trap files
    CSwitch 0, d???9, cmd_run, "$(%watcom)\\binw\\wd", ON
    CSwitch 0, d???8, cmd_run, "$(%watcom)\\n98bw\\wd", ON
    CSwitch 0, d???j, cmd_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, d????, cmd_run, "$(%watcom)\\binw\\wd", ON
    CSwitch 0, w???9, "", $(%watcom)\binw\wdw, ON
    CSwitch 0, wp6d8, cmd_run, "!Fullscreen $(%watcom)\\n98bw\\wdc.exe", ON
    CSwitch 0, wp6d?, cmd_run, "!Fullscreen $(%watcom)\\binw\\wdc.exe", ON
    CSwitch 0, np2d?, cmd_run, "$(%watcom)\\binnt\\wdw -powerbuilder -tr=std;2", ON
    CSwitch 0, l???w, cmd_run, "$(%watcom)\\binw\\wdw", ON
    CSwitch 0, l???j, cmd_run, "$(%watcom)\\binw\\wdw", ON
    CSwitch 0, l???8, cmd_run, "$(%watcom)\\n98bw\\wdw", ON
    CSwitch 0, l????, cmd_run, wdw, ON
::
:segment !C_FOR_PB
::
    VSwitch 1, dr2ew, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dw2ew, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dp2ew, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dt2ew, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, da2ew, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, db2ew, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dm2ew, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dr2ej, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dw2ej, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dp2ej, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dt2ew, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, da2ew, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, db2ew, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dm2ew, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dr2e8, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dw2e8, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dp2e8, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dt2e8, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, da2e8, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, db2e8, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dm2e8, tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dr2e9, tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dw2e9, tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dp2e9, tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dt2e9, tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, da2e9, tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, db2e9, tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dm2e9, tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, d???o, tr, -tr, =, ONE, REQ, "vdm;$&"
    VSwitch 1, d???n, tr, -tr, =, ONE, REQ, "vdm;$&"
    VSwitch 1, dr2e?, tr, -tr, =, ONE, REQ, "rsi"
    VSwitch 1, da2e?, tr, -tr, =, ONE, REQ, "rsi"
    VSwitch 1, db2e?, tr, -tr, =, ONE, REQ, "rsi"
    VSwitch 1, dm2e?, tr, -tr, =, ONE, REQ, "rsi"
    VSwitch 1, dw2e?, tr, -tr, =, ONE, REQ, "cw"
    VSwitch 1, dp2e?, tr, -tr, =, ONE, REQ, "pls"
    VSwitch 1, dt2e?, tr, -tr, =, ONE, REQ, "pls"
    VSwitch 1, l?2e?, tr, -tr, =, ONE, REQ, "nov;$&"
    VSwitch 1, ?????, tr, -tr, =, ONE, REQ, ""
::
:endsegment
::
  Family "Debugger Options"
    VSwitch 0, wp6d?, i, -i, =, ONE, REQ, "pbwd.dbg"
    VSwitch 0, np2d?, i, -i, =, ONE, REQ, "pbwdnt.dbg"
    VSwitch 0, ?????, i, -i, =, ONE, REQ, ""
    VSwitch 0, wp6d?, "", -initcmd, =, ONE, REQ, "{pbstart.dbg $&}"
:segment !C_FOR_PB
    CSwitch 0, ?????, noi, -noi, OFF
:endsegment
    CSwitch 0, ?????, symfile, :$*.sym, OFF
:segment !C_FOR_PB
    CSwitch 0, ?????, nosy, -nosy, OFF
    CSwitch 0, ?????, nof, -nof, OFF
    CSwitch 0, ?????, nom, -nom, OFF
    CSwitch 0, ?????, nog, -nog, OFF
    CSwitch 0, ?????, noch, -noch, OFF
    VSwitch 1, ?????, di, -di, =, MULTI, REQ, ""
    VSwitch 1, ?????, dy, -dy, =, ONE, REQ, ""
    VSwitch 1, ?????, ch, -ch, =, ONE, REQ, ""
:endsegment
  Family "Debugger Display"
    RGroup "Character mode screen protocol"
        RSwitch 1, ?????, defadapt, , ON
:segment !C_FOR_PB
        RSwitch 1, ?????, o, -o, OFF
:endsegment
        RSwitch 1, ?????, p, -p, OFF
        RSwitch 1, ?????, s, -s, OFF
        RSwitch 1, ?????, f, -f, OFF
        RSwitch 1, ?????, t, -t, OFF
    VSwitch 0, ?????, li, -li, =, ONE, REQ, ""
    VSwitch 0, ?????, co, -co, =, ONE, REQ, ""
    RGroup "Character mode screen"
        RSwitch 0, ?????, defadapt, , ON
        RSwitch 0, ?????, m, -m, OFF
        RSwitch 0, ?????, c, -c, OFF
        RSwitch 0, ?????, e, -e, OFF
        RSwitch 0, ?????, v, -v, OFF
    RGroup "Character mode screen protocol"
        RSwitch 1, ?????, defadapt, , ON
:segment !C_FOR_PB
        RSwitch 1, ?????, o, -o, OFF
:endsegment
        RSwitch 1, ?????, p, -p, OFF
        RSwitch 1, ?????, s, -s, OFF
        RSwitch 1, ?????, f, -f, OFF
        RSwitch 1, ?????, t, -t, OFF
    CSwitch 1, wp6d?, cmd_name, "$(%watcom)\\binw\\dlldbg.exe $@", ON
    CSwitch 1, np2d?, cmd_name,, ON
rem For remote debugging, don't put a full path name
    CSwitch 1, l????, cmd_name, $., ON
    CSwitch 1, ?????, cmd_name, $@, ON
:segment !C_FOR_PB
    VSwitch 1, ?????, args,,, ONE, REQ, ""
:endsegment
    CSwitch 1, dr2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, da2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, db2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, dm2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, dw2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=cw -once\n", ON
    CSwitch 1, dp2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON
    CSwitch 1, dt2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON
    CSwitch 1, dr2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, da2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, db2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, dm2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, dw2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=cw -once\n", ON
    CSwitch 1, dp2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON
    CSwitch 1, dt2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON
    CSwitch 1, dr2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, da2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, db2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, dm2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch 1, dw2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=cw -once\n", ON
    CSwitch 1, dp2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON
    CSwitch 1, dt2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON

Tool RDEBUG "Remote Debugging"
  IncludeTool CDEBUG
  SwitchText tr,       "Local trap file:", .
  SwitchText rem,      "Exists on target", .
  SwitchText do,       "Download to target", .
  SwitchText lo,       "Local info:", .
  Family "Debugging Session"
    RGroup "Debugger"
        RSwitch 0, ????o, pmmode, $(%watcom)\binp\wdw, ON
        RSwitch 0, ????s, winmode, $(%watcom)\binw\wdw, ON
        RSwitch 0, ????n, winmode, $(%watcom)\binnt\wdw, ON
        RSwitch 0, ????9, winmode, $(%watcom)\binnt\wdw, ON
        RSwitch 0, ????w, winmode, $(%watcom)\binw\wdw, ON
        RSwitch 0, ????j, winmode, $(%watcom)\binw\wdw, ON
        RSwitch 0, ????8, winmode, $(%watcom)\binw\wdw, ON
        RSwitch 0, ????a, winmode, $(%watcom)\axpnt\wdw, ON
        RSwitch 0, ????o, charmode, "!Fullscreen $(%watcom)\\binp\\wd", OFF
        RSwitch 0, ????s, charmode, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, ????n, charmode, $(%watcom)\binnt\wd, OFF
        RSwitch 0, ????9, charmode, $(%watcom)\binnt\wd, OFF
        RSwitch 0, ????w, charmode, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, ????j, charmode, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, ????8, charmode, "!Fullscreen $(%watcom)\\n98bw\\wdc.exe", OFF
        RSwitch 0, ????a, charmode, "!Fullscreen $(%watcom)\\axpnt\\wd.exe", OFF
    VSwitch 1, ?????, tr, -tr, =, ONE, REQ, ""
    RGroup "Target location"
        RSwitch 1, ?????, rem, -rem, OFF
        RSwitch 2, ?????, do, -do, ON
    VSwitch 1, ?????, lo, -lo, =, MULTI, REQ, ""
  Family "Debugger Options"
    VSwitch 0, wp6d?, i, -i, =, ONE, REQ, "pbwd.dbg"
    VSwitch 0, np2d?, i, -i, =, ONE, REQ, "pbwdnt.dbg"
    VSwitch 0, ?????, i, -i, =, ONE, REQ, ""
    VSwitch 0, wp6d?, "", -initcmd, =, ONE, REQ, "{pbstart.dbg $&}"
    CSwitch 0, ?????, noi, -noi, OFF
    CSwitch 0, ?????, symfile, :$*.sym, OFF
    CSwitch 0, ?????, nosy, -nosy, OFF
    CSwitch 0, ?????, nof, -nof, OFF
    CSwitch 0, ?????, noch, -noch, OFF
    VSwitch 0, ?????, di, -di, =, MULTI, REQ, ""
    VSwitch 0, ?????, dy, -dy, =, ONE, REQ, ""
    CSwitch 0, ?????, cmd_name, $., ON
    VSwitch 1, ?????, li, -li, =, ONE, REQ, ""
    VSwitch 1, ?????, co, -co, =, ONE, REQ, ""
    RGroup "Character mode screen"
        RSwitch 1, ?????, defadapt, , ON
        RSwitch 1, ?????, m, -m, OFF
        RSwitch 1, ?????, c, -c, OFF
        RSwitch 1, ?????, e, -e, OFF
        RSwitch 1, ?????, v, -v, OFF
    VSwitch 1, ?????, args,,, ONE, REQ, ""
::
:segment !C_FOR_PB
::

Tool WSAMPLE "Sampler"
  SwitchText b,         "Sample buffer size(1-63):", .
  SwitchText f,         "Sample file name:", .
  SwitchText r,         "Sample rate:", .
  SwitchText i,         "Sampler interrupt number(20x-FFx):", .
  SwitchText d,         "disable DOS interrupts", .
  SwitchText args,      "Application parameters:", .
  SwitchText cmd_app,   " app", .
  Family Sampler
    CSwitch 0, ????s, "", "!Error Sampler cannot run under Win-OS/2$", ON
    CSwitch 0, w???o, "", "!Error Windows executable cannot be sampled under OS/2$", ON
    CSwitch 0, x???d, "", "!Error Linux executable cannot be sampled under DOS$", ON
    CSwitch 0, x???o, "", "!Error Linux executable cannot be sampled under OS/2$", ON
    CSwitch 0, x???l, "", "!Error Linux executable cannot be sampled under Netware$", ON
    CSwitch 0, x???n, "", "!Error Linux executable cannot be sampled under Win32$", ON
    CSwitch 0, x???q, "", "!Error Linux executable cannot be sampled under QNX$", ON
    CSwitch 0, x???w, "", "!Error Linux executable cannot be sampled under Win16$", ON
    CSwitch 0, w???w, cmd_app, "!Fullscreen $(%watcom)\\binw\\wsamplew", ON
    CSwitch 0, w???j, cmd_app, "!Fullscreen $(%watcom)\\binw\\wsamplew", ON
    CSwitch 0, w???8, cmd_app, "!Fullscreen $(%watcom)\\binw\\wsamplew", ON
    CSwitch 0, w????, cmd_app, "!Fullscreen wsamplew", ON
    CSwitch 0, d?6??, "", "$(%watcom)\\binw\\wsample", ON
    CSwitch 0, dr2e?, "", "wsamprsi", ON
    CSwitch 0, dw2e?, "", "wsamprsi", ON
    CSwitch 0, da2e?, "", "wsamprsi", ON
    CSwitch 0, db2e?, "", "wsamprsi", ON
    CSwitch 0, dm2e?, "", "wsamprsi", ON
    CSwitch 0, dp2e?, "", "run386", ON
    CSwitch 0, dt2e?, "", "tnt", ON
    CSwitch 0, dp2e?, "", "wsamppls", ON
    CSwitch 0, dt2e?, "", "wsamppls", ON
    CSwitch 0, n????, "", "wsample", ON
    CSwitch 0, o????, "", "wsample", ON
    CSwitch 0, x????, "", "wsample", ON
    VSwitch 0, n????, b, -b, =, ONE, REQ, ""
    VSwitch 0, o????, b, -b, =, ONE, REQ, ""
    VSwitch 0, d????, b, -b, =, ONE, REQ, ""
    VSwitch 0, x????, b, -b, =, ONE, REQ, ""
    VSwitch 0, ?????, f, -f, =, ONE, REQ, "$*.smp"
    VSwitch 0, ?????, r, -r, =, ONE, REQ, ""
    VSwitch 0, d?6??, i, -i, =, ONE, REQ, ""
    CSwitch 0, d?6??, d, -d, OFF
    CSwitch 0, ?????, "", $@, ON
    VSwitch 0, ?????, args,,, ONE, REQ, ""

Tool WPROFILE "Profiler"
  SwitchText cmd_app,   " app", .
  Family Profiler
    CSwitch 0, w???w, cmd_app, "$(%watcom)\\binw\\wprofw", ON
    CSwitch 0, w???j, cmd_app, "$(%watcom)\\binw\\wprofw", ON
    CSwitch 0, w???8, cmd_app, "$(%watcom)\\binw\\wprofw", ON
    CSwitch 0, w???s, cmd_app, "$(%watcom)\\binw\\wprofw", ON
    CSwitch 0, ?????, cmd_app, wprof, ON
    CSwitch 0, ?????, "", $*.smp, ON
::
:endsegment
::

Tool WDIS "Disassembler"
  SwitchText l,  "Generate listing file:", .
  SwitchText i,  "Initial character of internal labels:", .
  SwitchText a,  "Generate assembleable output", .
  SwitchText e,  "Generate list of externs", .
  SwitchText p,  "Generate list of publics", .
  SwitchText o,  "Print list of operands beside instructions", .
  SwitchText m,  "Leave C++ names mangled", .
  SwitchText s,  "Include source lines:", .
  SwitchText fp, "Do not use instruction name pseudonyms", . , "do not use instruction name pseudonyms"
  SwitchText fr, "Do not use register name pseudonyms", . , "do not use register name pseudonyms"
  SwitchText fi, "Use alternate indexing format", . , "use alternate indexing format"
  SwitchText fu, "Instructions/registers in upper case", . , "instructions/registers in upper case"
  Family Disassembler
    CSwitch 0, ?????, "", "wdis $@", ON
    VSwitch 0, ?????, l, -l, =, ONE, ON, "$*.lst"
    VSwitch 0, ?????, i, -i, =, ONE, REQ, ""
    CSwitch 0, ?????, a, -a, OFF
    CSwitch 0, ?????, e, -e, OFF
    CSwitch 0, ?????, p, -p, OFF
    CSwitch 0, ?????, o, -o, OFF
    CSwitch 1, ?????, m, -m, OFF
    VSwitch 1, ?????, s, -s, =, ONE, ON, ""
    CSwitch 1, ?????, fp, -fp, OFF
    CSwitch 1, ??a??, fr, -fr, OFF
    CSwitch 1, ??2??, fi, -fi, OFF
    CSwitch 1, ??6??, fi, -fi, OFF
    CSwitch 1, ?????, fu, -fu, OFF

Tool WCG "Code Generator"
  SwitchText defcompil, "Compiler default", .
  SwitchText od,        "No optimizations", .
  SwitchText ox,        "Average space and time", .
  SwitchText os,        "Space optimizations", .
  SwitchText ot,        "Time optimizations", .
  SwitchText otexan,    "Fastest possible code", .
  SwitchText noframe,   "Do not generate stack frames", .
  SwitchText of,        "Generate as needed", .
  SwitchText of+,       "Always generate", .
  SwitchText s,         "Disable stack depth checking", .
  SwitchText ob,        "Branch prediction", .
  SwitchText ol,        "Loop optimizations", .
  SwitchText ol+,       "Loop unrolling", .
  SwitchText oc,        "Call/return optimizations", .
  SwitchText oi,        "In-line intrinsic functions", .
  SwitchText oa,        "Relax alias checking", .
  SwitchText or,        "Instruction scheduling", .
  SwitchText oh,        "Allow repeated optimizations", .
  SwitchText om,        "Math optimizations", .
  SwitchText on,        "Numerically unstable optimizations", .
  SwitchText op,        "Consistent FP results", .
  SwitchText oe,        "Expand function in-line:", .
  SwitchText nodebuginfo, "No debugging information", .
  SwitchText d1,        "Line number information", .
  SwitchText d2,        "Full debugging info", .
  SwitchText d3,        "Full info plus unused types", .
  SwitchText hw,        "Watcom debugging format", . , "WATCOM debugging format"
  SwitchText hd,        "Dwarf debugging format", .
  SwitchText hc,        "CodeView debugging format", .
  SwitchText db,        "Emit Browser information", .
  SwitchText en,        "Emit routine names in code", .
  SwitchText ep,        "Call prolog routine:", .
  SwitchText ee,        "Call epilog hook routine", .
  SwitchText et,        "Pentium profiling code", .
  SwitchText bm,        "Multithreaded application", .
  SwitchText bw,        "Default-window application", .
  SwitchText fp2,       "Inline 80287 instructions", . , "In-line 80287 instructions"
  SwitchText fp3,       "Inline 80387 instructions", . , "In-line 80387 instructions"
  SwitchText fp5,       "Inline Pentium instructions", . , "In-line Pentium instructions"
  SwitchText fp6,       "Inline Pentium Pro instructions", . , "In-line Pentium Pro instructions"
  SwitchText fpi,       "Inline with emulator", . , "In-line with emulator"
  SwitchText fpi87,     "Inline with coprocessor", . , "In-line with coprocessor"
  SwitchText fpc,       "Floating-point calls", .
  SwitchText deffprnd,  "Default rounding", .
  SwitchText zro,       "Omit rounding", .
  SwitchText zri,       "Inline rounding", .
  SwitchText zdp,       "DS pegged to DGROUP", .
  SwitchText zdf,       "DS can float", .
  SwitchText zfp,       "FS cannot be used", .
  SwitchText zff,       "FS can be used", .
  SwitchText zgp,       "GS cannot be used", .
  SwitchText zgf,       "GS can be used", .
  SwitchText nc,        "Name of code class:", . , "Name Code Class:"
  SwitchText g,         "Name of code group:", . , "Name Code Group:"
  SwitchText nd,        "Name of data segment:", . , "Name Data Segment:"
  SwitchText nt,        "Name of text segment:", . , "Name Text Segment:"
  SwitchText nm,        "Name of module:", . , "Name of Module:"
  SwitchText zl,        "Don't generate default library information", . , "Don't gen default library info"
  SwitchText zld,       "No file dependency information", .
  SwitchText ez,        "Generate Easy OMF object files", .
  SwitchText r,         "Save/restore segment registers", . , "Save/Restore segment registers"
  SwitchText zu,        "SS not assumed equal to DS", .
  SwitchText zm,        "Put functions in separate segments", . , "Put functions in separate segs"
  SwitchText zc,        "Constants in code segment", .
  SwitchText zdl,       "Load DS from DGROUP", .
  SwitchText 0,         "8086", .
  SwitchText 1,         "80186", .
  SwitchText 2,         "80286", .
  SwitchText 3,         "80386", .
  SwitchText 4,         "80486", .
  SwitchText 5,         "Pentium", .
  SwitchText 6,         "Pentium Pro", .
  SwitchText 3r,        "80386 register-based calling", . , "80386 Register based calling"
  SwitchText 3s,        "80386 stack-based calling", . , "80386 Stack based calling"
  SwitchText 4r,        "80486 register-based calling", . , "80486 Register based calling"
  SwitchText 4s,        "80486 stack-based calling", . , "80486 Stack based calling"
  SwitchText 5r,        "Pentium register-based calling", . , "Pentium Register based calling"
  SwitchText 5s,        "Pentium stack-based calling", . , "Pentium Stack based calling"
  SwitchText 6r,        "Pentium Pro register-based calling", . , "Pentium Pro Register based calling"
  SwitchText 6s,        "Pentium Pro stack-based calling", . , "Pentium Pro Stack based calling"
  SwitchText ms,        "Small model", . , "32-bit small model" , "32bit Small model"
  SwitchText mm,        "Medium model", .
  SwitchText mc,        "Compact model", . , "32-bit compact model"
  SwitchText ml,        "Large model", .
  SwitchText mh,        "Huge model", .
  SwitchText mf,        "Flat model", . , "32-bit flat model" , "32bit Flat model"
  Family Optimization
    RGroup "Style of optimization"
        RSwitch 0, ?????, od, -od, OFF, ON
        RSwitch 0, ?????, ox, -ox , OFF
        RSwitch 0, ?????, os, -os, OFF
        RSwitch 0, ?????, ot, -ot, OFF
        RSwitch 0, ?????, otexan, -otexan, ON, OFF
:segment !C_FOR_PB
    RGroup "Stack frames"
        RSwitch 0, ?????, noframe, , ON
        RSwitch 0, ?????, of, -of, OFF
        RSwitch 0, ?????, of+, -of+, OFF
:endsegment
    CSwitch 0, ?????, s, -s, OFF
    CSwitch 1, ?????, ob, -ob, OFF
    CSwitch 1, ?????, ol, -ol, OFF
    CSwitch 1, ?????, ol+, -ol+, OFF
    CSwitch 1, ?????, oc, -oc, OFF
    CSwitch 1, ?????, oi, -oi, OFF
    CSwitch 1, ?????, oa, -oa, OFF
    CSwitch 1, ?????, or, -or, OFF
    CSwitch 1, ?????, oh, -oh, OFF
    CSwitch 1, ?????, om, -om, OFF
    CSwitch 1, ?????, on, -on, OFF
    CSwitch 1, ?????, op, -op, OFF
    VSwitch 1, ?????, oe, -oe,, ONE, OFF, "20"
  Family Debugging
    RGroup "Debugging style"
        RSwitch 0, ?????, nodebuginfo, , OFF
        RSwitch 0, ?????, d1, -d1, ON, OFF
        RSwitch 0, ?????, d2, -d2, OFF, ON
        RSwitch 0, ?????, d3, -d3, OFF
    RGroup "Debugging format"
        RSwitch 0, ?????, defcompil, , ON
        RSwitch 0, ?????, hw, -hw, OFF
:segment !C_FOR_PB
        RSwitch 0, ?????, hd, -hd, OFF
        RSwitch 0, ?????, hc, -hc, OFF
    CSwitch 1, ?????, db, -db, OFF
    CSwitch 1, ?????, en, -en, OFF
    VSwitch 1, ?????, ep, -ep,, ONE, OFF, ""
    CSwitch 1, ?????, ee, -ee, OFF
    CSwitch 1, ?????, et, -et, OFF
:endsegment
  Family "Code Generation Strategy"
    CSwitch 0, n?2d?, "", -bd, ON
    CSwitch 0, n?ad?, "", -bd, ON
    CSwitch 0, o??d?, "", -bd, ON
    CSwitch 0, w??d?, "", -bd, ON
    CSwitch 0, x??d?, "", -bd, ON
    CSwitch 0, d?2d?, "", -bd, ON
    CSwitch 0, r?2d?, "", -bd, ON
:segment !C_FOR_PB
    CSwitch 0, nm???, bm, -bm, ON
    CSwitch 0, na???, bm, -bm, ON
    CSwitch 0, no???, bm, -bm, ON
    CSwitch 0, nu???, bm, -bm, ON
    CSwitch 0, o????, bm, -bm, OFF
    CSwitch 0, n????, bm, -bm, OFF
    CSwitch 0, r????, bm, -bm, ON
    CSwitch 0, w????, bw, -bw, OFF
:endsegment
    RGroup "Floating-point level"
        RSwitch 0, ?????, defcompil, , ON
        RSwitch 0, ?????, fp2, -fp2, OFF
        RSwitch 0, ?????, fp3, -fp3, OFF
        RSwitch 0, ?????, fp5, -fp5, OFF
        RSwitch 0, ?????, fp6, -fp6, OFF
    RGroup "Floating-point model"
        RSwitch 1, r????, defcompil, , OFF
        RSwitch 1, ?????, defcompil, , ON
        RSwitch 1, ?????, fpi, -fpi, OFF
        RSwitch 1, r????, fpi87, -fpi87, ON
        RSwitch 1, ?????, fpi87, -fpi87, OFF
        RSwitch 1, ?????, fpc, -fpc, OFF
    RGroup "Floating-point rounding"
        RSwitch, 1, ?????, deffprnd, , ON
        RSwitch, 1, ?????, zro, -zro, OFF
        RSwitch, 1, ??2??, zri, -zri, OFF
:segment !C_FOR_PB
  Family "Register Usage"
    RGroup "DS register"
        RSwitch 0, rk2e?, defcompil, , OFF
        RSwitch 0, ?????, defcompil, , ON
        RSwitch 0, rk2e?, zdp, -zdp, ON
        RSwitch 0, ?????, zdp, -zdp, OFF
        RSwitch 0, ?????, zdf, -zdf, OFF
    RGroup "FS register"
        RSwitch 0, rk2e?, defcompil, , OFF
        RSwitch 0, ?????, defcompil, , ON
        RSwitch 0, ?????, zfp, -zfp, OFF
        RSwitch 0, rk2e?, zff, -zff, ON
        RSwitch 0, ?????, zff, -zff, OFF
    RGroup "GS register"
        RSwitch 0, ?????, defcompil, , ON
        RSwitch 0, ?????, zgp, -zgp, OFF
        RSwitch 0, ?????, zgf, -zgf, OFF
:endsegment
  Family "Code Generation Option"
:segment !C_FOR_PB
    VSwitch 0, ?????, nc, -nc,, ONE, REQ, ""
    VSwitch 0, ?????, g, -g,, ONE, REQ, ""
    VSwitch 0, ?????, nd, -nd,, ONE, REQ, ""
    VSwitch 0, ?????, nt, -nt,, ONE, REQ, ""
    VSwitch 0, ?????, nm, -nm,, ONE, REQ, ""
    CSwitch 1, ?????, zl, -zl, OFF
    CSwitch 1, ?????, zld, -zld, OFF
    CSwitch 1, ??2??, ez, -ez, OFF
    CSwitch 1, ?????, r, -r, OFF
:endsegment
:segment !C_FOR_PB | FOR_WIN
    CSwitch 1, ??6d?, "", -zu, ON
    CSwitch 1, rk2e?, zu, -zu, ON
    CSwitch 1, ???e?, zu, -zu, OFF
    CSwitch 1, ???s?, zu, -zu, OFF
:endsegment
:segment !C_FOR_PB
    CSwitch 1, ?????, zm, -zm, OFF
    CSwitch 1, ?????, zc, -zc, OFF
    CSwitch 1, ??2??, zdl, -zdl, OFF
:endsegment
  Family "Memory Model and Processor"
    RGroup "Target processor"
        RSwitch 0, wm???, 0, ,  OFF
        RSwitch 0, wa???, 0, ,  OFF
        RSwitch 0, ??6??, 0, ,  ON
        RSwitch 0, ??6??, 1, -1, OFF
        RSwitch 0, wm???, 2, -2, ON
        RSwitch 0, wa???, 2, -2, ON
        RSwitch 0, ??6??, 2, -2, OFF
        RSwitch 0, ??6??, 3, -3, OFF
        RSwitch 0, ??6??, 4, -4, OFF
        RSwitch 0, ??6??, 5, -5, OFF
        RSwitch 0, ??6??, 6, -6, OFF
        RSwitch 0, r?2??, 3r, -3r, ON
        RSwitch 0, ??2??, 3r, -3r, OFF
        RSwitch 0, ??2??, 3s, -3s, OFF
        RSwitch 0, ??2??, 4r, -4r, OFF
        RSwitch 0, ??2??, 4s, -4s, OFF
        RSwitch 0, ??2??, 5r, -5r, OFF
        RSwitch 0, ??2??, 5s, -5s, OFF
        RSwitch 0, ds2??, 6r, -6r, OFF
        RSwitch 0, dy2??, 6r, -6r, OFF
        RSwitch 0, l?2??, 6r, -6r, OFF
        RSwitch 0, r?2??, 6r, -6r, OFF
        RSwitch 0, ??2??, 6r, -6r, ON
        RSwitch 0, ds2??, 6s, -6s, ON
        RSwitch 0, dy2??, 6s, -6s, ON
        RSwitch 0, l?2??, 6s, -6s, ON
        RSwitch 0, ??2??, 6s, -6s, OFF
    CSwitch 0, w????, "", -bt=windows, ON
    CSwitch 0, d????, "", -bt=dos, ON
    CSwitch 0, o????, "", -bt=os2, ON
    CSwitch 0, n????, "", -bt=nt, ON
    CSwitch 0, l????, "", -bt=netware, ON
    CSwitch 0, x????, "", -bt=linux, ON
    CSwitch 0, rp???, "", -bt=rdos, ON
    CSwitch 0, rk6??, "", -bt=rdos_dev16, ON
    CSwitch 0, rk2??, "", -bt=rdosdev, ON
    CSwitch 0, rb6??, "", -bt=rdos_bin16, ON
    CSwitch 0, rb2??, "", -bt=rdos_bin32, ON
    CSwitch 0, rm6??, "", -bt=rdos_mboot, ON
    CSwitch 0, ?????, "", -fo=.obj, ON
:segment C_FOR_PB
    CSwitch 1, n?2d?, mf, -mf, ON
:endsegment
:segment !C_FOR_PB
    RGroup "Memory model"
        RSwitch 1, ??A??, defcompil, , ON
        RSwitch 1, ?????, defcompil, , OFF
        RSwitch 1, dc6e?, ms, -ms, ON
        RSwitch 1, rk6??, ms, -ms, ON
        RSwitch 1, rb6??, ms, -ms, ON
        RSwitch 1, rm6??, ms, -ms, ON
        RSwitch 1, ??6??, ms, -ms, OFF
        RSwitch 1, ??6??, mm, -mm, OFF
        RSwitch 1, ??6??, mc, -mc, OFF
        RSwitch 1, dc6e?, ml, -ml, OFF
        RSwitch 1, rk6??, ml, -ml, OFF
        RSwitch 1, rb6??, ml, -ml, OFF
        RSwitch 1, rm6??, ml, -ml, OFF
        RSwitch 1, ??6??, ml, -ml, ON
        RSwitch 1, ??6??, mh, -mh, OFF
        RSwitch 1, l?2??, mf, -mf, OFF
        RSwitch 1, rk2??, mf, -mf, OFF
        RSwitch 1, ??2??, mf, -mf, ON
        RSwitch 1, l?2??, ms, -ms, ON
        RSwitch 1, rk2??, mc, -mc, ON
        RSwitch 1, ??2??, ms, -ms, OFF
:endsegment
    CSwitch 1, wp6d?, "", -ml, ON

Tool WLANG "Languages"
  IncludeTool WCG
  SwitchText defcompil, "", "Default byte alignment"
  SwitchText i,        "Include directories:", .
  SwitchText fi,       "Include files:", .
  SwitchText fh,       "Use precompiled headers", .
  SwitchText w0,       "Warning level 0", .
  SwitchText w1,       "Warning level 1", .
  SwitchText w2,       "Warning level 2", .
  SwitchText w3,       "Warning level 3", .
  SwitchText w4,       "Warning level 4", .
  SwitchText we,       "Treat warnings as errors", .
  SwitchText e,        "Error count:", .
  SwitchText za,       "Force ANSI compliance", .
  SwitchText sg,       "Automatic stack growing", .
  SwitchText st,       "Touch stack through SS first", .
  SwitchText wcd,      "Disable warning messages", . , "Disable Warning message"
  SwitchText wce,      "Enable warning messages", . , "Enable Warning message"
  SwitchText d+,       "Extended macro definitions", .
  SwitchText u,        "Undefine macros:", .
  SwitchText d,        "Macro definitions:", .
  SwitchText j,        "Change char default to signed", .
  SwitchText ei,       "Force enums to be type int", .
  SwitchText zp1,      "1-byte alignment", . , "1 byte alignment"
  SwitchText zp2,      "2-byte alignment", . , "2 byte alignment"
  SwitchText zp4,      "4-byte alignment", . , "4 byte alignment"
  SwitchText zp8,      "8-byte alignment", . , "8 byte alignment"
  SwitchText zp16,     "16-byte alignment", . , "16 byte alignment"
  SwitchText zt,       "Data Threshold:", .
  SwitchText sbcs,     "No double-byte character support", .
  SwitchText zk0,      "Kanji", .
  SwitchText zk0u,     "Kanji (Unicode)", .
  SwitchText zk1,      "Traditional Chinese", . , "Chinese/Taiwanese", "Chineese/Taiwanese"
  SwitchText zk3,      "Simplified Chinese", .
  SwitchText zk2,      "Korean", .
  SwitchText zkl,      "Current code page", .
  SwitchText zku,      "Unicode translate table:", .
  SwitchText zq,       "Quiet operation", .
  SwitchText others,   "Other options:", .
  Family "File Option"
    VSwitch 0, w????, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/win\""
    VSwitch 0, d????, i, -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, o?6??, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/os21x\""
    VSwitch 0, o?2??, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/os2\""
    VSwitch 0, nm???, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, na???, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, no???, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, nu???, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, n????, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt\""
    VSwitch 0, l????, i, -i, =, MULTI, REQ, "\"$(%watcom)/novh\""
    VSwitch 0, x????, i, -i, =, MULTI, REQ, "\"$(%watcom)/lh\""
    VSwitch 0, r????, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/rdos\""
    VSwitch 0, ?????, fi, -fi, =, MULTI, REQ, ""
    CSwitch 0, wm???, fh, -fhq, ON
    CSwitch 0, wa???, fh, -fhq, ON
    CSwitch 0, wp???, fh, -fhq, ON
    CSwitch 0, nm???, fh, -fhq, ON
    CSwitch 0, na???, fh, -fhq, ON
    CSwitch 0, nu???, fh, -fhq, ON
    CSwitch 0, no???, fh, -fhq, ON
    CSwitch 0, ?????, fh, -fh, OFF
  Family Diagnostics
    RGroup "Warning level"
        RSwitch 0, ?????, w0, -w0, OFF
        RSwitch 0, ?????, w1, -w1, OFF
        RSwitch 0, wa???, w2, -w2, ON
        RSwitch 0, wm???, w2, -w2, ON
        RSwitch 0, ?????, w2, -w2, OFF
        RSwitch 0, nm???, w3, -w3, ON
        RSwitch 0, na???, w3, -w3, ON
        RSwitch 0, nu???, w3, -w3, ON
        RSwitch 0, no???, w3, -w3, ON
        RSwitch 0, ?????, w3, -w3, OFF
        RSwitch 0, wm???, w4, -w4, OFF
        RSwitch 0, wa???, w4, -w4, OFF
        RSwitch 0, nm???, w4, -w4, OFF
        RSwitch 0, na???, w4, -w4, OFF
        RSwitch 0, no???, w4, -w4, OFF
        RSwitch 0, nu???, w4, -w4, OFF
        RSwitch 0, ?????, w4, -w4, ON
    CSwitch 0, ?????, we, -we, OFF
    VSwitch 0, ?????, e, -e,, ONE, REQ, "25"
    CSwitch 1, ?????, za, -za, OFF
    CSwitch 1, o?2??, sg, -sg, OFF
    CSwitch 1, o?2??, st, -st, OFF
    VSwitch 1, ?????, wcd, -wcd,, MULTI, REQ, ""
    VSwitch 1, ?????, wce, -wce,, MULTI, REQ, ""
  Family Source
    CSwitch 0, ?????, d+, -d+, OFF
    VSwitch 0, ?????, u, -u,, MULTI, REQ, ""
    VSwitch 0, nm?e?, d, -d,, MULTI, REQ, "_X86_ _MBCS", "_X86_ _DEBUG _MBCS"
    VSwitch 0, nm?d?, d, -d,, MULTI, REQ, "_X86_ _USRDLL _WINDLL _MBCS", "_X86_ _USRDLL _WINDLL _DEBUG _MBCS"
    VSwitch 0, na?e?, d, -d,, MULTI, REQ, "_AFXDLL _X86_ _MBCS", "_AFXDLL _X86_ _DEBUG _MBCS"
    VSwitch 0, na?d?, d, -d,, MULTI, REQ, "_AFXDLL _WINDLL _AFXEXT _X86_ _MBCS", "_AFXDLL _WINDLL _AFXEXT _X86_ _DEBUG _MBCS"
    VSwitch 0, no?e?, d, -d,, MULTI, REQ, "_UNICODE _AFXDLL _X86_", "_UNICODE _AFXDLL _X86_ _DEBUG"
    VSwitch 0, no?d?, d, -d,, MULTI, REQ, "_UNICODE _AFXDLL _WINDLL _AFXEXT _X86_", "_UNICODE _AFXDLL _WINDLL _AFXEXT _X86_ _DEBUG"
    VSwitch 0, nu?e?, d, -d,, MULTI, REQ, "_UNICODE _X86_", "_UNICODE _X86_ _DEBUG"
    VSwitch 0, nu?d?, d, -d,, MULTI, REQ, "_UNICODE _USRDLL _WINDLL _X86_", "_UNICODE _USRDLL _WINDLL _X86_ _DEBUG"
    VSwitch 0, wa?e?, d, -d,, MULTI, REQ, "_AFXDLL", "_AFXDLL _DEBUG"
    VSwitch 0, wa?d?, d, -d,, MULTI, REQ, "_AFXDLL _WINDLL", "_AFXDLL _WINDLL _DEBUG"
    VSwitch 0, wm6e?, d, -d,, MULTI, REQ, "", "_DEBUG"
    VSwitch 0, wm6d?, d, -d,, MULTI, REQ, "_USRDLL _WINDLL", "_WINDLL _USRDLL _DEBUG"
    VSwitch 0, ?????, d, -d,, MULTI, REQ, ""
    CSwitch 0, nm???, j, -j, ON
    CSwitch 0, na???, j, -j, ON
    CSwitch 0, no???, j, -j, ON
    CSwitch 0, nu???, j, -j, ON
    CSwitch 0, ?????, j, -j, OFF
    CSwitch 0, nm???, ei, -ei, ON
    CSwitch 0, na???, ei, -ei, ON
    CSwitch 0, no???, ei, -ei, ON
    CSwitch 0, nu???, ei, -ei, ON
    CSwitch 0, ?????, ei, -ei, OFF
    RGroup "Structure alignment"
        RSwitch 1, nm???, defcompil, , OFF
        RSwitch 1, na???, defcompil, , OFF
        RSwitch 1, no???, defcompil, , OFF
        RSwitch 1, nu???, defcompil, , OFF
        RSwitch 1, r????, defcompil, , OFF
        RSwitch 1, ?????, defcompil, , ON
        RSwitch 1, r????, zp1, -zp1, ON
        RSwitch 1, ?????, zp1, -zp1, OFF
        RSwitch 1, ?????, zp2, -zp2, OFF
        RSwitch 1, nm???, zp4, -zp4, ON
        RSwitch 1, na???, zp4, -zp4, ON
        RSwitch 1, no???, zp4, -zp4, ON
        RSwitch 1, nu???, zp4, -zp4, ON
        RSwitch 1, ?????, zp4, -zp4, OFF
        RSwitch 1, ?????, zp8, -zp8, OFF
        RSwitch 1, ?????, zp16, -zp16, OFF
  Family Miscellaneous
    VSwitch 0, ??6??, zt, -zt,, ONE, REQ, ""
    RGroup "Double byte characters"
        RSwitch 0, ?????, sbcs, , ON
        RSwitch 0, ?????, zk0, -zk0, OFF
        RSwitch 0, ?????, zk0u, -zk0u, OFF
        RSwitch 0, ?????, zk1, -zk1, OFF
        RSwitch 0, ?????, zk3, -zk3, OFF
        RSwitch 0, ?????, zk2, -zk2, OFF
        RSwitch 0, ?????, zkl, -zkl, OFF
    VSwitch 1, ?????, zku, -zku, =, ONE, REQ, ""
    CSwitch 1, ?????, zq, -zq, ON
    VSwitch 1, ?????, others,,, MULTI, REQ, ""

Tool CCOMP "C Compiler"
  SwitchText ri,        "Return char and short as int", .
  SwitchText nodeffile, "No definition file", .
  SwitchText v,         "With typedef names", .
  SwitchText zg,        "With base type names", .
  Family "C Compiler"
    CSwitch 0, ?????, ri, -ri, OFF
    RGroup "Create definition file (.def)"
        RSwitch 0, ?????, nodeffile,  , ON
        RSwitch 0, ?????, v, -v, OFF
        RSwitch 0, ?????, zg, -zg, OFF

Tool CPPCOMP "C++ Compiler"
  SwitchText ew,  "Shorter error messages", .
  SwitchText wx,  "Full warnings", .
  SwitchText zws, "Smart Windows callbacks", . , "Smart Windows Callbacks"
  SwitchText xd,  "None", .
  SwitchText xs,  "Normal exception handling", .
  SwitchText xst, "Direct calls for destruction", .
  SwitchText xss, "Table driven destructors", .
  SwitchText zmf, "Functions in separate segments.", . , "Functions in Separate Segs.", "Functions in separate segs."
  SwitchText xr,  "Enable RTTI", .
  SwitchText zv,  "Virtual function removal", . , "Virtual Function Removal"
  SwitchText ft,  "Check for truncated filenames", .
  SwitchText fx,  "Do NOT check for truncated filenames", . , "Do NOT Check for truncated filenames"
  Family "C++ Compiler"
    CSwitch 0, ?????, ew, -ew, OFF
    CSwitch 0, ?????, wx, -wx, OFF
    CSwitch 0, wm?e?, zws, -zws, ON
    CSwitch 0, wa?e?, zws, -zws, ON
    CSwitch 0, w?6??, zws, -zws, OFF
    RGroup "Exception handling"
        RSwitch 0, ?????, xd, -xd, OFF
        RSwitch 0, ?????, xs, -xs, ON
        RSwitch 0, ?????, xst, -xst, OFF
        RSwitch 0, ?????, xss, -xss, OFF
    CSwitch 0, wm6??, zmf, -zmf, ON
    CSwitch 0, wa6??, zmf, -zmf, ON
    CSwitch 0, ?????, zmf, -zmf, OFF
    CSwitch 1, ?????, xr, -xr, ON
    CSwitch 1, ?????, zv, -zv, OFF
    CSwitch 1, ?????, ft, -ft, OFF
    CSwitch 1, ?????, fx, -fx, OFF

Tool WCC "C Compiler"
  IncludeTool WLANG
  IncludeTool CCOMP
  Family Compiler
    CSwitch 0, ??6??, "", "wcc $@", ON
    CSwitch 0, ??2??, "", "wcc386 $@", ON
    CSwitch 0, ??a??, "", "wccaxp $@", ON

Tool WPP "C++ Compiler"
  IncludeTool WLANG
  IncludeTool CPPCOMP
  Family Compiler
    CSwitch 0, ??6??, "", "wpp $@", ON
    CSwitch 0, ??2??, "", "wpp386 $@", ON
    CSwitch 0, ??a??, "", "wppaxp $@", ON
::
:segment !C_FOR_PB
::

Tool WFOR "Fortran Compiler"
  SwitchText defcompil, "Compiler default", . , "Compiler Default"
  SwitchText nolist,    "No listing file", .
  SwitchText lispr,     "Listing file to printer", .
  SwitchText listy,     "Listing file to screen", .
  SwitchText lisdi,     "Listing file to disk", .
  SwitchText incl,      "Include files in listing file", .
  SwitchText noer,      "Generate error file", "", .
  SwitchText incp,      "Path for include files:", .
  SwitchText exp,       "Require symbol declaration", .
  SwitchText ex,        "Warn when extensions used", .
  SwitchText nor,       "Warn about unreferenced symbols", . , "Warn about unref'ed symbols"
  SwitchText wil,       "Ignore wild branches", .
  SwitchText nowa,      "Enable warning messages", .
  SwitchText note,      "Print diagnostics to the screen", .
  SwitchText st,        "Perform stack checking", .
  SwitchText for,       "Perform format type checking", .
  SwitchText tr,        "Generate runtime trace backs", . , "Generate run-time trace backs"
  SwitchText bo,        "Add runtime bounds checks", . , "Add run-time bounds checks"
  SwitchText res,       "Store messages as resources", .
  SwitchText nodebuginfo, "No debugging information", .
  SwitchText d1,        "Line number information", .
  SwitchText d2,        "Full debugging information", .
  SwitchText hw,        "Watcom debugging format", . , "WATCOM debugging format"
  SwitchText hc,        "CodeView debugging format", . , "Codeview debugging format"
  SwitchText hd,        "DWARF debugging format", .
  SwitchText 3,         "Assume 80386", .
  SwitchText 4,         "Optimize for 80486", .
  SwitchText 5,         "Optimize for Pentium", .
  SwitchText 6,         "Optimize for Pentium Pro", .
  SwitchText mf,        "Flat model", .
  SwitchText ms,        "Small model", .
  SwitchText ml,        "Large model", .
  SwitchText mm,        "Medium model", .
  SwitchText fp2,       "Inline 80287 instructions", . , "In-line 80287 instructions"
  SwitchText fp3,       "Inline 80387 instructions", . , "In-line 80387 instructions"
  SwitchText fp5,       "Inline Pentium instructions", . , "In-line Pentium instructions"
  SwitchText fp6,       "Inline Pentium Pro instructions", . , "In-line Pentium Pro instructions"
  SwitchText fpc,       "Floating-point calls", .
  SwitchText fpi,       "Emulate 80x87 instructions", .
  SwitchText fpi87,     "Inline 80x87 instructions", . , "In-line 80x87 instructions"
  SwitchText fpr,       "80x87 reverse compatibility", .
  SwitchText dt,        "Set data threshold:", .
  SwitchText co,        "Put constants in code segment", .
  SwitchText sa,        "SAVE local variables", .
  SwitchText sh,        "Short INTEGER/LOGICAL size", .
  SwitchText al,        "Align COMMON segments", .
  SwitchText au,        "Local variables on the stack", .
  SwitchText des,       "Pass character descriptors", .
  SwitchText sr,        "Save segment registers", .
  SwitchText nogs,      "Compiler can use GS", .
  SwitchText nofs,      "Compiler can use FS", .
  SwitchText fs,        "Compiler can use FS", .
  SwitchText ss,        "SS points to DGROUP", .
  SwitchText od,        "Disable optimizations", .
  SwitchText ot,        "Optimize for time", .
  SwitchText os,        "Optimize for space", .
  SwitchText ox,        "Fastest possible code", .
  SwitchText ob,        "Base pointer optimizations", .
  SwitchText obp,       "Branch prediction", .
  SwitchText oc,        "No call-return optimizations", .
  SwitchText of,        "No stack frame optimizations", .
  SwitchText oh,        "Optimize at expense of compile-time", .
  SwitchText oi,        "Statement functions inline", . , "Statement functions in-line"
  SwitchText ok,        "Move register saves into flow path", .
  SwitchText ol,        "Loop optimizations", .
  SwitchText ol+,       "Loop unrolling", .
  SwitchText om,        "Math optimizations", .
  SwitchText on,        "Numerical optimizations", .
  SwitchText op,        "Precision optimizations", .
  SwitchText or,        "Instruction scheduling", .
  SwitchText odo,       "No loop overflow checking", .
  SwitchText bw,        "Default windowed application", .
  SwitchText bm,        "Multithreaded application", .
  SwitchText bd,        "Dynamic link library", .
  SwitchText win,       "Generate code for Windows", . , "Generate code for windows"
  SwitchText def,       "Define macro:", .
  SwitchText others,    "Other options(,):", .
  SwitchText sy,        "Syntax check only", .
  SwitchText xf,        "Extend floating-point precision", . , "Extend float-pt. precision"
  SwitchText q,         "Operate quietly", .
  SwitchText dep,       "Generate file dependencies", .
  SwitchText lf,        "LF with FF", .
  SwitchText nolib,     "Include default library info", .
  SwitchText cc,        "Unit 6 is a CC device", .
  SwitchText chi,       "Chinese character set", . , "Chineese character set"
  SwitchText j,         "Japanese character set", .
  SwitchText ko,        "Korean characer set", .
  SwitchText noxl,      "Standard length", .
  SwitchText xl,        "Extended length", .
  Family Compiler
    CSwitch 0, ??6??, "", "wfc $@", ON
    CSwitch 0, ??2??, "", "wfc386 $@", ON
    CSwitch 0, ??a??, "", "wfcaxp $@", ON
  Family "File Option"
    RGroup "Listing file"
        RSwitch 0, ?????, defcompil, , ON
        RSwitch 0, ?????, nolis, -nolis , OFF
        RSwitch 0, ?????, lispr, "-lis -pr" , OFF
        RSwitch 0, ?????, listy, "-lis -ty" , OFF
        RSwitch 0, ?????, lisdi, "-lis -di" , OFF
    CSwitch 0, ?????, incl, -incl, OFF
    C2Switch 0, ?????, noer, "", -noer, ON
    VSwitch 1, w????, incp, -incp, =, ONE, REQ, "\"$(%watcom)/src/fortran/win;$(%watcom)/src/fortran\""
    VSwitch 1, o????, incp, -incp, =, ONE, REQ, "\"$(%watcom)/src/fortran/os2;$(%watcom)/src/fortran\""
    VSwitch 1, d????, incp, -incp, =, ONE, REQ, "\"$(%watcom)/src/fortran/dos;$(%watcom)/src/fortran\""
    VSwitch 1, ?????, incp, -incp, =, ONE, REQ, ""
  Family "Compile-time Diagnostic"
    CSwitch 0, ?????, exp, -exp, OFF
    CSwitch 0, ?????, ex, -ex, OFF
    C2Switch 0, ?????, nor, "", -nor, ON
    CSwitch 0, ?????, wil, -wil, OFF
    C2Switch 0, ?????, nowa, "", -nowa, ON
    C2Switch 0, ?????, note, "", -note, ON
  Family "Run-time Diagnostic"
    CSwitch 0, ?????, st, -st, OFF
    CSwitch 0, ?????, for, -for, OFF
    CSwitch 0, ?????, tr, -tr, OFF
    CSwitch 0, ?????, bo, -bo, OFF
    CSwitch 0, ?????, res, -res, OFF
  Family "Debugging"
    RGroup "Debugging style"
        RSwitch 0, ?????, nodebuginfo, , OFF
        RSwitch 0, ?????, d1, -d1, ON, OFF
        RSwitch 0, ?????, d2, -d2, OFF, ON
    RGroup "Debugging format"
        RSwitch 0, ?????, defcompil, , ON
        RSwitch 0, ?????, hw, -hw, OFF
        RSwitch 0, ?????, hc, -hc, OFF
        RSwitch 0, ?????, hd, -hd, OFF
  Family "Code Generation Strategy"
    RGroup "Target CPU"
      RSwitch 0, ?????, defcompil, , ON
      RSwitch 0, ?????, 3, -3 , OFF
      RSwitch 0, ?????, 4, -4 , OFF
      RSwitch 0, ?????, 5, -5 , OFF
      RSwitch 0, ?????, 6, -6 , OFF
    RGroup "Memory model"
      RSwitch 0, ?????, defcompil, , ON
      RSwitch 0, ??2??, mf, -mf, OFF
      RSwitch 0, ??2??, ms, -ms, OFF
      RSwitch 0, ??6??, ml, -ml, OFF
      RSwitch 0, ??6??, mm, -mm, OFF
    RGroup "Floating-point level"
      RSwitch 1, ?????, defcompil, , ON
      RSwitch 1, ?????, fp2, -fp2, OFF
      RSwitch 1, ?????, fp3, -fp3, OFF
      RSwitch 1, ?????, fp5, -fp5, OFF
      RSwitch 1, ?????, fp6, -fp6, OFF
    RGroup "Floating-point model"
      RSwitch 1, r????, defcompil, , OFF
      RSwitch 1, ?????, defcompil, , ON
      RSwitch 1, ?????, fpc, -fpc, OFF
      RSwitch 1, ?????, fpi, -fpi, OFF
      RSwitch 1, r????, fpi87, -fpi87, ON
      RSwitch 1, ?????, fpi87, -fpi87, OFF
    CSwitch 1, ?????, fpr, -fpr OFF
  Family "Code Generation Option"
      VSwitch 0, ?????, dt, -dt, =, ONE, REQ, ""
      CSwitch 0, ?????, co, -co, OFF
      CSwitch 0, ?????, sa, -sa, OFF
      CSwitch 0, ?????, sh, -sh, OFF
      CSwitch 0, ?????, al, -al, OFF
      CSwitch 0, ?????, au, -au, OFF
      CSwitch 0, ?????, des, -des, OFF
  Family "Register Usage"
      CSwitch 0, ?????, sr, -sr, OFF
      C2Switch 0, ?????, nogs, "", -nogs, ON
      C2Switch 0, ??6??, nofs, "", -nofs, ON
      C2Switch 0, ?????, fs, -fs, "", OFF
      C2Switch 0, o?6??, ss, "", -ss, OFF
      C2Switch 0, ??6??, ss, "", -ss, ON
  Family "Optimizations"
    RGroup "Optimization Style"
          RSwitch 0, ?????, defcompil, , ON
          RSwitch 0, ?????, od, -od , OFF
          RSwitch 0, ?????, ot, -ot, OFF
          RSwitch 0, ?????, os, -os , OFF
          RSwitch 0, ?????, ox, -ox , OFF
      CSwitch 1, ?????, ob, -ob, OFF
      CSwitch 1, ?????, obp, -obp, OFF
      CSwitch 1, ?????, oc, -oc, OFF
      CSwitch 1, ?????, of, -of, OFF
      CSwitch 1, ?????, oh, -oh, OFF
      CSwitch 1, ?????, oi, -oi, OFF
      CSwitch 1, ?????, ok, -ok, OFF
      CSwitch 1, ?????, ol, -ol, OFF
      CSwitch 1, ?????, ol+, -ol+, OFF
      CSwitch 1, ?????, om, -om, OFF
      CSwitch 1, ?????, on, -on, OFF
      CSwitch 1, ?????, op, -op, OFF
      CSwitch 1, ?????, or, -or, OFF
      CSwitch 1, ?????, odo, -odo, OFF
    Family "Application Type"
      CSwitch 0, w????, bw, -bw, OFF
      CSwitch 0, r????, bm, -bm, ON
      CSwitch 0, ?????, bm, -bm, OFF
      CSwitch 0, ???d?, bd, -bd, ON
      CSwitch 0, ?????, bd, -bd, OFF
      CSwitch 0, w????, win, -win, ON
      CSwitch 0, ?????, win, -win, OFF
    Family "Miscellaneous"
      VSwitch 0, ?????, def, -def, =, ONE, REQ, ""
      VSwitch  0, ?????, others,,, MULTI, REQ, ""
      CSwitch 0, ?????, sy, -sy, OFF
      CSwitch 0, ?????, xf, -xf, OFF
      CSwitch 0, ?????, q, -q, ON
      CSwitch 0, ?????, dep, -dep, ON
      CSwitch 0, ?????, lf, -lf, OFF
      C2Switch 0, ?????, nolib, "", -nolib, ON
      CSwitch 0, ?????, cc, -cc, OFF
      RGroup "Character set"
        RSwitch 1, ?????, defcompil, , ON
        RSwitch 1, ?????, chi, -chi, OFF
        RSwitch 1, ?????, j, -j, OFF
        RSwitch 1, ?????, ko, -ko , OFF
      RGroup "Line length"
        RSwitch 1, ?????, defcompil, , ON
        RSwitch 1, ?????, noxl, -noxl , OFF
        RSwitch 1, ?????, xl, -xl , OFF
::
:endsegment
::

Tool WASM "Assembler"
  SwitchText defcompil, "Compiler default", .
  SwitchText fi,        "Include file:", .
  SwitchText i,         "Include directories:", .
  SwitchText fe,        "Name error file:", .
  SwitchText mt,        "Tiny model", .
  SwitchText ms,        "Small model", . , "32-bit small model" , "32bit Small model"
  SwitchText mm,        "Medium model", .
  SwitchText mc,        "Compact model", .
  SwitchText ml,        "Large model", .
  SwitchText mh,        "Huge model", .
  SwitchText mf,        "Flat model", . , "32-bit flat model" , "32bit Flat model"
  SwitchText 0,         "8086", .
  SwitchText 1,         "80186", .
  SwitchText 2,         "80286", .
  SwitchText 3,         "80386", .
  SwitchText 4,         "80486", .
  SwitchText 5,         "Pentium", .
  SwitchText 6,         "Pentium Pro", .
  SwitchText 3r,        "80386 register-based calling", . , "80386 Register based calling"
  SwitchText 3s,        "80386 stack-based calling", . , "80386 Stack based calling"
  SwitchText 4r,        "80486 register-based calling", . , "80486 Register based calling"
  SwitchText 4s,        "80486 stack-based calling", . , "80486 Stack based calling"
  SwitchText 5r,        "Pentium register-based calling", . , "Pentium Register based calling"
  SwitchText 5s,        "Pentium stack-based calling", . , "Pentium Stack based calling"
  SwitchText 6r,        "Pentium Pro register-based calling", . , "Pentium Pro Register based calling"
  SwitchText 6s,        "Pentium Pro stack-based calling", . , "Pentium Pro Stack based calling"
  SwitchText nd,        "Name of data segment:", . , "Name Data Segment:"
  SwitchText nt,        "Name of text segment:", . , "Name Text Segment:"
  SwitchText nm,        "Name of module:", . , "Name of Module:"
  SwitchText zcm,       "Mangle C names like MASM", .
  SwitchText o,         "Allow C-style octal constants", . , "Allow C-style octal consants"
  SwitchText fp0,       "Inline 8087 instructions", . , "Inline 8087 instructions"
  SwitchText fp2,       "Inline 80287 instructions", . , "Inline 80287 instructions"
  SwitchText fp3,       "Inline 80387 instructions", . , "Inline 80387 instructions"
  SwitchText fp5,       "Inline Pentium instructions", . , "Inline Pentium instructions"
  SwitchText fp6,       "Inline Pentium Pro instructions", . , "Inline Pentium Pro instructions"
  SwitchText fpi,       "Inline with emulator", . , "Inline with emulator"
  SwitchText fpi87,     "Inline with coprocessor", . , "Inline with coprocessor"
  SwitchText fpc,       "Floating-point calls", .
  SwitchText nodebuginfo, "No debugging information", .
  SwitchText d1,        "Line number information", .
  SwitchText w0,        "Warning level 0", .
  SwitchText w1,        "Warning level 1", .
  SwitchText w2,        "Warning level 2", .
  SwitchText w3,        "Warning level 3", .
  SwitchText w4,        "Warning level 4", .
  SwitchText we,        "Treat warnings as errors", .
  SwitchText e#,        "Error count:", .
  SwitchText ef,        "Full pathnames in error messages:", .
  SwitchText e,         "Stop reading at END", .
  SwitchText j,         "Signed types for signed values", .
  SwitchText zq,        "Quiet operation", .
  SwitchText d,         "Macro definitions:", .
  SwitchText c,         "No data in code records", .
  SwitchText others,    "Other options:", .
  Family "File Option"
    CSwitch 0, ?????, "", "wasm $@", ON
    VSwitch 0, ?????, fi, -fi, =, MULTI, REQ, ""
    VSwitch 0, w????, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/win\""
    VSwitch 0, d????, i, -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, o?6??, i, -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, o?2??, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%toolkit)/c/os2h\""
    VSwitch 0, n????, i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt\""
    VSwitch 0, l????, i, -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, x????, i, -i, =, MULTI, REQ, "\"$(%watcom)/lh\""
    VSwitch 0, ?????, fe, -fe, =, ONE, REQ, ""
  Family "Memory Model and Processor"
    RGroup "Memory model"
        RSwitch 0, ??6??, mt, -mt, OFF
        RSwitch 0, rk6??, ms, -ms, ON
        RSwitch 0, rb6??, ms, -ms, ON
        RSwitch 0, rm6??, ms, -ms, ON
        RSwitch 0, ??6??, ms, -ms, OFF
        RSwitch 0, ??6??, mm, -mm, OFF
        RSwitch 0, ??6??, mc, -mc, OFF
        RSwitch 0, rk6??, ml, -ml, OFF
        RSwitch 0, rb6??, ml, -ml, OFF
        RSwitch 0, rm6??, ml, -ml, OFF
        RSwitch 0, ??6??, ml, -ml, ON
        RSwitch 0, ??6??, mh, -mh, OFF
        RSwitch 0, ??2??, mf, -mf, ON
        RSwitch 0, ??2??, ms, -ms, OFF
    RGroup "Target processor"
        RSwitch 1, rk6??, 0, ,  OFF
        RSwitch 1, rb6??, 0, ,  OFF
        RSwitch 1, rm6??, 0, ,  OFF
        RSwitch 1, ??6??, 0, ,  ON
        RSwitch 1, ??6??, 1, -1, OFF
        RSwitch 1, rk6??, 2, -2, ON
        RSwitch 1, ??6??, 2, -2, OFF
        RSwitch 1, rb6??, 3, -3, ON
        RSwitch 1, rm6??, 3, -3, ON
        RSwitch 1, ??6??, 3, -3, OFF
        RSwitch 1, ??6??, 4, -4, OFF
        RSwitch 1, ??6??, 5, -5, OFF
        RSwitch 1, ??6??, 6, -6, OFF
        RSwitch 1, ??2??, 3r, -3r, OFF
        RSwitch 1, ??2??, 3s, -3s, OFF
        RSwitch 1, ??2??, 4r, -4r, OFF
        RSwitch 1, ??2??, 4s, -4s, OFF
        RSwitch 1, ??2??, 5r, -5r, OFF
        RSwitch 1, ??2??, 5s, -5s, OFF
        RSwitch 1, ds2??, 6r, -6r, OFF
        RSwitch 1, dy2??, 6r, -6r, OFF
        RSwitch 1, ??2??, 6r, -6r, ON
        RSwitch 1, ds2??, 6s, -6s, ON
        RSwitch 1, dy2??, 6s, -6s, ON
        RSwitch 1, ??2??, 6s, -6s, OFF
  Family "Code Generation Options"
    VSwitch 0, ?????, nd, -nd,, ONE, REQ, ""
    VSwitch 0, ?????, nt, -nt,, ONE, REQ, ""
    VSwitch 0, ?????, nm, -nm,, ONE, REQ, ""
    CSwitch 0, ?????, zcm, -zcm
    CSwitch 0, ?????, o, -o, OFF
    RGroup "Floating-point level"
        RSwitch 1, ?????, defcompil, , ON
        RSwitch 1, ?????, fp0, -fp0, OFF
        RSwitch 1, ?????, fp2, -fp2, OFF
        RSwitch 1, ?????, fp3, -fp3, OFF
        RSwitch 1, ?????, fp5, -fp5, OFF
        RSwitch 1, ?????, fp6, -fp6, OFF
    RGroup "Floating-point model"
        RSwitch 1, r????, defcompil, , OFF
        RSwitch 1, ?????, defcompil, , ON
        RSwitch 1, ?????, fpi, -fpi, OFF
        RSwitch 1, r????, fpi87, -fpi87, ON
        RSwitch 1, ?????, fpi87, -fpi87, OFF
        RSwitch 1, ?????, fpc, -fpc, OFF
  Family Debugging
    RGroup "Debugging style"
        RSwitch 0, ?????, nodebuginfo, , ON, OFF
        RSwitch 0, ?????, d1, -d1, OFF, ON
  Family Diagnostics
    RGroup "Warning level"
        RSwitch 0, ?????, w0, -w0, OFF
        RSwitch 0, ?????, w1, -w1, OFF
        RSwitch 0, ?????, w2, -w2, OFF
        RSwitch 0, ?????, w3, -w3, OFF
        RSwitch 0, ?????, w4, -w4, ON
    CSwitch 0, ?????, we, -we, OFF
    VSwitch 0, ?????, e#, -e,, ONE, REQ, "25"
    CSwitch 0, ?????, ef, -ef
  Family Miscellaneous
    CSwitch 0, ?????, e, -e, OFF
    CSwitch 0, ?????, j, -j, OFF
    CSwitch 0, ?????, zq, -zq, ON
    VSwitch 0, ?????, d, -d,, MULTI, REQ, ""
    CSwitch 0, ?????, c, -c, OFF
    VSwitch 0, ?????, others,,, MULTI, REQ, ""

Tool WLINK "Linker"
  SwitchText nodebuginfo, "No debug information", .
  SwitchText dcodeview,   "Debug CodeView", . , "Debug Codeview"
  SwitchText ddwarf,      "Debug Dwarf", .
  SwitchText dnovell,     "Netware symbols", .
  SwitchText dwatcom,     "Debug Watcom", .
  SwitchText dlines,      "Debug line numbers", .
  SwitchText dtypes,      "Debug types", .
  SwitchText dlocals,     "Debug locals", .
  SwitchText dall,        "Debug all", .
  SwitchText sys,         "System:", .
  SwitchText opscr,       "Output screen", .
  SwitchText opstack,     "Stack:", .
  SwitchText opheap,      "Heap:", .
  SwitchText opoffset,    "Image base:", .
  SwitchText opcodesel,   "Code selector:", .
  SwitchText opdatasel,   "Data selector:", .
  SwitchText others,      "Other options(,):", .
  SwitchText opmap,       "Map file", .
  SwitchText opmaplines,  "Line numbers in map file", .
  SwitchText opinc,       "Incremental linking", . , "Incremental Linking"
  SwitchText opnostdcall, "No stdcall name decoration", . , "No Stdcall Name Decoration"
  SwitchText opnod,       "No default libraries", .
  SwitchText libpath,     "Library directories(;):", .
  SwitchText library,     "Libraries(,):", .
  SwitchText libfile,     "Library files(,):", .
  SwitchText import,      "Import names(,):", . , "Import files(,):"
  SwitchText export,      "Export names(,):", .
  SwitchText opverbose,   "Verbose map file", . , "Verbose", "Verbose Map file"
  SwitchText defcasesens, "Default", . , "Linker default"
  SwitchText opcasesens,  "Case sensitive:", . , "Yes:"
  SwitchText opnocasesens, "No case sensitive:", . , "No:"
  SwitchText opmaxe,      "Error count:", .
  SwitchText oprwr,       "Check relocation of RW data", .
  SwitchText opdosseg,    "Dosseg", .
  SwitchText opundef,     "Undefines OK", . , "Undefines ok"
  SwitchText opquiet,     "Quiet", .
  SwitchText opsymf,      "Produce symbol file", .
  SwitchText opvers,      "Version (Major.minor.rev):", .
  SwitchText opnamel,     "Name length:", .
  SwitchText modt,        "ModTrace names(,):", .
  SwitchText symt,        "SymTrace names(,):", .
  SwitchText ref,         "Reference names(,):", .
  SwitchText opstart,     "Starting address:", . , "Starting Address:"
  SwitchText opdesc,      "Description:", .
  SwitchText defdgroupsharing, "Use linker default", .
  SwitchText opone,       "DGROUP is shared", .
  SwitchText opmany,      "DGROUP is not shared", .
  SwitchText defguimode,  "Default GUI", . , "Native NT", "Windows 95"
  SwitchText ruwin310,    "Win32s Windows 3.x:", . , "Windows 3.x"
  SwitchText defcharmode, "Default character mode", .
  SwitchText ruos2310,    "16-bit OS/2 1.x:", . , "16Bit OS/2 1.x", "16Bit OS/2 1.x:"
  SwitchText rucon40,     "Character mode:", .
  SwitchText opstub,      "Stub executable", .
  SwitchText opelim,      "Eliminate dead code", .
  SwitchText cmd_name,    " name", .
  SwitchText cmd_linkdll, " Link DLL", .
  SwitchText cmd_initinst, " InitInst", .
  SwitchText cmd_terminst, " TermInst", .
  SwitchText cmd_mem,     " Mem", .
  Family Basic
    CSwitch  0, ?????, "", "wlink", ON
    CSwitch  0, ?????, cmd_name, "name $'", ON
    RGroup "Debugging information"
        RSwitch  0, ?????, nodbuginfo, , ON, OFF
        RSwitch  0, ?????, dcodeview, "d codeview op cvp", OFF
        RSwitch  0, ?????, ddwarf, "d dwarf", OFF
        RSwitch  0, l?2e?, dnovell, "d novell", OFF
        RSwitch  0, ?????, dwatcom, "d watcom", OFF
        RSwitch  0, ?????, dlines, "d lines", OFF
        RSwitch  0, ?????, dtypes, "d types", OFF
        RSwitch  0, ?????, dlocals, "d locals", OFF
        RSwitch  0, ?????, dall, "d all", OFF, ON
    VSwitch  1, w?6d?, sys, sys, " ", ONE, REQ, windows_dll
    VSwitch  1, w?6??, sys, sys, " ", ONE, REQ, windows
    VSwitch  1, d?6??, sys, sys, " ", ONE, REQ, dos
    CSwitch  1, dc6e?, "", com, ON
    VSwitch  1, o?6??, sys, sys, " ", ONE, REQ, os2
rem OS/2 dll?
    VSwitch  1, w?2??, sys, sys, " ", ONE, REQ, win386
    VSwitch  1, dr2??, sys, sys, " ", ONE, REQ, dos4g
    VSwitch  1, dw2??, sys, sys, " ", ONE, REQ, causeway
    VSwitch  1, dx2??, sys, sys, " ", ONE, REQ, cwdllr
    VSwitch  1, dy2??, sys, sys, " ", ONE, REQ, cwdlls
    VSwitch  1, dm2??, sys, sys, " ", ONE, REQ, pmodew
    VSwitch  1, da2??, sys, sys, " ", ONE, REQ, dos32a
    VSwitch  1, db2??, sys, sys, " ", ONE, REQ, dos32x
    VSwitch  1, dp2??, sys, sys, " ", ONE, REQ, pharlap
    VSwitch  1, dt2??, sys, sys, " ", ONE, REQ, tnt
    VSwitch  1, o?2??, sys, sys, " ", ONE, REQ, os2v2
    CSwitch  1, op?e?, "", pm, ON
    CSwitch  1, of?e?, "", full, ON
    CSwitch  1, od6e?, "", phys, ON
    CSwitch  1, ov2e?, "", virt, ON
    VSwitch  1, nw2??, sys, sys, " ", ONE, REQ, nt_win
    VSwitch  1, na2e?, sys, sys, " ", ONE, REQ, nt_win
    VSwitch  1, nu2e?, sys, sys, " ", ONE, REQ, nt_win
    VSwitch  1, no2e?, sys, sys, " ", ONE, REQ, nt_win
    VSwitch  1, nm2e?, sys, sys, " ", ONE, REQ, nt_win
    VSwitch  1, nm2d?, sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, na2d?, sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, no2d?, sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, nu2d?, sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, np2d?, sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, n_2d?, sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, nc2??, sys, sys, " ", ONE, REQ, nt
    VSwitch  1, nwa??, sys, sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, naae?, sys, sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, nuae?, sys, sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, noae?, sys, sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, nmae?, sys, sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, nmad?, sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, naad?, sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, noad?, sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, nuad?, sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, npad?, sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, n_ad?, sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, nca??, sys, sys, " ", ONE, REQ, ntaxp
    VSwitch  1, l?2??, sys, sys, " ", ONE, REQ, netware
rem Other Netware variations?
    VSwitch  1, df2??, sys, sys, " ", ONE, REQ, x32rv
    VSwitch  1, df2??, sys, sys, " ", ONE, REQ, x32r
    VSwitch  1, ds2??, sys, sys, " ", ONE, REQ, x32sv
    VSwitch  1, ds2??, sys, sys, " ", ONE, REQ, x32s
    VSwitch  1, x?2??, sys, sys, " ", ONE, REQ, linux
    VSwitch  1, rp2e?, sys, sys, " ", ONE, REQ, rdos
    VSwitch  1, rp2d?, sys, sys, " ", ONE, REQ, rdos_dll
    VSwitch  1, rk6??, sys, sys, " ", ONE, REQ, rdos_dev16
    VSwitch  1, rk2??, sys, sys, " ", ONE, REQ, rdosdev
    VSwitch  1, rb6??, sys, sys, " ", ONE, REQ, rdos_bin16
    VSwitch  1, rb2??, sys, sys, " ", ONE, REQ, rdos_bin32
    VSwitch  1, rm6??, sys, sys, " ", ONE, REQ, rdos_mboot
    CSwitch  1, d?2d?, cmd_linkdll, , ON
    CSwitch  1, w?2d?, cmd_linkdll, , ON
    CSwitch  1, w?6d?, cmd_linkdll, , ON
    CSwitch  1, n??d?, cmd_linkdll, , ON
    CSwitch  1, r??d?, cmd_linkdll, , ON
rem    CSwitch  1, x??d?, cmd_linkdll, , ON
    CSwitch  1, ???d?, cmd_linkdll, dll, ON
    CSwitch  1, wm6d?, cmd_initinst, initi, ON
    CSwitch  1, wa6d?, cmd_initinst, initi, ON
    CSwitch  1, na?d?, cmd_initinst, initi, ON
    CSwitch  1, nm?d?, cmd_initinst, initi, ON
    CSwitch  1, no?d?, cmd_initinst, initi, ON
    CSwitch  1, nu?d?, cmd_initinst, initi, ON
    CSwitch  1, o?2d?, cmd_initinst, initi, ON
    CSwitch  1, o?2d?, cmd_terminst, termi, ON
    CSwitch  1, nu?d?, cmd_terminst, termi, ON
    CSwitch  1, na?d?, cmd_terminst, termi, ON
    CSwitch  1, no?d?, cmd_terminst, termi, ON
    CSwitch  1, nm?d?, cmd_terminst, termi, ON
    CSwitch  1, wm6d?, cmd_mem, mem, ON
    CSwitch  1, wa6d?, cmd_mem, mem, ON
    VSwitch  1, l?2e?, opscr, "op scr", " '%s'", ONE, OFF, ""
    VSwitch  1, ?????, opstack, "op st", =, ONE, REQ, ""
    VSwitch  1, wm?e?, opheap, "op heap", =, ONE, REQ, 1K
    VSwitch  1, wa?e?, opheap, "op heap", =, ONE, REQ, 1K
    VSwitch  1, rp???, opoffset, "op offset", =, ONE, REQ, ""
    VSwitch  1, rk???, opcodesel, "op codesel", =, ONE, REQ, ""
    VSwitch  1, rk???, opdatasel, "op datasel", =, ONE, REQ, ""
    VSwitch  1, ?????, others,,, MULTI, REQ, ""
    CSwitch  1, ?????, opmap, "op m", ON
    CSwitch  1, ?????, opmaplines, "op maplines", OFF
    CSwitch  1, l????, opinc, "op inc", OFF
    CSwitch  1, d?6??, opinc, "op inc", OFF
    CSwitch  1, ????w, opinc, "op inc", OFF
    CSwitch  1, ????j, opinc, "op inc", OFF
    CSwitch  1, ????8, opinc, "op inc", OFF
    CSwitch  1, o????, opinc, "op inc", OFF
    CSwitch  1, na???, opinc, "op inc", OFF, ON
    CSwitch  1, no???, opinc, "op inc", OFF, ON
    CSwitch  1, nu???, opinc, "op inc", OFF, ON
    CSwitch  1, nm???, opinc, "op inc", OFF, ON
    CSwitch  1, wa???, opinc, "op inc", OFF, ON
    CSwitch  1, wm???, opinc, "op inc", OFF, ON
    CSwitch  1, ?????, opinc, "op inc", OFF
    CSwitch  1, n????, opnostdcall, "op nostdcall", OFF
  Family "Import, Export and Library"
    CSwitch  0, ?????, opnod, "op nod", OFF
    VSwitch  0, ?????, libpath, libp, " ", MULTI, REQ, ""
    VSwitch  0, wa6d?, library, libr, " ", MULTI, REQ, "mfw250.lib mfwo250.lib mfwd250.lib commdlg.lib shell.lib mfcoleui.lib compobj.lib storage.lib ole2.lib ole2disp.lib", "mfw250d.lib mfwo250d.lib mfwd250d.lib commdlg.lib shell.lib mfcoleui.lib compobj.lib storage.lib ole2.lib ole2disp.lib"
    VSwitch  0, ?????, library, libr, " ", MULTI, REQ, ""
    VSwitch  0, dc6e?, libfile, libf, " ", MULTI, REQ, "cstart_t"
    VSwitch  0, ???e?, libfile, libf, " ", MULTI, REQ, ""
    VSwitch  1, w????, import, imp, " ", ONE, REQ, ""
    VSwitch  1, n????, import, imp, " ", ONE, REQ, ""
    VSwitch  1, o????, import, imp, " ", ONE, REQ, ""
    VSwitch  1, l????, import, imp, " ", ONE, REQ, ""
    VSwitch  1, x????, import, imp, " ", ONE, REQ, ""
    VSwitch  1, dx???, import, imp, " ", ONE, REQ, ""
    VSwitch  1, dy???, import, imp, " ", ONE, REQ, ""
    VSwitch  1, rp???, import, imp, " ", ONE, REQ, ""
    VSwitch  1, w????, export, exp, " ", MULTI, REQ, ""
    VSwitch  1, n????, export, exp, " ", MULTI, REQ, ""
    VSwitch  1, o????, export, exp, " ", MULTI, REQ, ""
    VSwitch  1, l????, export, exp, " ", MULTI, REQ, ""
    VSwitch  1, x????, export, exp, " ", MULTI, REQ, ""
    VSwitch  1, dx???, export, exp, " ", MULTI, REQ, ""
    VSwitch  1, dy???, export, exp, " ", MULTI, REQ, ""
    VSwitch  1, rp???, export, exp, " ", MULTI, REQ, ""
  Family Advanced
    CSwitch  0, ?????, opverbose, "op v", OFF
    RGroup "Case-sensitive link"
        RSwitch 0, ?????, defcasesens, , ON
        RSwitch 0, ?????, opcasesens, "op c", OFF
        RSwitch 0, ?????, opnocasesens, "op nocase", OFF
    VSwitch  0, ?????, opmaxe, "op maxe", =, ONE, REQ, 25
    CSwitch  0, w?6??, oprwr, "op rwr", OFF
    CSwitch  0, ?????, opdosseg, "op d", OFF
    CSwitch  0, ?????, opundef, "op u", OFF
    CSwitch  0, ?????, opquiet, "op q", ON
:segment !C_FOR_PB
    CSwitch  0, ?????, opsymf, "op symf", ON
    VSwitch  0, w????, opvers, "op vers", = , ONE, REQ, ""
    VSwitch  0, n????, opvers, "op vers", = , ONE, REQ, ""
    VSwitch  0, o????, opvers, "op vers", = , ONE, REQ, ""
    VSwitch  0, l????, opvers, "op vers", = , ONE, REQ, ""
:endsegment
    VSwitch  1, ?????, opnamel, "op namel", =, ONE, REQ, ""
:segment !C_FOR_PB
    VSwitch  1, ?????, modt, modt, " ", ONE, REQ, ""
    VSwitch  1, ?????, symt, symt, " ", ONE, REQ, ""
    VSwitch  1, nm?d?, ref, ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, na?d?, ref, ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, nu?d?, ref, ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, no?d?, ref, ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, nm?e?, ref, ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, na?e?, ref, ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, no?e?, ref, ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, nu?e?, ref, ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, wm?d?, ref, ref, " ", MULTI, REQ, "__clib_WEP_"
    VSwitch  1, wa?d?, ref, ref, " ", MULTI, REQ, "__clib_WEP_"
    VSwitch  1, ?????, ref, ref, " ", MULTI, REQ, ""
    VSwitch  1, nm?e?, opstart, "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, na?e?, opstart, "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, no?e?, opstart, "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, nu?e?, opstart, "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, ?????, opstart, "op start", =, ONE, REQ, ""
:endsegment
    VSwitch  1, o????, opdesc, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, n????, opdesc, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, w????, opdesc, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dr???, opdesc, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dw???, opdesc, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, da???, opdesc, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, db???, opdesc, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dm???, opdesc, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dx???, opdesc, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dy???, opdesc, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, r????, opdesc, "op de", " '%s'", ONE, REQ, ""
rem PackCode, PackData for DOS, OS/2, QNX, Win16?
:segment !C_FOR_PB
  Family Special
    RGroup "DGROUP sharing"
        RSwitch  0, w?2d?, defdgroupsharing, "", ON
        RSwitch  0, n??d?, defdgroupsharing, "", ON
rem     RSwitch  0, x??d?, defdgroupsharing, "", ON
        RSwitch  0, w?6d?, defdgroupsharing, "", ON
        RSwitch  0, r??d?, defdgroupsharing, "", ON
        RSwitch  0, ???d?, defdgroupsharing, "", OFF
        RSwitch  0, ?????, defdgroupsharing, "", ON
        RSwitch  0, w?2d?, opone, "op one", OFF
        RSwitch  0, n??d?, opone, "op one", OFF
rem     RSwitch  0, x??d?, opone, "op one", OFF
        RSwitch  0, w?6d?, opone, "op one", OFF
        RSwitch  0, o?2d?, opone, "op one", OFF
        RSwitch  0, ???d?, opone, "op one", OFF
        RSwitch  0, ?????, opone, "op one", OFF
        RSwitch  0, n??d?, opmany, "op many", OFF
        RSwitch  0, w??d?, opmany, "op many", OFF
        RSwitch  0, r??d?, opmany, "op many", OFF
        RSwitch  0, ???d?, opmany, "op many", ON
        RSwitch  0, ?????, opmany, "op many", OFF
    RGroup "Runtime environment"
        RSwitch  0, nw???, defguimode, "", ON
        RSwitch  0, nw???, ruwin310, "ru win=3.10", OFF
        RSwitch  0, nc???, defcharmode, "", ON
        RSwitch  0, nc???, ruos2310, "ru os2=3.10", OFF
        RSwitch  0, n?2d?, defguimode, "", ON
        RSwitch  0, n?2d?, rucon40, "ru con=4.0", OFF
        RSwitch  0, n?2d?, ruwin310, "ru win=3.10", OFF
        RSwitch  0, r????, defcharmode, "", ON
    VSwitch  0, o????, opstub, "op stub", =, ONE, REQ, ""
    CSwitch  0, na?e?, opelim, "op el", ON, OFF
    CSwitch  0, no?e?, opelim, "op el", ON, OFF
    CSwitch  0, nu?e?, opelim, "op el", ON, OFF
    CSwitch  0, nm?e?, opelim, "op el", ON, OFF
    CSwitch  0, wm?e?, opelim, "op el", ON, OFF
    CSwitch  0, wa?e?, opelim, "op el", ON, OFF
    CSwitch  0, ?????, opelim, "op el", OFF
:endsegment
::
:segment !C_FOR_PB
::

Tool WLIB "Library Manager"
  SwitchText b,      "Don't create .bak file", .
  SwitchText c,      "Case sensitive", .
  SwitchText m,      "Display C++ mangled names", .
  SwitchText n,      "Always create new library", .
  SwitchText q,      "Quiet operation", .
  SwitchText s,      "Strip LINNUM records", .
  SwitchText t,      "Trim THEADR pathnames", .
  SwitchText x,      "Explode all objects in library", .
  SwitchText p,      "Page bound:", .
  SwitchText l,      "Listing file name:", .
  SwitchText d,      "Output directory:", .
  SwitchText o,      "Output library name:", .
  SwitchText noimport, "Do not import", .
  SwitchText impbyname, "Import by name", . , "Import by Name"
  SwitchText impbyord, "Import by ordinal", . , "Import by Ordinal"
  Family Basic
    CSwitch  0, ?????, "", "wlib", ON
    CSwitch  0, ???s?, b, -b, ON
    CSwitch  0, ???s?, c, -c, ON
    CSwitch  0, ???s?, m, -m, OFF
    CSwitch  0, ???s?, n, -n, ON
    CSwitch  0, ???s?, q, -q, ON
    CSwitch  0, ???s?, s, -s, OFF
    CSwitch  0, ???s?, t, -t, OFF
    CSwitch  0, ???s?, x, -x, OFF
    VSwitch  1, ???s?, p, -p, =, ONE, REQ, 512
    VSwitch  1, ???s?, l, -l, =, ONE, REQ, ""
    VSwitch  1, ???s?, d, -d, =, ONE, REQ, ""
    VSwitch  1, ???s?, o, -o, =, ONE, REQ, ""
  Family "Import Library"
    RGroup "Resident symbols"
        RSwitch  0, ???s?, noimport, , ON
        RSwitch  0, ???s?, impbyname, -irn, OFF
        RSwitch  0, ???s?, impbyord, -iro, OFF
    RGroup "Non-resident symbols"
        RSwitch  0, ???s?, noimport, , ON
        RSwitch  0, ???s?, impbyname, -inn, OFF
        RSwitch  0, ???s?, impbyord, -ino, OFF
::
:endsegment
::

Tool WRC "Resource Compiler"
  SwitchText btwin,  "Win16", . , "Windows"
  SwitchText btnt,   "Win32", . , "Windows NT"
  SwitchText btos2,  "OS/2", .
  SwitchText btrdos, "RDOS", .
  SwitchText sbcs,   "Single byte characters only", .
  SwitchText zk0,    "Kanji", .
  SwitchText zk1,    "Traditional Chinese", .
  SwitchText zk2,    "Korean", .
  SwitchText zk3,    "Simplified Chinese", .
  SwitchText zku8,   "Unicode UTF-8", .
  SwitchText d,      "Macro definitions:", .
  SwitchText i,      "Include directories:", .
  SwitchText x,      "Ignore INCLUDE environment variable", . , "Ignore INCLUDE env variable"
  SwitchText zm,     "Microsoft format .res file", . , "Microsoft format .RES file"
  SwitchText zn,     "Do not preprocess the file", . , "Do not proprocess the file"
  SwitchText q,      "Quiet operation", .
  SwitchText ad,     "Output autodepend info", .
  Family Resource
    CSwitch 0, ?????, "", "wrc $*.rc", ON
    RGroup "Build target"
        RSwitch  0, w????, btwin, -bt=windows, ON
:segment !C_FOR_PB
        RSwitch  0, w????, btnt, -bt=nt, OFF
        RSwitch  0, w????, btos2, -bt=os2, OFF
        RSwitch  0, n????, btwin, -bt=windows, OFF
        RSwitch  0, n????, btnt, -bt=nt, ON
        RSwitch  0, n????, btos2, -bt=os2, OFF
        RSwitch  0, o????, btwin, -bt=windows, OFF
        RSwitch  0, o????, btnt, -bt=nt, OFF
        RSwitch  0, o????, btos2, -bt=os2, ON
        RSwitch  0, r????, btrdos, -bt=nt, ON
:endsegment
    RGroup "Multi-byte character support"
        RSwitch  0, ?????, sbcs, , ON
        RSwitch  0, ?????, zk0, -zk0, OFF
        RSwitch  0, ?????, zk1, -zk1, OFF
        RSwitch  0, ?????, zk2, -zk2, OFF
        RSwitch  0, ?????, zk3, -zk3, OFF
        RSwitch  0, ?????, zku8, -zku8, OFF
    VSwitch 1, wa???, d, -d, , MULTI, REQ, "_AFXDLL"
    VSwitch 1, na???, d, -d, , MULTI, REQ, "_AFXDLL WIN32 _WIN32 __NT__"
    VSwitch 1, no???, d, -d, , MULTI, REQ, "_AFXDLL _UNICODE WIN32 _WIN32 __NT__"
    VSwitch 1, nu???, d, -d, , MULTI, REQ, "_UNICODE WIN32 _WIN32 __NT__"
    VSwitch 1, n????, d, -d, , MULTI, REQ, "WIN32 _WIN32 __NT__"
    VSwitch 1, ?????, d, -d, , MULTI, REQ, ""
    VSwitch 1, w????, i, -i, =, MULTI, REQ, "\"$[:;$(%watcom)/h;$(%watcom)/h/win\""
    VSwitch 1, n????, i, -i, =, MULTI, REQ, "\"$[:;$(%watcom)/h;$(%watcom)/h/nt\""
    VSwitch 1, o????, i, -i, =, MULTI, REQ, "\"$[:;$(%watcom)/h;$(%watcom)/h/os2\""
    VSwitch 1, ?????, i, -i, =, MULTI, REQ, ""
    CSwitch 1, ?????, x, -x, OFF
    CSwitch 1, w????, zm, -zm, OFF
    CSwitch 1, ?????, zn, -zn, OFF
    CSwitch 1, ?????, q, -q, ON
    CSwitch 1, ?????, ad, -ad, ON

Tool WRC2 "Resource Compiler (pass 2)"
  SwitchText q,  "Quiet operation", .
  SwitchText ad, "Output autodepend info", .
  SwitchText 30, "Requires Windows 3.0 or later", .
  SwitchText e,  "Uses global memory above EMS", . , "Uses global mem above EMS"
  SwitchText l,  "Uses LIM 3.2 EMS directly", .
  SwitchText m,  "EMS bank for each instance", .
  SwitchText p,  "Private DLL", .
  SwitchText t,  "Protected mode only", .
  SwitchText s0, "No fastload section", .
  SwitchText s1, "Preload only", .
  SwitchText s2, "Preload/Data/Non-discardable", .
  Family Resource
    CSwitch 0, ?????, "", wrc, ON
    CSwitch 0, ?????, q, -q, ON
    CSwitch 0, ?????, ad, -ad, ON
    CSwitch 0, ?????, 30, -30, OFF
    CSwitch 0, ?????, e, -e, OFF
    CSwitch 0, ?????, l, -l, OFF
    CSwitch 0, ?????, m, -m, OFF
    CSwitch 0, ?????, p, -p, OFF
    CSwitch 0, ?????, t, -t, OFF
    RGroup "Segements in fastload section"
        RSwitch 1, w????, s0, -s0, ON
        RSwitch 1, w????, s1, -s1, OFF
        RSwitch 1, w????, s2, -s2, OFF
::
:segment !C_FOR_PB
::

Tool ORC "OS/2 Resource Compiler"
  SwitchText i, "Include directories:", .
  Family Resource
    CSwitch 0, ?????, "", rc, ON
    VSwitch 0, ?????, i, -i, " ", MULTI, REQ, "$[: $(%watcom)\\h $(%watcom)\\h\\os2"

Tool ORC2 "OS/2 Resource Compiler (pass 2)"
  SwitchText i, "Include directories:", .
  Family Resource
    CSwitch 0, ?????, "", rc, ON
    VSwitch 0, ?????, i, -i, " ", MULTI, REQ, "$[: $(%watcom)\\h $(%watcom)\\h\\os2"

Tool ESQL "Embedded SQL compiler"
  SwitchText q,       "Preprocessor quiet", .
  SwitchText nooptim, "No optimizations", .
  SwitchText c,       "Favour code size", .
  SwitchText d,       "Favour data size", .
  SwitchText f,       "Generated static data is FAR", .
  SwitchText il,      "Use long ints", .
  SwitchText n,       "Generate line numbers", .
  SwitchText s,       "Maximum string constant length:", . , "Max string const length:"
  SwitchText l,       "Login(id,pswd):", .

  Family Processor
    CSwitch 0, ?????, "", "sqlpp $@ $*.cxx", ON
    CSwitch 0, ?????, q, -q, ON
    RGroup Optimizations
        RSwitch 0, ?????, nooptim, , ON
        RSwitch 0, ?????, c, -c, OFF
        RSwitch 0, ?????, d, -d, OFF
    CSwitch 0, ?????, f, -f, OFF
    CSwitch 0, ?????, il, -il, OFF
    CSwitch 0, ?????, n, -n, OFF
    VSwitch 0, ?????, s, -s, " ", ONE, REQ, ""
    VSwitch 0, ?????, l, -l, " ", ONE, REQ, ""
    CSwitch 0, d?6??, "", "-o DOS", ON
    CSwitch 0, d?2??, "", "-o DOS32", ON
    CSwitch 0, w?6??, "", "-o WINDOWS", ON
    CSwitch 0, w?2??, "", "-o WIN32", ON
    CSwitch 0, n????, "", "-o WINNT", ON
    CSwitch 0, o?6??, "", "-o OS2", ON
    CSwitch 0, o?2??, "", "-o OS232", ON

Tool ESQLWCC "C Compiler for ESQL"
  IncludeTool WLANG
  IncludeTool CCOMP
  Family Compiler
    CSwitch 0, ??6??, "", "wcc $*.cxx", ON
    CSwitch 0, ??2??, "", "wcc386 $*.cxx", ON
    CSwitch 0, ??a??, "", "wccaxp $*.cxx", ON

Tool ESQLWPP "C++ Compiler for ESQL"
  IncludeTool WLANG
  IncludeTool CPPCOMP
  Family Compiler
    CSwitch 0, ??6??, "", "wpp $*.cxx", ON
    CSwitch 0, ??2??, "", "wpp386 $*.cxx", ON
    CSwitch 0, ??a??, "", "wppaxp $*.cxx", ON

Tool ESQLC "ESQL/C compiler"
  IncludeTool ESQL
  IncludeTool ESQLWCC

Tool ESQLP "ESQL/C++ compiler"
  IncludeTool ESQL
  IncludeTool ESQLWPP
::
:endsegment
::

Tool IMAGEEDIT "Image Editor"
  SwitchText cmd_run,  " run", .
  Family ImageEditor
    CSwitch 0, o????, cmd_run, iconedit, ON
    CSwitch 0, ????n, cmd_run, "$(%watcom)\\binnt\\wimgedit /n", ON
    CSwitch 0, ????a, cmd_run, "$(%watcom)\\axpnt\\wimgedit /n", ON
    CSwitch 0, ????9, cmd_run, "$(%watcom)\\binnt\\wimgedit /n", ON
    CSwitch 0, ?????, cmd_run, "$(%watcom)\\binw\\wimgedit /n", ON
::
:segment !C_FOR_PB
::

Tool WINHC "Windows Help Compiler"
  SwitchText hhw,  "HTML Help", .
  SwitchText hcw,  "Version 4.0", .
  SwitchText hc31, "Version 3.1", .
  SwitchText hc30, "Version 3.0", .
  Family Compiler
    RGroup "Help Compiler"
rem These must be in the user's path
        RSwitch 0, ?????, hhw, hhw , OFF
        RSwitch 0, ?????, hcw, hcw , OFF
        RSwitch 0, ?????, hc31, hc31 , ON
        RSwitch 0, ?????, hc30, hc30 , OFF

Tool OIPF "OS/2 Help Compiler"
  SwitchText country,  "Country Code:", .
  SwitchText codepage, "Code page:", . , "CodePage:"
  SwitchText language, "Language:", .
  SwitchText w1,       "Warning level 1", . , "Warning Level 1"
  SwitchText w2,       "Warning level 2", . , "Warning Level 2"
  SwitchText w3,       "Warning level 3", . , "Warning Level 3"
  Family Compiler
    CSwitch 0, ?????, "", "ipfc ", ON
    CSwitch 0, ???i?, "", /inf, ON
    VSwitch 0, ?????, country, /COUNTRY, =, ONE, REQ, ""
    VSwitch 0, ?????, codepage, /CODEPAGE, =, ONE, REQ, ""
    VSwitch 0, ?????, language, /L, =, ONE, REQ, ""
    RGroup "Warning level"
        RSwitch 0, ?????, w1, /W1, OFF
        RSwitch 0, ?????, w2, /W2, OFF
        RSwitch 0, ?????, w3, /W3, ON
:endsegment

Rule ASMOBJ, WASM, ?????
  Source *.asm
  Target *.obj
  Command " *$<#>"

Rule CPPOBJ, WPP, ?????
  Source *.cpp *.cc *.cxx
  Target *.obj
  Autodepend
  BrowseSwitch -db
  Command " *$<#>"

Rule COBJ, WCC, ?????
  Source *.c
  Target *.obj
  Autodepend
  BrowseSwitch -db
  Command " *$<#>"
::
:segment !C_FOR_PB
::

Rule FOROBJ, WFOR, ?????
  Source *.for
  Target *.obj
  Autodepend
  Command " $<#>"

Rule ESQLOBJC, ESQLC, ?????
  Source *.sqc
  Target *.obj
  Autodepend
  BrowseSwitch -db
  Command " $<#ESQL>"
  Command " *$<#ESQLWCC>"

Rule ESQLOBJP, ESQLP, ?????
  Source *.sqp
  Target *.obj
  Autodepend
  BrowseSwitch -db
  Command " $<#ESQL>"
  Command " *$<#ESQLWPP>"

rem Change the next rule when wrc.exe is mature
Rule ORESC, ORC, o???o
  Source *.rc
  Target *.res
  Autodepend
  Command " *$<#> -r $*.rc $&.res"
rem  Command " *$<#> -r -fo=$&.res"

Rule ORESC, WRC, o????
  Source *.rc
  Target *.res
  Autodepend
  Command " *$<#> -r -fo=$&.res"
::
:endsegment
::

Rule WRESC, WRC, w????
  Source *.rc
  Target *.res
  Autodepend
  Command " *$<#> -r -fo=$&.res"

Rule NRESC, WRC, n????
  Source *.rc
  Target *.res
  Autodepend
  Command " *$<#> -r -fo=$&.res"

Rule WRESC, WRC, r????
  Source *.rc
  Target *.res
  Autodepend
  Command " *$<#> -r -fo=$&.res"

Rule SMAKEABLES
  Target *.obj, *.o, *.res
  Action &Make WMAKE OKFORMASK
    HotKey PLAIN F3
    Command "!Make $<#> -a $@"
    Hint "Make the result file"
::
:segment !C_FOR_PB
::

Rule SBROWSEABLE
  Target *.obj, *.o
  Action &Browse
    Command "!Browse Open d $*.mbr"
    Hint "Invoke the program browser on the source file"

rem --OS/2 Executables--
rem Change the next two when wrc.exe is mature
Tool OS2LINK "OS/2 Linking"
  IncludeTool WLINK
  IncludeTool ORC2

Rule OEXE, OS2LINK, o??eo
  Target *.exe
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol DLL, *.dll
  Symbol RES, *.res
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command " *$<#WLINK> @$'.lk1"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " $<#ORC2> $<RES!> $@"
  Command "!endif"

Tool OS2LINK2 "OS/2 Linking"
  IncludeTool WLINK
  IncludeTool WRC2

Rule OEXE, OS2LINK2, o??e?
  Target *.exe
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol DLL, *.dll
  Symbol RES, *.res
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command " *$<#WLINK> @$'.lk1"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " $<#WRC2> $<RES!> $@"
  Command "!endif"

Rule OHLP, OIPF, o??h?
  Target *.hlp
  Symbol IPF, *.ipf
  Symbol BMP, *.bmp
  Command " $<#> $<IPF!>"

Rule OINF, OIPF, o??i?
  Target *.inf
  Symbol IPF, *.ipf
  Symbol BMP, *.bmp
  Command " $<#> $<IPF!>"

rem --Windows Executables--
Rule WHLP, WINHC, w??h?
  Target *.hlp
  Symbol RTF, *.rtf
  Symbol HPJ, *.hpj
  Symbol INC, *.inc
  Symbol H, *.h
  Symbol BMP, *.bmp
  Command " $<#> $<HPJ!>"

Rule NHLP, WINHC, n??h?
  Target *.hlp
  Symbol RTF, *.rtf
  Symbol HPJ, *.hpj
  Symbol INC, *.inc
  Symbol H, *.h
  Symbol BMP, *.bmp
  Command " $<#> $<HPJ!>"
::
:endsegment
::

Tool WINLINK "Windows Linking"
  IncludeTool WLINK
  IncludeTool WRC2

Rule WEXE, WINLINK, w??e?
  Target *.exe
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol DLL, *.dll
  Symbol RES, *.res
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " @%append $*.lk1 op resource=$<RES!>",                               ??6??
  Command "!endif"
  Command " *$<#WLINK> @$'.lk1"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " wbind $* -q -s \"$(%watcom)/binw/win386.ext\" -R -q $<RES!> $*.exe",??2??
  Command "!else",                                                              ??2??
  Command " wbind $* -q -s \"$(%watcom)/binw/win386.ext\" -n",                  ??2??
  Command "!endif"

Rule NEXE, WINLINK, n??e?
  Target *.exe
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol DLL, *.dll
  Symbol RES, *.res
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " @%append $*.lk1 $<RES,>"
  Command "!endif"
  Command " *$<#WLINK> @$'.lk1"

Tool RDOSLINK "RDOS Linking"
  IncludeTool WLINK
  IncludeTool WRC2

Rule REXE, RDOSLINK, rp?e?
  Target *.exe
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol DLL, *.dll
  Symbol RES, *.res
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " @%append $*.lk1 $<RES,>"
  Command "!endif"
  Command " *$<#WLINK> @$'.lk1"

Tool RDEVLINK "RDOS Device Linking"
  IncludeTool WLINK

Rule RDEXE, RDEVLINK, rk?e?
  Target *.rdv
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command " *$<#WLINK> @$'.lk1"

Tool RBINLINK "RDOS Binary Linking"
  IncludeTool WLINK

Rule RBEXE, RBINLINK, rb?e?
  Target *.bin
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command " *$<#WLINK> @$'.lk1"

Tool RMBOOTLINK "RDOS Multiboot Stub Linking"
  IncludeTool WLINK

Rule RMEXE, RMBOOTLINK, rm6e?
  Target *.bin
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command " *$<#WLINK> @$'.lk1"

Rule NMEXEACTIONS, ,nm???
  Target *.exe
  Action "V&isual Programmer"
    Hint "Invoke Visual Programmer"
    Default
    Command "!VP $*"

Rule NAEXEACTIONS, ,na???
  Target *.exe
  Action "V&isual Programmer"
    Hint "Invoke Visual Programmer"
    Default
    Command "!VP $*"

Rule WMEXEACTIONS, ,wm???
  Target *.exe
  Action "V&isual Programmer"
    Hint "Invoke Visual Programmer"
    Default
    Command "!VP $*"

Rule WAEXEACTIONS, ,wa???
  Target *.exe
  Action "V&isual Programmer"
    Hint "Invoke Visual Programmer"
    Default
    Command "!VP $*"

rem --Netware Executables--
Tool COPYFILE "Copy target file"
  SwitchText destpath, "Destination directory", .
  Family "Destination Directory"
    VSwitch 0, ?????, destpath, "copy $@", " ", ONE, REQ, ""
    CSwitch 0, ?????, "", "#", ON

Tool NLMLINK "Link for Netware"
  IncludeTool WLINK
  IncludeTool COPYFILE

Rule NLM, NLMLINK, l?2e?
  Target *.nlm
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command " *$<#WLINK> @$'.lk1"
  Command " $<#COPYFILE>"

rem --Linux Executables--
Rule LNX, WLINK, x?2e?
  Target *.elf
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command " *$<#> @$'.lk1"

rem --All Other Executables--
Rule EXE, WLINK, ???e?
  Target *.exe, *.exp, *.com
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command " *$<#> @$'.lk1"

rem --DLLs--
::
:segment C_FOR_PB
::
:segment FOR_NT
Rule WPBDLL, WLINK, np2d?
:elsesegment
Rule WPBDLL, WLINK, wp6d?
:endsegment
  Target *.dll
  Autotrack *.cpp *.hpp
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol RES, *.res
  Symbol DLL, *.dll
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command "!ifneq BLANK \"$<RES!>\""
:segment FOR_NT
  Command " @%append $*.lk1 $<RES,>"
:elsesegment
  Command " @%append $*.lk1 op resource=$<RES!>"
:endsegment
  Command "!endif"
  Command " *$<#> @$'.lk1"
:segment FOR_NT
  Command " copy $@ $(%watcom)\\binnt\\$."
:elsesegment
  Command " wfscopy $@ $(%watcom)\\binw\\$."
:endsegment
:endsegment
::
:segment !C_FOR_PB
::

Rule WDLL, WLINK, w??d?
  Target *.dll
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol RES, *.res
  Symbol DLL, *.dll
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " @%append $*.lk1 op resource=$<RES!>",                                   ??6??
  Command "!endif"
  Command " *$<#> @$'.lk1"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " wbind $* -q -d -s \"$(%watcom)/binw/w386dll.ext\" -R -q $<RES!> $*.exe",??2??
  Command "!else",                                                                  ??2??
  Command " wbind $* -q -d -s \"$(%watcom)/binw/w386dll.ext\" -n",                  ??2??
  Command "!endif"
  Command " wlib -q -n -b $*.lib +$*.dll",                                          ??6??

rem Change this when wrc is mature
Rule ODLL, WLINK, o??do
  Target *.dll
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol RES, *.res
  Symbol DLL, *.dll
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command " *$<#> @$'.lk1"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " $<#ORC2> $<RES!> $@"
  Command "!endif"
  Command " wlib -q -n -b $*.lib +$*.dll"

Rule ODLL, WLINK, o??d?
  Target *.dll
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol RES, *.res
  Symbol DLL, *.dll
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command " *$<#> @$'.lk1"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " $<#WRC2> $<RES!> $@"
  Command "!endif"
  Command " wlib -q -n -b $*.lib +$*.dll"

Rule NDLL, WLINK, n??d?
  Target *.dll
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol RES, *.res
  Symbol DLL, *.dll
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " @%append $*.lk1 $<RES,>"
  Command "!endif"
  Command " *$<#> @$'.lk1"
  Command " wlib -q -n -b $*.lib +$*.dll"

Rule RDLL, WLINK, r??d?
  Target *.dll
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol RES, *.res
  Symbol DLL, *.dll
  Command "!ifneq BLANK \"$<FIL!>\""
  Command " @%write $*.lk1 $<FIL,>"
  Command "!else"
  Command " @%write $*.lk1 FIL $(%watcom)/lib386/rdos/resstub.obj"
  Command "!endif"
  Command " @%append $*.lk1 $<LIBR,>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " @%append $*.lk1 $<RES,>"
  Command "!endif"
  Command " *$<#> @$'.lk1"
  Command "!ifneq BLANK \"$<FIL!>\""
  Command " wlib -q -n -b $*.lib +$*.dll"
  Command "!endif"

rem Rule LNXDLL, WLINK, x??d?
rem   Target *.dll
rem   Autodepend
rem   Symbol FIL, *.obj
rem   Symbol LIBR, *.lib
rem   Symbol DLL, *.dll
rem   Command " @%write $*.lk1 $<FIL,>"
rem   Command " @%append $*.lk1 $<LIBR,>"
rem   Command " *$<#> @$'.lk1"

Rule CWDLL, WLINK, d?2d?
  Target *.dll
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command " *$<#> @$'.lk1"

rem --Static Libraries--
Rule LIB, WLIB, ???s?
  Target *.lib
  Autodepend
  Symbol  OBJ, *.obj
  Symbol  LIB, *.lib
  Command " %create $*.lb1"
  Command "!ifneq BLANK \"$<OBJ!>\""
  Command " @for %i in ($<OBJ!>) do @%append $*.lb1 +'%i'"
  Command "!endif"
  Command "!ifneq BLANK \"$<LIB!>\""
  Command " @for %i in ($<LIB!>) do @%append $*.lb1 +'%i'"
  Command "!endif"
  Command " *$<#> $*.lib @$*.lb1"
::
:endsegment
::

Rule TMAKEABLES
  Target *.exe *.exp *.com *.rdv *.nlm *. *.elf *.dll *.a *.so *.lib *.hlp *.inf
  Action &Make WMAKE OKFORMASK
    HotKey PLAIN F4
    Command "!Make $<#> $@"
    Hint "Make the target"
::
:segment !C_FOR_PB
::

Rule TBROWSEABLE
  Target *.exe *.exp *. *.elf *.dll *.a *.so *.lib
  Action &Browse
    Command "!Browse Open @$*.cbr"
    Hint "Invoke the program browser on the target browser database"
:endsegment

::
:segment FOR_NT
::
Rule PBDLL, , np2d?
:elsesegment
Rule PBDLL, , wp6d?
:endsegment
  Target *.dll
  Action &Debug WDEBUG
  HotKey CTRL D
  Command "$<#>"
  Hint "Invoke the debugger on the target executable"

Rule EXES, , ???e?
  Target *.exe *.exp *.com *.rdv *.elf
  Action &Run RUN
    HotKey CTRL R
    Command "$<#>"
    Hint "Run the target executable"
  Action "Local &Debug" WDEBUG
    HotKey CTRL D
    Command "$<#>"
    Hint "Invoke the debugger on the target executable"
  Action "Remote D&ebug" RDEBUG
    HotKey CTRL E
    Command "$<#>"
    Hint "Remotely debug the target executable"
::
:segment !C_FOR_PB
::
  Action Sam&ple WSAMPLE
    Command "$<#>"
    Hint "Run and sample the target executable"
  Action Pro&file WPROFILE
    Command "$<#>"
    Hint "Profile the target executable"

Rule NLMS, , l?2e?
  Target *.nlm
  Action &Debug WDEBUG
    HotKey CTRL D
    Command "$<#>"
    Hint "Invoke the debugger on the NLM"
  Action Pro&file WPROFILE
    Command "$<#>"
    Hint "Profile the target executable"
::
:endsegment
::

Rule TXT
  Target *.txt *.h *.hpp *.c *.cc *.cpp *.def *.cfg *.rc *.asm
  Target *.sqc *.sqp *.ipf *.for *.fap *.fi *.xml
  Action "&Edit Text"
    HotKey CTRL E
    Default
    Command "!Editor EditFileAtPos -f$@ 1 1"
    Hint "Edit the ASCII file"

Rule OBJ
  Target *.obj *.o
  Action &Disassemble WDIS
    Command "!Batch $<#>"
    Hint "Disassemble the object file"
  Action "Edit Disassembl&y"
    Command "!Editor EditFileAtPos -f$*.lst 1 1"
    Hint "Edit the disassembled file"

Rule ORES, , o????
  Target *.res
  Action "&Edit Resources"
    Default
    Command "dlgedit $!"
    Hint "Edit the .res file"

Rule WRES, , w????
  Target *.res
  Action "&Edit Resources"
    Default
    Command "wre -n $!"
    Hint "Edit the .res file"

Rule NRES, , n????
  Target *.res
  Action "&Edit Resources"
    Default
    Command "wre -n $!"
    Hint "Edit the .res file"

Rule RRES, , r????
  Target *.res
  Action "&Edit Resources"
    Default
    Command "wre -n $!"
    Hint "Edit the .res file"

Rule DLGEDIT, , o????
  Target *.dlg
    Action "&Edit Dialog"
    Default
    Command "dlgedit $*.res"
    Hint "Edit the .dlg/.res file"

Rule DLGEDIT, , ?????
  Target *.dlg
  Action "&Edit Dialog"
    Default
    Command "wde -n $!"
    Hint "Edit the .dlg file"

Rule IMAGE
  Target *.ico *.bmp, *.cur
  Action "&Edit Image" IMAGEEDIT
    Default
    Command "$<#> $@"
    Hint "Edit the IMAGE file"

Rule EDITRES
  Target *.str *.mnu, *.acc
  Action "&Edit Resource"
    Default
    Command "wre -nointerface -n $@"
    Hint "Edit the resource file"

Rule OFNT, , o????
  Target *.fnt
  Action "&Edit Font"
    Default
    Command "fontedit $!"
    Hint "Edit the .fnt file"

Project
  Filter "C Source (*.c)", *.c
  Filter "C++ Source (*.cpp)", *.cpp
  Filter "C++ Source (*.cxx)", *.cxx
  Filter "C Header (*.h)", *.h
  Filter "C++ Header (*.hpp)", *.hpp
  Filter "C++ Header (*.hxx)", *.hxx
  Filter "Fortran Source (*.for)", *.for
  Filter "Assembly (*.asm)", *.asm
  Filter "Libraries (*.lib)", *.lib
  Filter "Resouce Scripts (*.rc)", "*.rc"
  Filter "Dialogs (*.dlg)", "*.dlg"
  Filter "All Files (*.*)", "*.*"
:segment C_FOR_PB
  HelpAction "&User Object Help" WMAKE
    Command "!Help pbcppuo.hlp"
    Hint "Help on making PowerBuilder C++ user objects"
:endsegment
  Action "&Make All" WMAKE
    HotKey PLAIN F5
    Command "!Make $<#>"
    Hint "Make all targets in the project"
  Action &Run...
    Command "$\"Command:\""
    Hint "Execute command"
  Action "Run Ba&tch..."
    Command "!Batch $\"Command:\""
    Hint "Execute command"
  Action "&Edit Text..."
    Command "!Editor EditFileAtPos -f$\"Filename:==\" 1 1"
    Hint "Invoke editor"
:segment C_FOR_PB
  Action "&PowerBuilder"
:segment FOR_NT
    Command "$(%watcom)\\binnt\\watrun.exe PBFRAME040 \"PowerBuilder\" pb040.exe"
:elsesegment
    Command "$(%watcom)\\binw\\watrun.exe PBFRAME040 \"PowerBuilder\" pb040.exe"
:endsegment
    Hint "Return to PowerBuilder"
:endsegment
  Action "Mark &All Targets for Remake"
    Command "!RemakeAll foo"
    Hint "Mark all targets and their components for remake"

  ToolItem 1009 Source  &Edit*  "Edit the selected source file." Edit
  ToolItem 1010 Source  &Make   "Make the selected source file." "Make Source"
  ToolItem 1004 Target  &Make   "Make the current target." "Make Target"
:segment !C_FOR_PB
  ToolItem 1008 Target  &Run    "Run the current target." Run
:endsegment
  ToolItem 1005 Target  "Local &Debug"  "Locally debug the current target." Debug
  ToolItem 1014 Target  "Remote D&ebug"  "Remotely debug the current target." "Remote Debug"
:segment !C_FOR_PB
  ToolItem 1006 Target  &Browse "Browse the current target." Browse
  ToolItem 1007 Target  Sam&ple "Run and sample the current target." Sample
  ToolItem 1011 Target  Pro&file "Profile the current target." Profile
:endsegment
  ToolItem 1003 Project &Make*  "Make all targets in the project." "Make All"
:segment C_FOR_PB
  ToolItem 1013 Project &PowerBuilder "Return to PowerBuilder."
:endsegment

MsgLog
  Scan "<File: %f>"
  Scan "<wdis %* -l=%f %*>"
  Scan "<wasm %f %*>"
  Scan "<wpp %f %*>"
  Scan "<wpp386 %f %*>"
  Scan "<wcc %f %*>"
  Scan "<wcc386 %f %*>"
  Scan "<sqlpp %f %*>"
  Scan "<wrc %f %*>"
  Scan "<rc %f %*>"
  Scan "<wlink %*@%f%*>"
  Scan "<Warning %f %l:%*>"
  Scan "<Error %f %l:%*>"
  Scan "<%f(%l): *ERR* %i column %o%*>"
  Scan "<%f(%l): *WRN* %i column %o%*>"
  Scan "<%f(%l): *EXT* %i column %o%*>"
  Scan "<%f(%l): *ERR* %i%*>"
  Scan "<%f(%l): *WRN* %i%*>"
  Scan "<%f(%l): *EXT* %i%*>"
  Scan "<%f: *ERR* %i column %o%*>"
  Scan "<%f: *WRN* %i column %o%*>"
  Scan "<%f: *EXT* %i column %o%*>"
  Scan "<%f: *ERR* %i%*>"
  Scan "<%f: *WRN* %i%*>"
  Scan "<%f: *EXT* %i%*>"
  Scan "<%f(%l):%*! %h: (col %o)%*>"
  Scan "<%f(%l):%*! %h: col(%o)%*>"
  Scan "<%f(%l):%*! %h:%*>"
  Scan "<%f(%l):%*>"
  Scan "<%l:%*> <File: %f>"
  Scan "<%f:%*>"
  Scan "<%f>"
  Scan "<(%l,%o):%*> <File: %f>"

:segment C_FOR_PB
IncludeFile pbidex.cfg
:endsegment
:segment !C_FOR_PB
IncludeFile idex.cfg
:endsegment
  
HostMask ????@
