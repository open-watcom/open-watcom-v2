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


#ifndef __KEYSYM_H__
#define __KEYSYM_H__

#include <string.hpp>
#include <dr.h>

#include "wbrdefs.h"

class Symbol;
class CheckedFile;
class FileFilter;

enum KeySymType {
    KSTNoSyms       = 0x0000,
    KSTClasses      = 0x0001,
    KSTStructs      = 0x0002,       /* NYI */
    KSTUnions       = 0x0004,       /* NYI */
    KSTEnums        = 0x0008,
    KSTFunctions    = 0x0010,
    KSTTypedefs     = 0x0020,
    KSTVariables    = 0x0040,
    KSTMacros       = 0x0080,       /* NYI */
    KSTLabels       = 0x0100,       /* NYI */
    KSTExceptions   = 0x0200,       /* NYI */
    KSTAllSyms      = 0xffff,
};

class KeySymbol
{
public:
                        KeySymbol();
                        KeySymbol( const KeySymbol & o );
                        ~KeySymbol();

            bool        matches( Symbol * sym );
            bool        matches( dr_sym_context * ctxt );
            bool        matches( dr_handle drhdl, const char * name );

            void        setSymType( int type );
            void        setName( const char * );
            void        setContClass( const char * className );
            void        setContFunction( const char * functionName );
            void        setAnonymous( bool anon );
            void        setArtificial( bool art );
            void        setDeclaration( bool decl );

            int         symType() const { return _searchFor; }
    const   char *      name() { return _name; }
    const   char *      contClass() { return _contClass; }
    const   char *      contFunction() { return _contFunction; }
            bool        anonymous() const { return _anonymous; }
            bool        artificial() const { return _artificial; }
            bool        declaration() const { return _declaration; }

            FileFilter* fileFilter() { return _fileFilter; }
            void        read( CheckedFile & );
            void        write( CheckedFile & );

private:
            void        setSearchString( String & s, void ** prg, const char * name );

    String      _name;
    void *      _nameProg;

    String      _contClass;
    void *      _contClassProg;

    String      _contFunction;
    void *      _contFunctionProg;

    bool        _anonymous;
    bool        _artificial;
    bool        _declaration;

    FileFilter* _fileFilter;

    int         _searchFor;     // which types to search for (KeySymType)
};

#endif // __KEYSYM_H__
