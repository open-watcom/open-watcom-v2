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


#include <stdlib.h>
#include "wbrdefs.h"
#include "assure.h"
#include "module.h"
#include "util.h"
#include "projmodl.h"
#include "opstream.h"
#include "opscan.h"
#include "opterms.h"
#include "mem.h"
#include "gtclass.h"
#include "gtfunc.h"
#include "chbffile.h"

ProjectModel * activeProject = NULL;

ProjectModel::ProjectModel( char * cmdline, int len )
    :_appFile(NULL)
    ,_needsSave(FALSE)
    ,_busyCnt(0)
    ,_hasFile(FALSE)
//---------------------------------------------------
{
    #if 0 // FIXME
    _treeOpCluster[ TreeClassCluster ] = new TreeClassOptions;
    _treeOpCluster[ TreeFuncCluster ] = new TreeFuncOptions;
    #endif

    setAllDefault();
    REQUIRE( activeProject == NULL, "ProjectModel ctor ack 1!" );
    activeProject = this;

    try {
        startWork();

        if( *cmdline != '\0' ) {
            Scanner scan( cmdline, len );

            if( !yyparse() ) {
                if( !_hasFile ) {
                    _appFile = new WFileName( "noname.obr" );
                }
            }
        }

        updateModules();        // check to make sure all are up to date
        _needsSave = FALSE;     /* adding modules sets _needsSave */
        endWork();
    }
    catch(...) {                /* make sure endwork called! */
        endWork();
        throw;
    }
}

ProjectModel::ProjectModel( const char * filename )
    :_appFile(new WFileName(filename))
    ,_needsSave(FALSE)
    ,_busyCnt(0)
    ,_hasFile(TRUE)
//-------------------------------------------------
{
    #if 0
    _treeOpCluster[ TreeClassCluster ] = new TreeClassOptions;
    _treeOpCluster[ TreeFuncCluster ] = new TreeFuncOptions;
    #endif

    setAllDefault();
    REQUIRE( activeProject == NULL, "ProjectModel ctor ack 1!" );
    activeProject = this;

    try {
        startWork();

        Scanner        scan( filename );

        yyparse();

        updateModules();        // check to make sure all are up to date
        _needsSave = FALSE;
        endWork();
    }
    catch(...) {                /* make sure endwork called! */
        endWork();
        throw;
    }
}

ProjectModel::~ProjectModel()
//---------------------------
{
    _modules.deleteContents();

    activeProject = NULL;

    delete _appFile;

    #if 0
    delete _treeOpCluster[0];
    delete _treeOpCluster[1];
    #endif

}

bool ProjectModel::saveTo( const char *fileName )
//-----------------------------------------------
{
    if( busy() ) {
        errMessage( "Browse database is still being changed; try again in a moment" );
        return FALSE;
    }

    if( _appFile ) {
        WFileName oldFile( *_appFile );

        _appFile->puts( fileName );

        if( save() ) {
            _hasFile = TRUE;    // _hasFile may be false if "noname.obr"
            return( TRUE );
        } else {
            *_appFile = oldFile;
            return( FALSE );
        }
    } else {
        _appFile = new WFileName( fileName );

        if( save() ) {
            _hasFile = TRUE;
            return( TRUE );
        } else {
            delete _appFile;
            _appFile = NULL;
            return( FALSE );
        }
    }

#if 0
    char * oldPath = _appFile._fileName;
    _appFile._fileName = fileName;
    if( save() ) {
        return TRUE;
    } else {
        _appFile._fileName = oldPath;
        return FALSE;
    }

    fileName = fileName;
    return FALSE;
#endif

}

bool ProjectModel::save()
//-----------------------
{
    if( busy() ) {
        errMessage( "Browse database is still being changed; try again in a moment" );
        return FALSE;
    }

    /*
     * Backup the old file (if any) and create the new file.
     */
    CheckedBufferedFile saveFile( _appFile->gets() );
    WFileName bakName( *_appFile );
    WFileName tmpName( *_appFile );

    bakName.setExt( "BAK" );
    tmpName.renameFile( bakName.gets() );

    try {
        WString str;

        saveFile.open( CheckedFile::WriteText, CheckedFile::UserReadWrite );

        saveEnumStyle( saveFile );
        saveEdgeStyle( saveFile );
        saveTreeDirection( saveFile );
        saveFilterOption( saveFile, Filt_Members );
        saveFilterOption( saveFile, Filt_Methods );
        saveCaseIgnore( saveFile );
        saveSearchType( saveFile );
        saveMagicType( saveFile );
        saveMagicChars( saveFile );
        saveFiles( saveFile );

        saveFile.close();
        _needsSave=FALSE;
        bakName.removeFile();
    }
    catch( ... ) {
        saveFile.close();
        _appFile->removeFile();
        bakName.renameFile( _appFile->gets() );
        return( FALSE );
    }

    return( TRUE );
}

void ProjectModel::saveEnumStyle( CheckedBufferedFile& saveFile )
//---------------------------------------------------------------
{
    WString str;

    str.puts( findTokenString( T_EnumStyle ));
    str.concat( ' ' );
    switch( _enumStyle ) {
        case EV_Octal :
            str.concat( findTokenString( T_Octal ));
            break;
        case EV_decimal :
            str.concat( findTokenString( T_Decimal ));
            break;
        default :
            str.concat( findTokenString( T_Hex ));
            break;
    }
    saveFile.puts( str );
}

void ProjectModel::saveEdgeStyle( CheckedBufferedFile& saveFile )
//---------------------------------------------------------------
{
    WString str;

    str.puts( findTokenString( T_Edges ));
    str.concat( ' ' );
    if( getSmartEdges() ) {
        str.concat( findTokenString( T_SquareEdges ));
    } else {
        str.concat( findTokenString( T_StraightEdges ));
    }
    saveFile.puts( str );
}

void ProjectModel::saveTreeDirection( CheckedBufferedFile& saveFile )
//-------------------------------------------------------------------
{
    WString str;

    str.puts( findTokenString( T_TreeView ));
    str.concat( ' ' );
    if( getTreeDirection() == TreeVertical ) {
        str.concat( findTokenString( T_Vertical ));
    } else {
        str.concat( findTokenString( T_Horizontal ));
    }
    saveFile.puts( str );
}

void ProjectModel::saveFilterOption( CheckedBufferedFile & saveFile, FilterSets filt )
//------------------------------------------------------------------------------------
// writes filter options to saveFile if they have been changed from default
{
    FilterFlags &   filtFlags = _flags[ filt - 1 ];
    unsigned_16     flag;
    bool            modified = FALSE;
    WString         str;
    int             i;
    char * filterStrings[] =
    {
        findTokenString( T_Local ),
        findTokenString( T_Visible ),
        findTokenString( T_All ),
        findTokenString( T_Public ),
        findTokenString( T_Protected ),
        findTokenString( T_Private ),
        findTokenString( T_Static ),
        findTokenString( T_NonStatic ),
        NULL,
        findTokenString( T_NonVirtual ),
        findTokenString( T_Virtual ),
        NULL
    };


    if( filt == Filt_Members ) {
        str.puts( findTokenString( T_MemFilt ));
    } else {
        str.puts( findTokenString( T_MethFilt ));
    }

    flag = 0x0001;

    for( i = 0; filterStrings[ i ] != NULL; i += 1 ) {
        if( filtFlags & flag ) {
            str.concat( "," );
            str.concat( filterStrings[ i ]);
            modified = TRUE;
        }
        flag = (unsigned_16) (flag << 1);
    }

    if( filt == Filt_Methods ) {
        for( i += 1; filterStrings[ i ] != NULL ; i += 1 ) {
            if( filtFlags & flag ) {
                str.concat( "," );
                str.concat( filterStrings[ i ]);
                modified = TRUE;
            }
            flag = (unsigned_16) (flag << 1);
        }
    }

    // ITB -- why not just concat spaces?

    for( i = 0; str[ i ] != '\0'; i += 1 ) {
        if( str[ i ] == ',' ) {
            str.setChar( i, ' ' );
            break;
        }
    }

    if( modified ) {
        saveFile.puts( str );
    }
}

void ProjectModel::saveCaseIgnore( CheckedBufferedFile& saveFile )
//----------------------------------------------------------------
{
    WString str;

    str.puts( findTokenString( T_IgnoreCase ));
    str.concat( ' ' );
    if( CaseIgnore ) {                          // CaseIgnore is from regexp.c
        str.concat( findTokenString( T_On ));
    } else {
        str.concat( findTokenString( T_Off ));
    }
    saveFile.puts( str );
}

void ProjectModel::saveSearchType( CheckedBufferedFile& saveFile )
//----------------------------------------------------------------
{
    WString str;
    str.puts( findTokenString( T_SearchType ));
    str.concat( ' ' );
    #if 0
    if( SearchOptions::anchorRegExp ) {
        str.concat( findTokenString( T_StartsWith ));
    } else {
        str.concat( findTokenString( T_Contains ));
    }
    #endif

    saveFile.puts( str );
}

void ProjectModel::saveMagicType( CheckedBufferedFile& saveFile )
//---------------------------------------------------------------
{
    WString str;

    str.puts( findTokenString( T_Magic ));
    str.concat( ' ' );

    #if 0
    if( MagicFlag ) {                           // MagicFlag is from regexp.c
        str.concat( findTokenString( T_All ));
    } else if( SearchOptions::noMagic ) {
        str.concat( findTokenString( T_None ));
    } else {
        str.concat( findTokenString( T_Some ));
    }
    #endif
    saveFile.puts( str );
}

void ProjectModel::saveMagicChars( CheckedBufferedFile& /*saveFile*/ )
//----------------------------------------------------------------
{
    #if 0
    WString str;
    char * strTmp;

    str.puts( findTokenString( T_MagicChars ));
    str.concat( ' ' );
    str.concat( '"' );
    strTmp = SearchOptions::userMagicString;

    while( *strTmp != '\0' ) {
        switch( *strTmp ) {
            case '"' :
                str.concat( '\\' );
                str.concat( '"' );
                break;
            case '\\' :
                str.concat( '\\' );
                str.concat( '\\' );
                break;
            default :
                str.concat( *strTmp );
                break;
        }
        strTmp++;
    }
    str.concat( '"' );
    saveFile.puts( str );
    #endif
}

void ProjectModel::saveFiles( CheckedBufferedFile& saveFile )
//-----------------------------------------------------------
{
    saveFile = saveFile;

    #if 0
    WString str;
    char * strFile;
    char * strDisabled;
    int i;

    strFile = findTokenString( T_File );
    strDisabled = findTokenString( T_Disabled );

    for( i = 0; i < _enabledFiles.count(); i += 1 ) {
        str.printf( "%s %s", strFile,
            ((WString *) _enabledFiles[ i ])->gets() );
        saveFile.puts( str );
    }

    for( i = 0; i < _disabledFiles.count(); i += 1 ) {
        str.printf( "%s %s", strDisabled,
            ( (WString *) _disabledFiles[ i ])->gets() );
        saveFile.puts( str );
    }
    #endif
}

void ProjectModel::saveModules( CheckedBufferedFile& saveFile )
//-------------------------------------------------------------
{
    WString str;
    char *  strMergeFile = findTokenString( T_MergeFile );

    for( int i = 0; i < _modules.count(); i += 1 ) {
        str.printf( "%s %s", strMergeFile,
                    ((Module*)_modules[i])->fileName().gets() );
        saveFile.puts( str );
    }
}

void ProjectModel::updateModules()
//--------------------------------
{
    for( int i = 0; i < _modules.count(); i += 1 ) {
        ((Module *)_modules[i])->checkTimeDate();
    }
}

void ProjectModel::setSearchAnchor( bool anchor )
//-----------------------------------------------
{
    anchor = anchor;
    // NYI
}
