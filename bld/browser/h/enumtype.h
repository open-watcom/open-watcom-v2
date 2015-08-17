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


#ifndef __ENUMTYPE_H__
#define __ENUMTYPE_H__

#include "symbol.h"
#include "mempool.h"

class Module;

class EnumType : public Symbol
{
public:
                            EnumType( dr_handle drhdl, dr_handle drhdl_prt,
                                        Module * m, char * name );
    virtual                 ~EnumType() {}

    virtual dr_sym_type     symtype() const { return DR_SYM_ENUM; }

            void *          operator new( size_t );
            void            operator delete( void * );

            void            loadElements( WVList & );

private:

    static  bool            loadHook( char * name, unsigned_32 val, void * data );

            unsigned        _byteSize;

    static MemoryPool       _pool;
};

class EnumElement : public WObject
{
public:
                    EnumElement( char * name, unsigned byteSize,
                                 unsigned_32 val );
                   ~EnumElement();

    char *          name( char * = NULL );
    WString &       value( WString & );

private:

    unsigned        _byteSize;
    unsigned_32     _value;
    char *          _name;
};

#endif //__ENUMTYPE_H__
