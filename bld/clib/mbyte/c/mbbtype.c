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


#include "variety.h"
#include <mbctype.h>
#include <mbstring.h>
#include "farfunc.h"
#include "mbchar.h"



_WCRTLINK extern int _mbbtype( unsigned char c, int type )
{
    if( __IsDBCS ) {
        if( __MBCodePage == 932 ) {
            if( type != _MBC_LEAD ) {
                if( (c>=0x81 && c<=0x9F)  ||  (c>=0xE0 && c<=0xFC) ) {
                    return( _MBC_LEAD );
                } else if( (c>=0x20 && c<=0x7E)  ||  (c>=0xA1 && c<=0xDF) ) {
                    return( _MBC_SINGLE );
                } else {
                    return( _MBC_ILLEGAL );
                }
            } else {
                if( (c>=0x40 && c<=0x7E)  ||  (c>=0x80 && c<=0xFC) ) {
                    return( _MBC_TRAIL );
                } else {
                    return( _MBC_ILLEGAL );
                }
            }
        } else {
            if( type == _MBC_LEAD ) {
                return( _ismbbtrail(c) ? _MBC_TRAIL : _MBC_ILLEGAL );
            } else {
                return( _ismbblead(c) ? _MBC_LEAD : _MBC_SINGLE );
            }
        }
    } else {
        return( _MBC_SINGLE );
    }
}
