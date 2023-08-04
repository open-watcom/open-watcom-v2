/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Resource Compiler pass 2 structures and constants.
*
****************************************************************************/


#ifndef PASS2_INCLUDED
#define PASS2_INCLUDED

#include "pass2l1.h"

typedef struct RcPass2Info {
    ExeFileInfo     TmpFile;
    ExeFileInfo     OldFile;
    ResFileInfo     *ResFile;
    bool            AllResFilesOpen;
    void            *IoBuffer;
} RcPass2Info;

extern bool     StopInvoked;

extern bool     MergeResExePE( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *resfiles );
extern bool     MergeResExeLX( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *resfiles );
extern bool     MergeResExeWINNE( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *res );
extern bool     MergeResExeOS2NE( ExeFileInfo *src, ExeFileInfo *dst, ResFileInfo *res );

#endif
