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


#ifndef __MODULE_H__
#define __MODULE_H__

#include <string.hpp>

#include <dr.h>
#include <wobject.hpp>
#include <wvlist.hpp>

#include "wbrdefs.h"
#include "elffile.h"

class KeySymbol;
class WFileName;
class WString;
class WVList;
class Symbol;

template <class T> class WCValSList;

// users of module shouldn't have any
// knowledge of contents of SearchContext
typedef dr_search_context SearchContext;

class Module : public WObject
{
public:
                        Module( const char * f, WCValSList<String> & enabled,
                                WCValSList<String> & disabled );
    virtual             ~Module();

    const char *        fileName() { return _dataFile->getFileName(); }

    SearchContext *     findSymbols( WVList * dest,
                                     KeySymbol * filter,
                                     SearchContext * ctxt=NULL,
                                     int numItems = -1 );

    bool                findReferences( WVList * val, Symbol * data );
    bool                findRefSyms( WVList * val, Symbol * sym );

    void                addFile( const char * fileName, bool enabled );

    void                setModule();

    WVList &            enabledFiles() { return _enabledFiles; }
    WVList &            disabledFiles() { return _disabledFiles; }

    void                checkSourceTime();

/*
 * these are not used outside of module.cpp, but have to be public since they
 * are called by non-member functions
 */
    void                readSect( dr_section, void * buf, int len );
    void                seekSect( dr_section, long pos );

private:
    WVList                  _enabledFiles;
    WVList                  _disabledFiles;
    ElfFile *               _dataFile; // holds my Dwarf info
    dr_dbg_handle           _dbgInfo;
};
#endif // __MODULE_H__

