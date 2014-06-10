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


#ifndef __FUNCSYM_H__
#define __FUNCSYM_H__

#include "symbol.h"
#include "mempool.h"

class Module;

class FunctionSym : public Symbol
{
public:
                        FunctionSym( dr_handle h, dr_handle p,
                                    Module * m, char * name )
                                : Symbol( h, p, m, name ) {}
    virtual             ~FunctionSym() {}
    virtual dr_sym_type symtype() const { return DR_SYM_FUNCTION; }

            void *      operator new( size_t );
            void        operator delete( void * );

            void        localVars( WVList & );
            void        callees( WVList & );
            void        callers( WVList & );

private:
    static  bool        memberHook( dr_sym_type, dr_handle, char *, dr_handle, void * );
    static  bool        callHook( dr_handle, dr_ref_info *, char *, void * );

    static MemoryPool   _pool;
};

#endif // __FUNCSYM_H__
