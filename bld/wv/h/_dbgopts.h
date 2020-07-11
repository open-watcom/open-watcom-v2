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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


pick( OPT_INVOKE,                   "Invoke" )
pick( OPT_NOINVOKE,                 "NOInvoke" )
pick( OPT_NOSYMBOLS,                "NOSYmbols" )
pick( OPT_NOMOUSE,                  "NOMouse" )
pick( OPT_DIP,                      "DIp" )
pick( OPT_DYNAMIC,                  "DYnamic" )
pick( OPT_TRAP,                     "TRap" )
pick( OPT_REMOTE_FILES,             "REMotefiles" )
#ifdef BACKWARDS
pick( OPT_NO_FPU,                   "NOFpu" )
#endif
pick( OPT_LINES,                    "LInes" )
pick( OPT_COLUMNS,                  "COlumns" )
#ifdef BACKWARDS
pick( OPT_NO_ALTSYM,                "NOAltsym" )
pick( OPT_REGISTERS,                "REGisters" )
#endif
pick( OPT_INITCMD,                  "INItcmd" )
pick( OPT_POWERBUILDER,             "POWerbuilder" )
pick( OPT_LOCALINFO,                "LOcalinfo" )
pick( OPT_NOEXPORTS,                "NOExports" )
pick( OPT_DOWNLOAD,                 "DOwnload" )
pick( OPT_DEFERSYM,                 "DEfersymbols" )
pick( OPT_NOSOURCECHECK,            "NOSOurcecheck" )
pick( OPT_CONTINUE_UNEXPECTED_BREAK, "CONtinueunexpectedbreak" )
pick( OPT_HELP,                     "Help" )
#ifdef ENABLE_TRAP_LOGGING
pick( OPT_TRAP_DEBUG,               "TDebug" )
pick( OPT_TRAP_DEBUG_FLUSH,         "TFDebug" )
#endif
