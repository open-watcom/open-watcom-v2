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


#include <iostream.h>

// SYSTEM provides a hook before it prints "hello"

struct CALLBACK {
    virtual void hook( void * ) = 0;
};

struct SYSTEM {
    void doit( void )
    {
        _call_back_object->hook( _call_back_data );
        cout << "hello" << endl;
    }
    void register_call_back( CALLBACK *cb, void *data )
    {
        _call_back_object = cb;
        _call_back_data = data;
    }
private:
    CALLBACK *_call_back_object;
    void *_call_back_data;
};

// user of SYSTEM callback

struct use_BASE {
};

struct use_CALLBACK : use_BASE, CALLBACK {
    void hook( void *p )
    {
        unsigned *q = (unsigned *) p;

        cout << "call back called with " << *q << endl;
    }
};

SYSTEM zz;

main()
{
    unsigned x = 5;

    zz.register_call_back( new use_CALLBACK, &x );
    zz.doit();
}
