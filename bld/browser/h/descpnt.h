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


#ifndef __DESCPNT_H__
#define __DESCPNT_H__

#include "wbrwin.h"
#include "descrip.h"

template <class Type> class WCPtrOrderedVector;

class DescriptionPart {
public:
                        DescriptionPart( const char * txt, Symbol * sym,
                                         const WRect & r );
                        ~DescriptionPart();

                void    paint( WBRWindow * prnt, bool sel );

                bool    operator== ( const DescriptionPart & o ) const {
                            return( this ==  &o );
                        }

    WString             _text;
    Symbol *            _symbol;
    WRect               _rect;
};

class DescriptionPaint {
public:
                    DescriptionPaint( WBRWindow * prnt, const WRect & r, Symbol * sym );
                    ~DescriptionPaint();

            bool    paint();
            bool    select( int x, int y );
            bool    tabHit( bool shift );
            bool    enterHit();
    const   WRect&  rect() { return _rect; }
    const   WRect&  partRect( int idx );
            int     current( void ) { return( _current ); }

private:
            DescriptionPart *   findPart( int x, int y, int &idx );

            WBRWindow *     _parent;
            WRect           _rect;
            int             _current;

            WCPtrOrderedVector<DescriptionPart> *   _parts;
};

#endif // __DESCPNT_H__
