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
* Description:  define special functions
*
****************************************************************************/


    SPEC_FUN( NONE      ,""             )
,   SPEC_FUN( MAIN      ,"main"         )
,   SPEC_FUN( WINMAIN   ,"WinMain"      )
,   SPEC_FUN( LIBMAIN   ,"LibMain"      )
,   SPEC_FUN( DLLMAIN   ,"DllMain"      )
,   SPEC_FUN( LONGJMP   ,"longjmp"      )
,   SPEC_FUN( SETJMP    ,"_setjmp"      )
#if _CPU == _AXP
,   SPEC_FUN( SETJMP1   ,"_setjmpex"    )
,   SPEC_FUN( SETJMP2   ,"_setjmpexRfp" )
,   SPEC_FUN( SETJMP3   ,"setjmp"       )       // temporary
#endif
,   SPEC_FUN( wMAIN     ,"wmain"        )
,   SPEC_FUN( wWINMAIN  ,"wWinMain"     )
,   SPEC_FUN( wLIBMAIN  ,"wLibMain"     )
,   SPEC_FUN( wDLLMAIN  ,"wDllMain"     )
