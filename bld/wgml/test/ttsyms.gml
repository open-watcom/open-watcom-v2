:set symbol="target"    value="DOS".
:set symbol="computer"  value="Intel".
:set symbol="company"   value="Open Watcom".
:set symbol="scompany"  value="Watcom International Corp.".
:set symbol="pcompany"  value="Sybase".
:set symbol="spcompany" value="Sybase, Inc.".
.*
.   :set symbol="langsuff"   value="c".
.   .*
.   :set symbol="header"     value="header".
.   :set symbol="hdrsuff"    value="h".
.   .*
.   :set symbol="product"    value="&company C/C++".
.   :set symbol="cmpname"    value="&company C/C++".
.   .*
.   :set symbol="ccmd16"     value="wcc".
.   :set symbol="ccmd32"     value="wcc386".
.   :set symbol="ccmdAX"     value="wccaxp".
.   :set symbol="pcmd16"     value="wpp".
.   :set symbol="pcmd32"     value="wpp386".
.   :set symbol="pcmdAX"     value="wppaxp".
.   .*
.   .   :set symbol="cvar16"     value="wcc".
.   .   :set symbol="cvar32"     value="wcc386".
.   .   :set symbol="cvarAX"     value="wccaxp".
.   .   :set symbol="pvar16"     value="wpp".
.   .   :set symbol="pvar32"     value="wpp386".
.   .   :set symbol="pvarAX"     value="wppaxp".
.   .   :set symbol="wclname"    value="&company Compile and Link".
.   .   :set symbol="wclvar16"   value="wcl".
.   .   :set symbol="wclvarup16" value="WCL".
.   .   :set symbol="wclcmd16"   value="wcl".
.   .   :set symbol="wclcmdup16" value="WCL".
.   .   :set symbol="wclvar32"   value="wcl386".
.   .   :set symbol="wclvarup32" value="WCL386".
.   .   :set symbol="wclcmd32"   value="wcl386".
.   .   :set symbol="wclcmdup32" value="WCL386".
.   .   :set symbol="wclvarAX"   value="wclaxp".
.   .   :set symbol="wclvarupAX" value="WCLAXP".
.   .   :set symbol="wclcmdAX"   value="wclaxp".
.   .   :set symbol="wclcmdupAX" value="WCLAXP".
.   .   :set symbol="wcltemp"    value="__WCL__".
.   .   :set symbol="hdrdir"     value="\h".
.   .   :set symbol="hdrdirup"   value="\H".
.   .   :set symbol="frcvar"     value="force".
.   .   :set symbol="incvar"     value="include".
.*
.sr langsuffup=&u'&langsuff
.sr hdrsuffup=&u'&hdrsuff
.sr cvarup16=&u'&cvar16
.sr ccmdup16=&u'&ccmd16
.sr cvarup32=&u'&cvar32
.sr ccmdup32=&u'&ccmd32
.sr cvarupAX=&u'&cvarAX
.sr ccmdupAX=&u'&ccmdAX
.sr pvarup16=&u'&pvar16
.sr pcmdup16=&u'&pcmd16
.sr pvarup32=&u'&pvar32
.sr pcmdup32=&u'&pcmd32
.sr pvarupAX=&u'&pvarAX
.sr pcmdupAX=&u'&pcmdAX
.sr frcvarup=&u'&frcvar
.sr incvarup=&u'&incvar
.*
:set symbol="optdag"    value="".
:set symbol="cdate"     value="1997/07/16 09:22:47".

:cmt. fix problem with using the Script control word separator ";"
.ti 3b fb
.tr fb 3b

:set symbol="prompt"    value="C>".
:set symbol="drive"     value="drive".
:set symbol="dr1"       value="a:".
:set symbol="dr2"       value="b:".
:set symbol="dr3"       value="c:".
:set symbol="dr4"       value="d:".
:set symbol="exc"       value=".EXE".
:set symbol="exe"       value=".exe".
:set symbol="exeup"     value=".EXE".
:set symbol="axt"       value=".asm".
:set symbol="obj"       value=".obj".
:set symbol="setcmd"    value="set".
:set symbol="setcmdup"  value="SET".
:set symbol="setdelim"  value="".

:set symbol="cxt"       value=".c".
:set symbol="xxt"       value=".cpp".
:set symbol="def"       value=".def".
:set symbol="pxt"       value=".i".
:set symbol="hxt"       value=".h".
:set symbol="prf"       value=".prf".

:set symbol="err"       value=".err".
:set symbol="lib"       value=".lib".
:set symbol="lst"       value=".lst".
:set symbol="n87var"    value="no87".
:set symbol="pthvar"    value="path".
:set symbol="tmpvar"    value="tmp".
:set symbol="wcmvar"    value="wcgmemory".

:set symbol="n87varup"  value="NO87".
:set symbol="pthvarup"  value="PATH".
:set symbol="tmpvarup"  value="TMP".
:set symbol="wcmvarup"  value="WCGMEMORY".
.*
.   :set symbol="libdir16"   value="\lib286".
.   :set symbol="libdirup16" value="\LIB286".

:set symbol="calref" value="cal86".
:set symbol="calttl" value="16-bit Assembly Language Considerations".
:set symbol="machint" value="short int".
:set symbol="intsize" value="2".
:set symbol="maxint" value="32767".
:set symbol="umaxint" value="65535".
:set symbol="minint" value="-32768".
:set symbol="ax" value="ax".
:set symbol="axup" value="AX".
:set symbol="bx" value="bx".
:set symbol="bxup" value="BX".
:set symbol="cx" value="cx".
:set symbol="cxup" value="CX".
:set symbol="dx" value="dx".
:set symbol="dxup" value="DX".
:set symbol="di" value="di".
:set symbol="diup" value="DI".
:set symbol="si" value="si".
:set symbol="siup" value="SI".
:set symbol="bp" value="bp".
:set symbol="bpup" value="BP".
:set symbol="sp" value="sp".
:set symbol="spup" value="SP".
:set symbol="reg4" value="DX AX".
:set symbol="reg8" value="AX BX CX DX".
:set symbol="arg_2_regs" value="long int".
:set symbol="typ_2_regs" value="long int".
:set symbol="nearsize" value="1 word (16 bits)".
:set symbol="farsize" value="2 words (32 bits)".
:set symbol="bitmode" value="16-bit mode".
:set symbol="nptrsz" value="2".
:set symbol="nargsz" value="2".
:set symbol="fptrsz" value="4".
:set symbol="fargsz" value="4".

:SET symbol='kwasm'         value='__asm'.
:SET symbol='kwasm_sp'      value='_&SYSRB._asm'.
:CMT.
:SET symbol='kwbased'       value='__based'.
:SET symbol='kwbased_sp'    value='_&SYSRB._based'.
:CMT.
:SET symbol='kwbldiflt'     value='__builtin_isfloat'.
:SET symbol='kwbldif_sp'    value='_&SYSRB._builtin_isfloat'.
:CMT.
:SET symbol='kwicdecl'      value='_Cdecl'.
:SET symbol='kwicdec_sp'    value='_Cdecl'.
:CMT.
:SET symbol='kwcdecl'       value='__cdecl'.
:SET symbol='kwcdecl_sp'    value='_&SYSRB._cdecl'.
:CMT.
:SET symbol='kwdeclspec'    value='__declspec'.
:SET symbol='kwdclsp_sp'    value='_&SYSRB._declspec'.
:CMT.
:SET symbol='kwiexcept'     value='_Except'.
:SET symbol='kwiexcp_sp'    value='_Except'.
:CMT.
:SET symbol='kwexcept'      value='__except'.
:SET symbol='kwexcpt_sp'    value='_&SYSRB._except'.
:CMT.
:SET symbol='kwiexport'     value='_Export'.
:SET symbol='kwiexpr_sp'    value='_Export'.
:CMT.
:SET symbol='kwexport'      value='__export'.
:SET symbol='kwexprt_sp'    value='_&SYSRB._export'.
:CMT.
:SET symbol='kwfar'         value='__far'.
:SET symbol='kwfar_sp'      value='_&SYSRB._far'.
:CMT.
:SET symbol='kwifar16'      value='_Far16'.
:SET symbol='kwifa16_sp'    value='_Far16'.
:CMT.
:SET symbol='kwfar16'       value='__far16'.
:SET symbol='kwfar16_sp'    value='_&SYSRB._far16'.
:CMT.
:SET symbol='kwifastcal'    value='_Fastcall'.
:SET symbol='kwifstc_sp'    value='_Fastcall'.
:CMT.
:SET symbol='kwfastcall'    value='__fastcall'.
:SET symbol='kwfastc_sp'    value='_&SYSRB._fastcall'.
:CMT.
:SET symbol='kwifinally'    value='_Finally'.
:SET symbol='kwifnly_sp'    value='_Finally'.
:CMT.
:SET symbol='kwfinally'     value='__finally'.
:SET symbol='kwfnly_sp'     value='_&SYSRB._finally'.
:CMT.
:SET symbol='kwfortran'     value='__fortran'.
:SET symbol='kwfortr_sp'    value='_&SYSRB._fortran'.
:CMT.
:SET symbol='kwhuge'        value='__huge'.
:SET symbol='kwhuge_sp'     value='_&SYSRB._huge'.
:CMT.
:SET symbol='kwinline'      value='__inline'.
:SET symbol='kwinlin_sp'    value='_&SYSRB._inline'.
:CMT.
:SET symbol='kwint64'       value='__int64'.
:SET symbol='kwint64_sp'    value='_&SYSRB._int64'.
:CMT.
:SET symbol='kwintrpt'      value='__interrupt'.
:SET symbol='kwintr_sp'     value='_&SYSRB._interrupt'.
:CMT.
:SET symbol='kwileave'      value='_Leave'.
:SET symbol='kwileav_sp'    value='_Leave'.
:CMT.
:SET symbol='kwleave'       value='__leave'.
:SET symbol='kwleave_sp'    value='_&SYSRB._leave'.
:CMT.
:SET symbol='kwloadds'      value='__loadds'.
:SET symbol='kwlodds_SP'    value='_&SYSRB._loadds'.
:CMT.
:SET symbol='kwnear'        value='__near'.
:SET symbol='kwnear_sp'     value='_&SYSRB._near'.
:CMT.
:SET symbol='kwimagunit'    value='__ow_imaginary_unit'.
:SET symbol='kwimagu_sp'    value='_&SYSRB._ow_imaginary_unit'.
:CMT.
:SET symbol='kwipacked'     value='_Packed'.
:SET symbol='kwipckd_sp'    value='_Packed'.
:CMT.
:SET symbol='kwipascal'     value='_Pascal'.
:SET symbol='kwipasc_sp'    value='_Pascal'.
:CMT.
:SET symbol='kwpascal'      value='__pascal'.
:SET symbol='kwpascl_sp'    value='_&SYSRB._pascal'.
:CMT.
:SET symbol='kwsaveregs'    value='__saveregs'.
:SET symbol='kwsvreg_sp'    value='_&SYSRB._saveregs'.
:CMT.
:SET symbol='kwsegment'     value='__segment'.
:SET symbol='kwsegm_sp'     value='_&SYSRB._segment'.
:CMT.
:SET symbol='kwsegname'     value='__segname'.
:SET symbol='kwsegnm_sp'    value='_&SYSRB._segname'.
:CMT.
:SET symbol='kwiseg16'      value='_Seg16'.
:SET symbol='kwisg16_sp'    value='_Seg16'.
:CMT.
:SET symbol='kwself'        value='__self'.
:SET symbol='kwself_sp'     value='_&SYSRB._self'.
:CMT.
:SET symbol='kwisyscall'    value='_Syscall'.
:SET symbol='kwisysc_sp'    value='_Syscall'.
:CMT.
:SET symbol='kwsyscall'     value='__syscall'.
:SET symbol='kwsyscl_sp'    value='_&SYSRB._syscall'.
:CMT.
:SET symbol='kwstdcall'     value='__stdcall'.
:SET symbol='kwstdcl_sp'    value='_&SYSRB._stdcall'.
:CMT.
:SET symbol='kwisystem'     value='_System'.
:SET symbol='kwisyst_sp'    value='_System'.
:CMT.
:SET symbol='kwitry'        value='_Try'.
:SET symbol='kwitry_sp'     value='_Try'.
:CMT.
:SET symbol='kwtry'         value='__try'.
:SET symbol='kwtry_sp'      value='_&SYSRB._try'.
:CMT.
:SET symbol='kwunalign'     value='__unaligned'.
:SET symbol='kwunalg_sp'    value='_&SYSRB._unaligned'.
:CMT.
:SET symbol='kwwatcall'     value='__watcall'.
:SET symbol='kwwatcl_sp'    value='_&SYSRB._watcall'.
:CMT.
:CMT. ********************** Macros *************************************
:CMT.
:SET symbol='mkwCSGN'       value='__CHAR_SIGNED__'.
:SET symbol='mkwCSGN_sp'    value='_&SYSRB._CHAR_SIGNED_&SYSRB._'.
:CMT.
:SET symbol='mkwCHPW'       value='__CHEAP_WINDOWS__'.
:SET symbol='mkwCHPW_sp'    value='_&SYSRB._CHEAP_WINDOWS_&SYSRB._'.
:CMT.
:SET symbol='mkwCOMPACT'    value='__COMPACT__'.
:SET symbol='mkwCOMP_sp'    value='_&SYSRB._COMPACT_&SYSRB._'.
:CMT.
:SET symbol='mkwDATE'       value='__DATE__'.
:SET symbol='mkwDATE_sp'    value='_&SYSRB._DATE_&SYSRB._'.
:CMT.
:SET symbol='mkwDOS'        value='__DOS__'.
:SET symbol='mkwDOS_sp'     value='_&SYSRB._DOS_&SYSRB._'.
:CMT.
:SET symbol='mkwEXPRESS'    value='__EXPRESSC__'.
:SET symbol='mkwEXPR_sp'    value='_&SYSRB._EXPRESSC_&SYSRB._'.
:CMT.
:SET symbol='mkwFILE'       value='__FILE__'.
:SET symbol='mkwFILE_sp'    value='_&SYSRB._FILE_&SYSRB._'.
:CMT.
:SET symbol='mkwFLAT'       value='__FLAT__'.
:SET symbol='mkwFLAT_sp'    value='_&SYSRB._FLAT_&SYSRB._'.
:CMT.
:SET symbol='mkwFPI'        value='__FPI__'.
:SET symbol='mkwFPI_sp'     value='_&SYSRB._FPI_&SYSRB._'.
:CMT.
:SET symbol='mkwFNC'        value='__func__'.
:SET symbol='mkwFNC_sp'     value='_&SYSRB._func_&SYSRB._'.
:CMT.
:SET symbol='mkwFUNC'       value='__FUNCTION__'.
:SET symbol='mkwFUNC_sp'    value='_&SYSRB._FUNCTION_&SYSRB._'.
:CMT.
:SET symbol='mkwHUGE'       value='__HUGE__'.
:SET symbol='mkwHUGE_sp'    value='_&SYSRB._HUGE_&SYSRB._'.
:CMT.
:SET symbol='mkwINLN'       value='__INLINE_FUNCTIONS__'.
:SET symbol='mkwINLN_sp'    value='_&SYSRB._INLINE_FUNCTIONS_&SYSRB._'.
:CMT.
:SET symbol='mkwLARGE'      value='__LARGE__'.
:SET symbol='mkwLARG_sp'    value='_&SYSRB._LARGE_&SYSRB._'.
:CMT.
:SET symbol='mkwLINE'       value='__LINE__'.
:SET symbol='mkwLINE_sp'    value='_&SYSRB._LINE_&SYSRB._'.
:CMT.
:SET symbol='mkwLINUX'      value='__LINUX__'.
:SET symbol='mkwLNX_sp'     value='_&SYSRB._LINUX_&SYSRB._'.
:CMT.
:SET symbol='mkwMEDIUM'     value='__MEDIUM__'.
:SET symbol='mkwMED_sp'     value='_&SYSRB._MEDIUM_&SYSRB._'.
:CMT.
:SET symbol='mkwNULLSEG'    value='_NULLSEG'.
:SET symbol='mkwNSEG_sp'    value='_NULLSEG'.
:CMT.
:SET symbol='mkwNULLOFF'    value='_NULLOFF'.
:SET symbol='mkwNOFF_sp'    value='_NULLOFF'.
:CMT.
:SET symbol='mkwNET'        value='__NETWARE_386__'.
:SET symbol='mkwNET_sp'     value='_&SYSRB._NETWARE_386_&SYSRB._'.
:CMT.
:SET symbol='mkwNT'         value='__NT__'.
:SET symbol='mkwNT_sp'      value='_&SYSRB._NT_&SYSRB._'.
:CMT.
:SET symbol='mkwOS2'        value='__OS2__'.
:SET symbol='mkwOS2_sp'     value='_&SYSRB._OS2_&SYSRB._'.
:CMT.
:SET symbol='mkwQNX'        value='__QNX__'.
:SET symbol='mkwQNX_sp'     value='_&SYSRB._QNX_&SYSRB._'.
:CMT.
:SET symbol='mkwSMALL'      value='__SMALL__'.
:SET symbol='mkwSMAL_sp'    value='_&SYSRB._SMALL_&SYSRB._'.
:CMT.
:SET symbol='mkwSTDC'       value='__STDC__'.
:SET symbol='mkwSTDC_sp'    value='_&SYSRB._STDC_&SYSRB._'.
:CMT.
:SET symbol='mkwSTDC'       value='__STDC__'.
:SET symbol='mkwSTDC_sp'    value='_&SYSRB._STDC_&SYSRB._'.
:CMT.
:SET symbol='mkwSTDCH'      value='__STDC_HOSTED__'.
:SET symbol='mkwSTDCHsp'    value='_&SYSRB._STDC_HOSTED_&SYSRB._'.
:CMT.
:SET symbol='mkwSTDCX'      value='__STDC_LIB_EXT1__'.
:SET symbol='mkwSTDCXsp'    value='_&SYSRB._STDC_LIB_EXT1_&SYSRB._'.
:CMT.
:SET symbol='mkwSTDCV'      value='__STDC_VERSION__'.
:SET symbol='mkwSTDCVsp'    value='_&SYSRB._STDC_VERSION_&SYSRB._'.
:CMT.
:SET symbol='mkwTIME'       value='__TIME__'.
:SET symbol='mkwTIME_sp'    value='_&SYSRB._TIME_&SYSRB._'.
:CMT.
:SET symbol='mkwWATCOMC'    value='__WATCOMC__'.
:SET symbol='mkwWATC_sp'    value='_&SYSRB._WATCOMC_&SYSRB._'.
:CMT.
:SET symbol='mkwWIN_386'    value='__WINDOWS_386__'.
:SET symbol='mkwW386_sp'    value='_&SYSRB._WINDOWS_386_&SYSRB._'.
:CMT.
:SET symbol='mkwWINDOWS'    value='__WINDOWS__'.
:SET symbol='mkwWIND_sp'    value='_&SYSRB._WINDOWS_&SYSRB._'.
:CMT.
:SET symbol='mkw386'        value='__386__'.
:SET symbol='mkw386_sp'     value='_&SYSRB._386_&SYSRB._'.
:CMT.
:CMT. ******* Microsoft macros with leading underscores **************
:CMT.
:SET symbol='mkwM_IX86'     value='_M_IX86'.
:SET symbol='mkwMX86_sp'    value='_M_IX86'.
:CMT.
:CMT. ******* Microsoft macros without leading underscores **************
:CMT.
:SET symbol='mkwM_I386'     value='M_I386'.
:SET symbol='mkwM386_sp'    value='M_I386'.
:CMT.
:SET symbol='mkwM_I86'      value='M_I86'.
:SET symbol='mkwM86_sp'     value='M_I86'.
:CMT.
:SET symbol='mkwM_I86CM'    value='M_I86CM'.
:SET symbol='mkwM86C_sp'    value='M_I86CM'.
:CMT.
:SET symbol='mkwM_I86HM'    value='M_I86HM'.
:SET symbol='mkwM86H_sp'    value='M_I86HM'.
:CMT.
:SET symbol='mkwM_I86LM'    value='M_I86LM'.
:SET symbol='mkwM86L_sp'    value='M_I86LM'.
:CMT.
:SET symbol='mkwM_I86MM'    value='M_I86MM'.
:SET symbol='mkwM86M_sp'    value='M_I86MM'.
:CMT.
:SET symbol='mkwM_I86SM'    value='M_I86SM'.
:SET symbol='mkwM86S_sp'    value='M_I86SM'.
:CMT.
:SET symbol='mkwMSDOS'      value='MSDOS'.
:SET symbol='mkwMDOS_sp'    value='MSDOS'.
:CMT.
:SET symbol='mkwNKEY'       value='NO_EXT_KEYS'.
:SET symbol='mkwNKEY_sp'    value='NO_EXT_KEYS'.

:set symbol="cnt_ttl"   value=" ".
:set symbol="pubdate"   value="July, 1997".
:set symbol="headtxt0$" value=" ".
:set symbol="headtxt1$" value=" ".
:set symbol="headtext$" value=" ".
:set symbol="WDWlvl"    value="10".
:set symbol="SCTlvl"    value="1".
:set symbol="INDillust" value="2".
:set symbol="NTEpr2"    value=" ".
:set symbol="NTEpr1"    value=" ".
:set symbol="NTEset"    value=" ".
:set symbol="NTEnhi"    value="termhi=3".
:set symbol="NTEphi"    value="termhi=2".
:set symbol="ANTset"    value=" ".
:set symbol="APTset"    value="compact".
:set symbol="INDxmp"    value="0.3i".
:set symbol="INDlvl"    value="5".
.*
:SET symbol='SYSRB'     value='~b'.
:SET symbol='DAGGER'    value='~D'.

.se cpp_co=&$co.
.se cpp_ju=&$ju.

.'se getarea=';.i get area;.ct '
.'se putarea=';.i put area;.ct '
.'se rsvarea=';.i reserve area;.ct '

:cmt.:set symbol="arrow"   value=";.sf7;.ct ~L;.esf;.ct ".
:set symbol="arrow"   value=";.sf7 ~L;.esf;".

:set symbol="ver"       value="2".
:set symbol="rev"       value="0".
:set symbol="pver"      value="10".
:set symbol="prev"      value="6".
:set symbol="version"   value="200".
:set symbol="vermacro"  value="2000".
:set symbol="minram"    value="8 MB".
:set symbol="mfc16"     value="MFC 2.52b".
:set symbol="mfc32"     value="MFC 4.1".
:set symbol="alpha"     value="noAXP".
:set symbol="cmpcname"  value="&company C".
:set symbol="cmppname"  value="&company C++".
:set symbol="cmpfname"  value="&company FORTRAN 77".
.*
:set symbol="watpathup" value="WATCOM".
.*
:set symbol="pathnam"   value="\watcom".
:set symbol="pathnamup" value="\WATCOM".
:set symbol="sw"        value="/".
:set symbol="pc"        value="\".
:set symbol="ps"        value="~;".
:set symbol="psword"    value="semicolon".
.*
.* Powersoft Power++ Specific
.*
:set symbol="powerpp"   value="Power++".
.*
.* Open Watcom Debugger Specific
.*
:set symbol="dbgname"   value="&company Debugger".
:set symbol="dbgvar"    value="wd".
:set symbol="dbgvarup"  value="WD".
:set symbol="dbgcmd"    value="wd".
:set symbol="dbgcmdup"  value="WD".
:set symbol="dbgsuff"   value="dbg".
:set symbol="dbgsuffup" value="DBG".
.*
.* Open Watcom Linker/Librarian/Strip Specific
.*
:set symbol="lnkname"   value="&company Linker".
:set symbol="lnkcmd"    value="wlink".
:set symbol="overlay"   value="yes".
:set symbol="m2wcmd"    value="ms2wlink".
:set symbol="fcename"   value="&company Far Call Optimization Enabling Utility"
:set symbol="fcecmd"    value="fcenable".
:set symbol="libname"   value="&company Library Manager".
:set symbol="libcmd"    value="wlib".
:set symbol="stripname" value="&company Strip Utility".
:set symbol="stripcmd"  value="wstrip".
:set symbol="e2bname"   value="&company Exe2bin".
:set symbol="e2bcmd"    value="exe2bin".
:set symbol="e2bcmdu"   value="Exe2bin".
:set symbol="lnkcmdup"  value="WLINK".
:set symbol="m2wcmdup"  value="MS2WLINK".
:set symbol="fcecmdup"  value="FCENABLE".
:set symbol="libcmdup"  value="WLIB".
:set symbol="stripcmdup" value="WSTRIP".
:set symbol="e2bcmdup"  value="EXE2BIN".
:set symbol="libvar"    value="lib".
:set symbol="pathvar"   value="watcom".
:set symbol="libvarup"  value="LIB".
:set symbol="pathvarup" value="WATCOM".
.*
.* Open Watcom Assembler Specific
.*
:set symbol="asmname"   value="&company Assembler".
:set symbol="asmcmd"    value="wasm".
:set symbol="asmcmdup"  value="WASM".
:set symbol="wasname"   value="&company AXP Assembler".
:set symbol="wascmd"    value="wasaxp".
:set symbol="wascmdup"  value="WASAXP".
.*
.* Open Watcom Disassembler Specific
.*
:set symbol="disname"   value="&company Disassembler".
:set symbol="discmd"    value="wdis".
:set symbol="discmdup"  value="wdis".
:set symbol="discmdup"  value="WDIS".
.*
.* Open Watcom Patch Utility Specific
.*
:set symbol="patchname" value="&company Patch Utility".
:set symbol="patchcmd"  value="bpatch".
:set symbol="patchcmdup" value="BPATCH".
.*
.* Open Watcom Make and Touch Specific and IDE2MAKE
.*
:set symbol="makname"   value="&company Make".
:set symbol="maksname"  value="Make".
:set symbol="makcmd"    value="wmake".
:set symbol="makcmdup"  value="WMAKE".
:set symbol="tchname"   value="&company Touch".
:set symbol="tchcmd"    value="wtouch".
:set symbol="tchcmdup"  value="WTOUCH".
:set symbol="id2mkname" value="IDE2MAKE"
:set symbol="id2mkcmd"  value="ide2make"
.*
.* Open Watcom Profiler Specific
.*
:set symbol="prfname"   value="&company Execution Profiler".
:set symbol="prfcmd"    value="wprof".
:set symbol="smpname"   value="&company Execution Sampler".
:set symbol="smpcmd"    value="wsample".
:set symbol="prfcmdup"  value="WPROF".
:set symbol="smpcmdup"  value="WSAMPLE".
.*
.* Open Watcom Resource Compiler Specific
.*
:set symbol="wrcname"   value="&company Resource Compiler".
:set symbol="wrccmd"    value="wrc".
:set symbol="wrccmdup"  value="WRC".
.*
.* GUI Tools
.*
:set symbol="drwc"      value="Dr. Watcom".
:set symbol="dr95"      value="Dr. Watcom".
:set symbol="drnt"      value="Dr. Watcom".
:set symbol="spy"       value="Spy".
:set symbol="dde"       value="DDE Spy".
:set symbol="edname"    value="&company Editor".
:set symbol="editor"    value="Editor".
:set symbol="ide"       value="Integrated Development Environment".
:set symbol="wide"      value="&company &ide".
:set symbol="vip"       value="IDE".
:set symbol="vip_cfg"   value="ide.cfg".
:set symbol="dbgname"   value="&company Debugger".
:set symbol="dbgcmd"    value="wd".
:set symbol="dbgcmdup"  value="WD".
:set symbol="brname"    value="&company Browser".
:set symbol="br"        value="Browser".
:set symbol="watc"      value="&company C/C++".
:set symbol="watf"      value="&company FORTRAN 77".
.* following are used in "Getting Started"
:set symbol="watc16"    value="&company C/C++&S'16.".
:set symbol="watc32"    value="&company C/C++&S'32.".
:set symbol="watf16"    value="&company FORTRAN 77&S'16.".
:set symbol="watf32"    value="&company FORTRAN 77&S'32.".
.*
.* Blue Sky Visual Programmer
.*
:set symbol="vpname"    value="Visual Programmer".
.*
.* AmerEnglish spelling
.*
.sr colour='color'
.sr ccolour='Color'

.   ..sr wc286="&company. C&S'16."
.   ..sr wc386="&company. C&S'32."
.   ..sr wcboth="&company. C&S'16. and C&S'32."
.   ..sr c286="C&S'16."
.   ..sr c386="C&S'32."
.   ..sr wcall="&company. C&S'16., C&S'32. and Waterloo C"
..sr wcgeneric="&company. C"
..sr cboth="C"
..sr wlooc="Waterloo C"
..sr target='PC'
..sr targetQNX=0
:SET symbol='userguide' value="User's Guide".
:SET symbol='pragma'    value="User's Guide".
:SET symbol='libref'    value="&company C Library Reference manual".
:SET symbol='linkref'   value="&lnkname User's Guide".
:SET symbol='loadgo'    value="&company Express C".
:SET symbol='portable'  value="Writing Portable Programs".
:SET symbol='basedptr'  value="Based Pointers for &wcboth.".
:SET symbol='ptr86'     value="Special Pointer Types for &wc286.".
:SET symbol='ptr386'    value="Special Pointer Types for &wc386.".
:SET symbol='hugekw'    value="The &kwhuge_sp. Keyword".
:SET symbol='appkw'     value="Compiler Keywords".
:SET symbol='struct'    value="Structures".
:SET symbol='inttoflt'  value="Integer to Floating-Point Conversion".
:SET symbol='numlimit'  value="Macros for Numerical Limits".
:SET symbol='exdecl'    value="Examples of Declarations".
:SET symbol='charset'   value="Character Sets".
:SET symbol='initobj'   value="Initialization of Objects".
.*
..sr tabchar=$
.*

