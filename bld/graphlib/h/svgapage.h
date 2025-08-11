/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Graph library SVGA page related declarations
*
****************************************************************************/


// The page setting functions are defined as FARC pointers.
// This is _WCI86FAR for 16-bit, and nothing for 32-bit flat model.
// QNX 32-bit uses small model, and FARC is defined as _WCI86FAR.
// For QNX 32-bit, we still want only near pointers in the table though,
// to avoid segment relocations in the executable.
// The assignment to _SetVGAPage provides the CS value at runtime.

#if defined( _SUPERVGA )

#if defined( _M_I86 )
    #define _FARC               __far
#elif defined( __QNX__ )
    #define _FARC               __far
#else
    #define _FARC
#endif

#pragma aux VGAPAGE_FUNC "*" __parm __caller [__al]

typedef void __pascal vgapage_fn( unsigned char );

#pragma aux (VGAPAGE_FUNC) vgapage_fn;

extern vgapage_fn       _FARC *_SetVGAPage;         // function to set SVGA page

#endif
