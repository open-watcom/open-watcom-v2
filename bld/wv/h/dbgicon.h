/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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


#define DEFICONS \
pick( MainIcon,      ICON_MAIN,      "" ) \
pick( AsmIcon,       ICON_ASM,       "Asm" ) \
pick( BrkIcon,       ICON_BRK,       "Brk" ) \
pick( CallIcon,      ICON_CALL,      "Call" ) \
pick( FPUIcon,       ICON_FPU,       "FPU" ) \
pick( MMXIcon,       ICON_REG,       "MMX" ) \
pick( XMMIcon,       ICON_REG,       "XMM" ) \
pick( FuncIcon,      ICON_FUNC,      "Func" ) \
pick( GlobIcon,      ICON_GLOB,      "Glob" ) \
pick( AclIcon,       ICON_ACL,       "Accl" ) \
pick( ImgIcon,       ICON_EXE,       "EXE" ) \
pick( IOIcon,        ICON_IO,        "IO" ) \
pick( LogIcon,       ICON_LOG,       "Log" ) \
pick( MemIcon,       ICON_MEM,       "Mem" ) \
pick( ModIcon,       ICON_MOD,       "Mod" ) \
pick( RegIcon,       ICON_REG,       "Reg" ) \
pick( RepIcon,       ICON_REPL,      "Repl" ) \
pick( SrcIcon,       ICON_SRC,       "Src" ) \
pick( SrchIcon,      ICON_SRCH,      "Srch" ) \
pick( StkIcon,       ICON_MEM,       "Stk" ) \
pick( TrdIcon,       ICON_THRD,      "Thrd" ) \
pick( VarIcon,       ICON_VAR,       "Var" ) \
pick( LocIcon,       ICON_LOC,       "Loc" ) \
pick( WatIcon,       ICON_WAT,       "Wtch" )

#define pick( x, y, z ) extern gui_resource x;
    DEFICONS
#undef pick
