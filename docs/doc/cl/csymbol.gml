:CMT.
:CMT.*********************** Keywords ***********************************
:CMT.
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
