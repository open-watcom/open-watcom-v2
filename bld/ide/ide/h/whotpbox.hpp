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


#ifndef whotpickbox_class
#define whotpickbox_class

#include "whotlist.hpp"
#include "wview.hpp"
#include "wpicklst.hpp"

typedef int (WObject::*icb)();

WCLASS WHotPickBox : public WHotSpotList, public WView {
    public:
        WEXPORT WHotPickBox( WPickList& plist, cbs gname, icb gindex, bcb gincluded,
                WWindow *parent, const WRect& r, WHotSpots* hs );
        WEXPORT ~WHotPickBox();
        void* WEXPORT selectedTagPtr();
        void WEXPORT updateView();
        void WEXPORT modelGone();

        virtual bool            gettingFocus( WWindow* );
        virtual int             count();
        virtual int             width() { return( _tagsWidth ); }
        virtual const char *    getString( int index );
        virtual int             getHotSpot( int index, bool pressed );
        virtual int             getHotOffset( int );

        virtual int topIndex() { return( _topIndex ); }
        virtual void setTopIndex( int top );

        virtual void* tagPtr( int index );
        virtual void setTagPtr( int index, void* ptr );
        void selectSameTag( void* );

    private:
        cbs             _gname;
        icb             _gindex;
        bcb             _gincluded;
        bool            _sorted;
        void            name( int index, WString& str );
        void            fillBox();
        void**          _tags;
        int             _tagsCount;
        int             _tagsWidth;
};

#endif
