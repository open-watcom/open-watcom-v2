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


#include "gdefn.h"


#if defined( __386__ )
    #define Copy( dst, src, len )   memcpy( dst, src, len )
#else
    #define Copy( dst, src, len )   _fmemcpy( dst, src, len )
#endif


unsigned char _WCI86FAR * _WCI86FAR _CGRAPH _getfillmask( unsigned char _WCI86FAR *mask )
/*========================================================================

   This function qeuries the pattern for area filling. */

{
    if( mask != NULL ) {
        Copy( mask, _FillMask, MASK_LEN );
    }
    if( _HaveMask ) {
        return( mask );
    } else {
        return( NULL );
    }
}

Entry( _GETFILLMASK, _getfillmask ) // alternate entry-point


void _WCI86FAR _CGRAPH _setfillmask( unsigned char _WCI86FAR * mask )
/*=========================================================

   This routine sets the pattern for area filling. */

{
    if( mask == NULL ) {
        _HaveMask = 0;
        memcpy( _FillMask, _DefMask, MASK_LEN );
    } else {
        Copy( _FillMask, mask, MASK_LEN );
        if( memcmp( _FillMask, _DefMask, MASK_LEN ) == 0 ) {
            _HaveMask = 0;
        } else {
            _HaveMask = 1;
        }
    }
}

Entry( _SETFILLMASK, _setfillmask ) // alternate entry-point
