/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  FP patches data definition pick file
*
****************************************************************************/


//                                          WLINK
//        enum        name     alt_name     win         alt_win     others      alt_others
pick_fp( FPP_WAIT,   "FIWRQQ", NULL,        6,          FPP_IGNORE, 1,          FPP_IGNORE  )   // int 3Dh
pick_fp( FPP_NORMAL, "FIDRQQ", NULL,        5,          FPP_IGNORE, 2,          FPP_IGNORE  )   // int 34-3Bh
pick_fp( FPP_ES,     "FIERQQ", NULL,        4,          FPP_IGNORE, 3,          FPP_IGNORE  )   // int 3Ch
pick_fp( FPP_CS,     "FICRQQ", "FJCRQQ",    3,          FPP_IGNORE, 4,          FPP_IGNORE  )   // int 3Ch
pick_fp( FPP_SS,     "FISRQQ", "FJSRQQ",    2,          FPP_IGNORE, 5,          FPP_IGNORE  )   // int 3Ch
pick_fp( FPP_DS,     "FIARQQ", "FJARQQ",    1,          FPP_IGNORE, 6,          FPP_IGNORE  )   // int 3Ch
pick_fp( FPP_FS,     "FIFRQQ", "FJFRQQ",    FPP_IGNORE, FPP_IGNORE, FPP_IGNORE, FPP_IGNORE  )   // int 3Ch
pick_fp( FPP_GS,     "FIGRQQ", "FJGRQQ",    FPP_IGNORE, FPP_IGNORE, FPP_IGNORE, FPP_IGNORE  )   // int 3Ch
