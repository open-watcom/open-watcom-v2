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
    MACRO.HPP
    ---------
    Module for processing macro information for the browse/merge DLL.
    Two kinds of macro information are handled here: dependancies
    (macros of the form "#ifdef __FILE_HPP_INCLUDED", also know as
    guard macros) and macro symbols.
*/

#ifndef _MACRO_HPP
#define _MACRO_HPP

#ifndef _BRMTYPES_H
extern "C" {
#include "brmtypes.h"
};
#endif

class CacheOutFile;
class CacheInFile;
class StringIDs;
class FileGuardInfo;
template<class T> class AvlTree;

struct DependValRec {
        BRI_StringID            macroNameID;
        uint_32                 numParams;
        uint_32                 length;
        uint_8                  *defn;
};

/*  DependList:
      Record the dependancy information (i.e. "#ifdef _FILE_INCLUDED" macros)
      that appears when a new source file is included, and determine if
      the inclusion is redundant.
*/

class DependList {
    public:
        DependList();
        ~DependList();

        // Record information for a new instance of a source file.
        void    StartFile( BRI_StringID filename );

        // Add various dependancy information.
        void    AddDepDefined( BRI_StringID macroName );
        void    AddDepUndefed( BRI_StringID macroName );
        void    AddDepMacValue( DependValRec *value );

        // Record that a file has been exited.
        void    EndFile();

        // Is the instance of the file redundant?
        WBool   IncludeFile();

        // Record the start and end of template instatiations (which
        // affect the "current source file" in weird ways)
        void    StartTemplate();
        void    EndTemplate();

        WBool   Ignore() { return _state==IGNORE; }

        // Save/Load functionality.
        WBool   SaveTo( CacheOutFile *cache );
        WBool   LoadFrom( CacheInFile *cache );

    private:
        AvlTree<FileGuardInfo>  *_fileInfo;
        int                     _count;
        FileGuardInfo           *_temp;

        enum {
            USE,
            IGNORE
        }       _state;
        int     _ignoreDepth;
        int     _templateDepth;
};


/*  MacroList:
      Storage for macro symbol information, namely what macros have been
      defined and where.
*/

class MacroList {
    public:
        MacroList();
        ~MacroList();

        WBool   HaveSeen( BRI_StringID  macroName );
        void    AddMacroDecl( BRI_StringID macroName, BRI_SymbolID symId );

        WBool   SaveTo( CacheOutFile *cache );
        WBool   LoadFrom( CacheInFile *cache );

    private:
        StringIDs       *_macroNames;
};
#endif  // _MACRO_HPP
