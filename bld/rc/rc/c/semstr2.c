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
* Description:  OS/2 String and Message table related semantic actions.
*
****************************************************************************/


#include "watcom.h"
#include "global.h"
#include "rcerrors.h"
#include "semantic.h"
#include "semantc2.h"
#include "rcrtns.h"
#include "rccore.h"


static bool ResOS2WriteStringTableBlock( StringTableBlock *currblock, FILE *fp, uint_32 codepage )
/************************************************************************************************/
{
    int         stringid;
    bool        error;
    WResIDName  *name;

    // Write string table codepage
    error = ResWriteUint16( codepage, fp );
    if( error )
        return( error );

    error = false;
    for( stringid = 0; stringid < STRTABLE_STRS_PER_BLOCK && !error; stringid++ ) {
        name = currblock->String[stringid];
        if( name == NULL ) {
            // Write an empty string
            error = ResWriteUint16( 1, fp );
        } else {
            size_t  len;
            // The string can't be longer than 255 chars
            len = name->NumChars + 1;
            if( len > 255 )
                len = 255;
            error = ResWriteUint8( len, fp );
            if( !error )
                error = ResWriteStringLen( name->Name, false, fp, len - 1 );
            // The terminating NULL is not stored in the table, need to add it now
            if( !error ) {
                error = ResWriteUint8( 0, fp );
            }
        }
    }

    return( error );
} /* ResOS2WriteStringTableBlock */

FullStringTable *SemOS2NewStringTable( void )
/*******************************************/
{
    FullStringTable     *newtable;

    newtable = RESALLOC( sizeof( FullStringTable ) );
    if( newtable != NULL ) {
        newtable->Head = NULL;
        newtable->Tail = NULL;
        newtable->next = NULL;
        newtable->lang.lang = DEF_LANG;
        newtable->lang.sublang = DEF_SUBLANG;
    }

    return( newtable );
} /* SemOS2NewStringTable */

static void SemOS2FreeStringTable( FullStringTable *oldtable )
/*****************************************************/
{
    FullStringTableBlock        *currblock;
    FullStringTableBlock        *nextblock;

    for( currblock = oldtable->Head; currblock != NULL; currblock = nextblock ) {
        nextblock = currblock->Next;
        ResFreeStringTableBlock( &(currblock->Block) );
        RESFREE( currblock );
    }
    RESFREE( oldtable );
} /* SemOS2FreeStringTable */

static FullStringTableBlock *findStringTableBlock( FullStringTable *table,
                        uint_16 blocknum )
/************************************************************************/
{
    FullStringTableBlock        *currblock;

    for( currblock = table->Head; currblock != NULL; currblock = currblock->Next ) {
        if( currblock->BlockNum == blocknum ) {
            break;
        }
    }

    return( currblock );
} /* findStringTableBlock */

static FullStringTableBlock *newStringTableBlock( void )
/******************************************************/
{
    FullStringTableBlock        *newblock;

    newblock = RESALLOC( sizeof( FullStringTableBlock ) );
    if( newblock != NULL ) {
        newblock->Next = NULL;
        newblock->Prev = NULL;
        newblock->BlockNum = 0;
        newblock->UseUnicode = (CmdLineParms.TargetOS == RC_TARGET_OS_WIN32);
        newblock->Flags = 0;
        newblock->codePage = 850;
        ResInitStringTableBlock( &(newblock->Block) );
    }

    return( newblock );
} /* newStringTableBlock */

void SemOS2AddStrToStringTable( FullStringTable *currtable,
                                 uint_16 stringid, char *string )
/***************************************************************/
{
    FullStringTableBlock        *currblock;
    uint_16                     blocknum;
    uint_16                     stringnum;

    blocknum = stringid >> 4;
    stringnum = stringid & 0x000f;

    currblock = findStringTableBlock( currtable, blocknum );
    if( currblock != NULL ) {
        if( currblock->Block.String[stringnum] != NULL ) {
            /* duplicate stringid */
            RcError( ERR_DUPLICATE_STRING_CONST, stringid );
            ErrorHasOccured = true;
        }
    } else {
        currblock = newStringTableBlock();
        currblock->BlockNum = blocknum;
        ResAddLLItemAtEnd( (void **)&(currtable->Head), (void **)&(currtable->Tail), currblock );
    }

    currblock->Block.String[stringnum] = WResIDNameFromStr( string );
} /* SemOS2AddStrToStringTable */

static void mergeStringTableBlocks( FullStringTableBlock *currblock,
                                FullStringTableBlock *oldblock )
/*******************************************************************/
{
    int     stringid;

    for( stringid = 0; stringid < STRTABLE_STRS_PER_BLOCK; stringid++ ) {
        if( currblock->Block.String[stringid] == NULL ) {
            currblock->Block.String[stringid] =
                                oldblock->Block.String[stringid];
            oldblock->Block.String[stringid] = NULL;
        } else {
            if( oldblock->Block.String[stringid] != NULL ) {
                RcError( ERR_DUPLICATE_STRING_CONST,
                            ( currblock->BlockNum << 4 ) + stringid );
                ErrorHasOccured = true;
            }
        }
    }
} /* mergeStringTableBlocks */

static void semMergeStringTables( FullStringTable *currtable,
            FullStringTable *oldtable, ResMemFlags newblockflags,
            uint_32 codepage )
/****************************************************************/
/* merge oldtable into currtable and free oldtable when done */
/* returns TRUE if there was one or more duplicate entries */
{
    FullStringTableBlock        *currblock;
    FullStringTableBlock        *oldblock;
    FullStringTableBlock        *nextblock;

    /* run through the list of block in oldtable */
    for( oldblock = oldtable->Head; oldblock != NULL; oldblock = nextblock ) {
        /* find oldblock in currtable if it is there */
        nextblock = oldblock->Next;
        currblock = findStringTableBlock( currtable, oldblock->BlockNum );
        if( currblock == NULL ) {
            /* if oldblock in not in currtable move it there from oldtable */
            ResDeleteLLItem( (void **)&(oldtable->Head), (void **)&(oldtable->Tail), oldblock );
            oldblock->Flags = newblockflags;
            oldblock->codePage = codepage;
            ResAddLLItemAtEnd( (void **)&(currtable->Head), (void **)&(currtable->Tail), oldblock );
        } else {
            /* otherwise move the WSemID's to that block */
            mergeStringTableBlocks( currblock, oldblock );
        }
    }

    SemOS2FreeStringTable( oldtable );
} /* semMergeStringTables */

static void setStringTableFlags( FullStringTable *currtable,
                                ResMemFlags flags, uint_32 codepage )
/*******************************************************************/
{
    FullStringTableBlock    *currblock;

    for( currblock = currtable->Head; currblock != NULL;
                currblock = currblock->Next ) {
        currblock->Flags = flags;
        currblock->codePage = codepage;
    }
}

static void addTable( FullStringTable **tables, FullStringTable *newtable )
/*************************************************************************/
{
    while( *tables != NULL )
        tables = &( ( *tables )->next );

    *tables = newtable;
    newtable->next = NULL;
}

static FullStringTable *findTable( FullStringTable *tables )
/**********************************************************/
{
    return( tables );
}

void SemOS2MergeStrTable( FullStringTable *currtable, ResMemFlags flags, uint_32 codepage )
/*****************************************************************************************/
{
    FullStringTable     *table;

    table = findTable( CurrResFile.StringTable );
    if( table == NULL ) {
        setStringTableFlags( currtable, flags, codepage );
        addTable( &CurrResFile.StringTable, currtable );
    } else {
        semMergeStringTables( table, currtable, flags, codepage );
    }
}

void SemOS2MergeMsgTable( FullStringTable *currtable, ResMemFlags flags )
/***********************************************************************/
{
    FullStringTable     *table;

    table = findTable( CurrResFile.ErrorTable );
    if( table == NULL ) {
        setStringTableFlags( currtable, flags, SemOS2DefaultCodepage() );
        addTable( &CurrResFile.ErrorTable, currtable );
    } else {
        semMergeStringTables( table, currtable, flags, SemOS2DefaultCodepage() );
    }
}

void SemOS2WriteStringTable( FullStringTable *currtable, WResID *type )
/****************************************************************************/
/* write the table identified by currtable as a table of type type and then */
/* free the memory that it occupied */
{
    FullStringTableBlock    *currblock;
    FullStringTable         *nexttable;
    WResID                  *name;
    bool                    error;
    ResLocation             loc;

    for( ; currtable != NULL; currtable = nexttable ) {
        nexttable = currtable->next;
        for( currblock = currtable->Head; currblock != NULL; currblock = currblock->Next ) {
            loc.start = SemStartResource();

            error = ResOS2WriteStringTableBlock( &(currblock->Block),
                                                 CurrResFile.fp,
                                                 currblock->codePage );
            if( error ) {
                RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, LastWresErrStr() );
                ErrorHasOccured = true;
                SemOS2FreeStringTable( currtable );
                return;
            }

            loc.len = SemEndResource( loc.start );
            /* +1 because WResID's can't be 0 */
            name = WResIDFromNum( currblock->BlockNum + 1 );
            SemAddResource( name, type, currblock->Flags, loc );
            RESFREE( name );
        }
        SemOS2FreeStringTable( currtable );
    }
    RESFREE( type );
    return;
}
