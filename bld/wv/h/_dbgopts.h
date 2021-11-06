/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


pick( "Invoke",                  OPT_INVOKE                     )
pick( "NOInvoke",                OPT_NOINVOKE                   )
pick( "NOSYmbols",               OPT_NOSYMBOLS                  )
pick( "NOMouse",                 OPT_NOMOUSE                    )
pick( "DIp",                     OPT_DIP                        )
pick( "DYnamic",                 OPT_DYNAMIC                    )
pick( "TRap",                    OPT_TRAP                       )
pick( "REMotefiles",             OPT_REMOTE_FILES               )
#ifdef BACKWARDS
pick( "NOFpu",                   OPT_NO_FPU                     )
#endif
pick( "LInes",                   OPT_LINES                      )
pick( "COlumns",                 OPT_COLUMNS                    )
#ifdef BACKWARDS
pick( "NOAltsym",                OPT_NO_ALTSYM                  )
pick( "REGisters",               OPT_REGISTERS                  )
#endif
pick( "INItcmd",                 OPT_INITCMD                    )
pick( "POWerbuilder",            OPT_POWERBUILDER               )
pick( "LOcalinfo",               OPT_LOCALINFO                  )
pick( "NOExports",               OPT_NOEXPORTS                  )
pick( "DOwnload",                OPT_DOWNLOAD                   )
pick( "DEfersymbols",            OPT_DEFERSYM                   )
pick( "NOSOurcecheck",           OPT_NOSOURCECHECK              )
pick( "CONtinueunexpectedbreak", OPT_CONTINUE_UNEXPECTED_BREAK  )
pick( "Help",                    OPT_HELP                       )
#ifdef ENABLE_TRAP_LOGGING
pick( "TDebug",                  OPT_TRAP_DEBUG                 )
pick( "TFDebug",                 OPT_TRAP_DEBUG_FLUSH           )
#endif
