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


#include "wpch.hpp"
#include "handle.hpp"
#include "brmfile.hpp"
#include "reindex.hpp"
#include "decl.hpp"
#include "defn.hpp"
#include "scopes.hpp"
#include "strtable.hpp"
#include "types.hpp"
#include "usage.hpp"
#include "macro.hpp"
#include "wbrowse.hpp"
#include "cache.hpp"
#include "debugprt.hpp"
#include "ppops.hpp"

#define FILE_ERR        "Unable to open %s\n"
#define BAD_CHECKSUM    "%s is not a valid .BRM file.\n"
#define EOF_ERR         "Premature EOF at position %d.\n"
#define UNKNOWN_FLAG    "Unknown record flag %02X at position %d.\n"
#define BAD_PCH_FILE    "The pre-compiled header could not be read."

#define MERGE_ERROR(type,arg1,arg2)  \
        DebugPrinter.DebugMsg(type,arg1,arg2)

Browser *               CurrBrowser;


Browser::Browser()
/**************/
{
    _usages = new UsageList;
    _scopes = new ScopeTable;
    _defns = new DefnList;
    _guards = new DependList;
    _strings = new StringTable;
    _types = new TypeList;
    _decls = new DeclList;
    _macros = new MacroList;

    _declIndex = new ReOrdering;
    _scopeIndex = new ReOrdering;
    _typeIndex = new ReOrdering;
    _stringIndex = new ReOrdering;

    _macrosToSyms = new ReOrdering;

    _defnReIndex = new LList<DefnRec>;
    _typeReIndex = new LList<TypeRec>;
    _usageReIndex = new LList<UsageRec>;

    _buffer = NULL;
    _bufLen = 0;
}


Browser::~Browser()
/***************/
{
    delete _usages;
    delete _scopes;
    delete _defns;
    delete _guards;
    delete _strings;
    delete _types;
    delete _decls;
    delete _macros;
    delete _declIndex;
    delete _scopeIndex;
    delete _typeIndex;
    delete _stringIndex;
    delete _macrosToSyms;
    delete _defnReIndex;
    delete _typeReIndex;
    delete _usageReIndex;

    delete[] _buffer;
}


WBool Browser::LoadOldBrowser( CacheInFile *cache )
/*********************************************/
{
    WBool       result;

    CurrBrowser = this;

    // These first four statements have to be in the given order
    // to ensure that various pointer variables are all set properly
    // after the load.
    result = _types->LoadFrom( cache );
    result = result && _decls->LoadFrom( cache );
    result = result && _scopes->LoadFrom( cache );
    result = result && _usages->LoadFrom( cache );

    result = result && _defns->LoadFrom( cache );
    result = result && _guards->LoadFrom( cache );
    result = result && _strings->LoadFrom( cache );
    result = result && _macros->LoadFrom( cache );

    // flush the re-orderings used by various LoadFrom functions.
    if( result ){
        _scopeIndex->Flush();
        _declIndex->Flush();
        _typeIndex->Flush();
    }

    result = result && _scopeIndex->LoadFrom( cache, "ScopeIndex" );
    result = result && _declIndex->LoadFrom( cache, "DeclIndex" );
    result = result && _typeIndex->LoadFrom( cache, "TypeIndex" );
    result = result && _stringIndex->LoadFrom( cache, "StringIndex" );

    CurrBrowser = NULL;

    // TODO:    more error handling
    return result;
}


WBool Browser::SaveBrowser( CacheOutFile *cache )
/*******************************************/
{
    _usages->SaveTo( cache );
    _scopes->SaveTo( cache );
    _defns->SaveTo( cache );
    _guards->SaveTo( cache );
    _strings->SaveTo( cache );
    _types->SaveTo( cache );
    _decls->SaveTo( cache );
    _macros->SaveTo( cache );
    _scopeIndex->SaveTo( cache, "ScopeIndex" );
    _declIndex->SaveTo( cache, "DeclIndex" );
    _typeIndex->SaveTo( cache, "TypeIndex" );
    _stringIndex->SaveTo( cache, "StringIndex" );

    return TRUE;
}


void Browser::CleanUp()
/********************/
{
    {
        DefnRec         *current;

        current = _defnReIndex->First();
        while( current != NULL ){
            current->index = _declIndex->NewId( current->index );
            current = _defnReIndex->Next();
        }
    }
    {
        TypeRec         *current;

        current = _typeReIndex->First();
        while( current != NULL ){
            WAssert( current->typeCode == BRI_TC_Class ||
                    current->typeCode == BRI_TC_Struct ||
                    current->typeCode == BRI_TC_Union ||
                    current->typeCode == BRI_TC_Enum );
            current->ns.symbolID = _declIndex->NewId( current->ns.symbolID );
            current = _typeReIndex->Next();
        }
    }
    {
        UsageRec        *current;

        current = _usageReIndex->First();
        while( current != NULL ){
            if( current->type == BRI_RT_TypeOf ){
                current->typeID = _typeIndex->NewId( current->typeID );
            } else {
                current->targetID = _declIndex->NewId( current->targetID );
            }
            current = _usageReIndex->Next();
        }
    }

    _defnReIndex->Clear();
    _typeReIndex->Clear();
    _usageReIndex->Clear();

    _declIndex->PCHStopUsing();
    _typeIndex->PCHStopUsing();
    _stringIndex->PCHStopUsing();
    _scopeIndex->PCHStopUsing();

    _usages->Reset();
    _defns->AcceptDefns();
}


int Browser::ReadDeclaration(BrmFile &infile)
/******************************************/
{
    static const int    result = sizeof(BRI_SymbolID)
                               + BRI_SIZE_SYMBOLATTRIBUTES
                               + sizeof(BRI_StringID)
                               + sizeof(BRI_TypeID);
    DeclRec *           declRec;
    DeclRec *           firstDecl;
    DeclRec *           otherDecl;
    ScopeRec *          scope;
    BRI_SymbolID        symID;

    if( !_scopes->Ignore() ){
        declRec = new DeclRec;

        // Read the data from the browse file
        infile.Read( &symID, sizeof(BRI_SymbolID), 1 );
        declRec->attribs = (BRI_SymbolAttributes) 0;
        infile.Read( &declRec->attribs, BRI_SIZE_SYMBOLATTRIBUTES, 1 );
        infile.Read( &declRec->nameID, sizeof(BRI_StringID), 1 );
        infile.Read( &declRec->typeID, sizeof(BRI_TypeID), 1 );

        // Re-index vital information now.
        declRec->nameID = _stringIndex->NewId( declRec->nameID );
        declRec->typeID = _typeIndex->NewId( declRec->typeID );

        // Handle symbols with compiler-generated names, like ".@ct"
        scope = _scopes->Current();
        declRec->nameID = renameSym( declRec->nameID,
                                     declRec->typeID,
                                     scope );

        // KLUDGE ALERT!!!!
        // To optimize the search below, each ScopeRec stores an
        // AvlTree of __linked lists of DeclRecs__.  Each linked
        // list contains DeclRecs with the same name...

        // See if this symbol already exists in the current scope
        firstDecl = otherDecl = scope->symbols.Find( declRec->nameID );
        while( otherDecl != NULL ){
            if( otherDecl->typeID == declRec->typeID &&
                (otherDecl->attribs&BRI_SA_TypeMask) ==
                    (declRec->attribs&BRI_SA_TypeMask) ){
                break;
            }
            // Here we use Browser's friend access to DeclRec
            otherDecl = otherDecl->_nextDecl;
        }

        if( otherDecl == NULL ){
            // Add the symbol to the global list and the current scope.
            declRec->index = _declIndex->Change( symID );
            declRec->enclosing = indexOf( scope );
            _decls->Insert( declRec );
            if( firstDecl == NULL ){
                declRec->_nextDecl = NULL;
                scope->symbols.Insert( declRec->nameID, declRec );
            } else {
                declRec->_nextDecl = firstDecl->_nextDecl;
                firstDecl->_nextDecl = declRec;
            }
        } else {
            _declIndex->ChangeTo( symID, otherDecl->index );
            if( otherDecl->attribs & BRI_SA_Temporary ){
                otherDecl->attribs = declRec->attribs;
            }
            delete declRec;
        }
    } else {
        infile.Skip( result );
    }

    return result;
}


int Browser::ReadFile(BrmFile &infile)
/***********************************/
{
    int                 result;
    BRI_StringID        filenameID;

    result = sizeof(BRI_StringID);
    // Read the data from the browse file.
    infile.Read( &filenameID, sizeof(BRI_StringID), 1 );
    // Reindex vital information now.
    filenameID = _stringIndex->NewId( filenameID );
    // Change the current file.
    _usages->SetFile( filenameID );

    _guards->StartFile( filenameID );
    if( !_guards->Ignore() ){
        _state = GUARD_SEARCH;
    }

    return result;
}


int Browser::ReadFileEnd(BrmFile &)
/********************************/
{
    _usages->EndFile();
    if( _guards->Ignore() ){
        _guards->EndFile();
    }
    return 0;
}


int Browser::ReadTemplate(BrmFile &infile)
/***************************************/
{
    static const int    result = sizeof(BRI_StringID);
    BRI_StringID        filenameID;

    // Read the data from the browse file
    infile.Read( &filenameID, sizeof(BRI_StringID), 1 );
    filenameID = _stringIndex->NewId( filenameID );
    // Change the current file.
    _usages->SetFile( filenameID );

    if( !_guards->Ignore() ){
        _guards->StartTemplate();
    }

    return result;
}


int Browser::ReadTemplateEnd(BrmFile &)
/************************************/
{
    _usages->EndFile();
    if( !_guards->Ignore() ){
        _guards->EndTemplate();
    }
    return 0;
}


int Browser::ReadScope(BrmFile &infile)
/************************************/
{
    int result = sizeof(BRI_ScopeID)
               + BRI_SIZE_SCOPETYPE
               + sizeof(BRI_TypeID);
    ScopeRec *          scopeRec;
    ScopeRec *          actualScope;
    BRI_StringID        fnName;
    BRI_TypeID          fnType;

    scopeRec = new ScopeRec;

    // Read enough data to determine the size of the record.
    infile.Read( &scopeRec->index, sizeof(BRI_ScopeID), 1 );
    scopeRec->flags = (BRI_ScopeType) 0;
    infile.Read( &scopeRec->flags, BRI_SIZE_SCOPETYPE, 1 );

    if( scopeRec->flags == BRI_ST_Function ){
        result += sizeof(BRI_StringID);
    }

    if( !_scopes->Ignore() ){
        // Add the scope to the scope list.
        if( scopeRec->flags == BRI_ST_Function ){
            infile.Read( &fnName, sizeof(BRI_StringID), 1 );
            infile.Read( &fnType, sizeof(BRI_TypeID), 1 );
            fnName = _stringIndex->NewId( fnName );
            fnType = _typeIndex->NewId( fnType );

            // Handle compiler names like ".@ct"
            fnName = renameSym( fnName, fnType, _scopes->Current() );

            actualScope = _scopes->OpenScope( scopeRec, fnName, fnType );
        } else {
            infile.Read( &scopeRec->symbolID, sizeof(BRI_SymbolID), 1 );
            actualScope = _scopes->OpenScope( scopeRec );
        }
    } else {
        _scopes->OpenDummyScope();
        if( scopeRec->flags == BRI_ST_Function ){
            infile.Skip( sizeof(BRI_StringID) + sizeof(BRI_TypeID) );
        } else {
            infile.Skip( sizeof(BRI_TypeID) );
        }
    }

    return result;
}


int Browser::ReadScopeEnd(BrmFile &)
/*********************************/
{
    _scopes->CloseScope();
    return 0;
}

int Browser::ReadDelta(BrmFile &infile)
/************************************/
{
    static const int    result = sizeof(uint_8)
                               + sizeof(uint_16);
    int_8               dcol;
    int_16              dline;

    // Read the data from the browse file.
    infile.Read( &dcol, sizeof(int_8), 1 );
    infile.Read( &dline, sizeof(int_16), 1 );

    // Update the current line and column.
    _usages->Delta( dcol, dline );

    return result;
}

int Browser::ReadUsage(BrmFile &infile)
/************************************/
{
    static const int    result = BRI_SIZE_REFERENCETYPE
                               + sizeof(int_8)
                               + sizeof(int_16)
                               + sizeof(BRI_SymbolID);
    UsageRec *          usageRec;
    int_8               dcol;
    int_16              dline;
    uint_32             newId;

    if( !_scopes->Ignore() && !_guards->Ignore() ){
        usageRec = new UsageRec;

        // Read the data from the disk.
        usageRec->type = (BRI_ReferenceType) 0;
        infile.Read( &usageRec->type, BRI_SIZE_REFERENCETYPE, 1 );
        infile.Read( &dcol, sizeof(int_8), 1 );
        infile.Read( &dline, sizeof(int_16), 1 );
        infile.Read( &usageRec->targetID, sizeof(BRI_SymbolID), 1 );

        // Record the current scope.
        usageRec->enclosing = indexOf( _scopes->Current() );

        // Add the reference.
        _usages->Delta( dcol, dline );
        _usages->Insert( usageRec );

        // If the target of this reference has already been processed,
        // set the target id to the correct value.  Otherwise, make
        // a note and get back to it later.

        if( usageRec->type == BRI_RT_TypeOf ){
            newId = _typeIndex->NewId( usageRec->typeID );
        } else {
            newId = _declIndex->NewId( usageRec->targetID );
        }
        if( newId != 0x0 ){
            usageRec->typeID = (BRI_TypeID) newId;
        } else {
            _usageReIndex->Append( usageRec );
        }
    } else {

        // Read the data from the browse file.
        infile.Skip( BRI_SIZE_REFERENCETYPE );
        infile.Read( &dcol, sizeof(int_8), 1 );
        infile.Read( &dline, sizeof(int_16), 1 );
        infile.Skip( sizeof(BRI_SymbolID) );

        // Update the current line and column.
        _usages->Delta( dcol, dline );
    }

    return result;
}


int Browser::ReadString(BrmFile &infile)
/*************************************/
{
    static const int    result = sizeof(BRI_StringID)
                               + sizeof(uint_32);
    uint_32             stringLen;
    BRI_StringID        oldStringID;
    BRI_StringID        newStringID;
    char                *buffer;

    // Read the data from the browse file.
    infile.Read( &oldStringID, sizeof(BRI_StringID), 1 );
    infile.Read( &stringLen, sizeof(uint_32), 1 );
    if( _bufLen < stringLen ){
        delete[] _buffer;
        _buffer = new uint_8[ stringLen ];
        _bufLen = stringLen;
    }
    buffer = (char *) _buffer;
    infile.Read( buffer, 1, stringLen );

    // Add the string to the string table.
    newStringID = _strings->Insert( buffer );
    _stringIndex->ChangeTo( oldStringID, newStringID );

    return result + stringLen;
}


int Browser::ReadType(BrmFile &infile)
/***********************************/
{
    static const int    result = sizeof(BRI_TypeID)
                               + BRI_SIZE_TYPECODE
                               + sizeof(uint_32);
    TypeRec *           typeRec = new TypeRec;
    TypeRec *           actualType;
    uint_32             numOps;
    uint_32 *           ops;
    int                 i;
    int                 mustReIndex = 0;

    // Read the data from the browse file.
    infile.Read( &typeRec->index, sizeof(BRI_TypeID), 1 );
    typeRec->typeCode = (BRI_TypeCode) 0;
    infile.Read( &typeRec->typeCode, BRI_SIZE_TYPECODE, 1 );

    infile.Read( &numOps, sizeof(uint_32), 1 );
    WAssert( numOps > 0 );
    if( numOps * sizeof(uint_32) > _bufLen ){
        delete[] _buffer;
        _buffer = new uint_8[ numOps * sizeof( uint_32 ) ];
        _bufLen = numOps * sizeof( uint_32 );
    }
    ops = (uint_32 *) _buffer;
    infile.Read( ops, sizeof(uint_32), numOps );

    // Oy!  Does C++ have a type system or what?
    switch( typeRec->typeCode ){
        case BRI_TC_None:
        break;

        case BRI_TC_BaseType:
            typeRec->bt.baseType = (BRI_BaseTypes) ops[0];
        break;

        case BRI_TC_Modifier:
            typeRec->fp.flags = ops[0];
            typeRec->fp.parent = _typeIndex->NewId( ops[1] );
        break;

        case BRI_TC_Pointer:
        case BRI_TC_Reference:
        case BRI_TC_TypeDef:
            typeRec->p.parent = _typeIndex->NewId( ops[0] );
        break;

        case BRI_TC_PtrToMember:
            typeRec->cm.host = _typeIndex->NewId( ops[0] );
            typeRec->cm.member = _typeIndex->NewId( ops[1] );
        break;

        case BRI_TC_Array:
            typeRec->se.size = ops[0];
            typeRec->se.element = _typeIndex->NewId( ops[1] );
        break;

        case BRI_TC_Function:
            typeRec->ra.args = new BRI_TypeID[numOps];
            typeRec->ra.numArgs = numOps;
            for( i=0; i<numOps; i++ ){
                typeRec->ra.args[i] = _typeIndex->NewId( ops[i] );
            }
        break;

        case BRI_TC_Class:
        case BRI_TC_Struct:
        case BRI_TC_Union:
        case BRI_TC_Enum:
            if( _scopes->Ignore() ){
                delete typeRec;
                return result + numOps*sizeof( uint_32 );
            }
            typeRec->ns.nameID = _stringIndex->NewId( ops[0] );
            typeRec->ns.symbolID = (BRI_SymbolID) ops[1];
            mustReIndex = 1;
        break;

        case BRI_TC_BitField:
            typeRec->w.width = ops[0];
        break;
    }

    // Add the type to the type list.
    actualType = _types->AddType( typeRec );
    if( actualType == typeRec ){
        if( mustReIndex ){
            _typeReIndex->Append( typeRec );
        }
    } else {
        if( mustReIndex && actualType->ns.symbolID == BRI_NULL_ID ){
            actualType->ns.symbolID = typeRec->ns.symbolID;
            _typeReIndex->Append( actualType );
        }
        delete typeRec;
    }

    return result + numOps * sizeof(uint_32);
}


int Browser::ReadGuard(BrmFile &infile)
/************************************/
{
    static const int    result = BRI_SIZE_GUARDTYPES
                               + sizeof(BRI_StringID)
                               + sizeof( uint_32 )
                               + sizeof( uint_32 );
    BRI_GuardTypes      type;
    BRI_StringID        stringID;
    BRI_SymbolID        symID;
    uint_32             numParams;
    uint_32             length;
    uint_8              *buffer;
    UsageRec            *usage;
    DefnRec             *defn;
    DependValRec        *guard;

    // Read the data from the browse file.
    type = (BRI_GuardTypes) 0;
    infile.Read( &type, BRI_SIZE_GUARDTYPES, 1 );
    infile.Read( &stringID, sizeof(BRI_StringID), 1 );
    infile.Read( &numParams, sizeof(uint_32), 1 );
    infile.Read( &length, sizeof(uint_32), 1 );

    if( length > 0 ){
        if( length > _bufLen ){
            delete[] _buffer;
            _buffer = new uint_8[ length ];
            _bufLen = length;
        }
        buffer = _buffer;
        infile.Read( buffer, sizeof(uint_8), length );
    } else {
        buffer = NULL;
    }

    stringID = _stringIndex->NewId( stringID );

    switch( type ){
        case BRI_GT_Value:
            if( _state == GUARD_SEARCH ){
                guard = new DependValRec;
                guard->macroNameID = stringID;
                guard->numParams = numParams;
                guard->length = length;
                if( length > 0 ){
                    guard->defn = new uint_8[ length ];
                    memcpy( guard->defn, buffer, length );
                } else {
                    guard->defn = NULL;
                }
                _guards->AddDepMacValue( guard );
            }
        break;

        case BRI_GT_Defined:
            if( _state == GUARD_SEARCH ){
                _guards->AddDepDefined( stringID );
            }
        break;

        case BRI_GT_NotDefined:
            if( _state == GUARD_SEARCH ){
                _guards->AddDepUndefed( stringID );
            }
        break;

        case BRI_GT_Declaration:
            if( !_macros->HaveSeen( stringID ) ){
                symID = _declIndex->StealId();
                _macros->AddMacroDecl( stringID, symID );
                _macrosToSyms->ChangeTo( stringID, symID );
            } else {
                symID = _macrosToSyms->NewId( stringID );
            }
            defn = new DefnRec;
            defn->index = symID;
            defn->column = _usages->GetColumn();
            defn->line = _usages->GetLine();
            defn->filenameID = _usages->GetFileName();
            _defns->AddDefinition( defn );
        break;

        case BRI_GT_RefValue:
        case BRI_GT_RefUndef:
            usage = new UsageRec;
            usage->type = BRI_RT_Macro;
            usage->targetID = _macrosToSyms->NewId( stringID );
            usage->enclosing = indexOf( _scopes->Head() );
            _usages->Insert( usage );
        break;
    }

    return result + length;
}


int Browser::ReadDefinition(BrmFile &infile)
/*****************************************/
{
    static const int    result = sizeof(uint_32)
                               + sizeof(uint_32)
                               + sizeof(BRI_StringID)
                               + sizeof(BRI_SymbolID);
    DefnRec *           defnRec;
    uint_32             newId;

    if( !_scopes->Ignore() ){
        defnRec = new DefnRec;

        // Read the data from the browse file.
        infile.Read( &defnRec->column, sizeof(uint_32), 1 );
        infile.Read( &defnRec->line, sizeof(uint_32), 1 );
        infile.Read( &defnRec->filenameID, sizeof(BRI_StringID), 1 );
        infile.Read( &defnRec->index, sizeof(BRI_SymbolID), 1 );

        // Re-index vital information now.
        defnRec->filenameID = _stringIndex->NewId( defnRec->filenameID );

        // If the symbol being defined has already been processed, set
        // the index number appropriately.  Otherwise, make a note and do
        // it later.
        newId = _declIndex->NewId( defnRec->index );
        if( newId != 0x0 ){
            defnRec->index = newId;
            _defns->AddDefinition( defnRec );
        } else {
            _defnReIndex->Append( defnRec );
            _defns->AddTempDefinition( defnRec );
        }
    } else {
        infile.Skip( result );
    }
    return result;
}


int Browser::ReadPCHInclude(BrmFile &infile)
/*****************************************/
{
    int                 result = sizeof(BRI_StringID);
    BRI_StringID        fileNameID;
    WBool               seenBefore = FALSE;

    if( !_scopes->Ignore() && !_guards->Ignore() ){
        infile.Read( &fileNameID, sizeof(BRI_StringID), 1 );
        fileNameID = _stringIndex->NewId( fileNameID );
        WAssert( fileNameID != BRI_NULL_ID );
        seenBefore = _declIndex->HaveSeenPCH( fileNameID );
        if( seenBefore ){
            _declIndex->PCHUse( fileNameID );
            _typeIndex->PCHUse( fileNameID );
            _stringIndex->PCHUse( fileNameID );
            _scopeIndex->PCHUse( fileNameID );
        } else {
            BrmFilePch  pchFile;
            char const  *fileName;
            fileName = _strings->Lookup( fileNameID );
            WAssert( fileName != NULL );

            _declIndex->PCHRecord( fileNameID );
            _typeIndex->PCHRecord( fileNameID );
            _stringIndex->PCHRecord( fileNameID );
            _scopeIndex->PCHRecord( fileNameID );
            pchFile.Open( fileName );
            if( !pchFile ){
                MERGE_ERROR( FILE_ERR, fileName, 0 );
                result = -1;
            } else {
                if( !ProcessFile( pchFile ) ){
                    result = -1;
                }
                pchFile.Close();
            }
            _declIndex->PCHStopRecording();
            _typeIndex->PCHStopRecording();
            _stringIndex->PCHStopRecording();
            _scopeIndex->PCHStopRecording();
        }
    } else {
        infile.Skip( result );
    }
    return result;
}


WBool Browser::ProcessFile(BrmFile &infile)
/****************************************/
{
    BRI_Header  briHeader;
    int         fileLen;
    int         delta;
    int         filePos;
    int         recordFlag;

    // Get the file length, for error checking.
    fileLen = infile.Size();

    // Read the browse file header.
    infile.Read( &briHeader, sizeof( briHeader ), 1 );
    if( briHeader.magic != BRI_MAGIC || briHeader.file_len != fileLen ){
        MERGE_ERROR( BAD_CHECKSUM, 0, 0 );
        return FALSE;
    }

    // Read the records from the browse file.
    filePos = sizeof( briHeader );
    while( filePos < fileLen ){
        recordFlag = infile.GetChar();
        if( recordFlag == EOF ){
            MERGE_ERROR( EOF_ERR, fileLen, 0 );
            CurrBrowser = NULL;
            return FALSE;
        }
        filePos ++;
        if( _state == GUARD_SEARCH ){
            if( recordFlag != BRI_Rec_String &&
                recordFlag != BRI_Rec_Guard ){
                _guards->IncludeFile();
                _state = NORMAL;
            }
        }
        switch( recordFlag ){
            case BRI_Rec_Declaration:
                filePos += ReadDeclaration(infile);
                briHeader.num_declaration--;
            break;

            case BRI_Rec_File:
                filePos += ReadFile(infile);
                briHeader.num_file--;
            break;

            case BRI_Rec_FileEnd:
                filePos += ReadFileEnd(infile);
            break;

            case BRI_Rec_Template:
                filePos += ReadTemplate(infile);
                briHeader.num_template--;
            break;

            case BRI_Rec_TemplateEnd:
                filePos += ReadTemplateEnd(infile);
            break;

            case BRI_Rec_Scope:
                filePos += ReadScope(infile);
                briHeader.num_scope--;
            break;

            case BRI_Rec_ScopeEnd:
                filePos += ReadScopeEnd(infile);
            break;

            case BRI_Rec_Delta:
                filePos += ReadDelta(infile);
                briHeader.num_delta--;
            break;

            case BRI_Rec_Usage:
                filePos += ReadUsage(infile);
                briHeader.num_usage--;
            break;

            case BRI_Rec_String:
                filePos += ReadString(infile);
                briHeader.num_string--;
            break;

            case BRI_Rec_Type:
                filePos += ReadType(infile);
                briHeader.num_type--;
            break;

            case BRI_Rec_Guard:
                filePos += ReadGuard(infile);
                briHeader.num_guard--;
            break;

            case BRI_Rec_Definition:
                filePos += ReadDefinition(infile);
                briHeader.num_definition--;
            break;

            case BRI_Rec_PCHInclude:
                delta = ReadPCHInclude(infile);
                if( delta < 0 ){
                    MERGE_ERROR( BAD_PCH_FILE, 0, 0 );
                    return FALSE;
                }
                filePos += delta;
                briHeader.num_pch--;
            break;

            default:
                MERGE_ERROR( UNKNOWN_FLAG, recordFlag, filePos );
                continue;
        }
    }

    return TRUE;
}


WBool Browser::AddFile(char const *filePath)
/*****************************************/
{
    WBool       result;
    BrmFile     infile;

    // Set the global (Browser *) for the benefit of the other modules.
    CurrBrowser = this;

    // Open the file.
    infile.Open( filePath );
    if( !infile ){
        MERGE_ERROR( FILE_ERR, filePath, 0 );
        return FALSE;
    }

    _state = NORMAL;

    result = ProcessFile( infile );

    // Update any information that had to wait until the entire
    // browse file was read.
    if( result ){
        CleanUp();
    }

    // Flush the re-orderings for the next call to AddFile.
    _declIndex->Flush();
    _scopeIndex->Flush();
    _typeIndex->Flush();
    _stringIndex->Flush();

    // Reset the global (Browser *) just to be picky.
    CurrBrowser = NULL;

    return result;
}


int Browser::DefnNameIter(char const *name, BCallBack cb, void * cookie )
/**********************************************************************/
{
    int                 result = 0;
    BRI_StringID        nameID;
    DeclRec             *decl;
    DefnRec             *defn;
    SourceLocn          locn;

    nameID = _strings->Lookup( name );
    if( nameID != BRI_NULL_ID ){
        locn.name = _strings->Lookup( nameID );
        decl = _decls->First();
        while( decl != NULL ){
            if( decl->nameID == nameID ){
                defn = _defns->FirstDefnFor( decl->index );
                while( defn != NULL ){
                    locn.filePath = _strings->Lookup( defn->filenameID );
                    locn.line = defn->line;
                    locn.col = defn->column;
                    (*cb)( &locn, decl, cookie );
                    result += 1;
                    defn = _defns->NextDefn();
                }
            }
            decl = _decls->Next();
        }
    }

    return result;
}


int Browser::DefnAtIter(SourceLocn *locn, BCallBack cb, void * cookie)
/*******************************************************************/
{
    int                 result = 0;
    char *              filename;
    BRI_StringID        fnameID;
    BRI_StringID        nameID;
    int                 fnameLen;
    UsageRec *          usage;
    DeclRec *           symbol = NULL;
    TypeRec *           type = NULL;
    DefnRec *           defn = NULL;
    SourceLocn          newLocn;
    enum {
        EXACT,
        TO_RIGHT,
        TO_LEFT
    }                   searchState;

    // Get a filename in lower case.  All filenames in browse files
    // are stored in lower case.
    fnameLen = strlen(locn->filePath)+1;
    filename = new char[fnameLen];
    memcpy( filename, locn->filePath, fnameLen*sizeof(char) );
    strlwr(filename);

    // Translate the given strings into id numbers.
    fnameID = _strings->Lookup( filename );
    nameID = _strings->Lookup( locn->name );

    // Try to find a reference at the given line and column.
    // If no reference exists there, check to the right first,
    // and then to the left.
    usage = _usages->At( fnameID, locn->line, locn->col );
    if( usage != NULL ){
        searchState = EXACT;
    } else {
        usage = _usages->NextAtRight();
        searchState = TO_RIGHT;
    }

    for(;;){
        if( usage == NULL ){
            if( searchState == TO_RIGHT && result == 0 ){
                searchState = TO_LEFT;
                usage = _usages->NextAtLeft();
                continue;
            } else {
                break;
            }
        }

        // Check the name of each valid reference, and if it matches
        // call the callback function.

        if( usage->type != BRI_RT_TypeOf ){
            symbol = _decls->Lookup( usage->targetID );
            if( symbol != NULL && symbol->nameID == nameID ){
                newLocn.name = locn->name;
                defn = _defns->FirstDefnFor( symbol->index );
                while( defn != NULL ){
                    newLocn.filePath = _strings->Lookup( defn->filenameID );
                    newLocn.line = defn->line;
                    newLocn.col = defn->column;
                    (*cb)(&newLocn, symbol, cookie);
                    result += 1;
                    defn = _defns->NextDefn();
                }
            }
        } else {
            type = _types->Lookup( usage->typeID );
            while( type != NULL && type->typeCode == BRI_TC_TypeDef ){
                type = _types->Lookup( type->p.parent );
            }
            if( type != NULL ){
                switch( type->typeCode ){
                    case BRI_TC_Class:
                    case BRI_TC_Struct:
                    case BRI_TC_Union:
                    case BRI_TC_Enum:
                        symbol = _decls->Lookup( type->ns.symbolID );
                    break;

                    default:
                        symbol = NULL;
                    break;
                }
            }
            if( symbol != NULL && symbol->nameID == nameID ){
                newLocn.name = locn->name;

                defn = _defns->FirstDefnFor( symbol->index );
                while( defn != NULL ){
                    newLocn.filePath = _strings->Lookup( defn->filenameID );
                    newLocn.line = defn->line;
                    newLocn.col = defn->column;
                    (*cb)(&newLocn, symbol, cookie);
                    result += 1;
                    defn = _defns->NextDefn();
                }
            }
        }

        switch( searchState ){
            case EXACT:         usage = _usages->NextAt(); break;
            case TO_RIGHT:      usage = _usages->NextAtRight(); break;
            case TO_LEFT:       usage = _usages->NextAtLeft(); break;
        }
    }

    return result;
}


BRI_StringID Browser::renameSym( BRI_StringID nameID,
                                BRI_TypeID typeID,
                                ScopeRec *enclosing )
/***************************************************/
{
    BRI_StringID        result;
    char const          *name;
    TypeRec             *type;
    int                 strLen;
    int                 op;

    result = nameID;
    name = _strings->Lookup( nameID );
    if( name != NULL && name[1] == '@' ){
        op = NameOfOp( name );
        if( op >= 0 ){
            result = _strings->Insert( OpNames[op] );
        } else {
            switch( op ){
            case CTOR_NAME:
                WAssert( enclosing != NULL );
                type = _types->Lookup( enclosing->typeID );
                WAssert( type != NULL );
                result = type->ns.nameID;
                break;

            case DTOR_NAME:
                WAssert( enclosing != NULL );
                type = _types->Lookup( enclosing->typeID );
                WAssert( type != NULL );
                name = _strings->Lookup( type->ns.nameID );
                strLen = strlen( name ) + 2;
                if( _bufLen < strLen ){
                    delete[] _buffer;
                    _bufLen = strLen;
                    _buffer = new uint_8[strLen];
                }
                ((char *) _buffer)[0] = '~';
                memcpy( ((char *) _buffer)+1, name, strLen-1 );
                result = _strings->Insert( (char *) _buffer );
                break;

            case CONV_NAME:
                {
                    WString     typeName( WTEXT("operator ") );

                    type = _types->Lookup( typeID );
                    WAssert(type != NULL && type->typeCode==BRI_TC_Function);
                    writeType( type->ra.args[0], typeName );
                    strLen = typeName.GetAnsiLength() + 1;
                    if( _bufLen < strLen ){
                        delete[] _buffer;
                        _bufLen = strLen;
                        _buffer = new uint_8[strLen];
                    }
                    memcpy( (char *) _buffer, typeName.GetAnsiText(), strLen );
                    result = _strings->Insert( (char *) _buffer );
                }
                break;
            }
        }
    }

    return result;
}


int Browser::DefnOfIter( DeclRec const *decl, BCallBack cb, void * cookie )
/*****************************************************************/
{
    DefnRec     *defn;
    SourceLocn  locn;
    int         result = 0;

    locn.name = _strings->Lookup( decl->nameID );

    defn = _defns->FirstDefnFor( decl->index );
    while( defn != NULL ){
        locn.filePath = _strings->Lookup( defn->filenameID );
        locn.line = defn->line;
        locn.col = defn->column;
        (*cb)( &locn, decl, cookie );
        result += 1;
        defn = _defns->NextDefn();
    }

    return result;
}


int Browser::FileClassesIter( const char *filepath, BCallBack cb,
                             void * cookie )
/**************************************************************/
{
    int                 result = 0;
    int                 fnameLen;
    char                *lowerPath;
    BRI_StringID        fnameID;
    DefnRec             *defn;
    DeclRec             *sym;
    ScopeRec            *scope;
    TypeRec             *type;
    SourceLocn          locn;
    AvlTree<DeclRec>    results;

    // Get a filename in lower case.  All filenames in browse files
    // are stored in lower case.
    fnameLen = strlen(filepath)+1;
    lowerPath = new char[fnameLen];
    memcpy( lowerPath, filepath, fnameLen*sizeof(char) );
    strlwr( lowerPath );

    // Translate the given strings into id numbers.
    fnameID = _strings->Lookup( lowerPath );
    if( fnameID == BRI_NULL_ID ){
        return 0;
    }

    defn = _defns->First();
    while( defn != NULL ){
        if( defn->filenameID == fnameID ){
            sym = _decls->Lookup( defn->index );
            WAssert( sym != NULL );
            if( (sym->attribs&BRI_SA_TypeMask) != BRI_SA_Class ){
                scope = _scopes->Lookup( sym->enclosing );
                WAssert( scope != NULL );
                while( scope->flags != BRI_ST_Class &&
                       scope->flags != BRI_ST_File ){
                    scope = scope->parent;
                }
                if( scope->flags == BRI_ST_Class ){
                    type = _types->Lookup( scope->typeID );
                    WAssert( type != NULL );
                    sym = _decls->Lookup( type->ns.symbolID );
                } else {
                    sym = NULL;
                }
            }
            if( sym != NULL && results.Find( indexOf(sym) ) == NULL ){
                locn.name = _strings->Lookup( sym->nameID );
                (*cb)( &locn, sym, cookie );
                result += 1;
                results.Insert( indexOf(sym), sym );
            }
        }
        defn = _defns->Next();
    }

    return result;
}


int Browser::ReferenceIter( DeclRec const *sym, BRI_ReferenceType refType,
                           BCallBack cb, void * cookie )
/*****************************************************************/
{
    UsageRec    *usage;
    SourceLocn  locn;
    int         result = 0;

    usage = _usages->First();
    while( usage != NULL ){
        if( refType == BRI_RT_None ||
            (usage->type == refType && usage->targetID == sym->index) ){
            locn.filePath = _strings->Lookup( _usages->GetFileName() );
            locn.line = _usages->GetLine();
            locn.col = _usages->GetColumn();
            if( refType != BRI_RT_InheritFrom ){
                (*cb)( &locn, NULL, cookie );
            } else {
                DeclRec *       source = NULL;
                ScopeRec *      scope = _scopes->Lookup( usage->enclosing );
                TypeRec *       classType;

                if( scope != NULL || scope->flags == BRI_ST_Class ){
                    classType = _types->Lookup( scope->typeID );
                    if( classType != NULL ){
                        source = _decls->Lookup( classType->ns.symbolID );
                    }
                }
                (*cb)( &locn, source, cookie );
            }
            result += 1;
        }
        usage = _usages->Next();
    }
    return result;
}


int Browser::SymbolIter( BRI_SymbolAttributes symType,
                           BCallBack cb, void * cookie )
/******************************************************/
{
    DeclRec                     *decl;
    SourceLocn                  locn;
    BRI_SymbolAttributes        declType;
    int                         result = 0;

    decl = _decls->First();
    while( decl != NULL ){
        declType = (BRI_SymbolAttributes)(decl->attribs & BRI_SA_TypeMask);
        if( symType == BRI_SA_None || declType == symType ){
            locn.name = _strings->Lookup( decl->nameID );
            (*cb)( &locn, decl, cookie );
            result += 1;
        }
        decl = _decls->Next();
    }
    return result;
}


int Browser::UsedFnsIter( BCallBack cb, void * cookie )
/****************************************************/
{
    DeclRec                     *decl;
    UsageRec                    *reference;
    ScopeRec                    *scope;
    SourceLocn                  locn;
    AvlTree<DeclRec>            usedFns;
    int                         result = 0;

    reference = _usages->First();
    while( reference != NULL ){
        if( reference->type == BRI_RT_Function ){
            // Add the function being called to the list.
            decl = _decls->Lookup( reference->targetID );
            if( decl != NULL ){
                usedFns.Insert( reference->targetID, decl );
            }
            // Find the calling function, and add it to the list.
            scope = _scopes->Lookup( reference->enclosing );
            while( scope != NULL && scope->flags != BRI_ST_Function ){
                scope = scope->parent;
            }
            if( scope != NULL ){
                decl = _decls->Lookup( scope->symbolID );
                if( decl != NULL ){
                    usedFns.Insert( scope->symbolID, decl );
                }
            }
        }
        reference = _usages->Next();
    }

    decl = usedFns.First();
    while( decl != NULL ){
        locn.name = _strings->Lookup( decl->nameID );
        (*cb)( &locn, decl, cookie );
        result += 1;
        decl = usedFns.Next();
    }
    return result;
}


int Browser::UserClassesIter( BCallBack cb, void * cookie )
/********************************************************/
{
    DeclRec                     *decl;
    SourceLocn                  locn;
    BRI_SymbolAttributes        declType;
    int                         result = 0;

    decl = _decls->First();
    while( decl != NULL ){
        declType = (BRI_SymbolAttributes)(decl->attribs & BRI_SA_TypeMask);
        if( declType == BRI_SA_Class ){
            if( !_declIndex->IsPCHIndex( indexOf( decl ) ) ){
                locn.name = _strings->Lookup( decl->nameID );
                (*cb)( &locn, decl, cookie );
                result += 1;
            }
        }
        decl = _decls->Next();
    }
    return result;
}


int Browser::CallingFnsIter( DeclRec const *sym, BCallBack cb, void * cookie )
/*********************************************************************/
{
    UsageRec            *reference;
    DeclRec             *caller;
    ScopeRec            *scope;
    SourceLocn          locn;
    BRI_SymbolID        id;
    AvlTree<DeclRec>    results;
    int                 result = 0;

    id = sym->index;
    reference = _usages->First();
    while( reference != NULL ){
        if( reference->type == BRI_RT_Function &&
            reference->targetID == id ){
            scope = _scopes->Lookup( reference->enclosing );
            while( scope != NULL && scope->flags != BRI_ST_Function ){
                scope = scope->parent;
            }
            if( scope != NULL && results.Find( scope->symbolID ) == NULL ){
                caller = _decls->Lookup( scope->symbolID );
                locn.name = _strings->Lookup( caller->nameID );
                (*cb)( &locn, caller, cookie );
                results.Insert( scope->symbolID, caller );
                result += 1;
            }
        }
        reference = _usages->Next();
    }
    return result;
}


int Browser::CalledFnsIter( DeclRec const *sym, BCallBack cb, void * cookie )
/********************************************************************/
{
    UsageRec            *reference;
    DeclRec             *callee;
    ScopeRec            *topScope;
    BRI_ScopeID         current;
    ScopeRec            *curScope;
    SourceLocn          locn;
    AvlTree<DeclRec>    results;
    WBool               isChild;
    int                 result = 0;

    topScope = _scopes->FindFuncScope( sym->index );
    if( topScope == NULL ){
        return 0;
    }
    current = (BRI_ScopeID) 0;
    isChild = FALSE;
    reference = _usages->First();
    while( reference != NULL ){
        if( reference->enclosing != current ){
            current = reference->enclosing;
            curScope = _scopes->Lookup( current );
            isChild = FALSE;
            while( curScope != NULL ){
                if( curScope == topScope ){
                    isChild = TRUE;
                    break;
                }
                curScope = curScope->parent;
            }
        }
        if( isChild && reference->type == BRI_RT_Function &&
            results.Find( reference->targetID ) == NULL ){
            callee = _decls->Lookup( reference->targetID );
            if( callee != NULL ){
                locn.name = _strings->Lookup( callee->nameID );
                (*cb)( &locn, callee, cookie );
                result += 1;
            }
            results.Insert( reference->targetID, callee );
        }
        reference = _usages->Next();
    }
    return result;
}


int Browser::ParentClassesIter( DeclRec const *sym, BCallBack cb, void *cookie )
/***********************************************************************/
{
    UsageRec            *reference;
    DeclRec             *parent;
    ScopeRec            *classScope;
    BRI_ScopeID         classScopeID;
    SourceLocn          locn;
    AvlTree<DeclRec>    results;
    int                 result=0;

    classScope = _scopes->FindClassScope( sym->typeID );
    if( classScope == NULL ){
        return 0;
    }
    classScopeID = indexOf( classScope );
    reference = _usages->First();
    while( reference != NULL ){
        if( reference->type == BRI_RT_InheritFrom &&
            reference->enclosing == classScopeID &&
            results.Find( reference->targetID ) == NULL ){
            parent = _decls->Lookup( reference->targetID );
            if( parent != NULL ){
                locn.name = _strings->Lookup( parent->nameID );
                (*cb)( &locn, parent, cookie );
                result += 1;
            }
            results.Insert( reference->targetID, parent );
        }
        reference = _usages->Next();
    }
    return result;
}


int Browser::ChildClassesIter( DeclRec const *sym, BCallBack cb, void *cookie )
/**********************************************************************/
{
    UsageRec            *reference;
    DeclRec             *child;
    TypeRec             *childType;
    ScopeRec            *classScope;
    SourceLocn          locn;
    BRI_SymbolID        id;
    AvlTree<DeclRec>    results;
    int                 result = 0;

    id = sym->index;
    reference = _usages->First();
    while( reference != NULL ){
        if( reference->type == BRI_RT_InheritFrom &&
            reference->targetID == id &&
            results.Find( reference->enclosing ) == NULL ){
            classScope = _scopes->Lookup( reference->enclosing );
            if( classScope != NULL ){
                WAssert( classScope->flags == BRI_ST_Class );
                childType = _types->Lookup( classScope->typeID );
                if( childType != NULL ){
                    child = _decls->Lookup( childType->ns.symbolID );
                    if( child != NULL ){
                        locn.name = _strings->Lookup( child->nameID );
                        (*cb)( &locn, child, cookie );
                        results.Insert( reference->enclosing, child );
                        result += 1;
                    }
                }
            }
        }
        reference = _usages->Next();
    }

    return result;
}


int Browser::MemberFnsIter( DeclRec const *sym, BCallBack cb, void *cookie )
/*******************************************************************/
{
    int         result = 0;
    ScopeRec    *classScope;
    DeclRec     *memberFn;
    SourceLocn  locn;

    classScope = _scopes->FindClassScope( sym->typeID );
    WAssert( classScope != NULL );
    memberFn = classScope->symbols.First();
    while( memberFn != NULL ){
        if( (memberFn->attribs&BRI_SA_TypeMask) == BRI_SA_Function ){
            locn.name = _strings->Lookup( memberFn->nameID );
            (*cb)( &locn, memberFn, cookie );
            result += 1;
        }
        memberFn = memberFn->_nextDecl;
        if( memberFn == NULL ){
            memberFn = classScope->symbols.Next();
        }
    }
    return result;
}


WString Browser::UnScopedName( DeclRec const *sym )
/******************************************/
{
    WString     result;

    result.SetAnsiText( _strings->Lookup( sym->nameID ) );
    if( sym->attribs|BRI_SA_TempInst ){
        ScopeRec        *scope;
        DeclRec         *tempArg;
        int             count;

        scope = _scopes->Lookup( sym->enclosing );
        while( scope != NULL ){
            if( scope->flags == BRI_ST_TemplateParm ){
                break;
            }
            scope = scope->parent;
        }
        if( scope != NULL ){
            count = 0;
            tempArg = scope->symbols.First();
            while( tempArg != NULL ){
                if( (tempArg->attribs&BRI_SA_TypeMask)==BRI_SA_Typedef ){
                    if( count == 0 ){
                        result.Concat( WTEXT( "<" ) );
                    } else {
                        result.Concat( WTEXT( "," ) );
                    }
                    writeType( tempArg->typeID, result );
                    count += 1;
                }
                tempArg = scope->symbols.Next();
            }
            if( count > 0 ){
                result.Concat( WTEXT( ">" ) );
            }
        }
    }
    return result;
}


WString Browser::ScopedName( DeclRec const *sym )
/****************************************/
{
    WString     result;
    WString     temp;
    ScopeRec    *current;
    TypeRec     *type;
    DeclRec     *classSym;

    result = UnScopedName( sym );
    current = _scopes->Lookup( sym->enclosing );
    if( (sym->attribs & BRI_SA_TypeMask) == BRI_SA_Class && current != NULL ){
        current = current->parent;
    }
    while( current != NULL ){
        if( current->flags == BRI_ST_Class ){
            type = _types->Lookup( current->typeID );
            WAssert( type != NULL );
            classSym = _decls->Lookup( type->ns.symbolID );
            WAssert( classSym != NULL );
            temp = UnScopedName( classSym );
            temp += WTEXT( "::" );
            temp += result;
            result = temp;
        }
        current = current->parent;
    }
    return result;
}


WString Browser::NameOfScope( DeclRec const *sym )
/*****************************************/
{
    WString     result;
    WString     temp;
    ScopeRec    *current;
    TypeRec     *type;
    DeclRec     *classSym;

    current = _scopes->Lookup( sym->enclosing );
    if( (sym->attribs & BRI_SA_TypeMask) == BRI_SA_Class && current != NULL ){
        current = current->parent;
    }
    while( current != NULL ){
        if( current->flags == BRI_ST_Class ){
            type = _types->Lookup( current->typeID );
            WAssert( type != NULL );
            classSym = _decls->Lookup( type->ns.symbolID );
            WAssert( classSym != NULL );
            temp = UnScopedName( classSym );
            if( !result.GetEmpty() ){
                temp += WTEXT( "::" );
            }
            temp += result;
            result = temp;
        }
        current = current->parent;
    }

    return result;
}


WString &Browser::writeType( BRI_TypeID id, WString &current,
                            DeclRec const * symbol )
/**********************************************************/
{
    TypeRec             *typePtr;
    int                 i;
    char const          *buf;

    typePtr = _types->Lookup( id );
    WAssert( typePtr != NULL );
    switch( typePtr->typeCode ){
    case BRI_TC_BaseType:
        switch( typePtr->bt.baseType ){
        case BRI_TYP_BOOL:
            current.Concat( WTEXT("bool") );
            break;

        case BRI_TYP_CHAR:
            current.Concat( WTEXT("char") );
            break;

        case BRI_TYP_SCHAR:
            current.Concat( WTEXT("signed char") );
            break;

        case BRI_TYP_UCHAR:
            current.Concat( WTEXT("unsigned char") );
            break;

        case BRI_TYP_WCHAR:
            current.Concat( WTEXT("wchar_t") );
            break;

        case BRI_TYP_SSHORT:
            current.Concat( WTEXT("short") );
            break;

        case BRI_TYP_USHORT:
            current.Concat( WTEXT("unsigned short") );
            break;

        case BRI_TYP_SINT:
            current.Concat( WTEXT("int") );
            break;

        case BRI_TYP_UINT:
            current.Concat( WTEXT("unsigned") );
            break;

        case BRI_TYP_SLONG:
            current.Concat( WTEXT("long") );
            break;

        case BRI_TYP_ULONG:
            current.Concat( WTEXT("unsigned long") );
            break;

        case BRI_TYP_SLONG64:
            current.Concat( WTEXT("__int64") );
            break;

        case BRI_TYP_ULONG64:
            current.Concat( WTEXT("unsigned __int64") );
            break;

        case BRI_TYP_FLOAT:
            current.Concat( WTEXT("float") );
            break;

        case BRI_TYP_DOUBLE:
            current.Concat( WTEXT("double") );
            break;

        case BRI_TYP_LONG_DOUBLE:
            current.Concat( WTEXT("long double") );
            break;

        case BRI_TYP_VOID:
            current.Concat( WTEXT("void") );
            break;

        case BRI_TYP_DOT_DOT_DOT:
            current.Concat( WTEXT("...") );
            break;

        case BRI_TYP_GENERIC:
            current.Concat( WTEXT("?") );
            break;
        }
        if( symbol != NULL ){
            current.Concat( WTEXT( " " ) );
            current.Concat( UnScopedName( symbol ) );
        }
        break;

    case BRI_TC_Modifier:
        if( typePtr->fp.flags & BRI_TCM_CONST ){
            current.Concat( WTEXT("const ") );
        } else {
            current.Concat( WTEXT("") );
        }
        writeType( typePtr->fp.parent, current, symbol );
        break;

    case BRI_TC_Pointer:
        writeType( typePtr->p.parent, current );
        current.Concat( WTEXT(" *") );
        if( symbol != NULL ){
            current.Concat( WTEXT(" ") );
            current.Concat( UnScopedName( symbol ) );
        }
        break;

    case BRI_TC_PtrToMember:
        current.Concat( WTEXT("(") );
        writeType( typePtr->cm.host, current );
        current.Concat( WTEXT("::*") );
        if( symbol != NULL ){
            current.Concat( UnScopedName( symbol ) );
        }
        current.Concat( WTEXT(")(") );
        writeType( typePtr->cm.member, current );
        current.Concat( WTEXT(")") );
        break;

    case BRI_TC_Reference:
        writeType( typePtr->p.parent, current );
        current.Concat( WTEXT(" &") );
        if( symbol != NULL ){
            current.Concat( WTEXT(" ") );
            current.Concat( UnScopedName( symbol ) );
        }
        break;

    case BRI_TC_Array:
        writeType( typePtr->se.element, current );
        if( symbol != NULL ){
            current.Concat( WTEXT(" ") );
            current.Concat( UnScopedName( symbol ) );
        }
        current.Concatf( WTEXT("[%d]"), typePtr->se.size );
        break;

    case BRI_TC_Function:
        writeType( typePtr->ra.args[0], current );
        if( symbol != NULL ){
            current.Concat( WTEXT(" ") );
            current.Concat( UnScopedName( symbol ) );
            current.Concat( WTEXT("(") );
        } else {
            current.Concat( WTEXT("()(") );
        }
        for( i=1; i<typePtr->ra.numArgs; i++ ){
            writeType( typePtr->ra.args[i], current );
            if( i < typePtr->ra.numArgs-1 ){
                current.Concat( WTEXT(",") );
            }
        }
        current.Concat( WTEXT(")") );
        break;

    case BRI_TC_Class:
    case BRI_TC_Struct:
    case BRI_TC_Union:
    case BRI_TC_Enum:
        buf = _strings->Lookup( typePtr->ns.nameID );
        if( buf == NULL ){
            buf = "<ANONYMOUS>";
        }
        current.Concat( buf );
        if( symbol != NULL ){
            current.Concat( WTEXT(" ") );
            current.Concat( UnScopedName( symbol ) );
        }
        break;

    case BRI_TC_TypeDef:
        // current.Concat( WTEXT("typedef ") );
        writeType( typePtr->p.parent, current );
        if( symbol != NULL ){
            current.Concat( WTEXT(" ") );
            current.Concat( UnScopedName( symbol ) );
        }
        break;

    case BRI_TC_BitField:
        current.Concat( WTEXT("int ") );
        if( symbol != NULL ){
            current.Concat( WTEXT(" ") );
            current.Concat( UnScopedName( symbol ) );
        }
        current.Concatf( WTEXT(": %d"), typePtr->w.width );
        break;
    }

    return current;
}


WString Browser::TypedName( DeclRec const *sym )
/**************************************/
{
    WString                     result;
    BRI_SymbolAttributes        symType;

    symType = (BRI_SymbolAttributes) (sym->attribs & BRI_SA_TypeMask);
    if( symType == BRI_SA_Class ||
        symType == BRI_SA_Struct ||
        symType == BRI_SA_Enum ){
        writeType( sym->typeID, result );
    } else {
        writeType( sym->typeID, result, sym );
    }

    return result;
}


WString Browser::NameOfType( DeclRec const *sym )
/**************************************/
{
    WString     result;

    writeType( sym->typeID, result );

    return result;
}


WStringArray const * DebugMessages()
/**********************************/
{
    return DebugPrinter.GetMessages();
}


BRI_SymbolAttributes SymAttrs( DeclRec const *sym )
/*************************************************/
{
    if( sym == NULL ){
        return BRI_SA_None;
    } else {
        return sym->attribs;
    }
}


/*
        From this point onward, everything is just a wrapper around
        a Browser member function.
*/


BHandle NewBrowser()
/*****************/
{
    return new Browser;
}


BHandle LoadOldBrowser(char const *filepath)
/*****************************************/
{
    Browser     *result;
    CacheInFile cache;
    int         couldLoad;

    if( filepath == NULL ){
        return NULL;
    }

    result = new Browser;
    couldLoad = cache.Open(filepath);
    couldLoad = couldLoad && result->LoadOldBrowser(&cache);
    if( !couldLoad ){
        delete result;
        result = NULL;
    }

    return result;
}


WBool SaveBrowser(BHandle m, char const *filepath)
/***********************************************/
{
    CacheOutFile        cache;
    WBool               couldSave;

    if( filepath == NULL ){
        return 0;
    }
    couldSave = cache.Open(filepath);
    couldSave = couldSave && m->SaveBrowser(&cache);

    return couldSave;
}


void DestroyBrowser(BHandle m)
/***************************/
{
    if( m != NULL ){
        delete m;
    }
}


WBool AddFile( BHandle m, char const *filePath )
/**********************************************/
{
    if( m == NULL ){
        return 0;
    } else {
        return m->AddFile(filePath);
    }
}


int DefnNameIter( BHandle m, char const *name, BCallBack cb, void * cookie )
/**************************************************************************/
{
    if( m == NULL || name == NULL || cb == NULL ){
        return 0;
    } else {
        return m->DefnNameIter( name, cb, cookie );
    }
}


int DefnAtIter( BHandle m, SourceLocn *locn, BCallBack cb, void * cookie)
/********************************************************************/
{
    if( m == NULL || locn == NULL || cb == NULL ){
        return 0;
    } else {
        return m->DefnAtIter(locn,cb,cookie);
    }
}


int DefnOfIter( BHandle m, DeclRec const *sym, BCallBack cb, void * cookie )
/*******************************************************************/
{
    if( m == NULL || cb == NULL ){
        return 0;
    } else {
        return m->DefnOfIter(sym,cb,cookie);
    }
}


int FileClassesIter( BHandle m, const char *filepath, BCallBack cb,
                     void * cookie )
/*****************************************************************/
{
    if( m == NULL || filepath == NULL || cb == NULL ){
        return 0;
    } else {
        return m->FileClassesIter( filepath, cb, cookie );
    }
}


int ReferenceIter( BHandle m, DeclRec const *sym, BRI_ReferenceType refType,
                     BCallBack cb, void * cookie )
/*******************************************************************/
{
    if( m == NULL || cb == NULL ){
        return 0;
    } else {
        return m->ReferenceIter(sym,refType,cb,cookie);
    }
}


int SymbolIter( BHandle m, BRI_SymbolAttributes symType,
                  BCallBack cb, void * cookie )
/********************************************************/
{
    if( m == NULL || cb == NULL ){
        return 0;
    } else {
        return m->SymbolIter(symType,cb,cookie);
    }
}


int UsedFnsIter( BHandle m, BCallBack cb, void * cookie )
/*******************************************************/
{
    if( m == NULL || cb == NULL ){
        return 0;
    } else {
        return m->UsedFnsIter(cb,cookie);
    }
}


int UserClassesIter( BHandle m, BCallBack cb, void *cookie )
/**********************************************************/
{
    if( m == NULL || cb == NULL ){
        return 0;
    } else {
        return m->UserClassesIter(cb,cookie);
    }
}


int CallingFnsIter( BHandle m, DeclRec const *sym, BCallBack cb, void *cookie )
/**********************************************************************/
{
    if( m == NULL || cb == NULL || sym == NULL ){
        return 0;
    } else {
        return m->CallingFnsIter( sym, cb, cookie );
    }
}


int CalledFnsIter( BHandle m, DeclRec const *sym, BCallBack cb, void *cookie )
/*********************************************************************/
{
    if( m == NULL || cb == NULL || sym == NULL ){
        return 0;
    } else {
        return m->CalledFnsIter( sym, cb, cookie );
    }
}


int ParentClassesIter( BHandle m, DeclRec const *sym, BCallBack cb, void *cookie )
/**************************************************************************/
{
    if( m == NULL || cb == NULL || sym == NULL ||
        (sym->attribs&BRI_SA_TypeMask) != BRI_SA_Class ){
        return 0;
    } else {
        return m->ParentClassesIter( sym, cb, cookie );
    }
}


int ChildClassesIter( BHandle m, DeclRec const *sym, BCallBack cb, void *cookie )
/*************************************************************************/
{
    if( m == NULL || cb == NULL || sym == NULL ||
        (sym->attribs&BRI_SA_TypeMask) != BRI_SA_Class ){
        return 0;
    } else {
        return m->ChildClassesIter( sym, cb, cookie );
    }
}


int MemberFnsIter( BHandle m, DeclRec const *sym, BCallBack cb, void *cookie )
/**********************************************************************/
{
    if( m == NULL || cb == NULL || sym == NULL ||
        (sym->attribs&BRI_SA_TypeMask) != BRI_SA_Class ){
        return 0;
    } else {
        return m->MemberFnsIter( sym, cb, cookie );
    }
}


WString UnScopedName( BHandle m, DeclRec const *sym )
/*********************************************/
{
    if( m == NULL || sym == NULL ){
        return WString( WTEXT("") );
    } else {
        return m->UnScopedName( sym );
    }
}


WString ScopedName( BHandle m, DeclRec const *sym )
/*******************************************/
{
    if( m == NULL || sym == NULL ){
        return WString( WTEXT("") );
    } else {
        return m->ScopedName( sym );
    }
}


WString NameOfScope( BHandle m, DeclRec const *sym )
/********************************************/
{
    if( m == NULL || sym == NULL ){
        return WString( WTEXT("") );
    } else {
        return m->NameOfScope( sym );
    }
}


WString TypedName( BHandle m, DeclRec const *sym )
/*****************************************/
{
    if( m == NULL || sym == NULL ){
        return WString( WTEXT("") );
    } else {
        return m->TypedName( sym );
    }
}


WString NameOfType( BHandle m, DeclRec const *sym )
/*****************************************/
{
    if( m == NULL || sym == NULL ){
        return WString( WTEXT("") );
    } else {
        return m->NameOfType( sym );
    }
}
