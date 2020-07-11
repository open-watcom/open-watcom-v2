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

pick( CMD_ACCEL,        ProcAccel,      "Accelerate" )
pick( CMD_BREAK,        ProcBreak,      "Break" )
pick( CMD_CALL,         ProcCall,       "Call" )
pick( CMD_CAPTURE,      ProcCapture,    "CAPture" )
pick( CMD_DISPLAY,      ProcDisplay,    "Display" )
pick( CMD_DO,           ProcDo,         "DO" )
pick( CMD_ERROR,        ProcError,      "ERror" )
pick( CMD_EXAMINE,      ProcExamine,    "Examine" )
pick( CMD_FLIP,         ProcFlip,       "Flip" )
pick( CMD_GO,           ProcGo,         "Go" )
pick( CMD_HELP,         ProcHelp,       "Help" )
pick( CMD_HOOK,         ProcHook,       "HOok" )
pick( CMD_IF,           ProcIf,         "IF" )
pick( CMD_INVOKE,       ProcInvoke,     "INvoke" )
pick( CMD_LOG,          ProcLog,        "Log" )
pick( CMD_NEW,          ProcNew,        "NEW" )
pick( CMD_PAINT,        ProcPaint,      "PAint" )
pick( CMD_FONT,         ProcFont,       "FOnt" )
pick( CMD_PRINT,        ProcPrint,      "Print" )
pick( CMD_QUIT,         ProcQuit,       "Quit" )
pick( CMD_REGISTER,     ProcRegister,   "Register" )
pick( CMD_REMARK,       ProcRemark,     "REMark" )
pick( CMD_SET,          ProcSet,        "Set" )
pick( CMD_SHOW,         ProcShow,       "SHow" )
pick( CMD_SYSTEM,       ProcSystem,     "SYstem" )
pick( CMD_THREAD,       ProcThread,     "THread" )
pick( CMD_TRACE,        ProcTrace,      "Trace" )
pick( CMD_VIEW,         ProcView,       "View" )
pick( CMD_WHILE,        ProcWhile,      "While" )
pick( CMD_WINDOW,       ProcWindow,     "WIndow" )
pick( CMD_CONFIGFILE,   ProcConfigFile, "COnfigfile" )
pick( CMD_STACKPOS,     ProcStackPos,   "STackpos" )
pick( CMD_UNDO,         ProcUndo,       "Undo" )
pick( CMD_SKIP,         ProcSkip,       "SKip" )
pick( CMD_RECORD,       ProcRecord,     "RECord" )
pick( CMD_ASSIGN,       ProcAssign,     "ASsign" )
pick( CMD_MODIFY,       ProcModify,     "MOdify" )
#ifndef NDEBUG
pick( CMD_INTERNAL,     ProcInternal,   "XX" )
#endif
