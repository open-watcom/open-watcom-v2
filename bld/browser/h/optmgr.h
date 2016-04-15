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


#ifndef __OPTMGR_H__
#define __OPTMGR_H__

#include <wstd.h>
#include <string.hpp>

#include "menumgr.h"
#include "keysym.h"
#include "gtclsopt.h"
#include "gtfnopt.h"

class WFileName;
class RegExpCfg;
class QueryConfig;
class FileFilter;
class GTInheritOption;
class GTCallOption;

/* ----------- Define types / enumerations for option settings -----*/

struct MemberFilter {
    enum InheritLevel {
        ILNoInherited       = 0x00,
        ILVisibleInherited  = 0x01,
        ILAllInherited      = 0x02
    };

    enum AccessLevel {
        AccPublic       = 0x01,
        AccProtected    = 0x02,
        AccPrivate      = 0x04,
        AccAll          = AccPublic | AccProtected | AccPrivate
    };

    enum Members {
        MemVariables    = 0x01,
        MemVarStatic    = 0x02,
        MemFunctions    = 0x04,
        MemVirtual      = 0x08,
        MemFuncStatic   = 0x10,
        MemAll          = MemVariables | MemVarStatic |
                          MemFunctions | MemVirtual | MemFuncStatic
    };

        /* these variables are integers so they may be or'ed */
    uint_16     _inheritLevel   : 2;
    uint_16     _accessLevel    : 3;
    uint_16     _members        : 5;
};

// these are used as an index into an array in enumtype.cpp
enum {
    EV_HexLowerCase = 0,        // 0xa5
    EV_HexUpperCase = 1,        // 0XA5
    EV_HexMixedCase = 2,        // 0xA5
    EV_Octal        = 3,        // 0245
    EV_Decimal      = 4,        // 165
    EV_Character    = 5,        // 'A'
    EV_NumStyles
};

typedef uint_8 EnumViewStyle;


enum {
    TreeVertical    = 0,
    TreeHorizontal  = 1
};

typedef uint_8 TreeDirection;

enum {
    EdgesSquare,
    EdgesStraight
};

typedef uint_8 EdgeType;

#define NumMagicChars 14

struct RXOptions {
    bool _anchored;
    char _magicString[ NumMagicChars + 1 ];
};

class OptionManager : public MenuHandler {
public:

                            OptionManager();
                            ~OptionManager();

    virtual void            menuSelected( const MIMenuID & id );

            void            setMenus( MenuManager * mgr );
            void            unSetMenus( MenuManager * mgr );

            bool            exit();         // return true if ok to terminate
            bool            isModified() const;

            bool            load();         // get a filename first
            void            loadDefault();  // default options file
            void            loadFrom( const char * file );

            bool            save();         // get a filename first
            void            saveTo( const char * file );

            const char *    fileName() const;

            /*------------ get option settings ------------*/

            MemberFilter    getMemberFilter() const;
            EnumViewStyle   getEnumStyle() const;
            bool            getTreeAutoArrange() const;
            TreeDirection   getTreeDirection() const;
            EdgeType        getEdgeType() const;
            bool            getIgnoreCase() const;
            bool            getWholeWord() const;
            bool            getUseRX() const;
            bool            getAnchored() const;
    const   char *          getMagicString() const;
            bool            getAutoSave() const;
    const   GTClassOpts &   getClassOpts() const;
    const   GTFunctionOpts& getFunctionOpts() const;
    const   KeySymbol &     getQueryFilt() const;
    const   char *          getEditorName() const;
            bool            isEditorDLL() const;
    const   char *          getEditorParms() const;

            void            setMemberFilter( MemberFilter );
            void            setEnumStyle( EnumViewStyle );
            void            setTreeAutoArrange( bool );
            void            setTreeDirection( TreeDirection );
            void            setEdgeType( EdgeType );
            void            setIgnoreCase( bool );
            void            setWholeWord( bool );
            void            setUseRX( bool );
            void            setAnchored( bool );
            void            setMagicString( const char * );
            void            setAutoSave( bool );
            void            setClassOpts( const GTClassOpts * );
            void            setFunctionOpts( const GTFunctionOpts * );
            void            setEditor( const char *name, bool isDll,
                                       const char *parms );

            void            editMemberFilters();
            void            editEnumStyle();
            void            editInheritLegend();
            void            editCallLegend();
            void            editRegExp();
            void            editQuery();
            void            editEditorDLL();
protected:
            void            toggleTreeAutoArrange();
            void            toggleEdgeType();
            void            toggleTreeDirection();
            void            toggleAutoSave();

            void            checkMenus();
            void            modified();

            bool            hasFile() const;

private:
            MenuManager *   _menuManager;
            WFileName *     _fileName;
            bool            _modified;

            struct OptionSet {
                OptionSet();

                MemberFilter    _memberFilter;
                EnumViewStyle   _enumStyle;
                bool            _treeAutoArrange;
                TreeDirection   _treeDirection;
                EdgeType        _edgeType;

                bool            _ignoreCase;    // in querycfg
                bool            _wholeWord;
                bool            _useRX;

                RXOptions       _rxOptions;

                GTClassOpts     _classOpts;
                GTFunctionOpts  _functionOpts;

                bool            _autoSave;
            };

    static  OptionSet           _default;       // program default
            OptionSet           _initial;       // initial values in option file
            OptionSet           _current;       // current settings
            KeySymbol           _queryKey;      // filters for queries
            QueryConfig *       _queryConfig;   // query edit dialog
            GTInheritOption *   _inheritOption; // inherit legend dialog
            GTCallOption *      _callOption;    // call legend dialog
            String              _editorName;    // name of the editor
            bool                _editorIsDLL;   // is the editor a dll?
            String              _editorParms;   // parms to pass to editor
};

#endif  // __OPTMGR_H__
