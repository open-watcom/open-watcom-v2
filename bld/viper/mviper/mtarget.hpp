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


#ifndef mtarget_class
#define mtarget_class

#include "wtokfile.hpp"
#include "mcommand.hpp"
#include "wfilenam.hpp"

WCLASS MTarget : public WObject
{
    Declare( MTarget )
    public:
        MTarget() {}
        MTarget( WTokenFile& fil, WString& tok, WString& target_os );
        ~MTarget() {}
        int compare( const WObject* o ) const { return _name.compare( &((MTarget*)o)->_name ); }
        void name( WString& n );
        const WString& help() { return _help; }
        WString& mask() { return _mask; }
        WFileName& tgtMask() { return _tgtMask; }
        WString& targetOS() { return _targetOS; }
        WString& imageName() { return _name; }
    private:
        WFileName       _tgtMask;
        WString         _name;
        WString         _help;
        WString         _mask;
        WString         _targetOS;
};

#endif
