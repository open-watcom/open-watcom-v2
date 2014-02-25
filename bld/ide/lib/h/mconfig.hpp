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


#ifndef mconfig_class
#define mconfig_class

#define CUR_CFG_VERSION     4

#include "wobject.hpp"
#include "wpicklst.hpp"
#include "wfilenam.hpp"
#include "wtokfile.hpp"
#include "wwindow.hpp"
#include "wkeydefs.hpp"
#include "mtool.hpp"
#include "mswitch.hpp"
#include "mcommand.hpp"

#define LOG_HELP_WIDTH      3

#define MASK_SIZE           5

//these are used for indexing by MConfig::zapTargetMasks()
typedef enum HostType {
    #undef pick
    #define pick(enum,type,batchserv,editor,DLL,parms,pathsep,descr) enum,
    #include "hosttype.h"
    HOST_UNDEFINED
} HostType;

WCLASS MTool;
WCLASS MRule;
WCLASS MConfig : public WObject
{
    Declare( MConfig )
    public:
        MConfig( WFileName& filename, bool debug=FALSE, HostType host=HOST_UNDEFINED, const char *include_path=NULL );
        ~MConfig();
        bool ok() { return _ok; }
        const WString& errMsg() { return _errMsg; }
        bool isDirty() { return _dirty; }
        void setDirty( bool dirty=TRUE ) { _dirty = dirty; }
        bool readConfig();
        void writeConfig();

        MTool* nilTool() { return _nilTool; }
        MTool* findTool( WString& tooltag );
        MRule* nilRule() { return _nilRule; }
        MRule* findRule( WString& ruletag );
        MRule* findMatchingRule( WFileName& fn, WString& mask );
        int findMatchingRules( WFileName& fn, WString& mask, WVList& list );
        MRule* findMatchingRule( WFileName& fn, MRule* tgtRule, WString& mask );
        WPickList& tools() { return _tools; };
        WPickList& rules() { return _rules; };
        WPickList& targets() { return _targets; };
        WPickList& toolItems() { return _toolItems; };
        WPickList& actions() { return _actions; };
        WVList& helpactions() { return _helpactions; };
        WFileName& editor() { return _editor; }
        WFileName& browse() { return _browse; }
        WFileName& browseMerge() { return _browseMerge; }
        WFileName& batserv() { return _batserv; }
        WFileName& helpFile() { return _helpFile; }
        WFileName& htmlHelpFile() { return _htmlHelpFile; }
        const char* fileFilters() { return _fileFilters; }
        const MCommand& before() const { return _before; }
        const MCommand& after() const { return _after; }
        static MConfig* _configPtr;
        WFileName& filename() { return _filename; }
        bool debug() { return _debug; }
        WVList& logScanPatterns() { return _logScanPatterns; }
        WVList& logHelpFiles() { return _logHelpFiles; }
        WVList& logHtmlHelpFiles() { return _logHtmlHelpFiles; }
        int version() { return _version; }
        void setKludge( int k ) { _kludge = k; }
        void kludgeMask( WString& str );
        void zapMask( WString& mask );
        HostType hostType() { return _hostType; }
        void enumAccel( WObject *obj, bcbk fn );
        WVList& targetOSs() { return _targetOSs; }
        bool editorIsDLL() { return _editorIsDLL; }
        WFileName& editorParms() { return _editorParms; }
        char getPathSep() { return _pathsep; }
    private:
        bool            _ok;
        WString         _errMsg;
        bool            _dirty;
        WFileName       _filename;
        bool            _debug;
        WPickList       _tools;
        WPickList       _rules;
        WPickList       _actions;
        WVList          _helpactions;
        WPickList       _targets;
        WVList          _targetOSs;
        WPickList       _toolItems;
        WFileName       _editor;
        WFileName       _browse;
        WFileName       _browseMerge;
        WFileName       _batserv;
        WFileName       _helpFile;
        WFileName       _htmlHelpFile;
        MCommand        _before;
        MCommand        _after;
        char*           _fileFilters;
        int             _fileFilterSize;
        MTool*          _nilTool;
        MRule*          _nilRule;
        void configMsgLog( WTokenFile& fil, WString& tok );
        WVList          _logScanPatterns;
        WVList          _logHelpFiles;
        WVList          _logHtmlHelpFiles;
        void configProject( WTokenFile& fil, WString& tok );
        void addRules( WFileName& srcMask, WFileName& tgtMask, WVList& list, WString& tagMask );
        bool readFile( const WFileName& filename, bool reqd=TRUE );
        int             _version;
        int             _kludge;
        HostType        _hostType;
        void zapTargetMasks();
        WString         _hostMask;
        void buildTargetOSList();
        bool            _editorIsDLL;
        WFileName       _editorParms;
        void expandMacroes( WString &str );
        char            _pathsep;
        const char      *_include_path;
};

#define _config MConfig::_configPtr

#endif
