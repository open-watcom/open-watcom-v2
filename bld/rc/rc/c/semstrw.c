/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include "watcom.h"
#include "global.h"
#include "rcerrors.h"
#include "semantic.h"
#include "semantcw.h"
#include "rcrtns.h"
#include "rccore.h"


static void semFreeStringTable( FullStringTable *table )
/******************************************************/
{
    FullStringTableBlock    *currblock;
    FullStringTableBlock    *nextblock;

    for( currblock = table->Head; currblock != NULL; currblock = nextblock ) {
        nextblock = currblock->Next;
        ResFreeStringTableBlock( &(currblock->Block) );
        MemFree( currblock );
    }

    MemFree( table );
} /* semFreeStringTable */

static FullStringTableBlock *findStringTableBlock( FullStringTable *table,
                        uint_16 blocknum )
/************************************************************************/
{
    FullStringTableBlock    *currblock;

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
    FullStringTableBlock    *newblock;

    newblock = MemAllocSafe( sizeof( FullStringTableBlock ) );
    newblock->Next = NULL;
    newblock->Prev = NULL;
    newblock->BlockNum = 0;
    newblock->iswin32 = CmdLineParms.iswin32;
    newblock->res_flags = DEFAULT_FLAGS_NONE;
    ResInitStringTableBlock( &(newblock->Block) );

    return( newblock );
} /* newStringTableBlock */

FullStringTable *SemWINAddStrToStringTable( FullStringTable *table,
                            uint_16 stringid, char *string )
/**********************************************************/
{
    FullStringTableBlock        *currblock;
    uint_16                     blocknum;
    uint_16                     stringnum;

    if( table == NULL ) {
        table = MemAllocSafe( sizeof( FullStringTable ) );
        table->Head = NULL;
        table->Tail = NULL;
        table->next = NULL;
        table->lang.lang = DEF_LANG;
        table->lang.sublang = DEF_SUBLANG;
    }

    blocknum = stringid >> 4;
    stringnum = stringid & 0x000f;

    currblock = findStringTableBlock( table, blocknum );
    if( currblock != NULL ) {
        if( currblock->Block.String[stringnum] != NULL ) {
            /*
             * duplicate stringid
             */
            RcError( ERR_DUPLICATE_STRING_CONST, stringid );
            ErrorHasOccured = true;
        }
    } else {
        currblock = newStringTableBlock();
        currblock->BlockNum = blocknum;
        ResAddLLItemAtEnd( (void **)&(table->Head), (void **)&(table->Tail), currblock );
    }
    currblock->Block.String[stringnum] = WResStringIDNameFromStr( string );
    return( table );

} /* SemWINAddStrToStringTable */

static void mergeStringTableBlocks( FullStringTableBlock *currblock,
                                FullStringTableBlock *oldblock )
/******************************************************************/
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

static void semMergeStringTables( FullStringTable *table,
            FullStringTable *old_table, ResMemFlags res_flags )
/**************************************************************
 * merge old_table into table and free old_table when done
 * returns TRUE if there was one or more duplicate entries
 */
{
    FullStringTableBlock        *currblock;
    FullStringTableBlock        *oldblock;
    FullStringTableBlock        *nextblock;

    /*
     * run through the list of block in old_table
     */
    for( oldblock = old_table->Head; oldblock != NULL; oldblock = nextblock ) {
        /*
         * find oldblock in table if it is there
         */
        nextblock = oldblock->Next;
        currblock = findStringTableBlock( table, oldblock->BlockNum );
        if( currblock == NULL ) {
            /*
             * if oldblock in not in table move it there from old_table
             */
            ResDeleteLLItem( (void **)&(old_table->Head), (void **)&(old_table->Tail), oldblock );
            oldblock->res_flags = res_flags;
            ResAddLLItemAtEnd( (void **)&(table->Head), (void **)&(table->Tail), oldblock );
        } else {
            /*
             * otherwise move the WSemID's to that block
             */
            mergeStringTableBlocks( currblock, oldblock );
        }
    }

    semFreeStringTable( old_table );
} /* semMergeStringTables */

static void setStringTableMemFlags( FullStringTable *table,
                                    ResMemFlags res_flags )
/*************************************************************/
{
    FullStringTableBlock    *currblock;

    for( currblock = table->Head; currblock != NULL; currblock = currblock->Next ) {
        currblock->res_flags = res_flags;
    }
}

static void addTable( FullStringTable **tables, FullStringTable *table )
/**********************************************************************/
{
    while( *tables != NULL )
        tables = &( ( *tables )->next );
    *tables = table;
    table->next = NULL;
}

static FullStringTable *findTableFromLang( FullStringTable *tables,
                                       const WResLangType *lang )
/****************************************************************/
{
    FullStringTable     *cur;

    for( cur = tables; cur != NULL; cur = cur->next ) {
        if( cur->lang.lang == lang->lang
          && cur->lang.sublang == lang->sublang ) {
            break;
        }
    }
    return( cur );
}

void SemWINMergeStrTable( FullStringTable *table, ResMemFlags res_flags )
/***********************************************************************/
{
    FullStringTable     *old_table;
    const WResLangType  *lang;

    lang = SemGetResourceLanguage();
    table->lang = *lang;
    old_table = findTableFromLang( CurrResFile.StringTable, lang );
    if( old_table == NULL ) {
        setStringTableMemFlags( table, res_flags );
        addTable( &CurrResFile.StringTable, table );
    } else {
        semMergeStringTables( old_table, table, res_flags );
    }
}

void SemWINMergeErrTable( FullStringTable *table, ResMemFlags res_flags )
/***********************************************************************/
{
    FullStringTable     *old_table;
    const WResLangType  *lang;

    lang = SemGetResourceLanguage();
    table->lang = *lang;
    old_table = findTableFromLang( CurrResFile.ErrorTable, lang );
    if( old_table == NULL ) {
        setStringTableMemFlags( table, res_flags );
        addTable( &CurrResFile.ErrorTable, table );
    } else {
        semMergeStringTables( old_table, table, res_flags );
    }
}

void SemWINWriteStringTable( FullStringTable *table, WResID *type_id )
/*********************************************************************
 * write the table identified by table as a table of type type and then
 * free the memory that it occupied
 */
{
    FullStringTableBlock    *currblock;
    FullStringTable         *nexttable;
    WResID                  *res_id;
    bool                    error;
    ResLocation             loc;

    for( ; table != NULL; table = nexttable ) {
        nexttable = table->next;
        for( currblock = table->Head; currblock != NULL; currblock = currblock->Next ) {
            loc.start = SemStartResource();

            error = ResWriteStringTableBlock( &(currblock->Block), currblock->iswin32, CurrResFile.fp );
            if( !error
              && CmdLineParms.MSResFormat
              && CmdLineParms.iswin32 ) {
                error = ResWritePadDWord( CurrResFile.fp );
            }
            if( error ) {
                RcError( ERR_WRITTING_RES, CurrResFile.filename, LastWresErrStr() );
                ErrorHasOccured = true;
                semFreeStringTable( table );
                return;
            }

            loc.len = SemEndResource( loc.start );
            /*
             * +1 because WResID's can't be 0
             * ( see Microsoft Internal Res Docs )
             */
            res_id = WResIDFromNum( currblock->BlockNum + 1 );
            SemWINSetResourceLanguage( &table->lang, false );
            SemAddResource( res_id, type_id, currblock->res_flags, loc );
            MemFree( res_id );
        }
        semFreeStringTable( table );
    }
    MemFree( type_id );
}
