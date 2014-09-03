/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  tokens recognized by directives
*
****************************************************************************/

// Strings recognized by directives

/*     token              string          value           init_val       */
pick( TOK_READONLY,      "READONLY",     0,              0               ),
pick( TOK_BYTE,          "BYTE",         ALIGN_BYTE,     INIT_ALIGN      ),
pick( TOK_WORD,          "WORD",         ALIGN_WORD,     INIT_ALIGN      ),
pick( TOK_DWORD,         "DWORD",        ALIGN_DWORD,    INIT_ALIGN      ),
pick( TOK_PARA,          "PARA",         ALIGN_PARA,     INIT_ALIGN      ),
pick( TOK_PAGE,          "PAGE",         ALIGN_PAGE,     INIT_ALIGN      ),
pick( TOK_PRIVATE,       "PRIVATE",      COMB_INVALID,   INIT_COMBINE    ),
pick( TOK_PUBLIC,        "PUBLIC",       COMB_ADDOFF,    INIT_COMBINE    ),
pick( TOK_STACK,         "STACK",        COMB_STACK,     INIT_COMBINE    ),
pick( TOK_COMMON,        "COMMON",       COMB_COMMON,    INIT_COMBINE    ),
pick( TOK_MEMORY,        "MEMORY",       COMB_ADDOFF,    INIT_COMBINE    ),
pick( TOK_USE16,         "USE16",        false,          INIT_USE        ),
pick( TOK_USE32,         "USE32",        true,           INIT_USE        ),
pick( TOK_IGNORE,        "IGNORE",       0,              0               ),
pick( TOK_AT,            "AT",           0,              0               ),
pick( TOK_CLASS,         NULL,           0,              INIT_CLASS      ),
pick( TOK_TINY,          "TINY",         MOD_TINY,       INIT_MEMORY     ),
pick( TOK_SMALL,         "SMALL",        MOD_SMALL,      INIT_MEMORY     ),
pick( TOK_COMPACT,       "COMPACT",      MOD_COMPACT,    INIT_MEMORY     ),
pick( TOK_MEDIUM,        "MEDIUM",       MOD_MEDIUM,     INIT_MEMORY     ),
pick( TOK_LARGE,         "LARGE",        MOD_LARGE,      INIT_MEMORY     ),
pick( TOK_HUGE,          "HUGE",         MOD_HUGE,       INIT_MEMORY     ),
pick( TOK_FLAT,          "FLAT",         MOD_FLAT,       INIT_MEMORY     ),
pick( TOK_NEARSTACK,     "NEARSTACK",    STACK_NEAR,     INIT_STACK      ),
pick( TOK_FARSTACK,      "FARSTACK",     STACK_FAR,      INIT_STACK      ),
pick( TOK_EXT_NEAR,      "NEAR",         MT_NEAR,        0               ),
pick( TOK_EXT_FAR,       "FAR",          MT_FAR,         0               ),
pick( TOK_EXT_PROC,      "PROC",         MT_PROC,        0               ),
pick( TOK_EXT_BYTE,      "BYTE",         MT_BYTE,        0               ),
pick( TOK_EXT_SBYTE,     "SBYTE",        MT_BYTE,        0               ),
pick( TOK_EXT_WORD,      "WORD",         MT_WORD,        0               ),
pick( TOK_EXT_SWORD,     "SWORD",        MT_WORD,        0               ),
pick( TOK_EXT_PTR,       "PTR",          MT_WORD,        0               ),
pick( TOK_EXT_DWORD,     "DWORD",        MT_DWORD,       0               ),
pick( TOK_EXT_SDWORD,    "SDWORD",       MT_DWORD,       0               ),
pick( TOK_EXT_PWORD,     "PWORD",        MT_FWORD,       0               ),
pick( TOK_EXT_FWORD,     "FWORD",        MT_FWORD,       0               ),
pick( TOK_EXT_QWORD,     "QWORD",        MT_QWORD,       0               ),
pick( TOK_EXT_TBYTE,     "TBYTE",        MT_TBYTE,       0               ),
pick( TOK_EXT_OWORD,     "OWORD",        MT_OWORD,       0               ),
pick( TOK_EXT_ABS,       "ABS",          MT_ABS,         0               ),
pick( TOK_DS,            "DS",           ASSUME_DS,      0               ),
pick( TOK_ES,            "ES",           ASSUME_ES,      0               ),
pick( TOK_SS,            "SS",           ASSUME_SS,      0               ),
pick( TOK_FS,            "FS",           ASSUME_FS,      0               ),
pick( TOK_GS,            "GS",           ASSUME_GS,      0               ),
pick( TOK_CS,            "CS",           ASSUME_CS,      0               ),
pick( TOK_ERROR,         "ERROR",        ASSUME_ERROR,   0               ),
pick( TOK_NOTHING,       "NOTHING",      ASSUME_NOTHING, 0               ),
pick( TOK_PROC_FAR,      "FAR",          MT_FAR,         0               ),
pick( TOK_PROC_NEAR,     "NEAR",         MT_NEAR,        0               ),
pick( TOK_LANG_NOLANG,   "NOLANGUAGE",   LANG_NONE,      0               ),
pick( TOK_LANG_BASIC,    "BASIC",        LANG_BASIC,     0               ),
pick( TOK_LANG_FORTRAN,  "FORTRAN",      LANG_FORTRAN,   0               ),
pick( TOK_LANG_PASCAL,   "PASCAL",       LANG_PASCAL,    0               ),
pick( TOK_LANG_C,        "C",            LANG_C,         0               ),
pick( TOK_LANG_WATCOM_C, "WATCOM_C",     LANG_WATCOM_C,  0               ),
pick( TOK_LANG_STDCALL,  "STDCALL",      LANG_STDCALL,   0               ),
pick( TOK_LANG_SYSCALL,  "SYSCALL",      LANG_SYSCALL,   0               ),
pick( TOK_PROC_PRIVATE,  "PRIVATE",      VIS_PRIVATE,    0               ),
pick( TOK_PROC_PUBLIC,   "PUBLIC",       VIS_PUBLIC,     0               ),
pick( TOK_PROC_EXPORT,   "EXPORT",       VIS_EXPORT,     0               ),
pick( TOK_PROC_USES,     "USES",         0,              0               ),
pick( TOK_PROC_VARARG,   "VARARG",       0,              0               ),
pick( TOK_OS_OS2,        "OS_OS2",       OPSYS_OS2,      0               ),
pick( TOK_OS_DOS,        "OS_DOS",       OPSYS_DOS,      0               ),
