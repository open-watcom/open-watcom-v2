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


#include "cpplib.h"
#include "rtexcept.h"
#include "lock.h"
#define __LIB__
#include "demangle.h"

#include <new>
#include <typeinfo>

#pragma initialize 19;

struct __TID_NAME {
    __TID_NAME  *next;
    char        name[1];
};

#ifdef __MT__
static __lock semaphore;
#endif
static __TID_NAME *listNames;

namespace {
    struct __INITFINI {
        ~__INITFINI() {
            __TID_NAME *p;
            __TID_NAME *n;
#ifdef __MT__
            __fn_lock lock( &semaphore );
#endif

            p = listNames;
            listNames = NULL;
            for( ; p != NULL; p = n ) {
                n = p->next;
                char *t = (char*) p;
                delete [] t;
            }
        }
    };
}

__INITFINI __type_info_name_free;

namespace std {

  const char *type_info::name() const {
    const char *ret_name;
    unsigned len;
    __TID_NAME *name;
#ifdef __MT__
    __fn_lock lock( &semaphore );
#endif

    ret_name = __raw;
    name = (__typeof(name)) __data;
    if( name != NULL ) {
        ret_name = name->name;
    } else {
        len = __demangle_t( __raw, 0, NULL, 0 );
        if( len != 0 ) {
            name = (__typeof(name)) new char[ sizeof( __TID_NAME ) + len ];
            if( name != NULL ) {
                __demangle_t( __raw, 0, name->name, len + 1 );
                name->next = listNames;
                listNames = name;
                __data = name;
                ret_name = name->name;
            }
        }
    }
    return( ret_name );
  }

} // namespace std

