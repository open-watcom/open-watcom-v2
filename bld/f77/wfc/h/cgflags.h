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


typedef enum {
    CG_LOADED           = 0x0001,    // BEDLLLoad() succeeded
    CG_INIT             = 0x0002,    // BEInit() succeeded
    CG_BLIPS            = 0x0004,    // "blips" on the screen
    CG_FATAL            = 0x0008,    // fata error issued by code generator
    CG_MEM_LOW_ISSUED   = 0x0010,    // "low on memory" message isuued
    CG_STARTED          = 0x0020,    // code generator is started
    CG_USED_80x87       = 0x0040,    // code generated used 80x87
    CG_HAS_PROGRAM      = 0x0080,    // file contains PROGRAM
    CG_FP_MODEL_80x87   = 0x0100,    // floating-point model generates 80x87 instructions
} cgflags_t;
