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


enum {
    COND_CONFIG         = 0x0001,
    COND_SECTIONS       = 0x0002,
    COND_LIBRARIES      = 0x0004,
    COND_ALIASING       = 0x0008,
    COND_THREAD         = 0x0010,
    COND_THREAD_EXTRA   = 0x0020,
    COND_TRACE          = 0x0040,
    COND_BREAK          = 0x0080,
    COND_WATCH          = 0x0100,
    COND_USER           = 0x0200,
    COND_TERMINATE      = 0x0400,
    COND_EXCEPTION      = 0x0800,
    COND_MESSAGE        = 0x1000,
    COND_STOP           = 0x2000,
    COND_RUNNING        = 0x4000,
    COND_STOPPERS       = (COND_TRACE|COND_BREAK|COND_WATCH|COND_USER
                                |COND_TERMINATE|COND_EXCEPTION|COND_STOP)
};

