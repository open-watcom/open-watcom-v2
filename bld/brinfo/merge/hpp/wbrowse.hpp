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


/*
    BRMERGE.HPP
    -----------
    Interface to the browse/merge DLL.
*/

#ifndef _WBROWSE_HPP
#define _WBROWSE_HPP

#ifndef _WATCOM_H_INCLUDED_
#include <watcom.h>
#endif

extern "C" {
    #include "brmtypes.h"
};

typedef class Browser *BHandle;
class DeclRec;

struct SourceLocn {
    char const  *filePath;      // FULL path name of a source file
    char const  *name;          // name of a symbol
    int         line;
    int         col;
};

typedef void __stdcall (*BCallBack)(SourceLocn *, DeclRec const *, void *);


/* Allocate a new Browser.
*/
BHandle __export NewBrowser();

/* Save a Browser to a file.
*/
WBool   __export SaveBrowser(BHandle m, char const *filepath);

/* Load a Browser from a file.
*/
BHandle __export LoadOldBrowser(char const *filepath);

/* De-allocate a Browser.
*/
void    __export DestroyBrowser(BHandle);

/* Access whatever debug messages the DLL has deemed fit to emit.
*/
WStringArray const *    __export DebugMessages();


/*  Add a .BRM file to the Browser's data.
*/
WBool   __export AddFile(BHandle, char const *filepath);


/*  Find definitions of a symbol, given just the name.
    The name should be in ASCII text, not unicode.
    (If you're using WStrings, use WString::GetAnsiText(). )
*/

int     __export DefnNameIter( BHandle,
                               char const *name,
                               BCallBack cb,
                               void * cookie );


/*  Find definitions of a symbol, given the name and the location of a
    reference to the symbol.
*/
int     __export DefnAtIter(    BHandle,
                        SourceLocn *locn,
                        BCallBack cb,
                        void * cookie );


/*  Find definitions of a symbol, given a pointer to it's browse
    information.
*/
int     __export DefnOfIter(    BHandle,
                        DeclRec const *sym,
                        BCallBack cb,
                        void * cookie );


/*  Find all class symbols defined IN WHOLE OR IN PART in the given
    source file.  "filepath" should be a full file path in ASCII text.
*/
int     __export FileClassesIter( BHandle,
                                const char *filepath,
                                BCallBack cb,
                                void * cookie );


/*  Find all references of a given type to a given symbol.
    If refType==BRI_RT_None, find all references.
*/
int     __export ReferenceIter( BHandle,
                        DeclRec const *sym,
                        BRI_ReferenceType refType,
                        BCallBack cb,
                        void * cookie );


/*  Find all symbols of a given type (eg variables or classes).
    If symType==BRI_SA_None, find all symbols.
*/
int     __export SymbolIter(    BHandle,
                        BRI_SymbolAttributes symType,
                        BCallBack cb,
                        void * cookie );


/*  Find all functions which are part of the call graph, i.e. functions
    which either call something or are called by something.  To get
    functions which are not part of the call graph, use SymbolIter.
*/
int     __export UsedFnsIter(   BHandle,
                                BCallBack cb,
                                void * cookie );


int     __export UserClassesIter( BHandle,
                                  BCallBack cb,
                                  void * cookie );


/*  Find all functions which call a given function.
*/
int     __export CallingFnsIter(        BHandle,
                        DeclRec const *sym,
                        BCallBack cb,
                        void * cookie );


/*  Find all functions called by a given function.
*/
int     __export CalledFnsIter( BHandle,
                       DeclRec const *sym,
                       BCallBack cb,
                       void * cookie );

/*  Find all parent classes of a given class.
*/
int     __export ParentClassesIter( BHandle,
                           DeclRec const *sym,
                           BCallBack cb,
                           void *cookie );

/*  Find all child classes of a given class.
*/
int     __export ChildClassesIter( BHandle,
                          DeclRec const *sym,
                          BCallBack cb,
                          void *cookie );

/*  Find all member functions of a given class.
*/
int     __export MemberFnsIter( BHandle,
                        DeclRec const *sym,
                        BCallBack cb,
                        void *cookie );


/*  Various ways to get the name of a symbol.
*/
WString __export UnScopedName( BHandle, DeclRec const *sym );
WString __export ScopedName( BHandle, DeclRec const *sym );
WString __export NameOfScope( BHandle, DeclRec const *sym );

/*  Various ways to get the type of a symbol.
*/
WString __export TypedName( BHandle, DeclRec const *sym );
WString __export NameOfType( BHandle, DeclRec const *sym );

/*
*/
BRI_SymbolAttributes __export SymAttrs( DeclRec const *sym );

#endif  // _WBROWSE_HPP
