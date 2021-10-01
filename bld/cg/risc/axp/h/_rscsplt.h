/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  AXP specific reduce function prototype declarations
*
****************************************************************************/


extern instruction      *rLOAD_1( instruction * );
extern instruction      *rSTORE_1( instruction * );
extern instruction      *rLOAD_2( instruction * );
extern instruction      *rLOAD_2U( instruction * );
extern instruction      *rSTORE_2( instruction * );
extern instruction      *rSTORE_2U( instruction * );
extern instruction      *rLOAD_4U( instruction * );
extern instruction      *rSTORE_4U( instruction * );
extern instruction      *rLOAD_8U( instruction * );
extern instruction      *rSTORE_8U( instruction * );
extern instruction      *rMOVEXX_8( instruction * );
extern instruction      *rCONSTLOAD( instruction * );
extern instruction      *rALLOCA( instruction * );
