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


#ifndef vlistbox_class
#define vlistbox_class

// This class behaves like a list box.  It is implemented as a kludge
// so that we can handle keyboard input properly when a list box is
// embedded in a window

#include "whotlist.hpp"

WCLASS VListBox : public WHotSpotList {
    public:
        WEXPORT VListBox( WWindow *parent, const WRect& r );
        WEXPORT ~VListBox();

        void                    insertString( const char *str );
        void                    deleteString( int index );
        void                    resetContents( void );
        virtual bool            gettingFocus( WWindow* );
        virtual int             count() { return( _strings.count() ); }
        virtual int             width() { return( _maxWidth ); }
        virtual const char *    getString( int index );
        virtual int             getHotSpot( int, bool ) { return( 0 ); }
        virtual int             getHotOffset( int ) { return( 0 ); }

    private:
        WVList                  _strings;
        int                     _maxWidth;
};

#endif
