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


#include "rcdef.h"
#ifdef DEFINE_ICONS
#define DEFICON( x, y, z ) gui_resource x = { y, z };
#else
#define DEFICON( x, y, z ) extern gui_resource x;
#endif

DEFICON( MainIcon,      ICON_MAIN,      "" )
DEFICON( AsmIcon,       ICON_ASM,       "Asm" )
DEFICON( BrkIcon,       ICON_BRK,       "Brk" )
DEFICON( CallIcon,      ICON_CALL,      "Call" )
DEFICON( FPUIcon,       ICON_FPU,       "FPU" )
DEFICON( MMXIcon,       ICON_REG,       "MMX" )
DEFICON( XMMIcon,       ICON_REG,       "XMM" )
DEFICON( FuncIcon,      ICON_FUNC,      "Func" )
DEFICON( GlobIcon,      ICON_GLOB,      "Glob" )
DEFICON( AclIcon,       ICON_ACL,       "Accl" )
DEFICON( ImgIcon,       ICON_EXE,       "EXE" )
DEFICON( IOIcon,        ICON_IO,        "IO" )
DEFICON( LogIcon,       ICON_LOG,       "Log" )
DEFICON( MemIcon,       ICON_MEM,       "Mem" )
DEFICON( ModIcon,       ICON_MOD,       "Mod" )
DEFICON( RegIcon,       ICON_REG,       "Reg" )
DEFICON( RepIcon,       ICON_REPL,      "Repl" )
DEFICON( SrcIcon,       ICON_SRC,       "Src" )
DEFICON( SrchIcon,      ICON_SRCH,      "Srch" )
DEFICON( StkIcon,       ICON_MEM,       "Stk" )
DEFICON( TrdIcon,       ICON_THRD,      "Thrd" )
DEFICON( VarIcon,       ICON_VAR,       "Var" )
DEFICON( LocIcon,       ICON_LOC,       "Loc" )
DEFICON( WatIcon,       ICON_WAT,       "Wtch" )
#undef DEFICON
#undef DEFINE_ICONS
