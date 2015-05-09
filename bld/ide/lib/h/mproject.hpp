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


#ifndef mproject_class
#define mproject_class

#include "wobject.hpp"
#include "wfilenam.hpp"
#include "wvlist.hpp"
#include "contfile.hpp"
#include "wmodel.hpp"
#include "mconfig.hpp"

/* support for RCS stuff from vrcscli.hpp */
typedef int (WObject::*CheckoutFunction)( WFileName *, WString &, WString & );

WCLASS MComponent;
WCLASS MItem;
WCLASS MProject : public WObject, public WModel
{
        Declare( MProject )
        public:
                MProject( const char* filename );
                ~MProject();
                void setFilename( const WFileName& f );
                WFileName& filename() { return( _filename ); }
                WVList& components() { return( _components ); }
                MComponent* addComponent( MComponent* comp );
                MComponent* removeComponent( MComponent* comp );
                MComponent* attachComponent( WFileName& target );
                MComponent* findOldComponent( WFileName& fn );
                MComponent* findComponent( WFileName& fn );
                MItem* findSameResult( MItem* item, MComponent** compp );
                void resetRuleRefs();
                const MCommand& before() const { return( _before ); }
                void setBefore( const MCommand& before );
                const MCommand& after() const { return( _after ); }
                void setAfter( const MCommand& after );
                bool makeMakeFile( bool long_lines );
                void setDirty( bool dirty=true );
                bool isDirty() { return( _dirty ); }
                void setNeedsMake( bool needs=true ) { _needsMake = needs; }
                bool needsMake() { return( _needsMake ); }
                void refresh( bool autotrack_only=false );
                void insertBlanks( WString& s );
                void setRCS( WObject *obj, CheckoutFunction co );
                bool tryOpenTargetFile( long version, bool try_checkout, MComponent* comp );
        private:
                bool            _dirty;
                bool            _needsMake;
                WFileName       _filename;
                WFileName       _makefile;
                void setMakefile();
                WVList          _components;
                bool            _recursing;
                MCommand        _before;
                MCommand        _after;
                WObject         *_RCSCli;
                CheckoutFunction _checkout;
                void expand( ContFile& pmak, const MCommand& cmd, const char* head );
};

#endif

