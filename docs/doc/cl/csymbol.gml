:CMT.
:CMT.*********************** Keywords ***********************************
:CMT.
:SET symbol='kwasm'         value='__asm'.
:CMT.
:SET symbol='kwbased'       value='__based'.
:CMT.
:SET symbol='kwbldiflt'     value='__builtin_isfloat'.
:CMT.
:SET symbol='kwicdecl'      value='_Cdecl'.
:CMT.
:SET symbol='kwcdecl'       value='__cdecl'.
:CMT.
:SET symbol='kwdeclspec'    value='__declspec'.
:CMT.
:SET symbol='kwiexcept'     value='_Except'.
:CMT.
:SET symbol='kwexcept'      value='__except'.
:CMT.
:SET symbol='kwiexport'     value='_Export'.
:CMT.
:SET symbol='kwexport'      value='__export'.
:CMT.
:SET symbol='kwfar'         value='__far'.
:CMT.
:SET symbol='kwifar16'      value='_Far16'.
:CMT.
:SET symbol='kwfar16'       value='__far16'.
:CMT.
:SET symbol='kwifastcal'    value='_Fastcall'.
:CMT.
:SET symbol='kwfastcall'    value='__fastcall'.
:CMT.
:SET symbol='kwifinally'    value='_Finally'.
:CMT.
:SET symbol='kwfinally'     value='__finally'.
:CMT.
:SET symbol='kwfortran'     value='__fortran'.
:CMT.
:SET symbol='kwhuge'        value='__huge'.
:CMT.
:SET symbol='kwinline'      value='__inline'.
:CMT.
:SET symbol='kwint64'       value='__int64'.
:CMT.
:SET symbol='kwintrpt'      value='__interrupt'.
:CMT.
:SET symbol='kwileave'      value='_Leave'.
:CMT.
:SET symbol='kwleave'       value='__leave'.
:CMT.
:SET symbol='kwloadds'      value='__loadds'.
:CMT.
:SET symbol='kwnear'        value='__near'.
:CMT.
:SET symbol='kwimagunit'    value='__ow_imaginary_unit'.
:CMT.
:SET symbol='kwipacked'     value='_Packed'.
:CMT.
:SET symbol='kwipascal'     value='_Pascal'.
:CMT.
:SET symbol='kwpascal'      value='__pascal'.
:CMT.
:SET symbol='kwsaveregs'    value='__saveregs'.
:CMT.
:SET symbol='kwsegment'     value='__segment'.
:CMT.
:SET symbol='kwsegname'     value='__segname'.
:CMT.
:SET symbol='kwiseg16'      value='_Seg16'.
:CMT.
:SET symbol='kwself'        value='__self'.
:CMT.
:SET symbol='kwisyscall'    value='_Syscall'.
:CMT.
:SET symbol='kwsyscall'     value='__syscall'.
:CMT.
:SET symbol='kwstdcall'     value='__stdcall'.
:CMT.
:SET symbol='kwisystem'     value='_System'.
:CMT.
:SET symbol='kwitry'        value='_Try'.
:CMT.
:SET symbol='kwtry'         value='__try'.
:CMT.
:SET symbol='kwunalign'     value='__unaligned'.
:CMT.
:SET symbol='kwwatcall'     value='__watcall'.
:CMT.
:CMT. ********************** Macros *************************************
:CMT.
:SET symbol='mkwCSGN'       value='__CHAR_SIGNED__'.
:CMT.
:SET symbol='mkwCHPW'       value='__CHEAP_WINDOWS__'.
:CMT.
:SET symbol='mkwCOMPACT'    value='__COMPACT__'.
:CMT.
:SET symbol='mkwDATE'       value='__DATE__'.
:CMT.
:SET symbol='mkwDOS'        value='__DOS__'.
:CMT.
:SET symbol='mkwEXPRESS'    value='__EXPRESSC__'.
:CMT.
:SET symbol='mkwFILE'       value='__FILE__'.
:CMT.
:SET symbol='mkwFLAT'       value='__FLAT__'.
:CMT.
:SET symbol='mkwFPI'        value='__FPI__'.
:CMT.
:SET symbol='mkwFNC'        value='__func__'.
:CMT.
:SET symbol='mkwFUNC'       value='__FUNCTION__'.
:CMT.
:SET symbol='mkwHUGE'       value='__HUGE__'.
:CMT.
:SET symbol='mkwINLN'       value='__INLINE_FUNCTIONS__'.
:CMT.
:SET symbol='mkwLARGE'      value='__LARGE__'.
:CMT.
:SET symbol='mkwLINE'       value='__LINE__'.
:CMT.
:SET symbol='mkwLINUX'      value='__LINUX__'.
:CMT.
:SET symbol='mkwMEDIUM'     value='__MEDIUM__'.
:CMT.
:SET symbol='mkwNULLSEG'    value='_NULLSEG'.
:CMT.
:SET symbol='mkwNULLOFF'    value='_NULLOFF'.
:CMT.
:SET symbol='mkwNET'        value='__NETWARE_386__'.
:CMT.
:SET symbol='mkwNT'         value='__NT__'.
:CMT.
:SET symbol='mkwOS2'        value='__OS2__'.
:CMT.
:SET symbol='mkwQNX'        value='__QNX__'.
:CMT.
:SET symbol='mkwSMALL'      value='__SMALL__'.
:CMT.
:SET symbol='mkwSTDC'       value='__STDC__'.
:CMT.
:SET symbol='mkwSTDCH'      value='__STDC_HOSTED__'.
:CMT.
:SET symbol='mkwSTDCX'      value='__STDC_LIB_EXT1__'.
:CMT.
:SET symbol='mkwSTDCV'      value='__STDC_VERSION__'.
:CMT.
:SET symbol='mkwTIME'       value='__TIME__'.
:CMT.
:SET symbol='mkwWATCOMC'    value='__WATCOMC__'.
:CMT.
:SET symbol='mkwWIN_386'    value='__WINDOWS_386__'.
:CMT.
:SET symbol='mkwWINDOWS'    value='__WINDOWS__'.
:CMT.
:SET symbol='mkw386'        value='__386__'.
:CMT.
:CMT. ******* Microsoft macros with leading underscores **************
:CMT.
:SET symbol='mkwM_IX86'     value='_M_IX86'.
:CMT.
:CMT. ******* Microsoft macros without leading underscores **************
:CMT.
:SET symbol='mkwM_I386'     value='M_I386'.
:CMT.
:SET symbol='mkwM_I86'      value='M_I86'.
:CMT.
:SET symbol='mkwM_I86CM'    value='M_I86CM'.
:CMT.
:SET symbol='mkwM_I86HM'    value='M_I86HM'.
:CMT.
:SET symbol='mkwM_I86LM'    value='M_I86LM'.
:CMT.
:SET symbol='mkwM_I86MM'    value='M_I86MM'.
:CMT.
:SET symbol='mkwM_I86SM'    value='M_I86SM'.
:CMT.
:SET symbol='mkwMSDOS'      value='MSDOS'.
:CMT.
:SET symbol='mkwNKEY'       value='NO_EXT_KEYS'.
