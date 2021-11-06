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


#define MAX_CMD_NAME    12

pick( "Accelerate", CMD_ACCEL,      ProcAccel       )
pick( "Break",      CMD_BREAK,      ProcBreak       )
pick( "Call",       CMD_CALL,       ProcCall        )
pick( "CAPture",    CMD_CAPTURE,    ProcCapture     )
pick( "Display",    CMD_DISPLAY,    ProcDisplay     )
pick( "DO",         CMD_DO,         ProcDo          )
pick( "ERror",      CMD_ERROR,      ProcError       )
pick( "Examine",    CMD_EXAMINE,    ProcExamine     )
pick( "Flip",       CMD_FLIP,       ProcFlip        )
pick( "Go",         CMD_GO,         ProcGo          )
pick( "Help",       CMD_HELP,       ProcHelp        )
pick( "HOok",       CMD_HOOK,       ProcHook        )
pick( "IF",         CMD_IF,         ProcIf          )
pick( "INvoke",     CMD_INVOKE,     ProcInvoke      )
pick( "Log",        CMD_LOG,        ProcLog         )
pick( "NEW",        CMD_NEW,        ProcNew         )
pick( "PAint",      CMD_PAINT,      ProcPaint       )
pick( "FOnt",       CMD_FONT,       ProcFont        )
pick( "Print",      CMD_PRINT,      ProcPrint       )
pick( "Quit",       CMD_QUIT,       ProcQuit        )
pick( "Register",   CMD_REGISTER,   ProcRegister    )
pick( "REMark",     CMD_REMARK,     ProcRemark      )
pick( "Set",        CMD_SET,        ProcSet         )
pick( "SHow",       CMD_SHOW,       ProcShow        )
pick( "SYstem",     CMD_SYSTEM,     ProcSystem      )
pick( "THread",     CMD_THREAD,     ProcThread      )
pick( "Trace",      CMD_TRACE,      ProcTrace       )
pick( "View",       CMD_VIEW,       ProcView        )
pick( "While",      CMD_WHILE,      ProcWhile       )
pick( "WIndow",     CMD_WINDOW,     ProcWindow      )
pick( "COnfigfile", CMD_CONFIGFILE, ProcConfigFile  )
pick( "STackpos",   CMD_STACKPOS,   ProcStackPos    )
pick( "Undo",       CMD_UNDO,       ProcUndo        )
pick( "SKip",       CMD_SKIP,       ProcSkip        )
pick( "RECord",     CMD_RECORD,     ProcRecord      )
pick( "ASsign",     CMD_ASSIGN,     ProcAssign      )
pick( "MOdify",     CMD_MODIFY,     ProcModify      )
#ifndef NDEBUG
pick( "XX",         CMD_INTERNAL,   ProcInternal    )
#endif
