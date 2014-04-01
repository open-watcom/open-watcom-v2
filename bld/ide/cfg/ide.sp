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
rem     ru RDOS, UEFI loader
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


Version 4

IncludeFile override.cfg

Tool WMAKE "Make Utility"
  Family Make
    CSwitch 0, ?????, "", "wmake -f $(%_makefile)", ON
    CSwitch 0, ?????, "Make all targets", -a, OFF
    CSwitch 0, ?????, "Don't print header", -h, ON
    CSwitch 0, ?????, "Erase bad targets", -e, ON
    CSwitch 0, ?????, "Ignore bad targets", -i, OFF
    CSwitch 0, ?????, "Don't check for files made", -c, OFF
    CSwitch 0, ?????, "Don't use MAKEINIT file", -m, OFF
    CSwitch 0, ?????, "Use circular path", -o, OFF
    CSwitch 0, ?????, "Print info", -p, OFF
    CSwitch 0, ?????, "Ignore built-in defns", -r, OFF
    CSwitch 0, ?????, "Only touch files", -t, OFF
    CSwitch 0, ?????, "Do not erase files", -z, OFF
    CSwitch 1, ?????, "Echo progress of work", -d, OFF
    CSwitch 1, ?????, "Continue after an error", -k, OFF
    CSwitch 1, ?????, "Print without executing", -n, OFF
    CSwitch 1, ?????, "Query mode", -q, OFF
    CSwitch 1, ?????, "Silent mode", -s, OFF
    VSwitch 1, ?????, "Append output to log:", -l, " ", ONE, OFF, "$*.lst"
    VSwitch 1, ?????, "Other options:",,, ONE, REQ, ""

Tool RUN "User Application"
  Family Application
    CSwitch 0, x???d, "", "!Error Cannot run Linux target under DOS$", ON
    CSwitch 0, x???o, "", "!Error Cannot run Linux target under OS/2$", ON
    CSwitch 0, x???l, "", "!Error Cannot run Linux target under Netware$", ON
    CSwitch 0, x???n, "", "!Error Cannot run Linux target under Win32$", ON
    CSwitch 0, x???q, "", "!Error Cannot run Linux target under QNX$", ON
    CSwitch 0, x???w, "", "!Error Cannot run Linux target under Win16$", ON
rem make fullscreen an option where both windowed & fullscreen work
    CSwitch 0, oc?eo, "Fullscreen", "!Fullscreen", OFF
    CSwitch 0, of?eo, "Fullscreen", "!Fullscreen", ON
    CSwitch 0, ov?eo, "Fullscreen", "!Fullscreen", OFF
    CSwitch 0, d??eo, "Fullscreen", "!Fullscreen", ON
    CSwitch 0, dr2e?, "", $(%watcom)\binw\dos4gw, ON
    CSwitch 0, dw2e?, "", $(%watcom)\binw\cwstub, ON
rem Pharlap run386 must be in your path
    CSwitch 0, dp2e?, "", run386, ON
rem Pharlap TNT run386 must be in your path
    CSwitch 0, dt2e?, "", tnt, ON
    CSwitch 0, w?2eo, "", "!Fullscreen", ON
    CSwitch 0, ???eo, "",, ON
    CSwitch 0, ?????, "", $@, ON
    VSwitch 0, ?????, "Application parameters:",,, ONE, REQ, ""

Tool WDEBUG "Local Debugger"
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
        RSwitch 0, w???o, "Windowed", "!Fullscreen $(%watcom)\\binw\\wdw", ON
        RSwitch 0, w???n, "Windowed", $(%watcom)\binnt\wdw, ON
        RSwitch 0, w???a, "Windowed", $(%watcom)\axpnt\wdw, ON
        RSwitch 0, w???w, "Windowed", $(%watcom)\binw\wdw, ON
        RSwitch 0, w???j, "Windowed", $(%watcom)\binw\wdw, ON
        RSwitch 0, w???8, "Windowed", $(%watcom)\binw\wdw, ON
        RSwitch 0, w???s, "Windowed", $(%watcom)\binw\wdw, ON
        RSwitch 0, n????, "Windowed", $(%watcom)\binnt\wdw, ON
        RSwitch 0, w???n, "Character mode", "!Fullscreen $(%watcom)\\binnt\\wd.exe", OFF
        RSwitch 0, w???a, "Character mode", "!Fullscreen $(%watcom)\\axpnt\\wd.exe", OFF
        RSwitch 0, w???8, "Character mode", "!Fullscreen $(%watcom)\\n98bw\\wdc.exe", OFF
        RSwitch 0, w???w, "Character mode", "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, w???j, "Character mode", "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, w???o, "Character mode", "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, w???s, "Character mode", "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, n????, "Character mode", $(%watcom)\binnt\wd, OFF
        RSwitch 0, o????, "PM", $(%watcom)\binp\wdw, ON
        RSwitch 0, o????, "Character mode", "!Fullscreen $(%watcom)\\binp\\wd", OFF
:endsegment
    CSwitch 0, dr2ew, " Run", "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, dw2ew, " Run", "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, dp2ew, " Run", "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, dt2ew, " Run", "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, da2ew, " Run", "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, db2ew, " Run", "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, dm2ew, " Run", "$(%watcom)\\binw\\wd.pif", ON
    CSwitch 0, dr2ej, " Run", "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, dw2ej, " Run", "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, dp2ej, " Run", "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, dt2ej, " Run", "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, da2ej, " Run", "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, db2ej, " Run", "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, dm2ej, " Run", "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, dr2e8, " Run", "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, dw2e8, " Run", "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, dp2e8, " Run", "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, dt2e8, " Run", "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, da2e8, " Run", "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, db2e8, " Run", "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, dm2e8, " Run", "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch 0, d???o, " Run", "$(%watcom)\\binp\\wd", ON
    CSwitch 0, d???n, " Run", "$(%watcom)\\binnt\\wdw", ON
    CSwitch 0, d???a, " Run", "$(%watcom)\\axpnt\\wdw", ON
rem binw\wd is used because wdw doesn't load trap files
    CSwitch 0, d???9, " Run", "$(%watcom)\\binw\\wd", ON
    CSwitch 0, d???8, " Run", "$(%watcom)\\n98bw\\wd", ON
    CSwitch 0, d???j, " Run", "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch 0, d????, " Run", "$(%watcom)\\binw\\wd", ON
    CSwitch 0, w???9, "", $(%watcom)\binw\wdw, ON
    CSwitch 0, wp6d8, " Run", "!Fullscreen $(%watcom)\\n98bw\\wdc.exe", ON
    CSwitch 0, wp6d?, " Run", "!Fullscreen $(%watcom)\\binw\\wdc.exe", ON
    CSwitch 0, np2d?, " Run", "$(%watcom)\\binnt\\wdw -powerbuilder -tr=std;2", ON
    CSwitch 0, l???w, " Run", "$(%watcom)\\binw\\wdw", ON
    CSwitch 0, l???j, " Run", "$(%watcom)\\binw\\wdw", ON
    CSwitch 0, l???8, " Run", "$(%watcom)\\n98bw\\wdw", ON
    CSwitch 0, l????, " Run", wdw, ON
::
:segment !C_FOR_PB
::
    VSwitch 1, dr2ew, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dw2ew, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dp2ew, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dt2ew, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, da2ew, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, db2ew, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dm2ew, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dr2ej, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dw2ej, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dp2ej, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dt2ew, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, da2ew, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, db2ew, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dm2ew, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dr2e8, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dw2e8, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dp2e8, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dt2e8, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, da2e8, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, db2e8, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dm2e8, "Trap file:", -tr, =, ONE, REQ, "win"
    VSwitch 1, dr2e9, "Trap file:", -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dw2e9, "Trap file:", -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dp2e9, "Trap file:", -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dt2e9, "Trap file:", -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, da2e9, "Trap file:", -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, db2e9, "Trap file:", -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dm2e9, "Trap file:", -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, d???o, "Trap file:", -tr, =, ONE, REQ, "vdm;$&"
    VSwitch 1, d???n, "Trap file:", -tr, =, ONE, REQ, "vdm;$&"
    VSwitch 1, dr2e?, "Trap file:", -tr, =, ONE, REQ, "rsi"
    VSwitch 1, da2e?, "Trap file:", -tr, =, ONE, REQ, "rsi"
    VSwitch 1, db2e?, "Trap file:", -tr, =, ONE, REQ, "rsi"
    VSwitch 1, dm2e?, "Trap file:", -tr, =, ONE, REQ, "rsi"
    VSwitch 1, dw2e?, "Trap file:", -tr, =, ONE, REQ, "cw"
    VSwitch 1, dp2e?, "Trap file:", -tr, =, ONE, REQ, "pls"
    VSwitch 1, dt2e?, "Trap file:", -tr, =, ONE, REQ, "pls"
    VSwitch 1, l?2e?, "Trap file:", -tr, =, ONE, REQ, "nov;$&"
    VSwitch 1, ?????, "Trap file:", -tr, =, ONE, REQ, ""
::
:endsegment
::
  Family "Debugger Options"
    VSwitch 0, wp6d?, "Configuration file:", -i, =, ONE, REQ, "pbwd.dbg"
    VSwitch 0, np2d?, "Configuration file:", -i, =, ONE, REQ, "pbwdnt.dbg"
    VSwitch 0, ?????, "Configuration file:", -i, =, ONE, REQ, ""
    VSwitch 0, wp6d?, "", -initcmd, =, ONE, REQ, "{pbstart.dbg $&}"
:segment !C_FOR_PB
    CSwitch 0, ?????, "Don't invoke configuration", -noi, OFF
:endsegment
    CSwitch 0, ?????, "Use symbol file", :$*.sym, OFF
:segment !C_FOR_PB
    CSwitch 0, ?????, "Don't process symbolic info", -nosy, OFF
    CSwitch 0, ?????, "Ignore any floating point hardware", -nof, OFF
    CSwitch 0, ?????, "Ignore any attached mouse", -nom, OFF
    CSwitch 0, ?????, "Don't use graphical mouse", -nog, OFF
    CSwitch 0, ?????, "Don't do character remapping", -noch, OFF
    VSwitch 1, ?????, "Dip files:", -di, =, MULTI, REQ, ""
    VSwitch 1, ?????, "Dynamic memory:", -dy, =, ONE, REQ, ""
    VSwitch 1, ?????, "Free guarantee:", -ch, =, ONE, REQ, ""
:endsegment
  Family "Debugger Display"
    VSwitch 0, ?????, "Lines of character mode screen:", -li, =, ONE, REQ, ""
    VSwitch 0, ?????, "Columns of character mode screen:", -co, =, ONE, REQ, ""
    RGroup "Character mode screen"
        RSwitch 0, ?????, "Default adapter", , ON
        RSwitch 0, ?????, "Monochrome adapter", -m, OFF
        RSwitch 0, ?????, "Color adapter", -c, OFF
        RSwitch 0, ?????, "43-line EGA adapter", -e, OFF
        RSwitch 0, ?????, "50-line VGA adapter", -v, OFF
    RGroup "Character mode screen protocol"
        RSwitch 1, ?????, "Default protocol", , ON
:segment !C_FOR_PB
        RSwitch 1, ?????, "Overwrite protocol", -o, OFF
:endsegment
        RSwitch 1, ?????, "Page protocol", -p, OFF
        RSwitch 1, ?????, "Swap protocol", -s, OFF
        RSwitch 1, ?????, "Fast-swap protocol", -f, OFF
        RSwitch 1, ?????, "Two protocol", -t, OFF
    CSwitch 1, wp6d?, " Name", "$(%watcom)\\binw\\dlldbg.exe $@", ON
    CSwitch 1, np2d?, " Name",, ON
rem For remote debugging, don't put a full path name
    CSwitch 1, l????, " Name", $., ON
    CSwitch 1, ?????, " Name", $@, ON
:segment !C_FOR_PB
    VSwitch 1, ?????, "Application parameters:",,, ONE, REQ, ""
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
  Family "Debugging Session"
    RGroup "Debugger"
        RSwitch 0, ????o, "PM", $(%watcom)\binp\wdw, ON
        RSwitch 0, ????s, "Windowed", $(%watcom)\binw\wdw, ON
        RSwitch 0, ????n, "Windowed", $(%watcom)\binnt\wdw, ON
        RSwitch 0, ????9, "Windowed", $(%watcom)\binnt\wdw, ON
        RSwitch 0, ????w, "Windowed", $(%watcom)\binw\wdw, ON
        RSwitch 0, ????j, "Windowed", $(%watcom)\binw\wdw, ON
        RSwitch 0, ????8, "Windowed", $(%watcom)\binw\wdw, ON
        RSwitch 0, ????a, "Windowed", $(%watcom)\axpnt\wdw, ON
        RSwitch 0, ????o, "Character mode", "!Fullscreen $(%watcom)\\binp\\wd", OFF
        RSwitch 0, ????s, "Character mode", "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, ????n, "Character mode", $(%watcom)\binnt\wd, OFF
        RSwitch 0, ????9, "Character mode", $(%watcom)\binnt\wd, OFF
        RSwitch 0, ????w, "Character mode", "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, ????j, "Character mode", "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
        RSwitch 0, ????8, "Character mode", "!Fullscreen $(%watcom)\\n98bw\\wdc.exe", OFF
        RSwitch 0, ????a, "Character mode", "!Fullscreen $(%watcom)\\axpnt\\wd.exe", OFF
    VSwitch 1, ?????, "Local trap file:", -tr, =, ONE, REQ, ""
    RGroup "Target location"
        RSwitch 1, ?????, "Exists on target", -rem, OFF
        RSwitch 2, ?????, "Download to target", -do, ON
    VSwitch 1, ?????, "Local info:", -lo, =, MULTI, REQ, ""
  Family "Debugger Options"
    VSwitch 0, wp6d?, "Configuration file:", -i, =, ONE, REQ, "pbwd.dbg"
    VSwitch 0, np2d?, "Configuration file:", -i, =, ONE, REQ, "pbwdnt.dbg"
    VSwitch 0, ?????, "Configuration file:", -i, =, ONE, REQ, ""
    VSwitch 0, wp6d?, "", -initcmd, =, ONE, REQ, "{pbstart.dbg $&}"
    CSwitch 0, ?????, "Don't invoke configuration", -noi, OFF
    CSwitch 0, ?????, "Use symbol file", :$*.sym, OFF
    CSwitch 0, ?????, "Don't process symbolic info", -nosy, OFF
    CSwitch 0, ?????, "Ignore any floating point hardware", -nof, OFF
    CSwitch 0, ?????, "Don't do character remapping", -noch, OFF
    VSwitch 0, ?????, "Dip files:", -di, =, MULTI, REQ, ""
    VSwitch 0, ?????, "Dynamic memory:", -dy, =, ONE, REQ, ""
    CSwitch 0, ?????, " Name", $., ON
    VSwitch 1, ?????, "Lines of character mode screen:", -li, =, ONE, REQ, ""
    VSwitch 1, ?????, "Columns of character mode screen:", -co, =, ONE, REQ, ""
    RGroup "Character mode screen"
        RSwitch 1, ?????, "Default adapter", , ON
        RSwitch 1, ?????, "Monochrome adapter", -m, OFF
        RSwitch 1, ?????, "Color adapter", -c, OFF
        RSwitch 1, ?????, "43-line EGA adapter", -e, OFF
        RSwitch 1, ?????, "50-line VGA adapter", -v, OFF
    VSwitch 1, ?????, "Application parameters:",,, ONE, REQ, ""
::
:segment !C_FOR_PB
::

Tool WSAMPLE "Sampler"
  Family Sampler
    CSwitch 0, ????s, "", "!Error Sampler cannot run under Win-OS/2$", ON
    CSwitch 0, w???o, "", "!Error Windows executable cannot be sampled under OS/2$", ON
    CSwitch 0, x???d, "", "!Error Linux executable cannot be sampled under DOS$", ON
    CSwitch 0, x???o, "", "!Error Linux executable cannot be sampled under OS/2$", ON
    CSwitch 0, x???l, "", "!Error Linux executable cannot be sampled under Netware$", ON
    CSwitch 0, x???n, "", "!Error Linux executable cannot be sampled under Win32$", ON
    CSwitch 0, x???q, "", "!Error Linux executable cannot be sampled under QNX$", ON
    CSwitch 0, x???w, "", "!Error Linux executable cannot be sampled under Win16$", ON
    CSwitch 0, w???w, " app", "!Fullscreen $(%watcom)\\binw\\wsamplew", ON
    CSwitch 0, w???j, " app", "!Fullscreen $(%watcom)\\binw\\wsamplew", ON
    CSwitch 0, w???8, " app", "!Fullscreen $(%watcom)\\binw\\wsamplew", ON
    CSwitch 0, w????, " app", "!Fullscreen wsamplew", ON
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
    VSwitch 0, n????, "Sample buffer size(1-63):", -b, =, ONE, REQ, ""
    VSwitch 0, o????, "Sample buffer size(1-63):", -b, =, ONE, REQ, ""
    VSwitch 0, d????, "Sample buffer size(1-63):", -b, =, ONE, REQ, ""
    VSwitch 0, x????, "Sample buffer size(1-63):", -b, =, ONE, REQ, ""
    VSwitch 0, ?????, "Sample file name:", -f, =, ONE, REQ, "$*.smp"
    VSwitch 0, ?????, "Sample rate:", -r, =, ONE, REQ, ""
    VSwitch 0, d?6??, "Sampler interrupt number(20x-FFx):", -i, =, ONE, REQ, ""
    CSwitch 0, d?6??, "disable DOS interrupts", -d, OFF
    CSwitch 0, ?????, "", $@, ON
    VSwitch 0, ?????, "Application parameters:",,, ONE, REQ, ""

Tool WPROFILE "Profiler"
  Family Profiler
    CSwitch 0, w???w, " app", "$(%watcom)\\binw\\wprofw", ON
    CSwitch 0, w???j, " app", "$(%watcom)\\binw\\wprofw", ON
    CSwitch 0, w???8, " app", "$(%watcom)\\binw\\wprofw", ON
    CSwitch 0, w???s, " app", "$(%watcom)\\binw\\wprofw", ON
    CSwitch 0, ?????, " app", wprof, ON
    CSwitch 0, ?????, "", $*.smp, ON
::
:endsegment
::

Tool WDIS "Disassembler"
  Family Disassembler
    CSwitch 0, ?????, "", "wdis $@", ON
    VSwitch 0, ?????, "Generate listing file:", -l, =, ONE, ON, "$*.lst"
    VSwitch 0, ?????, "Initial character of internal labels:", -i, =, ONE, REQ, ""
    CSwitch 0, ?????, "Generate assembleable output", -a, OFF
    CSwitch 0, ?????, "Generate list of externs", -e, OFF
    CSwitch 0, ?????, "Generate list of publics", -p, OFF
    CSwitch 0, ?????, "Print list of operands beside instructions", -o, OFF
    CSwitch 1, ?????, "Leave C++ names mangled", -m, OFF
    VSwitch 1, ?????, "Include source lines:", -s, =, ONE, ON, ""
    CSwitch 1, ?????, "Do not use instruction name pseudonyms", -fp, OFF
    CSwitch 1, ??a??, "Do not use register name pseudonyms", -fr, OFF
    CSwitch 1, ??2??, "Use alternate indexing format", -fi, OFF
    CSwitch 1, ??6??, "Use alternate indexing format", -fi, OFF
    CSwitch 1, ?????, "Instructions/registers in upper case", -fu, OFF

Tool WCG "Code Generator"
  Family Optimization
    RGroup "Style of optimization"
        RSwitch 0, ?????, "No optimizations", -od, OFF, ON
        RSwitch 0, ?????, "Average space and time", -ox , OFF
        RSwitch 0, ?????, "Space optimizations", -os, OFF
        RSwitch 0, ?????, "Time optimizations", -ot, OFF
        RSwitch 0, ?????, "Fastest possible code", -otexan, ON, OFF
:segment !C_FOR_PB
    RGroup "Stack frames"
        RSwitch 0, ?????, "Do not generate stack frames", , ON
        RSwitch 0, ?????, "Generate as needed", -of, OFF
        RSwitch 0, ?????, "Always generate", -of+, OFF
:endsegment
    CSwitch 0, ?????, "Disable stack depth checking", -s, OFF
    CSwitch 1, ?????, "Branch prediction", -ob, OFF
    CSwitch 1, ?????, "Loop optimizations", -ol, OFF
    CSwitch 1, ?????, "Loop unrolling", -ol+, OFF
    CSwitch 1, ?????, "Call/return optimizations", -oc, OFF
    CSwitch 1, ?????, "In-line intrinsic functions", -oi, OFF
    CSwitch 1, ?????, "Relax alias checking", -oa, OFF
    CSwitch 1, ?????, "Instruction scheduling", -or, OFF
    CSwitch 1, ?????, "Allow repeated optimizations", -oh, OFF
    CSwitch 1, ?????, "Math optimizations", -om, OFF
    CSwitch 1, ?????, "Numerically unstable optimizations", -on, OFF
    CSwitch 1, ?????, "Consistent FP results", -op, OFF
    VSwitch 1, ?????, "Expand function in-line:", -oe,, ONE, OFF, "20"
  Family Debugging
    RGroup "Debugging style"
        RSwitch 0, ?????, "No debugging information", , OFF
        RSwitch 0, ?????, "Line number information", -d1, ON, OFF
        RSwitch 0, ?????, "Full debugging info", -d2, OFF, ON
        RSwitch 0, ?????, "Full info plus unused types", -d3, OFF
    RGroup "Debugging format"
        RSwitch 0, ?????, "Compiler default", , ON
        RSwitch 0, ?????, "Watcom debugging format", -hw, OFF
:segment !C_FOR_PB
        RSwitch 0, ?????, "Dwarf debugging format", -hd, OFF
        RSwitch 0, ?????, "CodeView debugging format", -hc, OFF
    CSwitch 1, ?????, "Emit Browser information", -db, OFF
    CSwitch 1, ?????, "Emit routine names in code", -en, OFF
    VSwitch 1, ?????, "Call prolog routine:", -ep,, ONE, OFF, ""
    CSwitch 1, ?????, "Call epilog hook routine", -ee, OFF
    CSwitch 1, ?????, "Pentium profiling code", -et, OFF
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
    CSwitch 0, nm???, "Multithreaded application", -bm, ON
    CSwitch 0, na???, "Multithreaded application", -bm, ON
    CSwitch 0, no???, "Multithreaded application", -bm, ON
    CSwitch 0, nu???, "Multithreaded application", -bm, ON
    CSwitch 0, o????, "Multithreaded application", -bm, OFF
    CSwitch 0, n????, "Multithreaded application", -bm, OFF
    CSwitch 0, r????, "Multithreaded application", -bm, ON
    CSwitch 0, w????, "Default-window application", -bw, OFF
:endsegment
    RGroup "Floating-point level"
        RSwitch 0, ?????, "Compiler default", , ON
        RSwitch 0, ?????, "Inline 80287 instructions", -fp2, OFF
        RSwitch 0, ?????, "Inline 80387 instructions", -fp3, OFF
        RSwitch 0, ?????, "Inline Pentium instructions", -fp5, OFF
        RSwitch 0, ?????, "Inline Pentium Pro instructions", -fp6, OFF
    RGroup "Floating-point model"
        RSwitch 1, r????, "Compiler default", , OFF
        RSwitch 1, ?????, "Compiler default", , ON
        RSwitch 1, ?????, "Inline with emulator", -fpi, OFF
        RSwitch 1, r????, "Inline with coprocessor", -fpi87, ON
        RSwitch 1, ?????, "Inline with coprocessor", -fpi87, OFF
        RSwitch 1, ?????, "Floating-point calls", -fpc, OFF
    RGroup "Floating-point rounding"
        RSwitch, 1, ?????, "Default rounding", , ON
        RSwitch, 1, ?????, "Omit rounding", -zro, OFF
        RSwitch, 1, ??2??, "Inline rounding", -zri, OFF
:segment !C_FOR_PB
  Family "Register Usage"
    RGroup "DS register"
        RSwitch 0, rk2e?, "Compiler default", , OFF
        RSwitch 0, ?????, "Compiler default", , ON
        RSwitch 0, rk2e?, "DS pegged to DGROUP", -zdp, ON
        RSwitch 0, ?????, "DS pegged to DGROUP", -zdp, OFF
        RSwitch 0, ?????, "DS can float", -zdf, OFF
    RGroup "FS register"
        RSwitch 0, rk2e?, "Compiler default", , OFF
        RSwitch 0, ?????, "Compiler default", , ON
        RSwitch 0, ?????, "FS cannot be used", -zfp, OFF
        RSwitch 0, rk2e?, "FS can be used", -zff, ON
        RSwitch 0, ?????, "FS can be used", -zff, OFF
    RGroup "GS register"
        RSwitch 0, ?????, "Compiler default", , ON
        RSwitch 0, ?????, "GS cannot be used", -zgp, OFF
        RSwitch 0, ?????, "GS can be used", -zgf, OFF
:endsegment
  Family "Code Generation Option"
:segment !C_FOR_PB
    VSwitch 0, ?????, "Name of code class:", -nc,, ONE, REQ, ""
    VSwitch 0, ?????, "Name of code group:", -g,, ONE, REQ, ""
    VSwitch 0, ?????, "Name of data segment:", -nd,, ONE, REQ, ""
    VSwitch 0, ?????, "Name of text segment:", -nt,, ONE, REQ, ""
    VSwitch 0, ?????, "Name of module:", -nm,, ONE, REQ, ""
    CSwitch 1, ?????, "Don't generate default library information", -zl, OFF
    CSwitch 1, ?????, "No file dependency information", -zld, OFF
    CSwitch 1, ??2??, "Generate Easy OMF object files", -ez, OFF
    CSwitch 1, ?????, "Save/restore segment registers", -r, OFF
:endsegment
:segment !C_FOR_PB | FOR_WIN
    CSwitch 1, ??6d?, "", -zu, ON
    CSwitch 1, rk2e?, "SS not assumed equal to DS", -zu, ON
    CSwitch 1, ???e?, "SS not assumed equal to DS", -zu, OFF
    CSwitch 1, ???s?, "SS not assumed equal to DS", -zu, OFF
:endsegment
:segment !C_FOR_PB
    CSwitch 1, ?????, "Put functions in separate segments", -zm, OFF
    CSwitch 1, ?????, "Constants in code segment", -zc, OFF
    CSwitch 1, ??2??, "Load DS from DGROUP", -zdl, OFF
:endsegment
  Family "Memory Model and Processor"
    RGroup "Target processor"
        RSwitch 0, wm???, "8086", ,  OFF
        RSwitch 0, wa???, "8086", ,  OFF
        RSwitch 0, ??6??, "8086", ,  ON
        RSwitch 0, ??6??, "80186", -1, OFF
        RSwitch 0, wm???, "80286", -2, ON
        RSwitch 0, wa???, "80286", -2, ON
        RSwitch 0, ??6??, "80286", -2, OFF
        RSwitch 0, ??6??, "80386", -3, OFF
        RSwitch 0, ??6??, "80486", -4, OFF
        RSwitch 0, ??6??, "Pentium", -5, OFF
        RSwitch 0, ??6??, "Pentium Pro", -6, OFF
        RSwitch 0, r?2??, "80386 register-based calling", -3r, ON
        RSwitch 0, ??2??, "80386 register-based calling", -3r, OFF
        RSwitch 0, ??2??, "80386 stack-based calling", -3s, OFF
        RSwitch 0, ??2??, "80486 register-based calling", -4r, OFF
        RSwitch 0, ??2??, "80486 stack-based calling", -4s, OFF
        RSwitch 0, ??2??, "Pentium register-based calling", -5r, OFF
        RSwitch 0, ??2??, "Pentium stack-based calling", -5s, OFF
        RSwitch 0, ds2??, "Pentium Pro register-based calling", -6r, OFF
        RSwitch 0, dy2??, "Pentium Pro register-based calling", -6r, OFF
        RSwitch 0, l?2??, "Pentium Pro register-based calling", -6r, OFF
        RSwitch 0, r?2??, "Pentium Pro register-based calling", -6r, OFF
        RSwitch 0, ??2??, "Pentium Pro register-based calling", -6r, ON
        RSwitch 0, ds2??, "Pentium Pro stack-based calling", -6s, ON
        RSwitch 0, dy2??, "Pentium Pro stack-based calling", -6s, ON
        RSwitch 0, l?2??, "Pentium Pro stack-based calling", -6s, ON
        RSwitch 0, ??2??, "Pentium Pro stack-based calling", -6s, OFF
    CSwitch 0, w????, "", -bt=windows, ON
    CSwitch 0, d????, "", -bt=dos, ON
    CSwitch 0, o????, "", -bt=os2, ON
    CSwitch 0, n????, "", -bt=nt, ON
    CSwitch 0, l????, "", -bt=netware, ON
    CSwitch 0, x????, "", -bt=linux, ON
    CSwitch 0, rp???, "", -bt=rdos, ON
    CSwitch 0, ru?d?, "", -bt=rdos, ON
    CSwitch 0, rk6??, "", -bt=rdos_dev16, ON
    CSwitch 0, rk2??, "", -bt=rdosdev, ON
    CSwitch 0, rb6??, "", -bt=rdos_bin16, ON
    CSwitch 0, rb2??, "", -bt=rdos_bin32, ON
    CSwitch 0, rm6??, "", -bt=rdos_mboot, ON
    CSwitch 0, ?????, "", -fo=.obj, ON
:segment C_FOR_PB
    CSwitch 1, n?2d?, "32-bit flat model", -mf, ON
:endsegment
:segment !C_FOR_PB
    RGroup "Memory model"
        RSwitch 1, ??A??, "Compiler default", , ON
        RSwitch 1, ?????, "Compiler default", , OFF
        RSwitch 1, dc6e?, "Small model", -ms, ON
        RSwitch 1, rk6??, "Small model", -ms, ON
        RSwitch 1, rb6??, "Small model", -ms, ON
        RSwitch 1, rm6??, "Small model", -ms, ON
        RSwitch 1, ??6??, "Small model", -ms, OFF
        RSwitch 1, ??6??, "Medium model", -mm, OFF
        RSwitch 1, ??6??, "Compact model", -mc, OFF
        RSwitch 1, dc6e?, "Large model", -ml, OFF
        RSwitch 1, rk6??, "Large model", -ml, OFF
        RSwitch 1, rb6??, "Large model", -ml, OFF
        RSwitch 1, rm6??, "Large model", -ml, OFF
        RSwitch 1, ??6??, "Large model", -ml, ON
        RSwitch 1, ??6??, "Huge model", -mh, OFF
        RSwitch 1, l?2??, "32-bit flat model", -mf, OFF
        RSwitch 1, rk2??, "32-bit flat model", -mf, OFF
        RSwitch 1, ??2??, "32-bit flat model", -mf, ON
        RSwitch 1, l?2??, "32-bit small model", -ms, ON
        RSwitch 1, rk2??, "32-bit small model", -mc, ON
        RSwitch 1, ??2??, "32-bit small model", -ms, OFF
:endsegment
    CSwitch 1, wp6d?, "", -ml, ON

Tool WLANG "Languages"
  IncludeTool WCG
  Family "File Option"
    VSwitch 0, w????, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/win\""
    VSwitch 0, d????, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, o?6??, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/os21x\""
    VSwitch 0, o?2??, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/os2\""
    VSwitch 0, nm???, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, na???, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, no???, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, nu???, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, n????, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt\""
    VSwitch 0, l????, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/novh\""
    VSwitch 0, x????, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/lh\""
    VSwitch 0, r????, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/rdos\""
    VSwitch 0, ?????, "Include files:", -fi, =, MULTI, REQ, ""
    CSwitch 0, wm???, "Use precompiled headers", -fhq, ON
    CSwitch 0, wa???, "Use precompiled headers", -fhq, ON
    CSwitch 0, wp???, "Use precompiled headers", -fhq, ON
    CSwitch 0, nm???, "Use precompiled headers", -fhq, ON
    CSwitch 0, na???, "Use precompiled headers", -fhq, ON
    CSwitch 0, nu???, "Use precompiled headers", -fhq, ON
    CSwitch 0, no???, "Use precompiled headers", -fhq, ON
    CSwitch 0, ?????, "Use precompiled headers", -fh, OFF
  Family Diagnostics
    RGroup "Warning level"
        RSwitch 0, ?????, "Warning level 0", -w0, OFF
        RSwitch 0, ?????, "Warning level 1", -w1, OFF
        RSwitch 0, wa???, "Warning level 2", -w2, ON
        RSwitch 0, wm???, "Warning level 2", -w2, ON
        RSwitch 0, ?????, "Warning level 2", -w2, OFF
        RSwitch 0, nm???, "Warning level 3", -w3, ON
        RSwitch 0, na???, "Warning level 3", -w3, ON
        RSwitch 0, nu???, "Warning level 3", -w3, ON
        RSwitch 0, no???, "Warning level 3", -w3, ON
        RSwitch 0, ?????, "Warning level 3", -w3, OFF
        RSwitch 0, wm???, "Warning level 4", -w4, OFF
        RSwitch 0, wa???, "Warning level 4", -w4, OFF
        RSwitch 0, nm???, "Warning level 4", -w4, OFF
        RSwitch 0, na???, "Warning level 4", -w4, OFF
        RSwitch 0, no???, "Warning level 4", -w4, OFF
        RSwitch 0, nu???, "Warning level 4", -w4, OFF
        RSwitch 0, ?????, "Warning level 4", -w4, ON
    CSwitch 0, ?????, "Treat warnings as errors", -we, OFF
    VSwitch 0, ?????, "Error count:", -e,, ONE, REQ, "25"
    CSwitch 1, ?????, "Force ANSI compliance", -za, OFF
    CSwitch 1, o?2??, "Automatic stack growing", -sg, OFF
    CSwitch 1, o?2??, "Touch stack through SS first", -st, OFF
    VSwitch 1, ?????, "Disable warning messages", -wcd,, MULTI, REQ, ""
    VSwitch 1, ?????, "Enable warning messages", -wce,, MULTI, REQ, ""
  Family Source
    CSwitch 0, ?????, "Extended macro definitions", -d+, OFF
    VSwitch 0, ?????, "Undefine macros:", -u,, MULTI, REQ, ""
    VSwitch 0, nm?e?, "Macro definitions:", -d,, MULTI, REQ, "_X86_ _MBCS", "_X86_ _DEBUG _MBCS"
    VSwitch 0, nm?d?, "Macro definitions:", -d,, MULTI, REQ, "_X86_ _USRDLL _WINDLL _MBCS", "_X86_ _USRDLL _WINDLL _DEBUG _MBCS"
    VSwitch 0, na?e?, "Macro definitions:", -d,, MULTI, REQ, "_AFXDLL _X86_ _MBCS", "_AFXDLL _X86_ _DEBUG _MBCS"
    VSwitch 0, na?d?, "Macro definitions:", -d,, MULTI, REQ, "_AFXDLL _WINDLL _AFXEXT _X86_ _MBCS", "_AFXDLL _WINDLL _AFXEXT _X86_ _DEBUG _MBCS"
    VSwitch 0, no?e?, "Macro definitions:", -d,, MULTI, REQ, "_UNICODE _AFXDLL _X86_", "_UNICODE _AFXDLL _X86_ _DEBUG"
    VSwitch 0, no?d?, "Macro definitions:", -d,, MULTI, REQ, "_UNICODE _AFXDLL _WINDLL _AFXEXT _X86_", "_UNICODE _AFXDLL _WINDLL _AFXEXT _X86_ _DEBUG"
    VSwitch 0, nu?e?, "Macro definitions:", -d,, MULTI, REQ, "_UNICODE _X86_", "_UNICODE _X86_ _DEBUG"
    VSwitch 0, nu?d?, "Macro definitions:", -d,, MULTI, REQ, "_UNICODE _USRDLL _WINDLL _X86_", "_UNICODE _USRDLL _WINDLL _X86_ _DEBUG"
    VSwitch 0, wa?e?, "Macro definitions:", -d,, MULTI, REQ, "_AFXDLL", "_AFXDLL _DEBUG"
    VSwitch 0, wa?d?, "Macro definitions:", -d,, MULTI, REQ, "_AFXDLL _WINDLL", "_AFXDLL _WINDLL _DEBUG"
    VSwitch 0, wm6e?, "Macro definitions:", -d,, MULTI, REQ, "", "_DEBUG"
    VSwitch 0, wm6d?, "Macro definitions:", -d,, MULTI, REQ, "_USRDLL _WINDLL", "_WINDLL _USRDLL _DEBUG"
    VSwitch 0, ?????, "Macro definitions:", -d,, MULTI, REQ, ""
    CSwitch 0, nm???, "Change char default to signed", -j, ON
    CSwitch 0, na???, "Change char default to signed", -j, ON
    CSwitch 0, no???, "Change char default to signed", -j, ON
    CSwitch 0, nu???, "Change char default to signed", -j, ON
    CSwitch 0, ?????, "Change char default to signed", -j, OFF
    CSwitch 0, nm???, "Force enums to be type int", -ei, ON
    CSwitch 0, na???, "Force enums to be type int", -ei, ON
    CSwitch 0, no???, "Force enums to be type int", -ei, ON
    CSwitch 0, nu???, "Force enums to be type int", -ei, ON
    CSwitch 0, ?????, "Force enums to be type int", -ei, OFF
    RGroup "Structure alignment"
        RSwitch 1, nm???, "Compiler default", , OFF
        RSwitch 1, na???, "Compiler default", , OFF
        RSwitch 1, no???, "Compiler default", , OFF
        RSwitch 1, nu???, "Compiler default", , OFF
        RSwitch 1, r????, "Compiler default", , OFF
        RSwitch 1, ?????, "Compiler default", , ON
        RSwitch 1, r????, "1-byte alignment", -zp1, ON
        RSwitch 1, ?????, "1-byte alignment", -zp1, OFF
        RSwitch 1, ?????, "2-byte alignment", -zp2, OFF
        RSwitch 1, nm???, "4-byte alignment", -zp4, ON
        RSwitch 1, na???, "4-byte alignment", -zp4, ON
        RSwitch 1, no???, "4-byte alignment", -zp4, ON
        RSwitch 1, nu???, "4-byte alignment", -zp4, ON
        RSwitch 1, ?????, "4-byte alignment", -zp4, OFF
        RSwitch 1, ?????, "8-byte alignment", -zp8, OFF
        RSwitch 1, ?????, "16-byte alignment", -zp16, OFF
  Family Miscellaneous
    VSwitch 0, ??6??, "Data Threshold:", -zt,, ONE, REQ, ""
    RGroup "Double byte characters"
        RSwitch 0, ?????, "No double-byte character support", , ON
        RSwitch 0, ?????, "Kanji", -zk0, OFF
        RSwitch 0, ?????, "Kanji (Unicode)", -zk0u, OFF
        RSwitch 0, ?????, "Traditional Chinese", -zk1, OFF
        RSwitch 0, ?????, "Simplified Chinese", -zk3, OFF
        RSwitch 0, ?????, "Korean", -zk2, OFF
        RSwitch 0, ?????, "Current code page", -zkl, OFF
    VSwitch 1, ?????, "Unicode translate table:", -zku, =, ONE, REQ, ""
    CSwitch 1, ?????, "Quiet operation", -zq, ON
    VSwitch 1, ?????, "Other options:",,, MULTI, REQ, ""

Tool CCOMP "C Compiler"
  Family "C Compiler"
    CSwitch 0, ?????, "Return char and short as int", -ri, OFF
    RGroup "Create definition file (.def)"
        RSwitch 0, ?????, "No definition file",  , ON
        RSwitch 0, ?????, "With typedef names", -v, OFF
        RSwitch 0, ?????, "With base type names", -zg, OFF

Tool CPPCOMP "C++ Compiler"
  Family "C++ Compiler"
    CSwitch 0, ?????, "Shorter error messages", -ew, OFF
    CSwitch 0, ?????, "Full warnings", -wx, OFF
    CSwitch 0, wm?e?, "Smart Windows callbacks", -zws, ON
    CSwitch 0, wa?e?, "Smart Windows callbacks", -zws, ON
    CSwitch 0, w?6??, "Smart Windows callbacks", -zws, OFF
    RGroup "Exception handling"
        RSwitch 0, ?????, "None", -xd, OFF
        RSwitch 0, ?????, "Normal exception handling", -xs, ON
        RSwitch 0, ?????, "Direct calls for destruction", -xst, OFF
        RSwitch 0, ?????, "Table driven destructors", -xss, OFF
    CSwitch 0, wm6??, "Functions in separate segments.", -zmf, ON
    CSwitch 0, wa6??, "Functions in separate segments.", -zmf, ON
    CSwitch 0, ?????, "Functions in separate segments.", -zmf, OFF
    CSwitch 1, ?????, "Enable RTTI", -xr, ON
    CSwitch 1, ?????, "Virtual function removal", -zv, OFF
    CSwitch 1, ?????, "Check for truncated filenames", -ft, OFF
    CSwitch 1, ?????, "Do NOT check for truncated filenames", -fx, OFF

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
  Family Compiler
    CSwitch 0, ??6??, "", "wfc $@", ON
    CSwitch 0, ??2??, "", "wfc386 $@", ON
    CSwitch 0, ??a??, "", "wfcaxp $@", ON
  Family "File Option"
    RGroup "Listing file"
        RSwitch 0, ?????, "Compiler default", , ON
        RSwitch 0, ?????, "No listing file", -nolis , OFF
        RSwitch 0, ?????, "Listing file to printer", "-lis -pr" , OFF
        RSwitch 0, ?????, "Listing file to screen", "-lis -ty" , OFF
        RSwitch 0, ?????, "Listing file to disk", "-lis -di" , OFF
    CSwitch 0, ?????, "Include files in listing file", -incl, OFF
    C2Switch 0, ?????, "Generate error file", "", -noer, ON
    VSwitch 1, w????, "Path for include files:", -incp, =, ONE, REQ, "\"$(%watcom)/src/fortran/win;$(%watcom)/src/fortran\""
    VSwitch 1, o????, "Path for include files:", -incp, =, ONE, REQ, "\"$(%watcom)/src/fortran/os2;$(%watcom)/src/fortran\""
    VSwitch 1, d????, "Path for include files:", -incp, =, ONE, REQ, "\"$(%watcom)/src/fortran/dos;$(%watcom)/src/fortran\""
    VSwitch 1, ?????, "Path for include files:", -incp, =, ONE, REQ, ""
  Family "Compile-time Diagnostic"
    CSwitch 0, ?????, "Require symbol declaration", -exp, OFF
    CSwitch 0, ?????, "Warn when extensions used", -ex, OFF
    C2Switch 0, ?????, "Warn about unreferenced symbols", "", -nor, ON
    CSwitch 0, ?????, "Ignore wild branches", -wil, OFF
    C2Switch 0, ?????, "Enable warning messages", "", -nowa, ON
    C2Switch 0, ?????, "Print diagnostics to the screen", "", -note, ON
  Family "Run-time Diagnostic"
    CSwitch 0, ?????, "Perform stack checking", -st, OFF
    CSwitch 0, ?????, "Perform format type checking", -for, OFF
    CSwitch 0, ?????, "Generate runtime trace backs", -tr, OFF
    CSwitch 0, ?????, "Add runtime bounds checks", -bo, OFF
    CSwitch 0, ?????, "Store messages as resources", -res, OFF
  Family "Debugging"
    RGroup "Debugging style"
        RSwitch 0, ?????, "No debugging information", , OFF
        RSwitch 0, ?????, "Line number information", -d1, ON, OFF
        RSwitch 0, ?????, "Full debugging information", -d2, OFF, ON
    RGroup "Debugging format"
        RSwitch 0, ?????, "Compiler default", , ON
        RSwitch 0, ?????, "Watcom debugging format", -hw, OFF
        RSwitch 0, ?????, "CodeView debugging format", -hc, OFF
        RSwitch 0, ?????, "DWARF debugging format", -hd, OFF
  Family "Code Generation Strategy"
    RGroup "Target CPU"
      RSwitch 0, ?????, "Compiler default", , ON
      RSwitch 0, ?????, "Assume 80386", -3 , OFF
      RSwitch 0, ?????, "Optimize for 80486", -4 , OFF
      RSwitch 0, ?????, "Optimize for Pentium", -5 , OFF
      RSwitch 0, ?????, "Optimize for Pentium Pro", -6 , OFF
    RGroup "Memory model"
      RSwitch 0, ?????, "Compiler default", , ON
      RSwitch 0, ??2??, "Flat model", -mf, OFF
      RSwitch 0, ??2??, "Small model", -ms, OFF
      RSwitch 0, ??6??, "Large model", -ml, OFF
      RSwitch 0, ??6??, "Medium model", -mm, OFF
    RGroup "Floating-point level"
      RSwitch 1, ?????, "Compiler default", , ON
      RSwitch 1, ?????, "Inline 80287 instructions", -fp2, OFF
      RSwitch 1, ?????, "Inline 80387 instructions", -fp3, OFF
      RSwitch 1, ?????, "Inline Pentium instructions", -fp5, OFF
      RSwitch 1, ?????, "Inline Pentium Pro instructions", -fp6, OFF
    RGroup "Floating-point model"
      RSwitch 1, r????, "Compiler default", , OFF
      RSwitch 1, ?????, "Compiler default", , ON
      RSwitch 1, ?????, "Floating-point calls", -fpc, OFF
      RSwitch 1, ?????, "Emulate 80x87 instructions", -fpi, OFF
      RSwitch 1, r????, "Inline 80x87 instructions", -fpi87, ON
      RSwitch 1, ?????, "Inline 80x87 instructions", -fpi87, OFF
    CSwitch 1, ?????, "80x87 reverse compatibility", -fpr OFF
  Family "Code Generation Option"
      VSwitch 0, ?????, "Set data threshold:", -dt, =, ONE, REQ, ""
      CSwitch 0, ?????, "Put constants in code segment", -co, OFF
      CSwitch 0, ?????, "SAVE local variables", -sa, OFF
      CSwitch 0, ?????, "Short INTEGER/LOGICAL size", -sh, OFF
      CSwitch 0, ?????, "Align COMMON segments", -al, OFF
      CSwitch 0, ?????, "Local variables on the stack", -au, OFF
      CSwitch 0, ?????, "Pass character descriptors", -des, OFF
  Family "Register Usage"
      CSwitch 0, ?????, "Save segment registers", -sr, OFF
      C2Switch 0, ?????, "Compiler can use GS", "", -nogs, ON
      C2Switch 0, ??6??, "Compiler can use FS", "", -nofs, ON
      C2Switch 0, ?????, "Compiler can use FS", -fs, "", OFF
      C2Switch 0, o?6??, "SS points to DGROUP", "", -ss, OFF
      C2Switch 0, ??6??, "SS points to DGROUP", "", -ss, ON
  Family "Optimizations"
    RGroup "Optimization Style"
          RSwitch 0, ?????, "Compiler default", , ON
          RSwitch 0, ?????, "Disable optimizations", -od , OFF
          RSwitch 0, ?????, "Optimize for time", -ot, OFF
          RSwitch 0, ?????, "Optimize for space", -os , OFF
          RSwitch 0, ?????, "Fastest possible code", -ox , OFF
      CSwitch 1, ?????, "Base pointer optimizations", -ob, OFF
      CSwitch 1, ?????, "Branch prediction", -obp, OFF
      CSwitch 1, ?????, "No call-return optimizations", -oc, OFF
      CSwitch 1, ?????, "No stack frame optimizations", -of, OFF
      CSwitch 1, ?????, "Optimize at expense of compile-time", -oh, OFF
      CSwitch 1, ?????, "Statement functions inline", -oi, OFF
      CSwitch 1, ?????, "Move register saves into flow path", -ok, OFF
      CSwitch 1, ?????, "Loop optimizations", -ol, OFF
      CSwitch 1, ?????, "Loop unrolling", -ol+, OFF
      CSwitch 1, ?????, "Math optimizations", -om, OFF
      CSwitch 1, ?????, "Numerical optimizations", -on, OFF
      CSwitch 1, ?????, "Precision optimizations", -op, OFF
      CSwitch 1, ?????, "Instruction scheduling", -or, OFF
      CSwitch 1, ?????, "No loop overflow checking", -odo, OFF
    Family "Application Type"
      CSwitch 0, w????, "Default windowed application", -bw, OFF
      CSwitch 0, r????, "Multithreaded application", -bm, ON
      CSwitch 0, ?????, "Multithreaded application", -bm, OFF
      CSwitch 0, ???d?, "Dynamic link library", -bd, ON
      CSwitch 0, ?????, "Dynamic link library", -bd, OFF
      CSwitch 0, w????, "Generate code for Windows", -win, ON
      CSwitch 0, ?????, "Generate code for Windows", -win, OFF
    Family "Miscellaneous"
      VSwitch 0, ?????, "Define macro:", -def, =, ONE, REQ, ""
      VSwitch  0, ?????, "Other options(,):",,, MULTI, REQ, ""
      CSwitch 0, ?????, "Syntax check only", -sy, OFF
      CSwitch 0, ?????, "Extend floating-point precision", -xf, OFF
      CSwitch 0, ?????, "Operate quietly",-q, ON
      CSwitch 0, ?????, "Generate file dependencies", -dep, ON
      CSwitch 0, ?????, "LF with FF", -lf, OFF
      C2Switch 0, ?????, "Include default library info", "", -nolib, ON
      CSwitch 0, ?????, "Unit 6 is a CC device", -cc, OFF
      RGroup "Character set"
        RSwitch 1, ?????, "Compiler default", , ON
        RSwitch 1, ?????, "Chinese character set", -chi, OFF
        RSwitch 1, ?????, "Japanese character set", -j, OFF
        RSwitch 1, ?????, "Korean characer set", -ko , OFF
      RGroup "Line length"
        RSwitch 1, ?????, "Compiler default", , ON
        RSwitch 1, ?????, "Standard length", -noxl , OFF
        RSwitch 1, ?????, "Extended length", -xl , OFF
::
:endsegment
::

Tool WASM "Assembler"
  Family "File Option"
    CSwitch 0, ?????, "", "wasm $@", ON
    VSwitch 0, ?????, "Include file:", -fi, =, MULTI, REQ, ""
    VSwitch 0, w????, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/win\""
    VSwitch 0, d????, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, o?6??, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, o?2??, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%toolkit)/c/os2h\""
    VSwitch 0, n????, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt\""
    VSwitch 0, l????, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, x????, "Include directories:", -i, =, MULTI, REQ, "\"$(%watcom)/lh\""
    VSwitch 0, ?????, "Name error file:", -fe, =, ONE, REQ, ""
  Family "Memory Model and Processor"
    RGroup "Memory model"
        RSwitch 0, ??6??, "Tiny model", -mt, OFF
        RSwitch 0, rk6??, "Small model", -ms, ON
        RSwitch 0, rb6??, "Small model", -ms, ON
        RSwitch 0, rm6??, "Small model", -ms, ON
        RSwitch 0, ??6??, "Small model", -ms, OFF
        RSwitch 0, ??6??, "Medium model", -mm, OFF
        RSwitch 0, ??6??, "Compact model", -mc, OFF
        RSwitch 0, rk6??, "Large model", -ml, OFF
        RSwitch 0, rb6??, "Large model", -ml, OFF
        RSwitch 0, rm6??, "Large model", -ml, OFF
        RSwitch 0, ??6??, "Large model", -ml, ON
        RSwitch 0, ??6??, "Huge model", -mh, OFF
        RSwitch 0, ??2??, "32-bit flat model", -mf, ON
        RSwitch 0, ??2??, "32-bit small model", -ms, OFF
    RGroup "Target processor"
        RSwitch 1, rk6??, "8086", ,  OFF
        RSwitch 1, rb6??, "8086", ,  OFF
        RSwitch 1, rm6??, "8086", ,  OFF
        RSwitch 1, ??6??, "8086", ,  ON
        RSwitch 1, ??6??, "80186", -1, OFF
        RSwitch 1, rk6??, "80286", -2, ON
        RSwitch 1, ??6??, "80286", -2, OFF
        RSwitch 1, rb6??, "80386", -3, ON
        RSwitch 1, rm6??, "80386", -3, ON
        RSwitch 1, ??6??, "80386", -3, OFF
        RSwitch 1, ??6??, "80486", -4, OFF
        RSwitch 1, ??6??, "Pentium", -5, OFF
        RSwitch 1, ??6??, "Pentium Pro", -6, OFF
        RSwitch 1, ??2??, "80386 register-based calling", -3r, OFF
        RSwitch 1, ??2??, "80386 stack-based calling", -3s, OFF
        RSwitch 1, ??2??, "80486 register-based calling", -4r, OFF
        RSwitch 1, ??2??, "80486 stack-based calling", -4s, OFF
        RSwitch 1, ??2??, "Pentium register-based calling", -5r, OFF
        RSwitch 1, ??2??, "Pentium stack-based calling", -5s, OFF
        RSwitch 1, ds2??, "Pentium Pro register-based calling", -6r, OFF
        RSwitch 1, dy2??, "Pentium Pro register-based calling", -6r, OFF
        RSwitch 1, ??2??, "Pentium Pro register-based calling", -6r, ON
        RSwitch 1, ds2??, "Pentium Pro stack-based calling", -6s, ON
        RSwitch 1, dy2??, "Pentium Pro stack-based calling", -6s, ON
        RSwitch 1, ??2??, "Pentium Pro stack-based calling", -6s, OFF
  Family "Code Generation Options"
    VSwitch 0, ?????, "Name of data segment:", -nd,, ONE, REQ, ""
    VSwitch 0, ?????, "Name of text segment:", -nt,, ONE, REQ, ""
    VSwitch 0, ?????, "Name of module:", -nm,, ONE, REQ, ""
    CSwitch 0, ?????, "Mangle C names like MASM", -zcm
    CSwitch 0, ?????, "Allow C-style octal constants", -o, OFF
    RGroup "Floating-point level"
        RSwitch 1, ?????, "Compiler default", , ON
        RSwitch 1, ?????, "Inline 8087 instructions", -fp0, OFF
        RSwitch 1, ?????, "Inline 80287 instructions", -fp2, OFF
        RSwitch 1, ?????, "Inline 80387 instructions", -fp3, OFF
        RSwitch 1, ?????, "Inline Pentium instructions", -fp5, OFF
        RSwitch 1, ?????, "Inline Pentium Pro instructions", -fp6, OFF
    RGroup "Floating-point model"
        RSwitch 1, r????, "Compiler default", , OFF
        RSwitch 1, ?????, "Compiler default", , ON
        RSwitch 1, ?????, "Inline with emulator", -fpi, OFF
        RSwitch 1, r????, "Inline with coprocessor", -fpi87, ON
        RSwitch 1, ?????, "Inline with coprocessor", -fpi87, OFF
        RSwitch 1, ?????, "Floating-point calls", -fpc, OFF
  Family Debugging
    RGroup "Debugging style"
        RSwitch 0, ?????, "No debugging information", , ON, OFF
        RSwitch 0, ?????, "Line number information", -d1, OFF, ON
  Family Diagnostics
    RGroup "Warning level"
        RSwitch 0, ?????, "Warning level 0", -w0, OFF
        RSwitch 0, ?????, "Warning level 1", -w1, OFF
        RSwitch 0, ?????, "Warning level 2", -w2, OFF
        RSwitch 0, ?????, "Warning level 3", -w3, OFF
        RSwitch 0, ?????, "Warning level 4", -w4, ON
    CSwitch 0, ?????, "Treat warnings as errors", -we, OFF
    VSwitch 0, ?????, "Error count:", -e,, ONE, REQ, "25"
    CSwitch 0, ?????, "Full pathnames in error messages:", -ef
  Family Miscellaneous
    CSwitch 0, ?????, "Stop reading at END", -e, OFF
    CSwitch 0, ?????, "Signed types for signed values", -j, OFF
    CSwitch 0, ?????, "Quiet operation", -zq, ON
    VSwitch 0, ?????, "Macro definitions:", -d,, MULTI, REQ, ""
    CSwitch 0, ?????, "No data in code records", -c, OFF
    VSwitch 0, ?????, "Other options:",,, MULTI, REQ, ""

Tool WLINK "Linker"
  Family Basic
    CSwitch  0, ?????, "", "wlink", ON
    CSwitch  0, ?????, " name", "name $'", ON
    RGroup "Debugging information"
        RSwitch  0, ?????, "No debug information", , ON, OFF
        RSwitch  0, ?????, "Debug CodeView", "d codeview op cvp", OFF
        RSwitch  0, ?????, "Debug Dwarf", "d dwarf", OFF
        RSwitch  0, l?2e?, "Netware symbols", "d novell", OFF
        RSwitch  0, ?????, "Debug Watcom", "d watcom", OFF
        RSwitch  0, ?????, "Debug line numbers", "d lines", OFF
        RSwitch  0, ?????, "Debug types", "d types", OFF
        RSwitch  0, ?????, "Debug locals", "d locals", OFF
        RSwitch  0, ?????, "Debug all", "d all", OFF, ON
    VSwitch  1, w?6d?, "System:", sys, " ", ONE, REQ, windows_dll
    VSwitch  1, w?6??, "System:", sys, " ", ONE, REQ, windows
    VSwitch  1, d?6??, "System:", sys, " ", ONE, REQ, dos
    CSwitch  1, dc6e?, "", com, ON
    VSwitch  1, o?6??, "System:", sys, " ", ONE, REQ, os2
rem OS/2 dll?
    VSwitch  1, w?2??, "System:", sys, " ", ONE, REQ, win386
    VSwitch  1, dr2??, "System:", sys, " ", ONE, REQ, dos4g
    VSwitch  1, dw2??, "System:", sys, " ", ONE, REQ, causeway
    VSwitch  1, dx2??, "System:", sys, " ", ONE, REQ, cwdllr
    VSwitch  1, dy2??, "System:", sys, " ", ONE, REQ, cwdlls
    VSwitch  1, dm2??, "System:", sys, " ", ONE, REQ, pmodew
    VSwitch  1, da2??, "System:", sys, " ", ONE, REQ, dos32a
    VSwitch  1, db2??, "System:", sys, " ", ONE, REQ, dos32x
    VSwitch  1, dp2??, "System:", sys, " ", ONE, REQ, pharlap
    VSwitch  1, dt2??, "System:", sys, " ", ONE, REQ, tnt
    VSwitch  1, o?2??, "System:", sys, " ", ONE, REQ, os2v2
    CSwitch  1, op?e?, "", pm, ON
    CSwitch  1, of?e?, "", full, ON
    CSwitch  1, od6e?, "", phys, ON
    CSwitch  1, ov2e?, "", virt, ON
    VSwitch  1, nw2??, "System:", sys, " ", ONE, REQ, nt_win
    VSwitch  1, na2e?, "System:", sys, " ", ONE, REQ, nt_win
    VSwitch  1, nu2e?, "System:", sys, " ", ONE, REQ, nt_win
    VSwitch  1, no2e?, "System:", sys, " ", ONE, REQ, nt_win
    VSwitch  1, nm2e?, "System:", sys, " ", ONE, REQ, nt_win
    VSwitch  1, nm2d?, "System:", sys, " ", ONE, REQ, nt_dll
    VSwitch  1, na2d?, "System:", sys, " ", ONE, REQ, nt_dll
    VSwitch  1, no2d?, "System:", sys, " ", ONE, REQ, nt_dll
    VSwitch  1, nu2d?, "System:", sys, " ", ONE, REQ, nt_dll
    VSwitch  1, np2d?, "System:", sys, " ", ONE, REQ, nt_dll
    VSwitch  1, n_2d?, "System:", sys, " ", ONE, REQ, nt_dll
    VSwitch  1, nc2??, "System:", sys, " ", ONE, REQ, nt
    VSwitch  1, nwa??, "System:", sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, naae?, "System:", sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, nuae?, "System:", sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, noae?, "System:", sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, nmae?, "System:", sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, nmad?, "System:", sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, naad?, "System:", sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, noad?, "System:", sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, nuad?, "System:", sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, npad?, "System:", sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, n_ad?, "System:", sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, nca??, "System:", sys, " ", ONE, REQ, ntaxp
    VSwitch  1, l?2??, "System:", sys, " ", ONE, REQ, netware
rem Other Netware variations?
    VSwitch  1, df2??, "System:", sys, " ", ONE, REQ, x32rv
    VSwitch  1, df2??, "System:", sys, " ", ONE, REQ, x32r
    VSwitch  1, ds2??, "System:", sys, " ", ONE, REQ, x32sv
    VSwitch  1, ds2??, "System:", sys, " ", ONE, REQ, x32s
    VSwitch  1, x?2??, "System:", sys, " ", ONE, REQ, linux
    VSwitch  1, rp2e?, "System:", sys, " ", ONE, REQ, rdos
    VSwitch  1, rp2d?, "System:", sys, " ", ONE, REQ, rdos_dll
    VSwitch  1, ru?d?, "System:", sys, " ", ONE, REQ, rdos_efi
    VSwitch  1, rk6??, "System:", sys, " ", ONE, REQ, rdos_dev16
    VSwitch  1, rk2??, "System:", sys, " ", ONE, REQ, rdosdev
    VSwitch  1, rb6??, "System:", sys, " ", ONE, REQ, rdos_bin16
    VSwitch  1, rb2??, "System:", sys, " ", ONE, REQ, rdos_bin32
    VSwitch  1, rm6??, "System:", sys, " ", ONE, REQ, rdos_mboot
    CSwitch  1, d?2d?, " Link DLL", , ON
    CSwitch  1, w?2d?, " Link DLL", , ON
    CSwitch  1, w?6d?, " Link DLL", , ON
    CSwitch  1, n??d?, " Link DLL", , ON
    CSwitch  1, r??d?, " Link DLL", , ON
rem    CSwitch  1, x??d?, " Link DLL", , ON
    CSwitch  1, ???d?, " Link DLL", dll, ON
    CSwitch  1, wm6d?, " InitInst", initi, ON
    CSwitch  1, wa6d?, " InitInst", initi, ON
    CSwitch  1, na?d?, " InitInst", initi, ON
    CSwitch  1, nm?d?, " InitInst", initi, ON
    CSwitch  1, no?d?, " InitInst", initi, ON
    CSwitch  1, nu?d?, " InitInst", initi, ON
    CSwitch  1, o?2d?, " InitInst", initi, ON
    CSwitch  1, o?2d?, " TermInst", termi, ON
    CSwitch  1, nu?d?, " TermInst", termi, ON
    CSwitch  1, na?d?, " TermInst", termi, ON
    CSwitch  1, no?d?, " TermInst", termi, ON
    CSwitch  1, nm?d?, " TermInst", termi, ON
    CSwitch  1, wm6d?, " Mem", mem, ON
    CSwitch  1, wa6d?, " Mem", mem, ON
    VSwitch  1, l?2e?, "Output screen", "op scr", " '%s'", ONE, OFF, ""
    VSwitch  1, ?????, "Stack:", "op st", =, ONE, REQ, ""
    VSwitch  1, wm?e?, "Heap:", "op heap", =, ONE, REQ, 1K
    VSwitch  1, wa?e?, "Heap:", "op heap", =, ONE, REQ, 1K
    VSwitch  1, rp???, "Image base:", "op offset", =, ONE, REQ, ""
    VSwitch  1, rk???, "Code selector:", "op codesel", =, ONE, REQ, ""
    VSwitch  1, rk???, "Data selector:", "op datasel", =, ONE, REQ, ""
    VSwitch  1, ?????, "Other options(,):",,, MULTI, REQ, ""
    CSwitch  1, ?????, "Map file", "op m", ON
    CSwitch  1, ?????, "Line numbers in map file", "op maplines", OFF
    CSwitch  1, l????, "Incremental linking", "op inc", OFF
    CSwitch  1, d?6??, "Incremental linking", "op inc", OFF
    CSwitch  1, ????w, "Incremental linking", "op inc", OFF
    CSwitch  1, ????j, "Incremental linking", "op inc", OFF
    CSwitch  1, ????8, "Incremental linking", "op inc", OFF
    CSwitch  1, o????, "Incremental linking", "op inc", OFF
    CSwitch  1, na???, "Incremental linking", "op inc", OFF, ON
    CSwitch  1, no???, "Incremental linking", "op inc", OFF, ON
    CSwitch  1, nu???, "Incremental linking", "op inc", OFF, ON
    CSwitch  1, nm???, "Incremental linking", "op inc", OFF, ON
    CSwitch  1, wa???, "Incremental linking", "op inc", OFF, ON
    CSwitch  1, wm???, "Incremental linking", "op inc", OFF, ON
    CSwitch  1, ?????, "Incremental linking", "op inc", OFF
    CSwitch  1, n????, "No stdcall name decoration", "op nostdcall", OFF
  Family "Import, Export and Library"
    CSwitch  0, ?????, "No default libraries", "op nod", OFF
    VSwitch  0, ?????, "Library directories(;):", libp, " ", MULTI, REQ, ""
    VSwitch  0, wa6d?, "Libraries(,):",libr, " ", MULTI, REQ, "mfw250.lib mfwo250.lib mfwd250.lib commdlg.lib shell.lib mfcoleui.lib compobj.lib storage.lib ole2.lib ole2disp.lib", "mfw250d.lib mfwo250d.lib mfwd250d.lib commdlg.lib shell.lib mfcoleui.lib compobj.lib storage.lib ole2.lib ole2disp.lib"
    VSwitch  0, ?????, "Libraries(,):",libr, " ", MULTI, REQ, ""
    VSwitch  0, dc6e?, "Library files(,):", libf, " ", MULTI, REQ, "cstart_t"
    VSwitch  0, ???e?, "Library files(,):", libf, " ", MULTI, REQ, ""
    VSwitch  1, w????, "Import names(,):", imp, " ", ONE, REQ, ""
    VSwitch  1, n????, "Import names(,):", imp, " ", ONE, REQ, ""
    VSwitch  1, o????, "Import names(,):", imp, " ", ONE, REQ, ""
    VSwitch  1, l????, "Import names(,):", imp, " ", ONE, REQ, ""
    VSwitch  1, x????, "Import names(,):", imp, " ", ONE, REQ, ""
    VSwitch  1, dx???, "Import names(,):", imp, " ", ONE, REQ, ""
    VSwitch  1, dy???, "Import names(,):", imp, " ", ONE, REQ, ""
    VSwitch  1, rp???, "Import names(,):", imp, " ", ONE, REQ, ""
    VSwitch  1, w????, "Export names(,):", exp, " ", MULTI, REQ, ""
    VSwitch  1, n????, "Export names(,):", exp, " ", MULTI, REQ, ""
    VSwitch  1, o????, "Export names(,):", exp, " ", MULTI, REQ, ""
    VSwitch  1, l????, "Export names(,):", exp, " ", MULTI, REQ, ""
    VSwitch  1, x????, "Export names(,):", exp, " ", MULTI, REQ, ""
    VSwitch  1, dx???, "Export names(,):", exp, " ", MULTI, REQ, ""
    VSwitch  1, dy???, "Export names(,):", exp, " ", MULTI, REQ, ""
    VSwitch  1, rp???, "Export names(,):", exp, " ", MULTI, REQ, ""
  Family Advanced
    CSwitch  0, ?????, "Verbose map file", "op v", OFF
    RGroup "Case-sensitive link"
        RSwitch 0, ?????, "Default", , ON
        RSwitch 0, ?????, "Case sensitive:", "op c", OFF
        RSwitch 0, ?????, "No case sensitive:", "op nocase", OFF
    VSwitch  0, ?????, "Error count:", "op maxe", =, ONE, REQ, 25
    CSwitch  0, w?6??, "Check relocation of RW data", "op rwr", OFF
    CSwitch  0, ?????, "Dosseg", "op d", OFF
    CSwitch  0, ?????, "Undefines OK", "op u", OFF
    CSwitch  0, ?????, "Quiet", "op q", ON
:segment !C_FOR_PB
    CSwitch  0, ?????, "Produce symbol file", "op symf", ON
    VSwitch  0, w????, "Version (Major.minor.rev):", "op vers", = , ONE, REQ, ""
    VSwitch  0, n????, "Version (Major.minor.rev):", "op vers", = , ONE, REQ, ""
    VSwitch  0, o????, "Version (Major.minor.rev):", "op vers", = , ONE, REQ, ""
    VSwitch  0, l????, "Version (Major.minor.rev):", "op vers", = , ONE, REQ, ""
:endsegment
    VSwitch  1, ?????, "Name length:", "op namel", =, ONE, REQ, ""
:segment !C_FOR_PB
    VSwitch  1, ?????, "ModTrace names(,):", modt, " ", ONE, REQ, ""
    VSwitch  1, ?????, "SymTrace names(,):", symt, " ", ONE, REQ, ""
    VSwitch  1, nm?d?, "Reference names(,):", ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, na?d?, "Reference names(,):", ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, nu?d?, "Reference names(,):", ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, no?d?, "Reference names(,):", ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, nm?e?, "Reference names(,):", ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, na?e?, "Reference names(,):", ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, no?e?, "Reference names(,):", ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, nu?e?, "Reference names(,):", ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, wm?d?, "Reference names(,):", ref, " ", MULTI, REQ, "__clib_WEP_"
    VSwitch  1, wa?d?, "Reference names(,):", ref, " ", MULTI, REQ, "__clib_WEP_"
    VSwitch  1, ?????, "Reference names(,):", ref, " ", MULTI, REQ, ""
    VSwitch  1, nm?e?, "Starting address:", "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, na?e?, "Starting address:", "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, no?e?, "Starting address:", "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, nu?e?, "Starting address:", "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, ?????, "Starting address:", "op start", =, ONE, REQ, ""
:endsegment
    VSwitch  1, o????, "Description:", "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, n????, "Description:", "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, w????, "Description:", "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dr???, "Description:", "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dw???, "Description:", "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, da???, "Description:", "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, db???, "Description:", "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dm???, "Description:", "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dx???, "Description:", "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dy???, "Description:", "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, r????, "Description:", "op de", " '%s'", ONE, REQ, ""
rem PackCode, PackData for DOS, OS/2, QNX, Win16?
:segment !C_FOR_PB
  Family Special
    RGroup "DGROUP sharing"
        RSwitch  0, w?2d?, "Use linker default", "", ON
        RSwitch  0, n??d?, "Use linker default", "", ON
rem     RSwitch  0, x??d?, "Use linker default", "", ON
        RSwitch  0, w?6d?, "Use linker default", "", ON
        RSwitch  0, r??d?, "Use linker default", "", ON
        RSwitch  0, ???d?, "Use linker default", "", OFF
        RSwitch  0, ?????, "Use linker default", "", ON
        RSwitch  0, w?2d?, "DGROUP is shared", "op one", OFF
        RSwitch  0, n??d?, "DGROUP is shared", "op one", OFF
rem     RSwitch  0, x??d?, "DGROUP is shared", "op one", OFF
        RSwitch  0, w?6d?, "DGROUP is shared", "op one", OFF
        RSwitch  0, o?2d?, "DGROUP is shared", "op one", OFF
        RSwitch  0, ???d?, "DGROUP is shared", "op one", OFF
        RSwitch  0, ?????, "DGROUP is shared", "op one", OFF
        RSwitch  0, n??d?, "DGROUP is not shared", "op many", OFF
        RSwitch  0, w??d?, "DGROUP is not shared", "op many", OFF
        RSwitch  0, r??d?, "DGROUP is not shared", "op many", OFF
        RSwitch  0, ???d?, "DGROUP is not shared", "op many", ON
        RSwitch  0, ?????, "DGROUP is not shared", "op many", OFF
    RGroup "Runtime environment"
        RSwitch  0, nw???, "Default GUI", "", ON
        RSwitch  0, nw???, "Win32s Windows 3.x:", "ru win=3.10", OFF
        RSwitch  0, nc???, "Default character mode", "", ON
        RSwitch  0, nc???, "16-bit OS/2 1.x:", "ru os2=3.10", OFF
        RSwitch  0, n?2d?, "Default GUI", "", ON
        RSwitch  0, n?2d?, "Character mode:", "ru con=4.0", OFF
        RSwitch  0, n?2d?, "Win32s Windows 3.x:", "ru win=3.10", OFF
        RSwitch  0, r????, "Default character mode", "", ON
    VSwitch  0, o????, "Stub executable", "op stub", =, ONE, REQ, ""
    CSwitch  0, na?e?, "Eliminate dead code", "op el", ON, OFF
    CSwitch  0, no?e?, "Eliminate dead code", "op el", ON, OFF
    CSwitch  0, nu?e?, "Eliminate dead code", "op el", ON, OFF
    CSwitch  0, nm?e?, "Eliminate dead code", "op el", ON, OFF
    CSwitch  0, wm?e?, "Eliminate dead code", "op el", ON, OFF
    CSwitch  0, wa?e?, "Eliminate dead code", "op el", ON, OFF
    CSwitch  0, ?????, "Eliminate dead code", "op el", OFF
:endsegment
::
:segment !C_FOR_PB
::

Tool WLIB "Library Manager"
  Family Basic
    CSwitch  0, ?????, "", "wlib", ON
    CSwitch  0, ???s?, "Don't create .bak file", -b, ON
    CSwitch  0, ???s?, "Case sensitive", -c, ON
    CSwitch  0, ???s?, "Display C++ mangled names", -m, OFF
    CSwitch  0, ???s?, "Always create new library", -n, ON
    CSwitch  0, ???s?, "Quiet operation", -q, ON
    CSwitch  0, ???s?, "Strip LINNUM records", -s, OFF
    CSwitch  0, ???s?, "Trim THEADR pathnames", -t, OFF
    CSwitch  0, ???s?, "Explode all objects in library", -x, OFF
    VSwitch  1, ???s?, "Page bound:", -p, =, ONE, REQ, 512
    VSwitch  1, ???s?, "Listing file name:", -l, =, ONE, REQ, ""
    VSwitch  1, ???s?, "Output directory:", -d, =, ONE, REQ, ""
    VSwitch  1, ???s?, "Output library name:", -o, =, ONE, REQ, ""
  Family "Import Library"
    RGroup "Resident symbols"
        RSwitch  0, ???s?, "Do not import", , ON
        RSwitch  0, ???s?, "Import by name", -irn, OFF
        RSwitch  0, ???s?, "Import by ordinal", -iro, OFF
    RGroup "Non-resident symbols"
        RSwitch  0, ???s?, "Do not import", , ON
        RSwitch  0, ???s?, "Import by name", -inn, OFF
        RSwitch  0, ???s?, "Import by ordinal", -ino, OFF
::
:endsegment
::

Tool WRC "Resource Compiler"
  Family Resource
    CSwitch 0, ?????, "", "wrc $*.rc", ON
    RGroup "Build target"
        RSwitch  0, w????, "Win16", -bt=windows, ON
:segment !C_FOR_PB
        RSwitch  0, w????, "Win32", -bt=nt, OFF
        RSwitch  0, w????, "OS/2", -bt=os2, OFF
        RSwitch  0, n????, "Win16", -bt=windows, OFF
        RSwitch  0, n????, "Win32", -bt=nt, ON
        RSwitch  0, n????, "OS/2", -bt=os2, OFF
        RSwitch  0, o????, "Win16", -bt=windows, OFF
        RSwitch  0, o????, "Win32", -bt=nt, OFF
        RSwitch  0, o????, "OS/2", -bt=os2, ON
        RSwitch  0, r????, "RDOS", -bt=nt, ON
:endsegment
    RGroup "Multi-byte character support"
        RSwitch  0, ?????, "Single byte characters only", , ON
        RSwitch  0, ?????, "Kanji", -zk0, OFF
        RSwitch  0, ?????, "Traditional Chinese", -zk1, OFF
        RSwitch  0, ?????, "Korean", -zk2, OFF
        RSwitch  0, ?????, "Simplified Chinese", -zk3, OFF
        RSwitch  0, ?????, "Unicode UTF-8", -zku8, OFF
    VSwitch 1, wa???, "Macro definitions:", -d, , MULTI, REQ, "_AFXDLL"
    VSwitch 1, na???, "Macro definitions:", -d, , MULTI, REQ, "_AFXDLL WIN32 _WIN32 __NT__"
    VSwitch 1, no???, "Macro definitions:", -d, , MULTI, REQ, "_AFXDLL _UNICODE WIN32 _WIN32 __NT__"
    VSwitch 1, nu???, "Macro definitions:", -d, , MULTI, REQ, "_UNICODE WIN32 _WIN32 __NT__"
    VSwitch 1, n????, "Macro definitions:", -d, , MULTI, REQ, "WIN32 _WIN32 __NT__"
    VSwitch 1, ?????, "Macro definitions:", -d, , MULTI, REQ, ""
    VSwitch 1, w????, "Include directories:", -i, =, MULTI, REQ, "\"$[:;$(%watcom)/h;$(%watcom)/h/win\""
    VSwitch 1, n????, "Include directories:", -i, =, MULTI, REQ, "\"$[:;$(%watcom)/h;$(%watcom)/h/nt\""
    VSwitch 1, o????, "Include directories:", -i, =, MULTI, REQ, "\"$[:;$(%watcom)/h;$(%watcom)/h/os2\""
    VSwitch 1, ?????, "Include directories:", -i, =, MULTI, REQ, ""
    CSwitch 1, ?????, "Ignore INCLUDE environment variable", -x, OFF
    CSwitch 1, w????, "Microsoft format .res file", -zm, OFF
    CSwitch 1, ?????, "Do not preprocess the file", -zn, OFF
    CSwitch 1, ?????, "Quiet operation", -q, ON
    CSwitch 1, ?????, "Output autodepend info", -ad, ON

Tool WRC2 "Resource Compiler (pass 2)"
  Family Resource
    CSwitch 0, ?????, "", wrc, ON
    CSwitch 0, ?????, "Quiet operation", -q, ON
    CSwitch 0, ?????, "Output autodepend info", -ad, ON
    CSwitch 0, ?????, "Requires Windows 3.0 or later", -30, OFF
    CSwitch 0, ?????, "Uses global memory above EMS", -e, OFF
    CSwitch 0, ?????, "Uses LIM 3.2 EMS directly", -l, OFF
    CSwitch 0, ?????, "EMS bank for each instance", -m, OFF
    CSwitch 0, ?????, "Private DLL", -p, OFF
    CSwitch 0, ?????, "Protected mode only", -t, OFF
    RGroup "Segements in fastload section"
        RSwitch 1, w????, "No fastload section", -s0, ON
        RSwitch 1, w????, "Preload only", -s1, OFF
        RSwitch 1, w????, "Preload/Data/Non-discardable", -s2, OFF
::
:segment !C_FOR_PB
::

Tool ORC "OS/2 Resource Compiler"
  Family Resource
    CSwitch 0, ?????, "", rc, ON
    VSwitch 0, ?????, "Include directories:", -i, " ", MULTI, REQ, "$[: $(%watcom)\\h $(%watcom)\\h\\os2"

Tool ORC2 "OS/2 Resource Compiler (pass 2)"
  Family Resource
    CSwitch 0, ?????, "", rc, ON
    VSwitch 0, ?????, "Include directories:", -i, " ", MULTI, REQ, "$[: $(%watcom)\\h $(%watcom)\\h\\os2"

Tool ESQL "Embedded SQL compiler"
  Family Processor
    CSwitch 0, ?????, "", "sqlpp $@ $*.cxx", ON
    CSwitch 0, ?????, "Preprocessor quiet", -q, ON
    RGroup Optimizations
        RSwitch 0, ?????, "No optimizations", , ON
        RSwitch 0, ?????, "Favour code size", -c, OFF
        RSwitch 0, ?????, "Favour data size", -d, OFF
    CSwitch 0, ?????, "Generated static data is FAR", -f, OFF
    CSwitch 0, ?????, "Use long ints", -il, OFF
    CSwitch 0, ?????, "Generate line numbers", -n, OFF
    VSwitch 0, ?????, "Maximum string constant length:", -s, " ", ONE, REQ, ""
    VSwitch 0, ?????, "Login(id,pswd):", -l, " ", ONE, REQ, ""
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
  Family ImageEditor
    CSwitch 0, o????, " run", iconedit, ON
    CSwitch 0, ????n, " run", "$(%watcom)\\binnt\\wimgedit /n", ON
    CSwitch 0, ????a, " run", "$(%watcom)\\axpnt\\wimgedit /n", ON
    CSwitch 0, ????9, " run", "$(%watcom)\\binnt\\wimgedit /n", ON
    CSwitch 0, ?????, " run", "$(%watcom)\\binw\\wimgedit /n", ON
::
:segment !C_FOR_PB
::

Tool WINHC "Windows Help Compiler"
  Family Compiler
    RGroup "Help Compiler"
rem These must be in the user's path
        RSwitch 0, ?????, "HTML Help", hhw , OFF
        RSwitch 0, ?????, "Version 4.0", hcw , OFF
        RSwitch 0, ?????, "Version 3.1", hc31 , ON
        RSwitch 0, ?????, "Version 3.0", hc30 , OFF

Tool OIPF "OS/2 Help Compiler"
  Family Compiler
    CSwitch 0, ?????, "", "ipfc ", ON
    CSwitch 0, ???i?, "", /inf, ON
    VSwitch 0, ?????, "Country Code:", /COUNTRY, =, ONE, REQ, ""
    VSwitch 0, ?????, "Code page:", /CODEPAGE, =, ONE, REQ, ""
    VSwitch 0, ?????, "Language:", /L, =, ONE, REQ, ""
    RGroup "Warning level"
        RSwitch 0, ?????, "Warning level 1", /W1, OFF
        RSwitch 0, ?????, "Warning level 2", /W2, OFF
        RSwitch 0, ?????, "Warning level 3", /W3, ON
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
  Family "Destination Directory"
    VSwitch 0, ?????, "Destination directory","copy $@"," ", ONE, REQ, ""
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

Rule RDLL, WLINK, rp?d?
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

Rule REFI, WLINK, ru?d?
  Target *.efi
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol RES, *.res
  Symbol EFI, *.efi
  Command " @%write $*.lk1 $<FIL,>"
  Command " @%append $*.lk1 $<LIBR,>"
  Command " *$<#> @$'.lk1"

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

rem Added for backwards compatability:
Compat "w????Import names(,):" "w????Import files(,):"
Compat "n????Import names(,):" "n????Import files(,):"
Compat "o????Import names(,):" "o????Import files(,):"
Compat "l????Import names(,):" "l????Import files(,):"
Compat "?????Chinese character set" "?????Chineese character set"
Compat "w????Chinese/Taiwanese" "w????Chineese/Taiwanese"
Compat "?????Do not preprocess the file" "?????Do not proprocess the file"

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
