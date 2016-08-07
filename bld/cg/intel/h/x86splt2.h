/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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


extern instruction      *rSPLITPUSH( instruction *ins );
extern instruction      *rMAKEU2( instruction *ins );
extern instruction      *rLOADLONGADDR( instruction *ins );
extern instruction      *rHIGHCMP( instruction *ins );
extern instruction      *rMAKEU4( instruction *ins );
extern instruction      *rCLRHI_D( instruction *ins );
extern instruction      *rEXT_PUSH1( instruction *ins );
extern instruction      *rEXT_PUSH2( instruction *ins );
extern instruction      *rINTCOMP( instruction *ins );
extern instruction      *rCDQ( instruction *ins );
extern instruction      *rCONVERT_UP( instruction *ins );
extern instruction      *rCYP_SEX( instruction *ins );
extern instruction      *rSPLIT8( instruction *ins );
extern instruction      *rSPLIT8BIN( instruction *ins );
extern instruction      *rSPLIT8NEG( instruction *ins );
extern instruction      *rSPLIT8TST( instruction *ins );
extern instruction      *rSPLIT8CMP( instruction *ins );
#if 0
extern instruction      *rCLRHIGH_DW( instruction *ins );
extern instruction      *rSEX_DW( instruction *ins );
#endif
extern instruction      *rCYPSHIFT( instruction *ins );
extern instruction      *rBYTESHIFT( instruction *ins );
extern instruction      *rMOVE8LOW( instruction *ins );
