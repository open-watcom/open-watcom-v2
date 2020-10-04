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
* Description:  WFL specific options definition data
*
****************************************************************************/


//                      Compile and Link Options
opt( "",           0,                CTG,         NULL,         NULL,       CL_CTG_COMPILE_AND_LINK ),
opt( "C",          0,                0,           NULL,         NULL,       CL_COMPILE_ONLY ),
opt( "Y",          0,                0,           NULL,         NULL,       CL_IGNORE_WFL ),
opt( "P",          0,                0,           NULL,         NULL,       CL_RUN_PROTECT ),
opt( "FM[=<fn>]",  0,                0,           NULL,         NULL,       CL_GENERATE_MAP ),
opt( "FE=<fn>",    0,                0,           NULL,         NULL,       CL_NAME_EXECUTABLE ),
opt( "FD[=<fn>]",  0,                0,           NULL,         NULL,       CL_KEEP_DIRECTIVE ),
opt( "FI=<fn>",    0,                0,           NULL,         NULL,       CL_INCLUDE_DIRECTIVE ),
#if _CPU == 8086
opt( "LP",         0,                0,           NULL,         NULL,       CL_LINK_FOR_OS2 ),
opt( "LR",         0,                0,           NULL,         NULL,       CL_LINK_FOR_DOS ),
#endif
opt( "L=<system>", 0,                0,           NULL,         NULL,       CL_LINK_FOR_SYSTEM ),
opt( "K=<n>",      0,                0,           NULL,         NULL,       CL_STACK_SIZE ),
opt( "\"<drctvs>\"", 0,              0,           NULL,         NULL,       CL_LINKER_DIRECTIVES ),
