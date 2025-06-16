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


Version 5

IncludeFile override.cfg

Tool WMAKE "Make Utility"
  Family Make
    CSwitch -1, ?????, "", "wmake -f $(%_makefile)", ON
    CSwitch 0, ?????, WMAKE_a, -a, OFF
    CSwitch 0, ?????, WMAKE_h, -h, ON
    CSwitch 0, ?????, WMAKE_e, -e, ON
    CSwitch 0, ?????, WMAKE_i, -i, OFF
    CSwitch 0, ?????, WMAKE_c, -c, OFF
    CSwitch 0, ?????, WMAKE_m, -m, OFF
    CSwitch 0, ?????, WMAKE_o, -o, OFF
    CSwitch 0, ?????, WMAKE_p, -p, OFF
    CSwitch 0, ?????, WMAKE_r, -r, OFF
    CSwitch 0, ?????, WMAKE_t, -t, OFF
    CSwitch 0, ?????, WMAKE_z, -z, OFF
    CSwitch 1, ?????, WMAKE_d, -d, OFF
    CSwitch 1, ?????, WMAKE_k, -k, OFF
    CSwitch 1, ?????, WMAKE_n, -n, OFF
    CSwitch 1, ?????, WMAKE_q, -q, OFF
    CSwitch 1, ?????, WMAKE_s, -s, OFF
    VSwitch 1, ?????, WMAKE_l, -l, " ", ONE, OFF, "$*.lst"
    VSwitch 1, ?????, WMAKE_others,,, ONE, REQ, ""
    SwitchText WMAKE_a, "Make all targets"
    SwitchText WMAKE_h, "Don't print header"
    SwitchText WMAKE_e, "Erase bad targets"
    SwitchText WMAKE_i, "Ignore bad targets"
    SwitchText WMAKE_c, "Don't check for files made"
    SwitchText WMAKE_m, "Don't use MAKEINIT file"
    SwitchText WMAKE_o, "Use circular path"
    SwitchText WMAKE_p, "Print info"
    SwitchText WMAKE_r, "Ignore built-in defns"
    SwitchText WMAKE_t, "Only touch files"
    SwitchText WMAKE_z, "Do not erase files"
    SwitchText WMAKE_d, "Echo progress of work"
    SwitchText WMAKE_k, "Continue after an error"
    SwitchText WMAKE_n, "Print without executing"
    SwitchText WMAKE_q, "Query mode"
    SwitchText WMAKE_s, "Silent mode"
    SwitchText WMAKE_l, "Append output to log:"
    SwitchText WMAKE_others, "Other options:"

Tool RUN "User Application"
  Family Application
    CSwitch -1, x???d, "", "!Error Cannot run Linux target under DOS$", ON
    CSwitch -1, x???o, "", "!Error Cannot run Linux target under OS/2$", ON
    CSwitch -1, x???l, "", "!Error Cannot run Linux target under Netware$", ON
    CSwitch -1, x???n, "", "!Error Cannot run Linux target under Win32$", ON
    CSwitch -1, x???q, "", "!Error Cannot run Linux target under QNX$", ON
    CSwitch -1, x???w, "", "!Error Cannot run Linux target under Win16$", ON
rem make fullscreen an option where both windowed & fullscreen work
    CSwitch 0, oc?eo, RUN_fullscr, "!Fullscreen", OFF
    CSwitch 0, of?eo, RUN_fullscr, "!Fullscreen", ON
    CSwitch 0, ov?eo, RUN_fullscr, "!Fullscreen", OFF
    CSwitch 0, d??eo, RUN_fullscr, "!Fullscreen", ON
    CSwitch -1, dr2e?, "", $(%watcom)\binw\dos4gw, ON
    CSwitch -1, dw2e?, "", $(%watcom)\binw\cwstub, ON
rem Pharlap run386 must be in your path
    CSwitch -1, dp2e?, "", run386, ON
rem Pharlap TNT run386 must be in your path
    CSwitch -1, dt2e?, "", tnt, ON
    CSwitch -1, w?2eo, "", "!Fullscreen", ON
    CSwitch -1, ???eo, "",, ON
    CSwitch -1, ?????, "", $@, ON
    VSwitch 0, ?????, RUN_parms,,, ONE, REQ, ""
    SwitchText RUN_fullscr, "Fullscreen"
    SwitchText RUN_parms, "Application parameters:"

Tool WDEBUG "Local Debugger"
  Family "Debugging Session"
    CSwitch -1, n???o, "", "!Error Cannot locally debug a Windows NT target in OS/2$", ON
    CSwitch -1, w?2?o, "", "!Error Cannot locally debug a 32bit Windows 3.x program in OS/2$", ON
    CSwitch -1, df2e?, "", "!Error Cannot locally debug a FlashTek executable$", ON
    CSwitch -1, ds2e?, "", "!Error Cannot locally debug a FlashTek executable$", ON
    CSwitch -1, x???d, "", "!Error Cannot locally debug a Linux target under DOS$", ON
    CSwitch -1, x???o, "", "!Error Cannot locally debug a Linux target under OS/2$", ON
    CSwitch -1, x???l, "", "!Error Cannot locally debug a Linux target under Netware$", ON
    CSwitch -1, x???n, "", "!Error Cannot locally debug a Linux target under Win32$", ON
    CSwitch -1, x???q, "", "!Error Cannot locally debug a Linux target under QNX$", ON
    CSwitch -1, x???w, "", "!Error Cannot locally debug a Linux target under Win16$", ON
    CSwitch -1, r????, "", "!Error Cannot locally debug a RDOS target$", ON
    CSwitch -1, dr2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, dw2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\cw.trp $&\n", ON
    CSwitch -1, dp2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch -1, dt2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch -1, da2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, db2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, dm2en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, d?6en, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\std.trp $&\n", ON
    CSwitch -1, dr2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, dw2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\cw.trp $&\n", ON
    CSwitch -1, dp2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch -1, dt2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch -1, da2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, db2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, dm2e9, "", "$(%watcom)\\binw\\winserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, dr2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, dw2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\cw.trp $&\n", ON
    CSwitch -1, dp2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch -1, dt2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\pls.trp $&\n", ON
    CSwitch -1, da2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, db2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, dm2eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\rsi.trp $&\n", ON
    CSwitch -1, d?6eo, "", "$(%watcom)\\binw\\vdmserv -once -tr=$(%watcom)\\binw\\std.trp $&\n", ON
:segment !C_FOR_PB
    RGroup "Debugger"
      RSwitch 0, w???o, WDEBUG_wind, "!Fullscreen $(%watcom)\\binw\\wdw", ON
      RSwitch 0, w???n, WDEBUG_wind, $(%watcom)\binnt\wdw, ON
      RSwitch 0, w???a, WDEBUG_wind, $(%watcom)\axpnt\wdw, ON
      RSwitch 0, w???w, WDEBUG_wind, $(%watcom)\binw\wdw, ON
      RSwitch 0, w???j, WDEBUG_wind, $(%watcom)\binw\wdw, ON
      RSwitch 0, w???8, WDEBUG_wind, $(%watcom)\binw\wdw, ON
      RSwitch 0, w???s, WDEBUG_wind, $(%watcom)\binw\wdw, ON
      RSwitch 0, n????, WDEBUG_wind, $(%watcom)\binnt\wdw, ON
      RSwitch 0, w???n, WDEBUG_char, "!Fullscreen $(%watcom)\\binnt\\wd.exe", OFF
      RSwitch 0, w???a, WDEBUG_char, "!Fullscreen $(%watcom)\\axpnt\\wd.exe", OFF
      RSwitch 0, w???8, WDEBUG_char, "!Fullscreen $(%watcom)\\n98bw\\wdc.exe", OFF
      RSwitch 0, w???w, WDEBUG_char, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
      RSwitch 0, w???j, WDEBUG_char, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
      RSwitch 0, w???o, WDEBUG_char, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
      RSwitch 0, w???s, WDEBUG_char, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
      RSwitch 0, n????, WDEBUG_char, $(%watcom)\binnt\wd, OFF
      RSwitch 0, o????, WDEBUG_pmwin, $(%watcom)\binp\wdw, ON
      RSwitch 0, o????, WDEBUG_char, "!Fullscreen $(%watcom)\\binp\\wd", OFF
      SwitchText WDEBUG_wind, "Windowed"
      SwitchText WDEBUG_char, "Character mode"
      SwitchText WDEBUG_pmwin, "PM"
:endsegment
    CSwitch -1, dr2ew, *WDEBUG_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch -1, dw2ew, *WDEBUG_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch -1, dp2ew, *WDEBUG_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch -1, dt2ew, *WDEBUG_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch -1, da2ew, *WDEBUG_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch -1, db2ew, *WDEBUG_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch -1, dm2ew, *WDEBUG_run, "$(%watcom)\\binw\\wd.pif", ON
    CSwitch -1, dr2ej, *WDEBUG_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch -1, dw2ej, *WDEBUG_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch -1, dp2ej, *WDEBUG_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch -1, dt2ej, *WDEBUG_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch -1, da2ej, *WDEBUG_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch -1, db2ej, *WDEBUG_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch -1, dm2ej, *WDEBUG_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch -1, dr2e8, *WDEBUG_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch -1, dw2e8, *WDEBUG_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch -1, dp2e8, *WDEBUG_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch -1, dt2e8, *WDEBUG_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch -1, da2e8, *WDEBUG_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch -1, db2e8, *WDEBUG_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch -1, dm2e8, *WDEBUG_run, "$(%watcom)\\n98bw\\wd.pif", ON
    CSwitch -1, d???o, *WDEBUG_run, "$(%watcom)\\binp\\wd", ON
    CSwitch -1, d???n, *WDEBUG_run, "$(%watcom)\\binnt\\wdw", ON
    CSwitch -1, d???a, *WDEBUG_run, "$(%watcom)\\axpnt\\wdw", ON
rem binw\wd is used because wdw doesn't load trap files
    CSwitch -1, d???9, *WDEBUG_run, "$(%watcom)\\binw\\wd", ON
    CSwitch -1, d???8, *WDEBUG_run, "$(%watcom)\\n98bw\\wd", ON
    CSwitch -1, d???j, *WDEBUG_run, "$(%watcom)\\binw\\wdj.pif", ON
    CSwitch -1, d????, *WDEBUG_run, "$(%watcom)\\binw\\wd", ON
    CSwitch -1, w???9, "", $(%watcom)\binw\wdw, ON
    CSwitch -1, wp6d8, *WDEBUG_run, "!Fullscreen $(%watcom)\\n98bw\\wdc.exe", ON
    CSwitch -1, wp6d?, *WDEBUG_run, "!Fullscreen $(%watcom)\\binw\\wdc.exe", ON
    CSwitch -1, np2d?, *WDEBUG_run, "$(%watcom)\\binnt\\wdw -powerbuilder -tr=std;2", ON
    CSwitch -1, l???w, *WDEBUG_run, "$(%watcom)\\binw\\wdw", ON
    CSwitch -1, l???j, *WDEBUG_run, "$(%watcom)\\binw\\wdw", ON
    CSwitch -1, l???8, *WDEBUG_run, "$(%watcom)\\n98bw\\wdw", ON
    CSwitch -1, l????, *WDEBUG_run, wdw, ON
::
:segment !C_FOR_PB
::
    VSwitch 1, dr2ew, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dw2ew, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dp2ew, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dt2ew, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, da2ew, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, db2ew, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dm2ew, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dr2ej, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dw2ej, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dp2ej, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dt2ew, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, da2ew, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, db2ew, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dm2ew, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dr2e8, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dw2e8, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dp2e8, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dt2e8, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, da2e8, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, db2e8, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dm2e8, WDEBUG_tr, -tr, =, ONE, REQ, "win"
    VSwitch 1, dr2e9, WDEBUG_tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dw2e9, WDEBUG_tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dp2e9, WDEBUG_tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dt2e9, WDEBUG_tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, da2e9, WDEBUG_tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, db2e9, WDEBUG_tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, dm2e9, WDEBUG_tr, -tr, =, ONE, REQ, "win;$&"
    VSwitch 1, d???o, WDEBUG_tr, -tr, =, ONE, REQ, "vdm;$&"
    VSwitch 1, d???n, WDEBUG_tr, -tr, =, ONE, REQ, "vdm;$&"
    VSwitch 1, dr2e?, WDEBUG_tr, -tr, =, ONE, REQ, "rsi"
    VSwitch 1, da2e?, WDEBUG_tr, -tr, =, ONE, REQ, "rsi"
    VSwitch 1, db2e?, WDEBUG_tr, -tr, =, ONE, REQ, "rsi"
    VSwitch 1, dm2e?, WDEBUG_tr, -tr, =, ONE, REQ, "rsi"
    VSwitch 1, dw2e?, WDEBUG_tr, -tr, =, ONE, REQ, "cw"
    VSwitch 1, dp2e?, WDEBUG_tr, -tr, =, ONE, REQ, "pls"
    VSwitch 1, dt2e?, WDEBUG_tr, -tr, =, ONE, REQ, "pls"
    VSwitch 1, l?2e?, WDEBUG_tr, -tr, =, ONE, REQ, "nov;$&"
    VSwitch 1, ?????, WDEBUG_tr, -tr, =, ONE, REQ, ""
    SwitchText WDEBUG_tr, "Trap file:"
::
:endsegment
::
  Family "Debugger Options"
    VSwitch 0, wp6d?, WDEBUG_i, -i, =, ONE, REQ, "pbwd.dbg"
    VSwitch 0, np2d?, WDEBUG_i, -i, =, ONE, REQ, "pbwdnt.dbg"
    VSwitch 0, ?????, WDEBUG_i, -i, =, ONE, REQ, ""
    VSwitch -1, wp6d?, "", -initcmd, =, ONE, REQ, "{pbstart.dbg $&}"
    SwitchText WDEBUG_i, "Configuration file:"
:segment !C_FOR_PB
    CSwitch 0, ?????, WDEBUG_noi, -noi, OFF
    SwitchText WDEBUG_noi, "Don't invoke configuration"
:endsegment
    CSwitch 0, ?????, WDEBUG_symf, :$*.sym, OFF
    SwitchText WDEBUG_symf, "Use symbol file"
:segment !C_FOR_PB
    CSwitch 0, ?????, WDEBUG_nosy, -nosy, OFF
    CSwitch 0, ?????, WDEBUG_nof, -nof, OFF
    CSwitch 0, ?????, WDEBUG_nom, -nom, OFF
    CSwitch 0, ?????, WDEBUG_nog, -nog, OFF
    CSwitch 0, ?????, WDEBUG_noch, -noch, OFF
    VSwitch 1, ?????, WDEBUG_di, -di, =, MULTI, REQ, ""
    VSwitch 1, ?????, WDEBUG_dy, -dy, =, ONE, REQ, ""
    VSwitch 1, ?????, WDEBUG_ch, -ch, =, ONE, REQ, ""
    SwitchText WDEBUG_nosy, "Don't process symbolic info"
    SwitchText WDEBUG_nof, "Ignore any floating point hardware", "Ignore any floating point hdwr"
    SwitchText WDEBUG_nom, "Ignore any attached mouse"
    SwitchText WDEBUG_nog, "Don't use graphical mouse"
    SwitchText WDEBUG_noch, "Don't do character remapping", "Don't do char remapping"
    SwitchText WDEBUG_di, "Dip files:"
    SwitchText WDEBUG_dy, "Dynamic memory:"
    SwitchText WDEBUG_ch, "Free guarantee:"
:endsegment
  Family "Debugger Display"
    VSwitch 0, ?????, WDEBUG_li, -li, =, ONE, REQ, ""
    VSwitch 0, ?????, WDEBUG_co, -co, =, ONE, REQ, ""
    SwitchText WDEBUG_li, "Lines of character mode screen:", "Lines of char mode screen:"
    SwitchText WDEBUG_co, "Columns of character mode screen:", "Columns of char mode screen:"
    RGroup "Character mode screen"
      RSwitch 0, ?????, WDEBUG_def_adapter, , ON
      RSwitch 0, ?????, WDEBUG_m, -m, OFF
      RSwitch 0, ?????, WDEBUG_c, -c, OFF
      RSwitch 0, ?????, WDEBUG_e, -e, OFF
      RSwitch 0, ?????, WDEBUG_v, -v, OFF
      SwitchText WDEBUG_def_adapter, "Default adapter"
      SwitchText WDEBUG_m, "Monochrome adapter"
      SwitchText WDEBUG_c, "Color adapter"
      SwitchText WDEBUG_e, "43-line EGA adapter"
      SwitchText WDEBUG_v, "50-line VGA adapter"
    RGroup "Character mode screen protocol"
      RSwitch 1, ?????, WDEBUG_def_protocol, , ON
      SwitchText WDEBUG_def_protocol, "Default protocol", "Default Adapter"
:segment !C_FOR_PB
      RSwitch 1, ?????, WDEBUG_o, -o, OFF
      SwitchText WDEBUG_o, "Overwrite protocol"
:endsegment
      RSwitch 1, ?????, WDEBUG_p, -p, OFF
      RSwitch 1, ?????, WDEBUG_s, -s, OFF
      RSwitch 1, ?????, WDEBUG_f, -f, OFF
      RSwitch 1, ?????, WDEBUG_t, -t, OFF
      SwitchText WDEBUG_p, "Page protocol"
      SwitchText WDEBUG_s, "Swap protocol"
      SwitchText WDEBUG_f, "Fast-swap protocol"
      SwitchText WDEBUG_t, "Two protocol"
    CSwitch -1, wp6d?, *WDEBUG_name, "$(%watcom)\\binw\\dlldbg.exe $@", ON
    CSwitch -1, np2d?, *WDEBUG_name,, ON
rem For remote debugging, don't put a full path name
    CSwitch -1, l????, *WDEBUG_name, $., ON
    CSwitch -1, ?????, *WDEBUG_name, $@, ON
:segment !C_FOR_PB
    VSwitch 1, ?????, WDEBUG_parms,,, ONE, REQ, ""
    SwitchText WDEBUG_parms, "Application parameters:"
:endsegment
    CSwitch -1, dr2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, da2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, db2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, dm2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, dw2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=cw -once\n", ON
    CSwitch -1, dp2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON
    CSwitch -1, dt2ew, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON
    CSwitch -1, dr2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, da2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, db2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, dm2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, dw2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=cw -once\n", ON
    CSwitch -1, dp2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON
    CSwitch -1, dt2ej, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON
    CSwitch -1, dr2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, da2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, db2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, dm2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=rsi -once\n", ON
    CSwitch -1, dw2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=cw -once\n", ON
    CSwitch -1, dp2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON
    CSwitch -1, dt2e8, "", "\n$(%watcom)\\binw\\wsrv.pif -tr=pls -once\n", ON

Tool RDEBUG "Remote Debugging"
  Family "Debugging Session"
    RGroup "Debugger"
      RSwitch 0, ????o, RDEBUG_pmwin, $(%watcom)\binp\wdw, ON
      RSwitch 0, ????s, RDEBUG_wind, $(%watcom)\binw\wdw, ON
      RSwitch 0, ????n, RDEBUG_wind, $(%watcom)\binnt\wdw, ON
      RSwitch 0, ????9, RDEBUG_wind, $(%watcom)\binnt\wdw, ON
      RSwitch 0, ????w, RDEBUG_wind, $(%watcom)\binw\wdw, ON
      RSwitch 0, ????j, RDEBUG_wind, $(%watcom)\binw\wdw, ON
      RSwitch 0, ????8, RDEBUG_wind, $(%watcom)\binw\wdw, ON
      RSwitch 0, ????a, RDEBUG_wind, $(%watcom)\axpnt\wdw, ON
      RSwitch 0, ????o, RDEBUG_char, "!Fullscreen $(%watcom)\\binp\\wd", OFF
      RSwitch 0, ????s, RDEBUG_char, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
      RSwitch 0, ????n, RDEBUG_char, $(%watcom)\binnt\wd, OFF
      RSwitch 0, ????9, RDEBUG_char, $(%watcom)\binnt\wd, OFF
      RSwitch 0, ????w, RDEBUG_char, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
      RSwitch 0, ????j, RDEBUG_char, "!Fullscreen $(%watcom)\\binw\\wdc.exe", OFF
      RSwitch 0, ????8, RDEBUG_char, "!Fullscreen $(%watcom)\\n98bw\\wdc.exe", OFF
      RSwitch 0, ????a, RDEBUG_char, "!Fullscreen $(%watcom)\\axpnt\\wd.exe", OFF
      SwitchText RDEBUG_wind, "Windowed"
      SwitchText RDEBUG_char, "Character mode"
      SwitchText RDEBUG_pmwin, "PM"
    VSwitch 1, ?????, RDEBUG_tr, -tr, =, ONE, REQ, ""
    SwitchText RDEBUG_tr, "Local trap file:"
    RGroup "Target location"
      RSwitch 1, ?????, RDEBUG_rem, -rem, OFF
      RSwitch 2, ?????, RDEBUG_do, -do, ON
      SwitchText RDEBUG_rem, "Exists on target"
      SwitchText RDEBUG_do, "Download to target"
    VSwitch 1, ?????, RDEBUG_lo, -lo, =, MULTI, REQ, ""
    SwitchText RDEBUG_lo, "Local info:"
  Family "Debugger Options"
    VSwitch 0, wp6d?, RDEBUG_i, -i, =, ONE, REQ, "pbwd.dbg"
    VSwitch 0, np2d?, RDEBUG_i, -i, =, ONE, REQ, "pbwdnt.dbg"
    VSwitch 0, ?????, RDEBUG_i, -i, =, ONE, REQ, ""
    VSwitch -1, wp6d?, "", -initcmd, =, ONE, REQ, "{pbstart.dbg $&}"
    CSwitch 0, ?????, RDEBUG_noi, -noi, OFF
    CSwitch 0, ?????, RDEBUG_symf, :$*.sym, OFF
    CSwitch 0, ?????, RDEBUG_nosy, -nosy, OFF
    CSwitch 0, ?????, RDEBUG_nof, -nof, OFF
    CSwitch 0, ?????, RDEBUG_noch, -noch, OFF
    VSwitch 0, ?????, RDEBUG_di, -di, =, MULTI, REQ, ""
    VSwitch 0, ?????, RDEBUG_dy, -dy, =, ONE, REQ, ""
    CSwitch -1, ?????, *RDEBUG_name, $., ON
    VSwitch 1, ?????, RDEBUG_li, -li, =, ONE, REQ, ""
    VSwitch 1, ?????, RDEBUG_co, -co, =, ONE, REQ, ""
    SwitchText RDEBUG_i, "Configuration file:"
    SwitchText RDEBUG_noi, "Don't invoke configuration"
    SwitchText RDEBUG_symf, "Use symbol file"
    SwitchText RDEBUG_nosy, "Don't process symbolic info"
    SwitchText RDEBUG_nof, "Ignore any floating point hardware"
    SwitchText RDEBUG_noch, "Don't do character remapping", "Don't do char remapping"
    SwitchText RDEBUG_di, "Dip files:"
    SwitchText RDEBUG_dy, "Dynamic memory:"
    SwitchText RDEBUG_li, "Lines of character mode screen:", "Lines of char mode screen:"
    SwitchText RDEBUG_co, "Columns of character mode screen:", "Columns of char mode screen:"
    RGroup "Character mode screen"
      RSwitch 1, ?????, RDEBUG_def_adapter, , ON
      RSwitch 1, ?????, RDEBUG_m, -m, OFF
      RSwitch 1, ?????, RDEBUG_c, -c, OFF
      RSwitch 1, ?????, RDEBUG_e, -e, OFF
      RSwitch 1, ?????, RDEBUG_v, -v, OFF
      SwitchText RDEBUG_def_adapter, "Default adapter"
      SwitchText RDEBUG_m, "Monochrome adapter"
      SwitchText RDEBUG_c, "Color adapter"
      SwitchText RDEBUG_e, "43-line EGA adapter"
      SwitchText RDEBUG_v, "50-line VGA adapter"
    VSwitch 1, ?????, RDEBUG_parms,,, ONE, REQ, ""
    SwitchText RDEBUG_parms, "Application parameters:"
::
:segment !C_FOR_PB
::

Tool WSAMPLE "Sampler"
  Family Sampler
    CSwitch -1, ????s, "", "!Error Sampler cannot run under Win-OS/2$", ON
    CSwitch -1, w???o, "", "!Error Windows executable cannot be sampled under OS/2$", ON
    CSwitch -1, x???d, "", "!Error Linux executable cannot be sampled under DOS$", ON
    CSwitch -1, x???o, "", "!Error Linux executable cannot be sampled under OS/2$", ON
    CSwitch -1, x???l, "", "!Error Linux executable cannot be sampled under Netware$", ON
    CSwitch -1, x???n, "", "!Error Linux executable cannot be sampled under Win32$", ON
    CSwitch -1, x???q, "", "!Error Linux executable cannot be sampled under QNX$", ON
    CSwitch -1, x???w, "", "!Error Linux executable cannot be sampled under Win16$", ON
    CSwitch -1, w???w, *WSAMPLE_run, "!Fullscreen $(%watcom)\\binw\\wsamplew", ON
    CSwitch -1, w???j, *WSAMPLE_run, "!Fullscreen $(%watcom)\\binw\\wsamplew", ON
    CSwitch -1, w???8, *WSAMPLE_run, "!Fullscreen $(%watcom)\\binw\\wsamplew", ON
    CSwitch -1, w????, *WSAMPLE_run, "!Fullscreen wsamplew", ON
    CSwitch -1, d?6??, "", "$(%watcom)\\binw\\wsample", ON
    CSwitch -1, dr2e?, "", "wsamprsi", ON
    CSwitch -1, dw2e?, "", "wsamprsi", ON
    CSwitch -1, da2e?, "", "wsamprsi", ON
    CSwitch -1, db2e?, "", "wsamprsi", ON
    CSwitch -1, dm2e?, "", "wsamprsi", ON
    CSwitch -1, dp2e?, "", "run386", ON
    CSwitch -1, dt2e?, "", "tnt", ON
    CSwitch -1, dp2e?, "", "wsamppls", ON
    CSwitch -1, dt2e?, "", "wsamppls", ON
    CSwitch -1, n????, "", "wsample", ON
    CSwitch -1, o????, "", "wsample", ON
    CSwitch -1, x????, "", "wsample", ON
    VSwitch 0, n????, WSAMPLE_b, -b, =, ONE, REQ, ""
    VSwitch 0, o????, WSAMPLE_b, -b, =, ONE, REQ, ""
    VSwitch 0, d????, WSAMPLE_b, -b, =, ONE, REQ, ""
    VSwitch 0, x????, WSAMPLE_b, -b, =, ONE, REQ, ""
    VSwitch 0, ?????, WSAMPLE_f, -f, =, ONE, REQ, "$*.smp"
    VSwitch 0, ?????, WSAMPLE_r, -r, =, ONE, REQ, ""
    VSwitch 0, d?6??, WSAMPLE_i, -i, =, ONE, REQ, ""
    CSwitch 0, d?6??, WSAMPLE_d, -d, OFF
    CSwitch -1, ?????, "", $@, ON
    VSwitch 0, ?????, WSAMPLE_parms,,, ONE, REQ, ""
    SwitchText WSAMPLE_b, "Sample buffer size(1-63):"
    SwitchText WSAMPLE_f, "Sample file name:"
    SwitchText WSAMPLE_r, "Sample rate:"
    SwitchText WSAMPLE_i, "Sampler interrupt number(20x-FFx):"
    SwitchText WSAMPLE_d, "disable DOS interrupts"
    SwitchText WSAMPLE_parms, "Application parameters:"

Tool WPROFILE "Profiler"
  Family Profiler
    CSwitch -1, w???w, *WPROFILE_run, "$(%watcom)\\binw\\wprofw", ON
    CSwitch -1, w???j, *WPROFILE_run, "$(%watcom)\\binw\\wprofw", ON
    CSwitch -1, w???8, *WPROFILE_run, "$(%watcom)\\binw\\wprofw", ON
    CSwitch -1, w???s, *WPROFILE_run, "$(%watcom)\\binw\\wprofw", ON
    CSwitch -1, ?????, *WPROFILE_run, wprof, ON
    CSwitch -1, ?????, "", $*.smp, ON
::
:endsegment
::

Tool WDIS "Disassembler"
  Family Disassembler
    CSwitch -1, ?????, "", "wdis $@", ON
    VSwitch 0, ?????, WDIS_l, -l, =, ONE, ON, "$*.lst"
    VSwitch 0, ?????, WDIS_i, -i, =, ONE, REQ, ""
    CSwitch 0, ?????, WDIS_a, -a, OFF
    CSwitch 0, ?????, WDIS_e, -e, OFF
    CSwitch 0, ?????, WDIS_p, -p, OFF
    CSwitch 0, ?????, WDIS_o, -o, OFF
    CSwitch 1, ?????, WDIS_m, -m, OFF
    VSwitch 1, ?????, WDIS_s, -s, =, ONE, ON, ""
    CSwitch 1, ?????, WDIS_fp, -fp, OFF
    CSwitch 1, ??a??, WDIS_fr, -fr, OFF
    CSwitch 1, ??2??, WDIS_fi, -fi, OFF
    CSwitch 1, ??6??, WDIS_fi, -fi, OFF
    CSwitch 1, ?????, WDIS_fu, -fu, OFF
    SwitchText WDIS_l, "Generate listing file:"
    SwitchText WDIS_i, "Initial character of internal labels:"
    SwitchText WDIS_a, "Generate assembleable output"
    SwitchText WDIS_e, "Generate list of externs"
    SwitchText WDIS_p, "Generate list of publics"
    SwitchText WDIS_o, "Print list of operands beside instructions"
    SwitchText WDIS_m, "Leave C++ names mangled"
    SwitchText WDIS_s, "Include source lines:"
    SwitchText WDIS_fp, "Do not use instruction name pseudonyms"
    SwitchText WDIS_fr, "Do not use register name pseudonyms"
    SwitchText WDIS_fi, "Use alternate indexing format"
    SwitchText WDIS_fu, "Instructions/registers in upper case"

Tool WCG "Code Generator"
  Family Optimization
    RGroup "Style of optimization"
      RSwitch 0, ?????, WCG_od, -od, OFF, ON
      RSwitch 0, ?????, WCG_ox, -ox , OFF
      RSwitch 0, ?????, WCG_os, -os, OFF
      RSwitch 0, ?????, WCG_ot, -ot, OFF
      RSwitch 0, ?????, WCG_otexan, -otexan, ON, OFF
      SwitchText WCG_od, "No optimizations"
      SwitchText WCG_ox, "Average space and time"
      SwitchText WCG_os, "Space optimizations"
      SwitchText WCG_ot, "Time optimizations"
      SwitchText WCG_otexan, "Fastest possible code"
:segment !C_FOR_PB
    RGroup "Stack frames"
      RSwitch 0, ?????, WCG_no_stack_frame, , ON
      RSwitch 0, ?????, WCG_of, -of, OFF
      RSwitch 0, ?????, WCG_of+, -of+, OFF
      SwitchText WCG_no_stack_frame, "Do not generate stack frames"
      SwitchText WCG_of, "Generate as needed"
      SwitchText WCG_of+, "Always generate"
:endsegment
    CSwitch 0, ?????, WCG_s, -s, OFF
    CSwitch 1, ?????, WCG_ob, -ob, OFF
    CSwitch 1, ?????, WCG_ol, -ol, OFF
    CSwitch 1, ?????, WCG_ol+, -ol+, OFF
    CSwitch 1, ?????, WCG_oc, -oc, OFF
    CSwitch 1, ?????, WCG_oi, -oi, OFF
    CSwitch 1, ?????, WCG_oa, -oa, OFF
    CSwitch 1, ?????, WCG_or, -or, OFF
    CSwitch 1, ?????, WCG_oh, -oh, OFF
    CSwitch 1, ?????, WCG_om, -om, OFF
    CSwitch 1, ?????, WCG_on, -on, OFF
    CSwitch 1, ?????, WCG_op, -op, OFF
    VSwitch 1, ?????, WCG_oe, -oe,, ONE, OFF, "20"
    SwitchText WCG_s, "Disable stack depth checking"
    SwitchText WCG_ob, "Branch prediction"
    SwitchText WCG_ol, "Loop optimizations"
    SwitchText WCG_ol+, "Loop unrolling"
    SwitchText WCG_oc, "Call/return optimizations"
    SwitchText WCG_oi, "In-line intrinsic functions"
    SwitchText WCG_oa, "Relax alias checking"
    SwitchText WCG_or, "Instruction scheduling"
    SwitchText WCG_oh, "Allow repeated optimizations"
    SwitchText WCG_om, "Math optimizations"
    SwitchText WCG_on, "Numerically unstable optimizations"
    SwitchText WCG_op, "Consistent FP results"
    SwitchText WCG_oe, "Expand function in-line:"
  Family Debugging
    RGroup "Debugging style"
      RSwitch 0, ?????, WCG_d0, , OFF
      RSwitch 0, ?????, WCG_d1, -d1, ON, OFF
      RSwitch 0, ?????, WCG_d2, -d2, OFF, ON
      RSwitch 0, ?????, WCG_d3, -d3, OFF
      SwitchText WCG_d0, "No debugging information"
      SwitchText WCG_d1, "Line number information"
      SwitchText WCG_d2, "Full debugging info"
      SwitchText WCG_d3, "Full info plus unused types"
    RGroup "Debugging format"
      RSwitch 0, ?????, WCG_def1, , ON
      RSwitch 0, ?????, WCG_hw, -hw, OFF
      SwitchText WCG_def1, "Compiler default"
      SwitchText WCG_hw, "Watcom debugging format"
:segment !C_FOR_PB
      RSwitch 0, ?????, WCG_hd, -hd, OFF
      RSwitch 0, ?????, WCG_hc, -hc, OFF
      SwitchText WCG_hd, "Dwarf debugging format"
      SwitchText WCG_hc, "CodeView debugging format"
    CSwitch 1, ?????, WCG_db, -db, OFF
    CSwitch 1, ?????, WCG_en, -en, OFF
    VSwitch 1, ?????, WCG_ep, -ep,, ONE, OFF, ""
    CSwitch 1, ?????, WCG_ee, -ee, OFF
    CSwitch 1, ?????, WCG_et, -et, OFF
    SwitchText WCG_db, "Emit Browser information"
    SwitchText WCG_en, "Emit routine names in code"
    SwitchText WCG_ep, "Call prolog routine:"
    SwitchText WCG_ee, "Call epilog hook routine"
    SwitchText WCG_et, "Pentium profiling code"
:endsegment
  Family "Code Generation Strategy"
    CSwitch -1, n?2d?, "", -bd, ON
    CSwitch -1, n?ad?, "", -bd, ON
    CSwitch -1, o??d?, "", -bd, ON
    CSwitch -1, w??d?, "", -bd, ON
    CSwitch -1, x??d?, "", -bd, ON
    CSwitch -1, d?2d?, "", -bd, ON
    CSwitch -1, r?2d?, "", -bd, ON
:segment !C_FOR_PB
    CSwitch 0, nm???, WCG_bm, -bm, ON
    CSwitch 0, na???, WCG_bm, -bm, ON
    CSwitch 0, no???, WCG_bm, -bm, ON
    CSwitch 0, nu???, WCG_bm, -bm, ON
    CSwitch 0, o????, WCG_bm, -bm, OFF
    CSwitch 0, n????, WCG_bm, -bm, OFF
    CSwitch 0, r????, WCG_bm, -bm, ON
    CSwitch 0, w????, WCG_bw, -bw, OFF
    SwitchText WCG_bm, "Multithreaded application"
    SwitchText WCG_bw, "Default-window application"
:endsegment
    RGroup "Floating-point level"
      RSwitch 0, ?????, WCG_def2, , ON
      RSwitch 0, ?????, WCG_fp2, -fp2, OFF
      RSwitch 0, ?????, WCG_fp3, -fp3, OFF
      RSwitch 0, ?????, WCG_fp5, -fp5, OFF
      RSwitch 0, ?????, WCG_fp6, -fp6, OFF
      SwitchText WCG_def2, "Compiler default"
      SwitchText WCG_fp2, "Inline 80287 instructions"
      SwitchText WCG_fp3, "Inline 80387 instructions"
      SwitchText WCG_fp5, "Inline Pentium instructions"
      SwitchText WCG_fp6, "Inline Pentium Pro instructions"
    RGroup "Floating-point model"
      RSwitch 1, r????, WCG_def3, , OFF
      RSwitch 1, ?????, WCG_def3, , ON
      RSwitch 1, ?????, WCG_fpi, -fpi, OFF
      RSwitch 1, r????, WCG_fpi87, -fpi87, ON
      RSwitch 1, ?????, WCG_fpi87, -fpi87, OFF
      RSwitch 1, ?????, WCG_fpc, -fpc, OFF
      SwitchText WCG_def3, "Compiler default"
      SwitchText WCG_fpi, "Inline with emulator"
      SwitchText WCG_fpi87, "Inline with coprocessor"
      SwitchText WCG_fpc, "Floating-point calls"
    RGroup "Floating-point rounding"
      RSwitch, 1, ?????, WCG_fpr_def, , ON
      RSwitch, 1, ?????, WCG_zro, -zro, OFF
      RSwitch, 1, ??2??, WCG_zri, -zri, OFF
      SwitchText WCG_fpr_def, "Default rounding"
      SwitchText WCG_zro, "Omit rounding"
      SwitchText WCG_zri, "Inline rounding"
:segment !C_FOR_PB
  Family "Register Usage"
    RGroup "DS register"
      RSwitch 0, rk2e?, WCG_def4, , OFF
      RSwitch 0, ?????, WCG_def4, , ON
      RSwitch 0, rk2e?, WCG_zdp, -zdp, ON
      RSwitch 0, ?????, WCG_zdp, -zdp, OFF
      RSwitch 0, ?????, WCG_zdf, -zdf, OFF
      SwitchText WCG_def4, "Compiler default"
      SwitchText WCG_zdp, "DS pegged to DGROUP"
      SwitchText WCG_zdf, "DS can float"
    RGroup "FS register"
      RSwitch 0, rk2e?, WCG_def5, , OFF
      RSwitch 0, ?????, WCG_def5, , ON
      RSwitch 0, ?????, WCG_zfp, -zfp, OFF
      RSwitch 0, rk2e?, WCG_zff, -zff, ON
      RSwitch 0, ?????, WCG_zff, -zff, OFF
      SwitchText WCG_def5, "Compiler default"
      SwitchText WCG_zfp, "FS cannot be used"
      SwitchText WCG_zff, "FS can be used"
    RGroup "GS register"
      RSwitch 0, ?????, WCG_def6, , ON
      RSwitch 0, ?????, WCG_zgp, -zgp, OFF
      RSwitch 0, ?????, WCG_zgf, -zgf, OFF
      SwitchText WCG_def6, "Compiler default"
      SwitchText WCG_zgp, "GS cannot be used"
      SwitchText WCG_zgf, "GS can be used"
:endsegment
  Family "Code Generation Option"
:segment !C_FOR_PB
    VSwitch 0, ?????, WCG_nc, -nc,, ONE, REQ, ""
    VSwitch 0, ?????, WCG_g, -g,, ONE, REQ, ""
    VSwitch 0, ?????, WCG_nd, -nd,, ONE, REQ, ""
    VSwitch 0, ?????, WCG_nt, -nt,, ONE, REQ, ""
    VSwitch 0, ?????, WCG_nm, -nm,, ONE, REQ, ""
    CSwitch 1, ?????, WCG_zl, -zl, OFF
    CSwitch 1, ?????, WCG_zld, -zld, OFF
    CSwitch 1, ??2??, WCG_ez, -ez, OFF
    CSwitch 1, ?????, WCG_r, -r, OFF
    SwitchText WCG_nc, "Name of code class:"
    SwitchText WCG_g, "Name of code group:"
    SwitchText WCG_nd, "Name of data segment:"
    SwitchText WCG_nt, "Name of text segment:"
    SwitchText WCG_nm, "Name of module:"
    SwitchText WCG_zl, "Don't generate default library information", "Don't gen default library info"
    SwitchText WCG_zld, "No file dependency information"
    SwitchText WCG_ez, "Generate Easy OMF object files"
    SwitchText WCG_r, "Save/restore segment registers"
:endsegment
:segment !C_FOR_PB | FOR_WIN
    CSwitch -1, ??6d?, "", -zu, ON
    CSwitch 1, rk2e?, WCG_zu, -zu, ON
    CSwitch 1, ???e?, WCG_zu, -zu, OFF
    CSwitch 1, ???s?, WCG_zu, -zu, OFF
    SwitchText WCG_zu, "SS not assumed equal to DS"
:endsegment
:segment !C_FOR_PB
    CSwitch 1, ?????, WCG_zm, -zm, OFF
    CSwitch 1, ?????, WCG_zc, -zc, OFF
    CSwitch 1, ??2??, WCG_zdl, -zdl, OFF
    SwitchText WCG_zm, "Put functions in separate segments", "Put functions in separate segs"
    SwitchText WCG_zc, "Constants in code segment"
    SwitchText WCG_zdl, "Load DS from DGROUP"
:endsegment
  Family "Memory Model and Processor"
    RGroup "Target processor"
      RSwitch 0, wm???, WCG_0, ,  OFF
      RSwitch 0, wa???, WCG_0, ,  OFF
      RSwitch 0, ??6??, WCG_0, ,  ON
      RSwitch 0, ??6??, WCG_1, -1, OFF
      RSwitch 0, wm???, WCG_2, -2, ON
      RSwitch 0, wa???, WCG_2, -2, ON
      RSwitch 0, ??6??, WCG_2, -2, OFF
      RSwitch 0, ??6??, WCG_3, -3, OFF
      RSwitch 0, ??6??, WCG_4, -4, OFF
      RSwitch 0, ??6??, WCG_5, -5, OFF
      RSwitch 0, ??6??, WCG_6, -6, OFF
      RSwitch 0, r?2??, WCG_3r, -3r, ON
      RSwitch 0, ??2??, WCG_3r, -3r, OFF
      RSwitch 0, ??2??, WCG_3s, -3s, OFF
      RSwitch 0, ??2??, WCG_4r, -4r, OFF
      RSwitch 0, ??2??, WCG_4s, -4s, OFF
      RSwitch 0, ??2??, WCG_5r, -5r, OFF
      RSwitch 0, ??2??, WCG_5s, -5s, OFF
      RSwitch 0, ds2??, WCG_6r, -6r, OFF
      RSwitch 0, dy2??, WCG_6r, -6r, OFF
      RSwitch 0, l?2??, WCG_6r, -6r, OFF
      RSwitch 0, r?2??, WCG_6r, -6r, OFF
      RSwitch 0, ??2??, WCG_6r, -6r, ON
      RSwitch 0, ds2??, WCG_6s, -6s, ON
      RSwitch 0, dy2??, WCG_6s, -6s, ON
      RSwitch 0, l?2??, WCG_6s, -6s, ON
      RSwitch 0, ??2??, WCG_6s, -6s, OFF
      SwitchText WCG_0, "8086"
      SwitchText WCG_1, "80186"
      SwitchText WCG_2, "80286"
      SwitchText WCG_3, "80386"
      SwitchText WCG_4, "80486"
      SwitchText WCG_5, "Pentium"
      SwitchText WCG_6, "Pentium Pro"
      SwitchText WCG_3r, "80386 register-based calling"
      SwitchText WCG_3s, "80386 stack-based calling"
      SwitchText WCG_4r, "80486 register-based calling"
      SwitchText WCG_4s, "80486 stack-based calling"
      SwitchText WCG_5r, "Pentium register-based calling"
      SwitchText WCG_5s, "Pentium stack-based calling"
      SwitchText WCG_6r, "Pentium Pro register-based calling"
      SwitchText WCG_6s, "Pentium Pro stack-based calling"
    CSwitch -1, w????, "", -bt=windows, ON
    CSwitch -1, d????, "", -bt=dos, ON
    CSwitch -1, o????, "", -bt=os2, ON
    CSwitch -1, n????, "", -bt=nt, ON
    CSwitch -1, l????, "", -bt=netware, ON
    CSwitch -1, x????, "", -bt=linux, ON
    CSwitch -1, rp???, "", -bt=rdos, ON
    CSwitch -1, ru?d?, "", -bt=rdos, ON
    CSwitch -1, rk6??, "", -bt=rdos_dev16, ON
    CSwitch -1, rk2??, "", -bt=rdosdev, ON
    CSwitch -1, rb6??, "", -bt=rdos_bin16, ON
    CSwitch -1, rb2??, "", -bt=rdos_bin32, ON
    CSwitch -1, rm6??, "", -bt=rdos_mboot, ON
    CSwitch -1, ?????, "", -fo=.obj, ON
:segment C_FOR_PB
    CSwitch 1, n?2d?, WCG_mf32, -mf, ON
    SwitchText WCG_mf32, "32-bit flat model", "32bit Flat model"
:elsesegment
    RGroup "Memory model"
      RSwitch 1, ??A??, WCG_def7, , ON
      RSwitch 1, ?????, WCG_def7, , OFF
      RSwitch 1, dc6e?, WCG_ms16, -ms, ON
      RSwitch 1, rk6??, WCG_ms16, -ms, ON
      RSwitch 1, rb6??, WCG_ms16, -ms, ON
      RSwitch 1, rm6??, WCG_ms16, -ms, ON
      RSwitch 1, ??6??, WCG_ms16, -ms, OFF
      RSwitch 1, ??6??, WCG_mm16, -mm, OFF
      RSwitch 1, ??6??, WCG_mc16, -mc, OFF
      RSwitch 1, dc6e?, WCG_ml16, -ml, OFF
      RSwitch 1, rk6??, WCG_ml16, -ml, OFF
      RSwitch 1, rb6??, WCG_ml16, -ml, OFF
      RSwitch 1, rm6??, WCG_ml16, -ml, OFF
      RSwitch 1, ??6??, WCG_ml16, -ml, ON
      RSwitch 1, ??6??, WCG_mh16, -mh, OFF
      RSwitch 1, l?2??, WCG_mf32, -mf, OFF
      RSwitch 1, rk2??, WCG_mf32, -mf, OFF
      RSwitch 1, ??2??, WCG_mf32, -mf, ON
      RSwitch 1, l?2??, WCG_ms32, -ms, ON
      RSwitch 1, rk2??, WCG_mc32, -mc, ON
      RSwitch 1, ??2??, WCG_ms32, -ms, OFF
      SwitchText WCG_def7, "Compiler default"
      SwitchText WCG_ms16, "Small model"
      SwitchText WCG_mm16, "Medium model"
      SwitchText WCG_mc16, "Compact model"
      SwitchText WCG_ml16, "Large model"
      SwitchText WCG_mh16, "Huge model"
      SwitchText WCG_ms32, "32-bit small model", "32bit Small model"
      SwitchText WCG_mc32, "32-bit compact model"
      SwitchText WCG_mf32, "32-bit flat model", "32bit Flat model"
:endsegment
    CSwitch 1, wp6d?, "", -ml, ON

Tool WLANG "Languages"
  IncludeTool WCG
  Family "File Option"
    VSwitch 0, w????, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/win\""
    VSwitch 0, d????, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, o?6??, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/os21x\""
    VSwitch 0, o?2??, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/os2\""
    VSwitch 0, nm???, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, na???, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, no???, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, nu???, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt;$(%watcom)/mfc/include\""
    VSwitch 0, n????, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt\""
    VSwitch 0, l????, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/novh\""
    VSwitch 0, x????, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/lh\""
    VSwitch 0, r????, WLANG_i, -i, =, MULTI, REQ, "\"$(%watcom)/rh\""
    VSwitch 0, ?????, WLANG_fi, -fi, =, MULTI, REQ, ""
    CSwitch 0, wm???, WLANG_fh, -fhq, ON
    CSwitch 0, wa???, WLANG_fh, -fhq, ON
    CSwitch 0, wp???, WLANG_fh, -fhq, ON
    CSwitch 0, nm???, WLANG_fh, -fhq, ON
    CSwitch 0, na???, WLANG_fh, -fhq, ON
    CSwitch 0, nu???, WLANG_fh, -fhq, ON
    CSwitch 0, no???, WLANG_fh, -fhq, ON
    CSwitch 0, ?????, WLANG_fh, -fh, OFF
    SwitchText WLANG_i, "Include directories:"
    SwitchText WLANG_fi, "Include files:"
    SwitchText WLANG_fh, "Use precompiled headers"
  Family Diagnostics
    RGroup "Warning level"
      RSwitch 0, ?????, WLANG_w0, -w0, OFF
      RSwitch 0, ?????, WLANG_w1, -w1, OFF
      RSwitch 0, wa???, WLANG_w2, -w2, ON
      RSwitch 0, wm???, WLANG_w2, -w2, ON
      RSwitch 0, ?????, WLANG_w2, -w2, OFF
      RSwitch 0, nm???, WLANG_w3, -w3, ON
      RSwitch 0, na???, WLANG_w3, -w3, ON
      RSwitch 0, nu???, WLANG_w3, -w3, ON
      RSwitch 0, no???, WLANG_w3, -w3, ON
      RSwitch 0, ?????, WLANG_w3, -w3, OFF
      RSwitch 0, wm???, WLANG_w4, -w4, OFF
      RSwitch 0, wa???, WLANG_w4, -w4, OFF
      RSwitch 0, nm???, WLANG_w4, -w4, OFF
      RSwitch 0, na???, WLANG_w4, -w4, OFF
      RSwitch 0, no???, WLANG_w4, -w4, OFF
      RSwitch 0, nu???, WLANG_w4, -w4, OFF
      RSwitch 0, ?????, WLANG_w4, -w4, ON
      SwitchText WLANG_w0, "Warning level 0"
      SwitchText WLANG_w1, "Warning level 1"
      SwitchText WLANG_w2, "Warning level 2"
      SwitchText WLANG_w3, "Warning level 3"
      SwitchText WLANG_w4, "Warning level 4"
    CSwitch 0, ?????, WLANG_we, -we, OFF
    VSwitch 0, ?????, WLANG_e, -e,, ONE, REQ, "25"
    CSwitch 1, ?????, WLANG_za, -za, OFF
    CSwitch 1, o?2??, WLANG_sg, -sg, OFF
    CSwitch 1, o?2??, WLANG_st, -st, OFF
    VSwitch 1, ?????, WLANG_wcd, -wcd,, MULTI, REQ, ""
    VSwitch 1, ?????, WLANG_wce, -wce,, MULTI, REQ, ""
    SwitchText WLANG_we, "Treat warnings as errors"
    SwitchText WLANG_e, "Error count:"
    SwitchText WLANG_za, "Force ANSI compliance"
    SwitchText WLANG_sg, "Automatic stack growing"
    SwitchText WLANG_st, "Touch stack through SS first"
    SwitchText WLANG_wcd, "Disable warning messages"
    SwitchText WLANG_wce, "Enable warning messages"
    RGroup "C Language standard"
      RSwitch 0, ?????, WLANG_zastd89, , ON
      RSwitch 0, ?????, WLANG_zastd99, -zastd=c99, OFF
      SwitchText WLANG_zastd89, "C89 Language standard"
      SwitchText WLANG_zastd99, "C99 Language standard"
  Family Source
    CSwitch 0, ?????, WLANG_d+, -d+, OFF
    VSwitch 0, ?????, WLANG_u, -u,, MULTI, REQ, ""
    VSwitch 0, nm?e?, WLANG_d, -d,, MULTI, REQ, "_X86_ _MBCS", "_X86_ _DEBUG _MBCS"
    VSwitch 0, nm?d?, WLANG_d, -d,, MULTI, REQ, "_X86_ _USRDLL _WINDLL _MBCS", "_X86_ _USRDLL _WINDLL _DEBUG _MBCS"
    VSwitch 0, na?e?, WLANG_d, -d,, MULTI, REQ, "_AFXDLL _X86_ _MBCS", "_AFXDLL _X86_ _DEBUG _MBCS"
    VSwitch 0, na?d?, WLANG_d, -d,, MULTI, REQ, "_AFXDLL _WINDLL _AFXEXT _X86_ _MBCS", "_AFXDLL _WINDLL _AFXEXT _X86_ _DEBUG _MBCS"
    VSwitch 0, no?e?, WLANG_d, -d,, MULTI, REQ, "_UNICODE _AFXDLL _X86_", "_UNICODE _AFXDLL _X86_ _DEBUG"
    VSwitch 0, no?d?, WLANG_d, -d,, MULTI, REQ, "_UNICODE _AFXDLL _WINDLL _AFXEXT _X86_", "_UNICODE _AFXDLL _WINDLL _AFXEXT _X86_ _DEBUG"
    VSwitch 0, nu?e?, WLANG_d, -d,, MULTI, REQ, "_UNICODE _X86_", "_UNICODE _X86_ _DEBUG"
    VSwitch 0, nu?d?, WLANG_d, -d,, MULTI, REQ, "_UNICODE _USRDLL _WINDLL _X86_", "_UNICODE _USRDLL _WINDLL _X86_ _DEBUG"
    VSwitch 0, wa?e?, WLANG_d, -d,, MULTI, REQ, "_AFXDLL", "_AFXDLL _DEBUG"
    VSwitch 0, wa?d?, WLANG_d, -d,, MULTI, REQ, "_AFXDLL _WINDLL", "_AFXDLL _WINDLL _DEBUG"
    VSwitch 0, wm6e?, WLANG_d, -d,, MULTI, REQ, "", "_DEBUG"
    VSwitch 0, wm6d?, WLANG_d, -d,, MULTI, REQ, "_USRDLL _WINDLL", "_WINDLL _USRDLL _DEBUG"
    VSwitch 0, ?????, WLANG_d, -d,, MULTI, REQ, ""
    CSwitch 0, nm???, WLANG_j, -j, ON
    CSwitch 0, na???, WLANG_j, -j, ON
    CSwitch 0, no???, WLANG_j, -j, ON
    CSwitch 0, nu???, WLANG_j, -j, ON
    CSwitch 0, ?????, WLANG_j, -j, OFF
    CSwitch 0, nm???, WLANG_ei, -ei, ON
    CSwitch 0, na???, WLANG_ei, -ei, ON
    CSwitch 0, no???, WLANG_ei, -ei, ON
    CSwitch 0, nu???, WLANG_ei, -ei, ON
    CSwitch 0, ?????, WLANG_ei, -ei, OFF
    SwitchText WLANG_d+, "Extended macro definitions"
    SwitchText WLANG_u, "Undefine macros:"
    SwitchText WLANG_d, "Macro definitions:"
    SwitchText WLANG_j, "Change char default to signed"
    SwitchText WLANG_ei, "Force enums to be type int"
    RGroup "Structure alignment"
      RSwitch 1, nm???, WLANG_zp_def, , OFF
      RSwitch 1, na???, WLANG_zp_def, , OFF
      RSwitch 1, no???, WLANG_zp_def, , OFF
      RSwitch 1, nu???, WLANG_zp_def, , OFF
      RSwitch 1, r????, WLANG_zp_def, , OFF
      RSwitch 1, ?????, WLANG_zp_def, , ON
      RSwitch 1, r????, WLANG_zp1, -zp1, ON
      RSwitch 1, ?????, WLANG_zp1, -zp1, OFF
      RSwitch 1, ?????, WLANG_zp2, -zp2, OFF
      RSwitch 1, nm???, WLANG_zp4, -zp4, ON
      RSwitch 1, na???, WLANG_zp4, -zp4, ON
      RSwitch 1, no???, WLANG_zp4, -zp4, ON
      RSwitch 1, nu???, WLANG_zp4, -zp4, ON
      RSwitch 1, ?????, WLANG_zp4, -zp4, OFF
      RSwitch 1, ?????, WLANG_zp8, -zp8, OFF
      RSwitch 1, ?????, WLANG_zp16, -zp16, OFF
      SwitchText WLANG_zp_def, "Compiler default", "Default byte alignment"
      SwitchText WLANG_zp1, "1-byte alignment"
      SwitchText WLANG_zp2, "2-byte alignment"
      SwitchText WLANG_zp4, "4-byte alignment"
      SwitchText WLANG_zp8, "8-byte alignment"
      SwitchText WLANG_zp16, "16-byte alignment"
  Family Miscellaneous
    VSwitch 0, ??6??, WLANG_zt, -zt,, ONE, REQ, ""
    SwitchText WLANG_zt, "Data Threshold:"
    RGroup "Double byte characters"
      RSwitch 0, ?????, WLANG_zk_def, , ON
      RSwitch 0, ?????, WLANG_zk0, -zk0, OFF
      RSwitch 0, ?????, WLANG_zk0u, -zk0u, OFF
      RSwitch 0, ?????, WLANG_zk1, -zk1, OFF
      RSwitch 0, ?????, WLANG_zk3, -zk3, OFF
      RSwitch 0, ?????, WLANG_zk2, -zk2, OFF
      RSwitch 0, ?????, WLANG_zkl, -zkl, OFF
      SwitchText WLANG_zk_def, "No double-byte character support"
      SwitchText WLANG_zk0, "Kanji"
      SwitchText WLANG_zk0u, "Kanji (Unicode)"
      SwitchText WLANG_zk1, "Traditional Chinese", "Chinese/Taiwanese", "Chineese/Taiwanese"
      SwitchText WLANG_zk2, "Korean"
      SwitchText WLANG_zk3, "Simplified Chinese"
      SwitchText WLANG_zkl, "Current code page"
    VSwitch 1, ?????, WLANG_zku, -zku, =, ONE, REQ, ""
    CSwitch 1, ?????, WLANG_zq, -zq, ON
    VSwitch 1, ?????, WLANG_others,,, MULTI, REQ, ""
    SwitchText WLANG_zku, "Unicode translate table:"
    SwitchText WLANG_zq, "Quiet operation"
    SwitchText WLANG_others, "Other options:"

Tool CCOMP "C Compiler"
  Family "C Compiler"
    CSwitch 0, ?????, CCOMP_ri, -ri, OFF
    SwitchText CCOMP_ri, "Return char and short as int"
    RGroup "Create definition file (.def)"
      RSwitch 0, ?????, CCOMP_nodef,  , ON
      RSwitch 0, ?????, CCOMP_v, -v, OFF
      RSwitch 0, ?????, CCOMP_zg, -zg, OFF
      SwitchText CCOMP_nodef, "No definition file"
      SwitchText CCOMP_v, "With typedef names"
      SwitchText CCOMP_zg, "With base type names"

Tool CPPCOMP "C++ Compiler"
  Family "C++ Compiler"
    CSwitch 0, ?????, CPPCOMP_ew, -ew, OFF
    CSwitch 0, ?????, CPPCOMP_wx, -wx, OFF
    CSwitch 0, wm?e?, CPPCOMP_zws, -zws, ON
    CSwitch 0, wa?e?, CPPCOMP_zws, -zws, ON
    CSwitch 0, w?6??, CPPCOMP_zws, -zws, OFF
    SwitchText CPPCOMP_ew, "Shorter error messages"
    SwitchText CPPCOMP_wx, "Full warnings"
    SwitchText CPPCOMP_zws, "Smart Windows callbacks"
    RGroup "Exception handling"
      RSwitch 0, ?????, CPPCOMP_xd, -xd, OFF
      RSwitch 0, ?????, CPPCOMP_xs, -xs, ON
      RSwitch 0, ?????, CPPCOMP_xst, -xst, OFF
      RSwitch 0, ?????, CPPCOMP_xss, -xss, OFF
      SwitchText CPPCOMP_xd, "No exception handling"
      SwitchText CPPCOMP_xs, "Normal exception handling"
      SwitchText CPPCOMP_xst, "Direct calls for destruction"
      SwitchText CPPCOMP_xss, "Table driven destructors"
    CSwitch 0, wm6??, CPPCOMP_zmf, -zmf, ON
    CSwitch 0, wa6??, CPPCOMP_zmf, -zmf, ON
    CSwitch 0, ?????, CPPCOMP_zmf, -zmf, OFF
    CSwitch 1, ?????, CPPCOMP_xr, -xr, ON
    CSwitch 1, ?????, CPPCOMP_zv, -zv, OFF
    CSwitch 1, ?????, CPPCOMP_ft, -ft, OFF
    CSwitch 1, ?????, CPPCOMP_fx, -fx, OFF
    SwitchText CPPCOMP_zmf, "Functions in separate segments.", "Functions in separate segs."
    SwitchText CPPCOMP_xr, "Enable RTTI"
    SwitchText CPPCOMP_zv, "Virtual function removal"
    SwitchText CPPCOMP_ft, "Check for truncated filenames"
    SwitchText CPPCOMP_fx, "Do NOT check for truncated filenames"

Tool WCC "C Compiler"
  IncludeTool WLANG
  IncludeTool CCOMP
  Family Compiler
    CSwitch -1, ??6??, "", "wcc $@", ON
    CSwitch -1, ??2??, "", "wcc386 $@", ON
    CSwitch -1, ??a??, "", "wccaxp $@", ON

Tool WPP "C++ Compiler"
  IncludeTool WLANG
  IncludeTool CPPCOMP
  Family Compiler
    CSwitch -1, ??6??, "", "wpp $@", ON
    CSwitch -1, ??2??, "", "wpp386 $@", ON
    CSwitch -1, ??a??, "", "wppaxp $@", ON
::
:segment !C_FOR_PB
::

Tool WFOR "Fortran Compiler"
  Family Compiler
    CSwitch -1, ??6??, "", "wfc $@", ON
    CSwitch -1, ??2??, "", "wfc386 $@", ON
    CSwitch -1, ??a??, "", "wfcaxp $@", ON
  Family "File Option"
    RGroup "Listing file"
      RSwitch 0, ?????, WFOR_def1, , ON
      RSwitch 0, ?????, WFOR_nolis, -nolis , OFF
      RSwitch 0, ?????, WFOR_lis_pr, "-lis -pr" , OFF
      RSwitch 0, ?????, WFOR_lis_ty, "-lis -ty" , OFF
      RSwitch 0, ?????, WFOR_lis_di, "-lis -di" , OFF
      SwitchText WFOR_def1, "Compiler default"
      SwitchText WFOR_nolis, "No listing file"
      SwitchText WFOR_lis_pr, "Listing file to printer"
      SwitchText WFOR_lis_ty, "Listing file to screen"
      SwitchText WFOR_lis_di, "Listing file to disk"
    CSwitch 0, ?????, WFOR_incl, -incl, OFF
    C2Switch 0, ?????, WFOR_noer, "", -noer, ON
    VSwitch 1, w????, WFOR_incp, -incp, =, ONE, REQ, "\"$(%watcom)/src/fortran/win;$(%watcom)/src/fortran\""
    VSwitch 1, o????, WFOR_incp, -incp, =, ONE, REQ, "\"$(%watcom)/src/fortran/os2;$(%watcom)/src/fortran\""
    VSwitch 1, d????, WFOR_incp, -incp, =, ONE, REQ, "\"$(%watcom)/src/fortran/dos;$(%watcom)/src/fortran\""
    VSwitch 1, ?????, WFOR_incp, -incp, =, ONE, REQ, ""
    SwitchText WFOR_incl, "Include files in listing file"
    SwitchText WFOR_noer, "Generate error file"
    SwitchText WFOR_incp, "Path for include files:"
  Family "Compile-time Diagnostic"
    CSwitch 0, ?????, WFOR_exp, -exp, OFF
    CSwitch 0, ?????, WFOR_ex, -ex, OFF
    C2Switch 0, ?????, WFOR_nor, "", -nor, ON
    CSwitch 0, ?????, WFOR_wil, -wil, OFF
    C2Switch 0, ?????, WFOR_nowa, "", -nowa, ON
    C2Switch 0, ?????, WFOR_note, "", -note, ON
    SwitchText WFOR_exp, "Require symbol declaration"
    SwitchText WFOR_ex, "Warn when extensions used"
    SwitchText WFOR_nor, "Warn about unreferenced symbols", "Warn about unref'ed symbols"
    SwitchText WFOR_wil, "Ignore wild branches"
    SwitchText WFOR_nowa, "Enable warning messages"
    SwitchText WFOR_note, "Print diagnostics to the screen"
  Family "Run-time Diagnostic"
    CSwitch 0, ?????, WFOR_st, -st, OFF
    CSwitch 0, ?????, WFOR_for, -for, OFF
    CSwitch 0, ?????, WFOR_tr, -tr, OFF
    CSwitch 0, ?????, WFOR_bo, -bo, OFF
    CSwitch 0, ?????, WFOR_res, -res, OFF
    SwitchText WFOR_st, "Perform stack checking"
    SwitchText WFOR_for, "Perform format type checking"
    SwitchText WFOR_tr, "Generate runtime trace backs", "Generate run-time trace backs"
    SwitchText WFOR_bo, "Add runtime bounds checks", "Add run-time bounds checks"
    SwitchText WFOR_res, "Store messages as resources"
  Family "Debugging"
    RGroup "Debugging style"
      RSwitch 0, ?????, "No debugging information", , OFF
      RSwitch 0, ?????, WFOR_d1, -d1, ON, OFF
      RSwitch 0, ?????, WFOR_d2, -d2, OFF, ON
      SwitchText WFOR_d1, "Line number information"
      SwitchText WFOR_d2, "Full debugging information"
    RGroup "Debugging format"
      RSwitch 0, ?????, WFOR_def2, , ON
      RSwitch 0, ?????, WFOR_hw, -hw, OFF
      RSwitch 0, ?????, WFOR_hc, -hc, OFF
      RSwitch 0, ?????, WFOR_hd, -hd, OFF
      SwitchText WFOR_def2, "Compiler default"
      SwitchText WFOR_hw, "Watcom debugging format"
      SwitchText WFOR_hc, "CodeView debugging format"
      SwitchText WFOR_hd, "DWARF debugging format"
  Family "Code Generation Strategy"
    RGroup "Target CPU"
      RSwitch 0, ?????, WFOR_def3, , ON
      RSwitch 0, ?????, WFOR_3, -3 , OFF
      RSwitch 0, ?????, WFOR_4, -4 , OFF
      RSwitch 0, ?????, WFOR_5, -5 , OFF
      RSwitch 0, ?????, WFOR_6, -6 , OFF
      SwitchText WFOR_def3, "Compiler default"
      SwitchText WFOR_3, "Assume 80386"
      SwitchText WFOR_4, "Optimize for 80486"
      SwitchText WFOR_5, "Optimize for Pentium"
      SwitchText WFOR_6, "Optimize for Pentium Pro"
    RGroup "Memory model"
      RSwitch 0, ?????, WFOR_def4, , ON
      RSwitch 0, ??2??, WFOR_mf, -mf, OFF
      RSwitch 0, ??2??, WFOR_ms, -ms, OFF
      RSwitch 0, ??6??, WFOR_ml, -ml, OFF
      RSwitch 0, ??6??, WFOR_mm, -mm, OFF
      SwitchText WFOR_def4, "Compiler default"
      SwitchText WFOR_mf, "Flat model"
      SwitchText WFOR_ms, "Small model"
      SwitchText WFOR_ml, "Large model"
      SwitchText WFOR_mm, "Medium model"
    RGroup "Floating-point level"
      RSwitch 1, ?????, WFOR_def5, , ON
      RSwitch 1, ?????, WFOR_fp2, -fp2, OFF
      RSwitch 1, ?????, WFOR_fp3, -fp3, OFF
      RSwitch 1, ?????, WFOR_fp5, -fp5, OFF
      RSwitch 1, ?????, WFOR_fp6, -fp6, OFF
      SwitchText WFOR_def5, "Compiler default"
      SwitchText WFOR_fp2, "Inline 80287 instructions"
      SwitchText WFOR_fp3, "Inline 80387 instructions"
      SwitchText WFOR_fp5, "Inline Pentium instructions"
      SwitchText WFOR_fp6, "Inline Pentium Pro instructions"
    RGroup "Floating-point model"
      RSwitch 1, r????, WFOR_def6, , OFF
      RSwitch 1, ?????, WFOR_def6, , ON
      RSwitch 1, ?????, WFOR_fpc, -fpc, OFF
      RSwitch 1, ?????, WFOR_fpi, -fpi, OFF
      RSwitch 1, r????, WFOR_fpi87, -fpi87, ON
      RSwitch 1, ?????, WFOR_fpi87, -fpi87, OFF
      SwitchText WFOR_def6, "Compiler default"
      SwitchText WFOR_fpc, "Floating-point calls"
      SwitchText WFOR_fpi, "Emulate 80x87 instructions"
      SwitchText WFOR_fpi87, "Inline 80x87 instructions"
    CSwitch 1, ?????, WFOR_fpr, -fpr OFF
    SwitchText WFOR_fpr, "80x87 reverse compatibility"
  Family "Code Generation Option"
    VSwitch 0, ?????, WFOR_dt, -dt, =, ONE, REQ, ""
    CSwitch 0, ?????, WFOR_co, -co, OFF
    CSwitch 0, ?????, WFOR_sa, -sa, OFF
    CSwitch 0, ?????, WFOR_sh, -sh, OFF
    CSwitch 0, ?????, WFOR_al, -al, OFF
    CSwitch 0, ?????, WFOR_au, -au, OFF
    CSwitch 0, ?????, WFOR_des, -des, OFF
    SwitchText WFOR_dt, "Set data threshold:"
    SwitchText WFOR_co, "Put constants in code segment"
    SwitchText WFOR_sa, "SAVE local variables"
    SwitchText WFOR_sh, "Short INTEGER/LOGICAL size"
    SwitchText WFOR_al, "Align COMMON segments"
    SwitchText WFOR_au, "Local variables on the stack"
    SwitchText WFOR_des, "Pass character descriptors"
  Family "Register Usage"
    CSwitch 0, ?????, WFOR_sr, -sr, OFF
    C2Switch 0, ?????, WFOR_nogs, "", -nogs, ON
    C2Switch 0, ??6??, WFOR_nofs, "", -nofs, ON
    C2Switch 0, ?????, WFOR_fs, -fs, "", OFF
    C2Switch 0, o?6??, WFOR_ss, "", -ss, OFF
    C2Switch 0, ??6??, WFOR_ss, "", -ss, ON
    SwitchText WFOR_sr, "Save segment registers"
    SwitchText WFOR_nogs, "Compiler can use GS"
    SwitchText WFOR_nofs, "Compiler can use FS"
    SwitchText WFOR_fs, "Compiler can use FS"
    SwitchText WFOR_ss, "SS points to DGROUP"
  Family "Optimizations"
    RGroup "Optimization Style"
      RSwitch 0, ?????, WFOR_def7, , ON
      RSwitch 0, ?????, WFOR_od, -od , OFF
      RSwitch 0, ?????, WFOR_ot, -ot, OFF
      RSwitch 0, ?????, WFOR_os, -os , OFF
      RSwitch 0, ?????, WFOR_ox, -ox , OFF
      SwitchText WFOR_def7, "Compiler default"
      SwitchText WFOR_od, "Disable optimizations"
      SwitchText WFOR_ot, "Optimize for time"
      SwitchText WFOR_os, "Optimize for space"
      SwitchText WFOR_ox, "Fastest possible code"
    CSwitch 1, ?????, WFOR_ob, -ob, OFF
    CSwitch 1, ?????, WFOR_obp, -obp, OFF
    CSwitch 1, ?????, WFOR_oc, -oc, OFF
    CSwitch 1, ?????, WFOR_of, -of, OFF
    CSwitch 1, ?????, WFOR_oh, -oh, OFF
    CSwitch 1, ?????, WFOR_oi, -oi, OFF
    CSwitch 1, ?????, WFOR_ok, -ok, OFF
    CSwitch 1, ?????, WFOR_ol, -ol, OFF
    CSwitch 1, ?????, WFOR_ol+, -ol+, OFF
    CSwitch 1, ?????, WFOR_om, -om, OFF
    CSwitch 1, ?????, WFOR_on, -on, OFF
    CSwitch 1, ?????, WFOR_op, -op, OFF
    CSwitch 1, ?????, WFOR_or, -or, OFF
    CSwitch 1, ?????, WFOR_odo, -odo, OFF
    SwitchText WFOR_ob, "Base pointer optimizations"
    SwitchText WFOR_obp, "Branch prediction"
    SwitchText WFOR_oc, "No call-return optimizations"
    SwitchText WFOR_of, "No stack frame optimizations"
    SwitchText WFOR_oh, "Optimize at expense of compile-time"
    SwitchText WFOR_oi, "Statement functions inline"
    SwitchText WFOR_ok, "Move register saves into flow path"
    SwitchText WFOR_ol, "Loop optimizations"
    SwitchText WFOR_ol+, "Loop unrolling"
    SwitchText WFOR_om, "Math optimizations"
    SwitchText WFOR_on, "Numerical optimizations"
    SwitchText WFOR_op, "Precision optimizations"
    SwitchText WFOR_or, "Instruction scheduling"
    SwitchText WFOR_odo, "No loop overflow checking"
  Family "Application Type"
    CSwitch 0, w????, WFOR_bw, -bw, OFF
    CSwitch 0, r????, WFOR_bm, -bm, ON
    CSwitch 0, ?????, WFOR_bm, -bm, OFF
    CSwitch 0, ???d?, WFOR_bd, -bd, ON
    CSwitch 0, ?????, WFOR_bd, -bd, OFF
    CSwitch 0, w????, WFOR_win, -win, ON
    CSwitch 0, ?????, WFOR_win, -win, OFF
    SwitchText WFOR_bw, "Default windowed application"
    SwitchText WFOR_bm, "Multithreaded application"
    SwitchText WFOR_bd, "Dynamic link library"
    SwitchText WFOR_win, "Generate code for Windows"
  Family "Miscellaneous"
    VSwitch 0, ?????, WFOR_def, -def, =, ONE, REQ, ""
    VSwitch  0, ?????, WFOR_others,,, MULTI, REQ, ""
    CSwitch 0, ?????, WFOR_sy, -sy, OFF
    CSwitch 0, ?????, WFOR_xf, -xf, OFF
    CSwitch 0, ?????, WFOR_q,-q, ON
    CSwitch 0, ?????, WFOR_dep, -dep, ON
    CSwitch 0, ?????, WFOR_lf, -lf, OFF
    C2Switch 0, ?????, WFOR_nolib, "", -nolib, ON
    CSwitch 0, ?????, WFOR_cc, -cc, OFF
    SwitchText WFOR_def, "Define macro:"
    SwitchText WFOR_others, "Other options(,):"
    SwitchText WFOR_sy, "Syntax check only"
    SwitchText WFOR_xf, "Extend floating-point precision"
    SwitchText WFOR_q, "Operate quietly"
    SwitchText WFOR_dep, "Generate file dependencies"
    SwitchText WFOR_lf, "LF with FF"
    SwitchText WFOR_nolib, "Include default library info"
    SwitchText WFOR_cc, "Unit 6 is a CC device"
    RGroup "Character set"
      RSwitch 1, ?????, WFOR_def8, , ON
      RSwitch 1, ?????, WFOR_chi, -chi, OFF
      RSwitch 1, ?????, WFOR_j, -j, OFF
      RSwitch 1, ?????, WFOR_ko, -ko , OFF
      SwitchText WFOR_def8, "Compiler default"
      SwitchText WFOR_chi, "Chinese character set", "Chineese character set"
      SwitchText WFOR_j, "Japanese character set"
      SwitchText WFOR_ko, "Korean characer set"
    RGroup "Line length"
      RSwitch 1, ?????, WFOR_def9, , ON
      RSwitch 1, ?????, WFOR_noxl, -noxl , OFF
      RSwitch 1, ?????, WFOR_xl, -xl , OFF
      SwitchText WFOR_def9, "Compiler default"
      SwitchText WFOR_noxl, "Standard length"
      SwitchText WFOR_xl, "Extended length"
::
:endsegment
::

Tool WASM "Assembler"
  Family "File Option"
    CSwitch -1, ?????, "", "wasm $@", ON
    VSwitch 0, ?????, WASM_fi, -fi, =, MULTI, REQ, ""
    VSwitch 0, w????, WASM_i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/win\""
    VSwitch 0, d????, WASM_i, -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, o?6??, WASM_i, -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, o?2??, WASM_i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%toolkit)/c/os2h\""
    VSwitch 0, n????, WASM_i, -i, =, MULTI, REQ, "\"$(%watcom)/h;$(%watcom)/h/nt\""
    VSwitch 0, l????, WASM_i, -i, =, MULTI, REQ, "\"$(%watcom)/h\""
    VSwitch 0, x????, WASM_i, -i, =, MULTI, REQ, "\"$(%watcom)/lh\""
    VSwitch 0, ?????, WASM_fe, -fe, =, ONE, REQ, ""
    SwitchText WASM_fi, "Include file:"
    SwitchText WASM_i, "Include directories:"
    SwitchText WASM_fe, "Name error file:"
  Family "Memory Model and Processor"
    RGroup "Memory model"
      RSwitch 0, ??6??, WASM_mt16, -mt, OFF
      RSwitch 0, rk6??, WASM_ms16, -ms, ON
      RSwitch 0, rb6??, WASM_ms16, -ms, ON
      RSwitch 0, rm6??, WASM_ms16, -ms, ON
      RSwitch 0, ??6??, WASM_ms16, -ms, OFF
      RSwitch 0, ??6??, WASM_mm16, -mm, OFF
      RSwitch 0, ??6??, WASM_mc16, -mc, OFF
      RSwitch 0, rk6??, WASM_ml16, -ml, OFF
      RSwitch 0, rb6??, WASM_ml16, -ml, OFF
      RSwitch 0, rm6??, WASM_ml16, -ml, OFF
      RSwitch 0, ??6??, WASM_ml16, -ml, ON
      RSwitch 0, ??6??, WASM_mh16, -mh, OFF
      RSwitch 0, ??2??, WASM_mf32, -mf, ON
      RSwitch 0, ??2??, WASM_ms32, -ms, OFF
      SwitchText WASM_mt16, "Tiny model"
      SwitchText WASM_ms16, "Small model"
      SwitchText WASM_mm16, "Medium model"
      SwitchText WASM_mc16, "Compact model"
      SwitchText WASM_ml16, "Large model"
      SwitchText WASM_mh16, "Huge model"
      SwitchText WASM_ms32, "32-bit small model"
      SwitchText WASM_mf32, "32-bit flat model"
    RGroup "Target processor"
      RSwitch 1, rk6??, WASM_0, ,  OFF
      RSwitch 1, rb6??, WASM_0, ,  OFF
      RSwitch 1, rm6??, WASM_0, ,  OFF
      RSwitch 1, ??6??, WASM_0, ,  ON
      RSwitch 1, ??6??, WASM_1, -1, OFF
      RSwitch 1, rk6??, WASM_2, -2, ON
      RSwitch 1, ??6??, WASM_2, -2, OFF
      RSwitch 1, rb6??, WASM_3, -3, ON
      RSwitch 1, rm6??, WASM_3, -3, ON
      RSwitch 1, ??6??, WASM_3, -3, OFF
      RSwitch 1, ??6??, WASM_4, -4, OFF
      RSwitch 1, ??6??, WASM_5, -5, OFF
      RSwitch 1, ??6??, WASM_6, -6, OFF
      RSwitch 1, ??2??, WASM_3r, -3r, OFF
      RSwitch 1, ??2??, WASM_3s, -3s, OFF
      RSwitch 1, ??2??, WASM_4r, -4r, OFF
      RSwitch 1, ??2??, WASM_4s, -4s, OFF
      RSwitch 1, ??2??, WASM_5r, -5r, OFF
      RSwitch 1, ??2??, WASM_5s, -5s, OFF
      RSwitch 1, ds2??, WASM_6r, -6r, OFF
      RSwitch 1, dy2??, WASM_6r, -6r, OFF
      RSwitch 1, ??2??, WASM_6r, -6r, ON
      RSwitch 1, ds2??, WASM_6s, -6s, ON
      RSwitch 1, dy2??, WASM_6s, -6s, ON
      RSwitch 1, ??2??, WASM_6s, -6s, OFF
      SwitchText WASM_0, "8086"
      SwitchText WASM_1, "80186"
      SwitchText WASM_2, "80286"
      SwitchText WASM_3, "80386"
      SwitchText WASM_4, "80486"
      SwitchText WASM_5, "Pentium"
      SwitchText WASM_6, "Pentium Pro"
      SwitchText WASM_3r, "80386 register-based calling"
      SwitchText WASM_3s, "80386 stack-based calling"
      SwitchText WASM_4r, "80486 register-based calling"
      SwitchText WASM_4s, "80486 stack-based calling"
      SwitchText WASM_5r, "Pentium register-based calling"
      SwitchText WASM_5s, "Pentium stack-based calling"
      SwitchText WASM_6r, "Pentium Pro register-based calling"
      SwitchText WASM_6s, "Pentium Pro stack-based calling"
  Family "Code Generation Options"
    VSwitch 0, ?????, WASM_nd, -nd,, ONE, REQ, ""
    VSwitch 0, ?????, WASM_nt, -nt,, ONE, REQ, ""
    VSwitch 0, ?????, WASM_nm, -nm,, ONE, REQ, ""
    CSwitch 0, ?????, WASM_zcm, -zcm
    CSwitch 0, ?????, WASM_o, -o, OFF
    SwitchText WASM_nd, "Name of data segment:"
    SwitchText WASM_nt, "Name of text segment:"
    SwitchText WASM_nm, "Name of module:"
    SwitchText WASM_zcm, "Mangle C names like MASM"
    SwitchText WASM_o, "Allow C-style octal constants", "Allow C-style octal consants"
    RGroup "Floating-point level"
      RSwitch 1, ?????, WASM_def1, , ON
      RSwitch 1, ?????, WASM_fp0, -fp0, OFF
      RSwitch 1, ?????, WASM_fp2, -fp2, OFF
      RSwitch 1, ?????, WASM_fp3, -fp3, OFF
      RSwitch 1, ?????, WASM_fp5, -fp5, OFF
      RSwitch 1, ?????, WASM_fp6, -fp6, OFF
      SwitchText WASM_def1, "Compiler default"
      SwitchText WASM_fp0, "Inline 8087 instructions"
      SwitchText WASM_fp2, "Inline 80287 instructions"
      SwitchText WASM_fp3, "Inline 80387 instructions"
      SwitchText WASM_fp5, "Inline Pentium instructions"
      SwitchText WASM_fp6, "Inline Pentium Pro instructions"
    RGroup "Floating-point model"
      RSwitch 1, r????, WASM_def2, , OFF
      RSwitch 1, ?????, WASM_def2, , ON
      RSwitch 1, ?????, WASM_fpi, -fpi, OFF
      RSwitch 1, r????, WASM_fpi87, -fpi87, ON
      RSwitch 1, ?????, WASM_fpi87, -fpi87, OFF
      RSwitch 1, ?????, WASM_fpc, -fpc, OFF
      SwitchText WASM_def2, "Compiler default"
      SwitchText WASM_fpi, "Inline with emulator"
      SwitchText WASM_fpi87, "Inline with coprocessor"
      SwitchText WASM_fpc, "Floating-point calls"
  Family Debugging
    RGroup "Debugging style"
      RSwitch 0, ?????, WASM_nodebug, , ON, OFF
      RSwitch 0, ?????, WASM_d1, -d1, OFF, ON
      SwitchText WASM_nodebug, "No debugging information"
      SwitchText WASM_d1, "Line number information"
  Family Diagnostics
    RGroup "Warning level"
      RSwitch 0, ?????, WASM_w0, -w0, OFF
      RSwitch 0, ?????, WASM_w1, -w1, OFF
      RSwitch 0, ?????, WASM_w2, -w2, OFF
      RSwitch 0, ?????, WASM_w3, -w3, OFF
      RSwitch 0, ?????, WASM_w4, -w4, ON
      SwitchText WASM_w0, "Warning level 0"
      SwitchText WASM_w1, "Warning level 1"
      SwitchText WASM_w2, "Warning level 2"
      SwitchText WASM_w3, "Warning level 3"
      SwitchText WASM_w4, "Warning level 4"
    CSwitch 0, ?????, WASM_we, -we, OFF
    VSwitch 0, ?????, WASM_e, -e,, ONE, REQ, "25"
    CSwitch 0, ?????, WASM_ef, -ef
    SwitchText WASM_we, "Treat warnings as errors"
    SwitchText WASM_e, "Error count:"
    SwitchText WASM_ef, "Full pathnames in error messages:"
  Family Miscellaneous
    CSwitch 0, ?????, WASM_e, -e, OFF
    CSwitch 0, ?????, WASM_j, -j, OFF
    CSwitch 0, ?????, WASM_zq, -zq, ON
    VSwitch 0, ?????, WASM_d, -d,, MULTI, REQ, ""
    CSwitch 0, ?????, WASM_c, -c, OFF
    VSwitch 0, ?????, WASM_others,,, MULTI, REQ, ""
    SwitchText WASM_e, "Stop reading at END"
    SwitchText WASM_j, "Signed types for signed values"
    SwitchText WASM_zq, "Quiet operation"
    SwitchText WASM_d, "Macro definitions:"
    SwitchText WASM_c, "No data in code records"
    SwitchText WASM_others, "Other options:"

Tool WLINK "Linker"
  Family Basic
    CSwitch  -1, ?????, "", "wlink", ON
    CSwitch  -1, ?????, *WLINK_name, "name $'", ON
    RGroup "Debugging information"
      RSwitch  0, ?????, "No debug information", , ON, OFF
      RSwitch  0, ?????, WLINK_d_cw, "d codeview op cvp", OFF
      RSwitch  0, ?????, WLINK_d_dw, "d dwarf", OFF
      RSwitch  0, l?2e?, WLINK_d_no, "d novell", OFF
      RSwitch  0, ?????, WLINK_d_wa, "d watcom", OFF
      RSwitch  0, ?????, WLINK_d_li, "d lines", OFF
      RSwitch  0, ?????, WLINK_d_ty, "d types", OFF
      RSwitch  0, ?????, WLINK_d_lo, "d locals", OFF
      RSwitch  0, ?????, WLINK_d_all, "d all", OFF, ON
      SwitchText WLINK_d_cw, "Debug CodeView"
      SwitchText WLINK_d_dw, "Debug Dwarf"
      SwitchText WLINK_d_no, "Netware symbols"
      SwitchText WLINK_d_wa, "Debug Watcom"
      SwitchText WLINK_d_li, "Debug line numbers"
      SwitchText WLINK_d_ty, "Debug types"
      SwitchText WLINK_d_lo, "Debug locals"
      SwitchText WLINK_d_all, "Debug all"
    VSwitch  1, w?6d?, WLINK_sys, sys, " ", ONE, REQ, windows_dll
    VSwitch  1, w?6??, WLINK_sys, sys, " ", ONE, REQ, windows
    VSwitch  1, d?6??, WLINK_sys, sys, " ", ONE, REQ, dos
    CSwitch  -1, dc6e?, "", com, ON
    VSwitch  1, o?6??, WLINK_sys, sys, " ", ONE, REQ, os2
rem OS/2 dll?
    VSwitch  1, w?2??, WLINK_sys, sys, " ", ONE, REQ, win386
    VSwitch  1, dr2??, WLINK_sys, sys, " ", ONE, REQ, dos4g
    VSwitch  1, dw2??, WLINK_sys, sys, " ", ONE, REQ, causeway
    VSwitch  1, dx2??, WLINK_sys, sys, " ", ONE, REQ, cwdllr
    VSwitch  1, dy2??, WLINK_sys, sys, " ", ONE, REQ, cwdlls
    VSwitch  1, dm2??, WLINK_sys, sys, " ", ONE, REQ, pmodew
    VSwitch  1, da2??, WLINK_sys, sys, " ", ONE, REQ, dos32a
    VSwitch  1, db2??, WLINK_sys, sys, " ", ONE, REQ, dos32x
    VSwitch  1, dp2??, WLINK_sys, sys, " ", ONE, REQ, pharlap
    VSwitch  1, dt2??, WLINK_sys, sys, " ", ONE, REQ, tnt
    VSwitch  1, o?2??, WLINK_sys, sys, " ", ONE, REQ, os2v2
    CSwitch  -1, op?e?, "", pm, ON
    CSwitch  -1, of?e?, "", full, ON
    CSwitch  -1, od6e?, "", phys, ON
    CSwitch  -1, ov2e?, "", virt, ON
    VSwitch  1, nw2??, WLINK_sys, sys, " ", ONE, REQ, nt_win
    VSwitch  1, na2e?, WLINK_sys, sys, " ", ONE, REQ, nt_win
    VSwitch  1, nu2e?, WLINK_sys, sys, " ", ONE, REQ, nt_win
    VSwitch  1, no2e?, WLINK_sys, sys, " ", ONE, REQ, nt_win
    VSwitch  1, nm2e?, WLINK_sys, sys, " ", ONE, REQ, nt_win
    VSwitch  1, nm2d?, WLINK_sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, na2d?, WLINK_sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, no2d?, WLINK_sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, nu2d?, WLINK_sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, np2d?, WLINK_sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, n_2d?, WLINK_sys, sys, " ", ONE, REQ, nt_dll
    VSwitch  1, nc2??, WLINK_sys, sys, " ", ONE, REQ, nt
    VSwitch  1, nwa??, WLINK_sys, sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, naae?, WLINK_sys, sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, nuae?, WLINK_sys, sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, noae?, WLINK_sys, sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, nmae?, WLINK_sys, sys, " ", ONE, REQ, ntaxp_win
    VSwitch  1, nmad?, WLINK_sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, naad?, WLINK_sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, noad?, WLINK_sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, nuad?, WLINK_sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, npad?, WLINK_sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, n_ad?, WLINK_sys, sys, " ", ONE, REQ, ntaxp_dll
    VSwitch  1, nca??, WLINK_sys, sys, " ", ONE, REQ, ntaxp
    VSwitch  1, l?2??, WLINK_sys, sys, " ", ONE, REQ, netware
rem Other Netware variations?
    VSwitch  1, df2??, WLINK_sys, sys, " ", ONE, REQ, x32rv
    VSwitch  1, df2??, WLINK_sys, sys, " ", ONE, REQ, x32r
    VSwitch  1, ds2??, WLINK_sys, sys, " ", ONE, REQ, x32sv
    VSwitch  1, ds2??, WLINK_sys, sys, " ", ONE, REQ, x32s
    VSwitch  1, x?2??, WLINK_sys, sys, " ", ONE, REQ, linux
    VSwitch  1, rp2e?, WLINK_sys, sys, " ", ONE, REQ, rdos
    VSwitch  1, rp2d?, WLINK_sys, sys, " ", ONE, REQ, rdos_dll
    VSwitch  1, ru?d?, WLINK_sys, sys, " ", ONE, REQ, rdos_efi
    VSwitch  1, rk6??, WLINK_sys, sys, " ", ONE, REQ, rdos_dev16
    VSwitch  1, rk2??, WLINK_sys, sys, " ", ONE, REQ, rdosdev
    VSwitch  1, rb6??, WLINK_sys, sys, " ", ONE, REQ, rdos_bin16
    VSwitch  1, rb2??, WLINK_sys, sys, " ", ONE, REQ, rdos_bin32
    VSwitch  1, rm6??, WLINK_sys, sys, " ", ONE, REQ, rdos_mboot
    CSwitch  -1, d?2d?, *WLINK_dll, , ON
    CSwitch  -1, w?2d?, *WLINK_dll, , ON
    CSwitch  -1, w?6d?, *WLINK_dll, , ON
    CSwitch  -1, n??d?, *WLINK_dll, , ON
    CSwitch  -1, r??d?, *WLINK_dll, , ON
rem    CSwitch  -1, x??d?, *WLINK_dll, , ON
    CSwitch  -1, ???d?, *WLINK_dll, dll, ON
    CSwitch  -1, wm6d?, *WLINK_initi, initi, ON
    CSwitch  -1, wa6d?, *WLINK_initi, initi, ON
    CSwitch  -1, na?d?, *WLINK_initi, initi, ON
    CSwitch  -1, nm?d?, *WLINK_initi, initi, ON
    CSwitch  -1, no?d?, *WLINK_initi, initi, ON
    CSwitch  -1, nu?d?, *WLINK_initi, initi, ON
    CSwitch  -1, o?2d?, *WLINK_initi, initi, ON
    CSwitch  -1, o?2d?, *WLINK_termi, termi, ON
    CSwitch  -1, nu?d?, *WLINK_termi, termi, ON
    CSwitch  -1, na?d?, *WLINK_termi, termi, ON
    CSwitch  -1, no?d?, *WLINK_termi, termi, ON
    CSwitch  -1, nm?d?, *WLINK_termi, termi, ON
    CSwitch  -1, wm6d?, *WLINK_mem, mem, ON
    CSwitch  -1, wa6d?, *WLINK_mem, mem, ON
    VSwitch  1, l?2e?, WLINK_op_scr, "op scr", " '%s'", ONE, OFF, ""
    VSwitch  1, ?????, WLINK_op_st, "op st", =, ONE, REQ, ""
    VSwitch  1, wm?e?, WLINK_op_heap, "op heap", =, ONE, REQ, 1K
    VSwitch  1, wa?e?, WLINK_op_heap, "op heap", =, ONE, REQ, 1K
    VSwitch  1, rp???, WLINK_op_offset, "op offset", =, ONE, REQ, ""
    VSwitch  1, rk???, WLINK_op_codesel, "op codesel", =, ONE, REQ, ""
    VSwitch  1, rk???, WLINK_op_datasel, "op datasel", =, ONE, REQ, ""
    VSwitch  1, ?????, WLINK_others,,, MULTI, REQ, ""
    CSwitch  1, ?????, WLINK_op_m, "op m", ON
    CSwitch  1, ?????, WLINK_op_maplines, "op maplines", OFF
    CSwitch  1, l????, WLINK_op_inc, "op inc", OFF
    CSwitch  1, d?6??, WLINK_op_inc, "op inc", OFF
    CSwitch  1, ????w, WLINK_op_inc, "op inc", OFF
    CSwitch  1, ????j, WLINK_op_inc, "op inc", OFF
    CSwitch  1, ????8, WLINK_op_inc, "op inc", OFF
    CSwitch  1, o????, WLINK_op_inc, "op inc", OFF
    CSwitch  1, na???, WLINK_op_inc, "op inc", OFF, ON
    CSwitch  1, no???, WLINK_op_inc, "op inc", OFF, ON
    CSwitch  1, nu???, WLINK_op_inc, "op inc", OFF, ON
    CSwitch  1, nm???, WLINK_op_inc, "op inc", OFF, ON
    CSwitch  1, wa???, WLINK_op_inc, "op inc", OFF, ON
    CSwitch  1, wm???, WLINK_op_inc, "op inc", OFF, ON
    CSwitch  1, ?????, WLINK_op_inc, "op inc", OFF
    CSwitch  1, n????, WLINK_op_nostdcall, "op nostdcall", OFF
    SwitchText WLINK_sys, "System:"
    SwitchText WLINK_op_scr, "Output screen"
    SwitchText WLINK_op_st, "Stack:"
    SwitchText WLINK_op_heap, "Heap:"
    SwitchText WLINK_op_offset, "Image base:"
    SwitchText WLINK_op_codesel, "Code selector:"
    SwitchText WLINK_op_datasel, "Data selector:"
    SwitchText WLINK_others, "Other options(,):"
    SwitchText WLINK_op_m, "Map file"
    SwitchText WLINK_op_maplines, "Line numbers in map file"
    SwitchText WLINK_op_inc, "Incremental linking"
    SwitchText WLINK_op_nostdcall, "No stdcall name decoration"
  Family "Import, Export and Library"
    CSwitch  0, ?????, WLINK_op_nod, "op nod", OFF
    VSwitch  0, ?????, WLINK_op_libp, libp, " ", MULTI, REQ, ""
    VSwitch  0, wa6d?, WLINK_op_libr, libr, " ", MULTI, REQ, "mfw250.lib mfwo250.lib mfwd250.lib commdlg.lib shell.lib mfcoleui.lib compobj.lib storage.lib ole2.lib ole2disp.lib", "mfw250d.lib mfwo250d.lib mfwd250d.lib commdlg.lib shell.lib mfcoleui.lib compobj.lib storage.lib ole2.lib ole2disp.lib"
    VSwitch  0, ?????, WLINK_op_libr, libr, " ", MULTI, REQ, ""
    VSwitch  0, dc6e?, WLINK_op_libf, libf, " ", MULTI, REQ, "cstart_t"
    VSwitch  0, ???e?, WLINK_op_libf, libf, " ", MULTI, REQ, ""
    VSwitch  1, w????, WLINK_op_imp, imp, " ", ONE, REQ, ""
    VSwitch  1, n????, WLINK_op_imp, imp, " ", ONE, REQ, ""
    VSwitch  1, o????, WLINK_op_imp, imp, " ", ONE, REQ, ""
    VSwitch  1, l????, WLINK_op_imp, imp, " ", ONE, REQ, ""
    VSwitch  1, x????, WLINK_op_imp, imp, " ", ONE, REQ, ""
    VSwitch  1, dx???, WLINK_op_imp, imp, " ", ONE, REQ, ""
    VSwitch  1, dy???, WLINK_op_imp, imp, " ", ONE, REQ, ""
    VSwitch  1, rp???, WLINK_op_imp, imp, " ", ONE, REQ, ""
    VSwitch  1, w????, WLINK_op_exp, exp, " ", MULTI, REQ, ""
    VSwitch  1, n????, WLINK_op_exp, exp, " ", MULTI, REQ, ""
    VSwitch  1, o????, WLINK_op_exp, exp, " ", MULTI, REQ, ""
    VSwitch  1, l????, WLINK_op_exp, exp, " ", MULTI, REQ, ""
    VSwitch  1, x????, WLINK_op_exp, exp, " ", MULTI, REQ, ""
    VSwitch  1, dx???, WLINK_op_exp, exp, " ", MULTI, REQ, ""
    VSwitch  1, dy???, WLINK_op_exp, exp, " ", MULTI, REQ, ""
    VSwitch  1, rp???, WLINK_op_exp, exp, " ", MULTI, REQ, ""
    SwitchText WLINK_op_nod, "No default libraries"
    SwitchText WLINK_op_libp, "Library directories(;):"
    SwitchText WLINK_op_libr, "Libraries(,):"
    SwitchText WLINK_op_libf, "Library files(,):"
    SwitchText WLINK_op_imp, "Import names(,):", "Import files(,):"
    SwitchText WLINK_op_exp, "Export names(,):"
  Family Advanced
    CSwitch  0, ?????, WLINK_op_v, "op v", OFF
    SwitchText WLINK_op_v, "Verbose map file", "Verbose"
    RGroup "Case-sensitive link"
      RSwitch 0, ?????, WLINK_def1, , ON
      RSwitch 0, ?????, WLINK_op_c, "op c", OFF
      RSwitch 0, ?????, WLINK_op_nocase, "op nocase", OFF
      SwitchText WLINK_def1, "Default", "Linker default"
      SwitchText WLINK_op_c, "Case sensitive:", "Yes:", "Case sensitive link"
      SwitchText WLINK_op_nocase, "No case sensitive:", "No:"
    VSwitch  0, ?????, WLINK_op_maxe, "op maxe", =, ONE, REQ, 25
    CSwitch  0, w?6??, WLINK_op_rwr, "op rwr", OFF
    CSwitch  0, ?????, WLINK_op_d, "op d", OFF
    CSwitch  0, ?????, WLINK_op_u, "op u", OFF
    CSwitch  0, ?????, WLINK_op_q, "op q", ON
    SwitchText WLINK_op_maxe, "Error count:"
    SwitchText WLINK_op_rwr, "Check relocation of RW data"
    SwitchText WLINK_op_d, "Dosseg"
    SwitchText WLINK_op_u, "Undefines OK"
    SwitchText WLINK_op_q, "Quiet"
:segment !C_FOR_PB
    CSwitch  0, ?????, WLINK_op_symf, "op symf", ON
    VSwitch  0, w????, WLINK_op_vers, "op vers", = , ONE, REQ, ""
    VSwitch  0, n????, WLINK_op_vers, "op vers", = , ONE, REQ, ""
    VSwitch  0, o????, WLINK_op_vers, "op vers", = , ONE, REQ, ""
    VSwitch  0, l????, WLINK_op_vers, "op vers", = , ONE, REQ, ""
    SwitchText WLINK_op_symf, "Produce symbol file"
    SwitchText WLINK_op_vers, "Version (Major.minor.rev):"
:endsegment
    VSwitch  1, ?????, WLINK_op_namel, "op namel", =, ONE, REQ, ""
    SwitchText WLINK_op_namel, "Name length:"
:segment !C_FOR_PB
    VSwitch  1, ?????, WLINK_op_modt, modt, " ", ONE, REQ, ""
    VSwitch  1, ?????, WLINK_op_symt, symt, " ", ONE, REQ, ""
    VSwitch  1, nm?d?, WLINK_op_ref, ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, na?d?, WLINK_op_ref, ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, nu?d?, WLINK_op_ref, ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, no?d?, WLINK_op_ref, ref, " ", MULTI, REQ, "_DllMain"
    VSwitch  1, nm?e?, WLINK_op_ref, ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, na?e?, WLINK_op_ref, ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, no?e?, WLINK_op_ref, ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, nu?e?, WLINK_op_ref, ref, " ", MULTI, REQ, "_wstart_"
    VSwitch  1, wm?d?, WLINK_op_ref, ref, " ", MULTI, REQ, "__clib_WEP_"
    VSwitch  1, wa?d?, WLINK_op_ref, ref, " ", MULTI, REQ, "__clib_WEP_"
    VSwitch  1, ?????, WLINK_op_ref, ref, " ", MULTI, REQ, ""
    VSwitch  1, nm?e?, WLINK_op_start, "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, na?e?, WLINK_op_start, "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, no?e?, WLINK_op_start, "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, nu?e?, WLINK_op_start, "op start", =, ONE, REQ, "_wstart_"
    VSwitch  1, ?????, WLINK_op_start, "op start", =, ONE, REQ, ""
    SwitchText WLINK_op_modt, "ModTrace names(,):"
    SwitchText WLINK_op_symt, "SymTrace names(,):"
    SwitchText WLINK_op_ref, "Reference names(,):"
    SwitchText WLINK_op_start, "Starting address:"
:endsegment
    VSwitch  1, o????, WLINK_op_de, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, n????, WLINK_op_de, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, w????, WLINK_op_de, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dr???, WLINK_op_de, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dw???, WLINK_op_de, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, da???, WLINK_op_de, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, db???, WLINK_op_de, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dm???, WLINK_op_de, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dx???, WLINK_op_de, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, dy???, WLINK_op_de, "op de", " '%s'", ONE, REQ, ""
    VSwitch  1, r????, WLINK_op_de, "op de", " '%s'", ONE, REQ, ""
    SwitchText WLINK_op_de, "Description:"
rem PackCode, PackData for DOS, OS/2, QNX, Win16?
:segment !C_FOR_PB
  Family Special
    RGroup "DGROUP sharing"
      RSwitch  0, w?2d?, WLINK_def2, "", ON
      RSwitch  0, n??d?, WLINK_def2, "", ON
rem     RSwitch  0, x??d?, WLINK_def2, "", ON
      RSwitch  0, w?6d?, WLINK_def2, "", ON
      RSwitch  0, r??d?, WLINK_def2, "", ON
      RSwitch  0, ???d?, WLINK_def2, "", OFF
      RSwitch  0, ?????, WLINK_def2, "", ON
      RSwitch  0, w?2d?, WLINK_op_one, "op one", OFF
      RSwitch  0, n??d?, WLINK_op_one, "op one", OFF
rem     RSwitch  0, x??d?, WLINK_op_one, "op one", OFF
      RSwitch  0, w?6d?, WLINK_op_one, "op one", OFF
      RSwitch  0, o?2d?, WLINK_op_one, "op one", OFF
      RSwitch  0, ???d?, WLINK_op_one, "op one", OFF
      RSwitch  0, ?????, WLINK_op_one, "op one", OFF
      RSwitch  0, n??d?, WLINK_op_many, "op many", OFF
      RSwitch  0, w??d?, WLINK_op_many, "op many", OFF
      RSwitch  0, r??d?, WLINK_op_many, "op many", OFF
      RSwitch  0, ???d?, WLINK_op_many, "op many", ON
      RSwitch  0, ?????, WLINK_op_many, "op many", OFF
      SwitchText WLINK_def2, "Use linker default"
      SwitchText WLINK_op_one, "DGROUP is shared"
      SwitchText WLINK_op_many, "DGROUP is not shared"
    RGroup "Runtime environment"
      RSwitch  0, nw???, WLINK_ru_windef, "", ON
      RSwitch  0, nw???, WLINK_ru_win310, "ru win=3.10", OFF
      RSwitch  0, nc???, WLINK_ru_condef, "", ON
      RSwitch  0, nc???, WLINK_ru_os2310, "ru os2=3.10", OFF
      RSwitch  0, n?2d?, WLINK_ru_windef, "", ON
      RSwitch  0, n?2d?, WLINK_ru_con40, "ru con=4.0", OFF
      RSwitch  0, n?2d?, WLINK_ru_win310, "ru win=3.10", OFF
      RSwitch  0, r????, WLINK_ru_condef, "", ON
      SwitchText WLINK_ru_win310, "Win32s Windows 3.x:"
      SwitchText WLINK_ru_os2310, "16-bit OS/2 1.x:", "16Bit OS/2 1.x"
      SwitchText WLINK_ru_con40, "Character mode:"
      SwitchText WLINK_ru_windef, "Default GUI", "Native NT"
      SwitchText WLINK_ru_condef, "Default character mode", "Character mode"
    VSwitch  0, o????, WLINK_op_stub, "op stub", =, ONE, REQ, ""
    CSwitch  0, na?e?, WLINK_op_el, "op el", ON, OFF
    CSwitch  0, no?e?, WLINK_op_el, "op el", ON, OFF
    CSwitch  0, nu?e?, WLINK_op_el, "op el", ON, OFF
    CSwitch  0, nm?e?, WLINK_op_el, "op el", ON, OFF
    CSwitch  0, wm?e?, WLINK_op_el, "op el", ON, OFF
    CSwitch  0, wa?e?, WLINK_op_el, "op el", ON, OFF
    CSwitch  0, ?????, WLINK_op_el, "op el", OFF
    SwitchText WLINK_op_stub, "Stub executable"
    SwitchText WLINK_op_el, "Eliminate dead code"
:endsegment
::
:segment !C_FOR_PB
::

Tool WLIB "Library Manager"
  Family Basic
    CSwitch  -1, ?????, "", "wlib", ON
    CSwitch  0, ???s?, WLIB_b, -b, ON
    CSwitch  0, ???s?, WLIB_c, -c, ON
    CSwitch  0, ???s?, WLIB_m, -m, OFF
    CSwitch  0, ???s?, WLIB_n, -n, ON
    CSwitch  0, ???s?, WLIB_q, -q, ON
    CSwitch  0, ???s?, WLIB_s, -s, OFF
    CSwitch  0, ???s?, WLIB_t, -t, OFF
    CSwitch  0, ???s?, WLIB_x, -x, OFF
    VSwitch  1, ???s?, WLIB_p, -p, =, ONE, REQ, 512
    VSwitch  1, ???s?, WLIB_l, -l, =, ONE, REQ, ""
    VSwitch  1, ???s?, WLIB_d, -d, =, ONE, REQ, ""
    VSwitch  1, ???s?, WLIB_o, -o, =, ONE, REQ, ""
    SwitchText WLIB_b, "Don't create .bak file"
    SwitchText WLIB_c, "Case sensitive"
    SwitchText WLIB_m, "Display C++ mangled names"
    SwitchText WLIB_n, "Always create new library"
    SwitchText WLIB_q, "Quiet operation"
    SwitchText WLIB_s, "Strip LINNUM records"
    SwitchText WLIB_t, "Trim THEADR pathnames"
    SwitchText WLIB_x, "Explode all objects in library"
    SwitchText WLIB_p, "Page bound:"
    SwitchText WLIB_l, "Listing file name:"
    SwitchText WLIB_d, "Output directory:"
    SwitchText WLIB_o, "Output library name:"
  Family "Import Library"
    RGroup "Resident symbols"
      RSwitch  0, ???s?, WLIB_not_import1, , ON
      RSwitch  0, ???s?, WLIB_irn, -irn, OFF
      RSwitch  0, ???s?, WLIB_iro, -iro, OFF
      SwitchText WLIB_not_import1, "Do not import"
      SwitchText WLIB_irn, "Import by name"
      SwitchText WLIB_iro, "Import by ordinal"
    RGroup "Non-resident symbols"
      RSwitch  0, ???s?, WLIB_not_import2, , ON
      RSwitch  0, ???s?, WLIB_inn, -inn, OFF
      RSwitch  0, ???s?, WLIB_ino, -ino, OFF
      SwitchText WLIB_not_import2, "Do not import"
      SwitchText WLIB_inn, "Import by name"
      SwitchText WLIB_ino, "Import by ordinal"
::
:endsegment
::

Tool WRC "Resource Compiler"
  Family Resource
    CSwitch -1, ?????, "", "wrc $*.rc", ON
    RGroup "Build target"
      RSwitch  0, w????, WRC_w16, -bt=windows, ON
:segment !C_FOR_PB
      RSwitch  0, w????, WRC_w32, -bt=nt, OFF
      RSwitch  0, w????, WRC_os2, -bt=os2, OFF
      RSwitch  0, n????, WRC_w16, -bt=windows, OFF
      RSwitch  0, n????, WRC_w32, -bt=nt, ON
      RSwitch  0, n????, WRC_os2, -bt=os2, OFF
      RSwitch  0, o????, WRC_w16, -bt=windows, OFF
      RSwitch  0, o????, WRC_w32, -bt=nt, OFF
      RSwitch  0, o????, WRC_os2, -bt=os2, ON
      RSwitch  0, r????, WRC_rdos, -bt=nt, ON
      SwitchText WRC_w32, "Win32", "Windows NT"
      SwitchText WRC_os2, "OS/2"
      SwitchText WRC_rdos, "RDOS"
:endsegment
      SwitchText WRC_w16, "Win16", "Windows"
    RGroup "Multi-byte character support"
      RSwitch  0, ?????, WRC_single_byte, , ON
      RSwitch  0, ?????, WRC_zk0, -zk0, OFF
      RSwitch  0, ?????, WRC_zk1, -zk1, OFF
      RSwitch  0, ?????, WRC_zk2, -zk2, OFF
      RSwitch  0, ?????, WRC_zk3, -zk3, OFF
      RSwitch  0, ?????, WRC_zku8, -zku8, OFF
      SwitchText WRC_single_byte, "Single byte characters only"
      SwitchText WRC_zk0, "Kanji"
      SwitchText WRC_zk1, "Traditional Chinese", "Chinese/Taiwanese", "Chineese/Taiwanese"
      SwitchText WRC_zk2, "Korean"
      SwitchText WRC_zk3, "Simplified Chinese"
      SwitchText WRC_zku8, "Unicode UTF-8"
    VSwitch 1, wa???, WRC_d, -d, , MULTI, REQ, "_AFXDLL"
    VSwitch 1, na???, WRC_d, -d, , MULTI, REQ, "_AFXDLL WIN32 _WIN32 __NT__"
    VSwitch 1, no???, WRC_d, -d, , MULTI, REQ, "_AFXDLL _UNICODE WIN32 _WIN32 __NT__"
    VSwitch 1, nu???, WRC_d, -d, , MULTI, REQ, "_UNICODE WIN32 _WIN32 __NT__"
    VSwitch 1, n????, WRC_d, -d, , MULTI, REQ, "WIN32 _WIN32 __NT__"
    VSwitch 1, ?????, WRC_d, -d, , MULTI, REQ, ""
    VSwitch 1, w????, WRC_i, -i, =, MULTI, REQ, "\"$[:;$(%watcom)/h;$(%watcom)/h/win\""
    VSwitch 1, n????, WRC_i, -i, =, MULTI, REQ, "\"$[:;$(%watcom)/h;$(%watcom)/h/nt\""
    VSwitch 1, o????, WRC_i, -i, =, MULTI, REQ, "\"$[:;$(%watcom)/h;$(%watcom)/h/os2\""
    VSwitch 1, ?????, WRC_i, -i, =, MULTI, REQ, ""
    CSwitch 1, ?????, WRC_x, -x, OFF
    CSwitch 1, w????, WRC_zm, -zm, OFF
    CSwitch 1, ?????, WRC_zn, -zn, OFF
    CSwitch 1, ?????, WRC_q, -q, ON
    CSwitch 1, ?????, WRC_ad, -ad, ON
    SwitchText WRC_d, "Macro definitions:"
    SwitchText WRC_i, "Include directories:"
    SwitchText WRC_x, "Ignore INCLUDE environment variable", "Ignore INCLUDE env variable"
    SwitchText WRC_zm, "Microsoft format .res file"
    SwitchText WRC_zn, "Do not preprocess the file", "Do not proprocess the file"
    SwitchText WRC_q, "Quiet operation"
    SwitchText WRC_ad, "Output autodepend info"

Tool WRC2 "Resource Compiler (pass 2)"
  Family Resource
    CSwitch -1, ?????, "", wrc, ON
    CSwitch 0, ?????, WRC2_q, -q, ON
    CSwitch 0, ?????, WRC2_ad, -ad, ON
    CSwitch 0, ?????, WRC2_30, -30, OFF
    CSwitch 0, ?????, WRC2_e, -e, OFF
    CSwitch 0, ?????, WRC2_l, -l, OFF
    CSwitch 0, ?????, WRC2_m, -m, OFF
    CSwitch 0, ?????, WRC2_p, -p, OFF
    CSwitch 0, ?????, WRC2_t, -t, OFF
    SwitchText WRC2_q, "Quiet operation"
    SwitchText WRC2_ad, "Output autodepend info"
    SwitchText WRC2_30, "Requires Windows 3.0 or later"
    SwitchText WRC2_e, "Uses global memory above EMS", "Uses global mem above EMS"
    SwitchText WRC2_l, "Uses LIM 3.2 EMS directly"
    SwitchText WRC2_m, "EMS bank for each instance"
    SwitchText WRC2_p, "Private DLL"
    SwitchText WRC2_t, "Protected mode only"
    RGroup "Segements in fastload section"
      RSwitch 1, w????, WRC2_s0, -s0, ON
      RSwitch 1, w????, WRC2_s1, -s1, OFF
      RSwitch 1, w????, WRC2_s2, -s2, OFF
      SwitchText WRC2_s0, "No fastload section"
      SwitchText WRC2_s1, "Preload only"
      SwitchText WRC2_s2, "Preload/Data/Non-discardable"
::
:segment !C_FOR_PB
::

Tool ORC "OS/2 Resource Compiler"
  Family Resource
    CSwitch -1, ?????, "", rc, ON
    VSwitch 0, ?????, ORC_i, -i, " ", MULTI, REQ, "$[: $(%watcom)\\h $(%watcom)\\h\\os2"
    SwitchText ORC_i, "Include directories:"

Tool ORC2 "OS/2 Resource Compiler (pass 2)"
  Family Resource
    CSwitch -1, ?????, "", rc, ON
    VSwitch 0, ?????, ORC2_i, -i, " ", MULTI, REQ, "$[: $(%watcom)\\h $(%watcom)\\h\\os2"
    SwitchText ORC2_i, "Include directories:"

Tool ESQL "Embedded SQL compiler"
  Family Processor
    CSwitch -1, ?????, "", "sqlpp $@ $*.cxx", ON
    CSwitch 0, ?????, ESQL_q, -q, ON
    SwitchText ESQL_q, "Preprocessor quiet"
    RGroup Optimizations
      RSwitch 0, ?????, ESQL_no_optim, , ON
      RSwitch 0, ?????, ESQL_c, -c, OFF
      RSwitch 0, ?????, ESQL_d, -d, OFF
      SwitchText ESQL_no_optim, "No optimizations"
      SwitchText ESQL_c, "Favour code size"
      SwitchText ESQL_d, "Favour data size"
    CSwitch 0, ?????, ESQL_f, -f, OFF
    CSwitch 0, ?????, ESQL_il, -il, OFF
    CSwitch 0, ?????, ESQL_n, -n, OFF
    VSwitch 0, ?????, ESQL_s, -s, " ", ONE, REQ, ""
    VSwitch 0, ?????, ESQL_l, -l, " ", ONE, REQ, ""
    SwitchText ESQL_f, "Generated static data is FAR"
    SwitchText ESQL_il, "Use long ints"
    SwitchText ESQL_n, "Generate line numbers"
    SwitchText ESQL_s, "Maximum string constant length:"
    SwitchText ESQL_l, "Login(id,pswd):"
    CSwitch -1, d?6??, "", "-o DOS", ON
    CSwitch -1, d?2??, "", "-o DOS32", ON
    CSwitch -1, w?6??, "", "-o WINDOWS", ON
    CSwitch -1, w?2??, "", "-o WIN32", ON
    CSwitch -1, n????, "", "-o WINNT", ON
    CSwitch -1, o?6??, "", "-o OS2", ON
    CSwitch -1, o?2??, "", "-o OS232", ON

Tool ESQLWCC "C Compiler for ESQL"
  IncludeTool WLANG
  IncludeTool CCOMP
  Family Compiler
    CSwitch -1, ??6??, "", "wcc $*.cxx", ON
    CSwitch -1, ??2??, "", "wcc386 $*.cxx", ON
    CSwitch -1, ??a??, "", "wccaxp $*.cxx", ON

Tool ESQLWPP "C++ Compiler for ESQL"
  IncludeTool WLANG
  IncludeTool CPPCOMP
  Family Compiler
    CSwitch -1, ??6??, "", "wpp $*.cxx", ON
    CSwitch -1, ??2??, "", "wpp386 $*.cxx", ON
    CSwitch -1, ??a??, "", "wppaxp $*.cxx", ON

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
    CSwitch -1, o????, *IMAGEEDIT_run, iconedit, ON
:segment FOR_X64
    CSwitch -1, ????n, *IMAGEEDIT_run, "$(%watcom)\\binnt64\\wimgedit /n", ON
:elsesegment
    CSwitch -1, ????n, *IMAGEEDIT_run, "$(%watcom)\\binnt\\wimgedit /n", ON
:endsegment
    CSwitch -1, ????a, *IMAGEEDIT_run, "$(%watcom)\\axpnt\\wimgedit /n", ON
:segment FOR_X64
    CSwitch -1, ????9, *IMAGEEDIT_run, "$(%watcom)\\binnt64\\wimgedit /n", ON
:elsesegment
    CSwitch -1, ????9, *IMAGEEDIT_run, "$(%watcom)\\binnt\\wimgedit /n", ON
:endsegment
    CSwitch -1, ?????, *IMAGEEDIT_run, "$(%watcom)\\binw\\wimgedit /n", ON
::
:segment !C_FOR_PB
::

Tool WINHC "Windows Help Compiler"
  Family Compiler
    RGroup "Help Compiler"
rem These must be in the user's path
      RSwitch 0, ?????, WINHC_hhw, hhw , OFF
      RSwitch 0, ?????, WINHC_hcw, hcw , OFF
      RSwitch 0, ?????, WINHC_hc31, hc31 , ON
      RSwitch 0, ?????, WINHC_hc30, hc30 , OFF
      SwitchText WINHC_hhw, "HTML Help", "HTMLHelp"
      SwitchText WINHC_hcw, "Version 4.0"
      SwitchText WINHC_hc31, "Version 3.1"
      SwitchText WINHC_hc30, "Version 3.0"

Tool OIPF "OS/2 Help Compiler"
  Family Compiler
    CSwitch -1, ?????, "", "ipfc ", ON
    CSwitch -1, ???i?, "", /inf, ON
    VSwitch 0, ?????, OIPF_cc, /COUNTRY, =, ONE, REQ, ""
    VSwitch 0, ?????, OIPF_cp, /CODEPAGE, =, ONE, REQ, ""
    VSwitch 0, ?????, OIPF_l, /L, =, ONE, REQ, ""
    SwitchText OIPF_cc, "Country Code:"
    SwitchText OIPF_cp, "Code page:", "CodePage:"
    SwitchText OIPF_l, "Language:"
    RGroup "Warning level"
      RSwitch 0, ?????, OIPF_w1, /W1, OFF
      RSwitch 0, ?????, OIPF_w2, /W2, OFF
      RSwitch 0, ?????, OIPF_w3, /W3, ON
      SwitchText OIPF_w1, "Warning level 1"
      SwitchText OIPF_w2, "Warning level 2"
      SwitchText OIPF_w3, "Warning level 3"
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
Tool OS2LINK "OS/2 Linking"
  IncludeTool WLINK
  IncludeTool WRC2

Rule OEXE, OS2LINK, o??e?
  Target *.exe
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol DLL, *.dll
  Symbol RES, *.res
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command " *$<#WLINK> @$'.lk1"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " *$<#WRC2> $<RES!> $@"
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
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
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
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
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
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
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
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
  Command " *$<#WLINK> @$'.lk1"

Tool RBINLINK "RDOS Binary Linking"
  IncludeTool WLINK

Rule RBEXE, RBINLINK, rb?e?
  Target *.bin
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
  Command " *$<#WLINK> @$'.lk1"

Tool RMBOOTLINK "RDOS Multiboot Stub Linking"
  IncludeTool WLINK

Rule RMEXE, RMBOOTLINK, rm6e?
  Target *.bin
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
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
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
  Command " *$<#WLINK> @$'.lk1"
  Command " $<#COPYFILE>"

rem --Linux Executables--
Rule LNX, WLINK, x?2e?
  Target *.elf
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
  Command " *$<#> @$'.lk1"

rem --All Other Executables--
Rule EXE, WLINK, ???e?
  Target *.exe, *.exp, *.com
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
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
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
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
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
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
  Command " *wlib -q -n -b $*.lib +$*.dll",                                          ??6??

rem Change this when wrc is mature
Rule ODLL, WLINK, o??d?
  Target *.dll
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol RES, *.res
  Symbol DLL, *.dll
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command " *$<#> @$'.lk1"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " *$<#WRC2> $<RES!> $@"
  Command "!endif"
  Command " *wlib -q -n -b $*.lib +$*.dll"

Rule NDLL, WLINK, n??d?
  Target *.dll
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol RES, *.res
  Symbol DLL, *.dll
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " @%append $*.lk1 $<RES,>"
  Command "!endif"
  Command " *$<#> @$'.lk1"
  Command " *wlib -q -n -b $*.lib +$*.dll"

Rule RDLL, WLINK, rp?d?
  Target *.dll
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol RES, *.res
  Symbol DLL, *.dll
  Command "!ifneq BLANK \"$<FIL'!>\""
  Command " @%write $*.lk1 $<FIL',>"
  Command "!else"
  Command " @%write $*.lk1 FIL $(%watcom)/lib386/rdos/resstub.obj"
  Command "!endif"
  Command " @%append $*.lk1 $<LIBR',>"
  Command "!ifneq BLANK \"$<DLL!>\""
  Command " *wlib -q -n -b $*.imp $<DLL!>"
  Command " @%append $*.lk1 LIBR $'.imp"
  Command "!endif"
  Command "!ifneq BLANK \"$<RES!>\""
  Command " @%append $*.lk1 $<RES,>"
  Command "!endif"
  Command " *$<#> @$'.lk1"
  Command "!ifneq BLANK \"$<FIL'!>\""
  Command " *wlib -q -n -b $*.lib +$*.dll"
  Command "!endif"

Rule REFI, WLINK, ru?d?
  Target *.efi
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Symbol RES, *.res
  Symbol EFI, *.efi
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
  Command " *$<#> @$'.lk1"

rem Rule LNXDLL, WLINK, x??d?
rem   Target *.dll
rem   Autodepend
rem   Symbol FIL, *.obj
rem   Symbol LIBR, *.lib
rem   Symbol DLL, *.dll
rem   Command " @%write $*.lk1 $<FIL',>"
rem   Command " @%append $*.lk1 $<LIBR',>"
rem   Command " *$<#> @$'.lk1"

Rule CWDLL, WLINK, d?2d?
  Target *.dll
  Autodepend
  Symbol FIL, *.obj
  Symbol LIBR, *.lib
  Command " @%write $*.lk1 $<FIL',>"
  Command " @%append $*.lk1 $<LIBR',>"
  Command " *$<#> @$'.lk1"

rem --Static Libraries--
Rule LIB, WLIB, ???s?
  Target *.lib
  Autodepend
  Symbol  OBJ, *.obj
  Symbol  LIB, *.lib
  Command " @%write $*.lb1 $<OBJ!>"
  Command " @%append $*.lb1 $<LIB!>"
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
