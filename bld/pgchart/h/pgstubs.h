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


// routines called from PGCHART.LIB

#if defined( __386__ )
    #define _WCI86FAR
#else
    #define _WCI86FAR    __far
#endif

void _WCI86FAR       _GR_ecvt( float _WCI86FAR *, int, int _WCI86FAR *, int _WCI86FAR *, char _WCI86FAR * );
void _WCI86FAR       _GR_fcvt( float _WCI86FAR *, int, int _WCI86FAR *, int _WCI86FAR *, char _WCI86FAR * );
void _WCI86FAR       _GR_ceil( float _WCI86FAR * );
void _WCI86FAR       _GR_cos( float _WCI86FAR * );
void _WCI86FAR       _GR_log( float _WCI86FAR * );
void _WCI86FAR       _GR_log10( float _WCI86FAR * );
void _WCI86FAR       _GR_sin( float _WCI86FAR * );
