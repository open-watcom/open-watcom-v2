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


#define MAX_CMD_NAME    12
pick(   CMD_ACCEL,      ProcAccel,       "Accelerate\0" )
pick(   CMD_BREAK,      ProcBreak,       "Break\0" )
pick(   CMD_CALL,       ProcCall,        "Call\0" )
pick(   CMD_CAPTURE,    ProcCapture,     "CAPture\0" )
pick(   CMD_DISPLAY,    ProcDisplay,     "Display\0" )
pick(   CMD_DO,         ProcDo,          "DO\0" )
pick(   CMD_ERROR,      ProcError,       "ERror\0" )
pick(   CMD_EXAMINE,    ProcExamine,     "Examine\0" )
pick(   CMD_FLIP,       ProcFlip,        "Flip\0" )
pick(   CMD_GO,         ProcGo,          "Go\0" )
pick(   CMD_HELP,       ProcHelp,        "Help\0" )
pick(   CMD_HOOK,       ProcHook,        "HOok\0" )
pick(   CMD_IF,         ProcIf,          "IF\0" )
pick(   CMD_INVOKE,     ProcInvoke,      "INvoke\0" )
pick(   CMD_LOG,        ProcLog,         "Log\0" )
pick(   CMD_NEW,        ProcNew,         "NEW\0" )
pick(   CMD_PAINT,      ProcPaint,       "PAint\0" )
pick(   CMD_FONT,       ProcFont,        "FOnt\0" )
pick(   CMD_PRINT,      ProcPrint,       "Print\0" )
pick(   CMD_QUIT,       ProcQuit,        "Quit\0" )
pick(   CMD_REGISTER,   ProcRegister,    "Register\0" )
pick(   CMD_REMARK,     ProcRemark,      "REMark\0" )
pick(   CMD_SET,        ProcSet,         "Set\0" )
pick(   CMD_SHOW,       ProcShow,        "SHow\0" )
pick(   CMD_SYSTEM,     ProcSystem,      "SYstem\0" )
pick(   CMD_THREAD,     ProcThread,      "THread\0" )
pick(   CMD_TRACE,      ProcTrace,       "Trace\0" )
pick(   CMD_VIEW,       ProcView,        "View\0" )
pick(   CMD_WHILE,      ProcWhile,       "While\0" )
pick(   CMD_WINDOW,     ProcWindow,      "WIndow\0" )
pick(   CMD_CONFIGFILE, ProcConfigFile,  "COnfigfile\0" )
pick(   CMD_STACKPOS,   ProcStackPos,    "STackpos\0" )
pick(   CMD_UNDO,       ProcUndo,        "Undo\0" )
pick(   CMD_SKIP,       ProcSkip,        "SKip\0" )
pick(   CMD_RECORD,     ProcRecord,      "RECord\0" )
pick(   CMD_ASSIGN,     ProcAssign,      "ASsign\0" )
pick(   CMD_MODIFY,     ProcModify,      "MOdify\0" )
#ifdef DBG_DBG
pick(   CMD_INTERNAL,   ProcInternal,    "XX\0" )
#endif
#undef pick
