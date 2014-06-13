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


#include "watcom.h"
#include "wresall.h"
#include "errors.h"
#include "global.h"
#include "semantic.h"
#include "rcrtns.h"


FullStringTable * SemWINNewStringTable( void )
/********************************************/
{
    FullStringTable *   newtable;

    newtable = RCALLOC( sizeof(FullStringTable) );
    if( newtable != NULL ) {
        newtable->Head = NULL;
        newtable->Tail = NULL;
        newtable->next = NULL;
        newtable->lang.lang = DEF_LANG;
        newtable->lang.sublang = DEF_SUBLANG;
    }

    return( newtable );
} /* SemWINNewStringTable */

void SemWINFreeStringTable( FullStringTable * oldtable )
/******************************************************/
{
    FullStringTableBlock *      currblock;
    FullStringTableBlock *      oldblock;

    currblock = oldtable->Head;
    while( currblock != NULL ) {
        ResFreeStringTableBlock( &(currblock->Block) );

        oldblock = currblock;
        currblock = currblock->Next;

        RCFREE( oldblock );
    }

    RCFREE( oldtable );
} /* SemWINFreeStringTable */

static FullStringTableBlock * findStringTableBlock( FullStringTable * table,
                        uint_16 blocknum )
/**************************************************************************/
{
    FullStringTableBlock *          currblock;

    for( currblock = table->Head; currblock != NULL;
                currblock = currblock->Next ) {
        if( currblock->BlockNum == blocknum)  break;
    }

    return( currblock );
} /* findStringTableBlock */

static FullStringTableBlock * newStringTableBlock( void )
/*******************************************************/
{
    FullStringTableBlock *      newblock;

    newblock = RCALLOC( sizeof(FullStringTableBlock) );
    if( newblock != NULL ) {
        newblock->Next = NULL;
        newblock->Prev = NULL;
        newblock->BlockNum = 0;
        newblock->UseUnicode =( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32);
        newblock->Flags = 0;
        ResInitStringTableBlock( &(newblock->Block) );
    }

    return( newblock );
} /* newStringTableBlock */

void SemWINAddStrToStringTable( FullStringTable * currtable,
                            uint_16 stringid, char * string )
/**************************************************************/
{
    FullStringTableBlock *      currblock;
    uint_16                     blocknum;
    uint_16                     stringnum;

    blocknum = stringid >> 4;
    stringnum = stringid & 0x000f;

    currblock = findStringTableBlock( currtable, blocknum );
    if( currblock != NULL ) {
        if( currblock->Block.String[ stringnum ] != NULL ) {
            /* duplicate stringid */
            RcError( ERR_DUPLICATE_STRING_CONST, stringid );
            ErrorHasOccured = TRUE;
        }
    } else {
        currblock = newStringTableBlock();
        currblock->BlockNum = blocknum;
        ResAddLLItemAtEnd( (void **) &(currtable->Head), (void **) &(currtable->Tail), currblock );
    }

    currblock->Block.String[ stringnum ] = WResIDNameFromStr( string );
} /* SemWINAddStrToStringTable */

static void mergeStringTableBlocks( FullStringTableBlock * currblock,
                                FullStringTableBlock * oldblock )
/*******************************************************************/
{
    int     stringid;

    for( stringid = 0; stringid < STRTABLE_STRS_PER_BLOCK; stringid++ ) {
        if( currblock->Block.String[ stringid ] == NULL ) {
            currblock->Block.String[ stringid ] =
                                oldblock->Block.String[ stringid ];
            oldblock->Block.String[ stringid ] = NULL;
        } else {
            if( oldblock->Block.String[ stringid ] != NULL ) {
                RcError( ERR_DUPLICATE_STRING_CONST,
                            ( currblock->BlockNum << 4 ) + stringid );
                ErrorHasOccured = TRUE;
            }
        }
    }
} /* mergeStringTableBlocks */

static void semMergeStringTables( FullStringTable * currtable,
            FullStringTable * oldtable, ResMemFlags newblockflags )
/*****************************************************************/
/* merge oldtable into currtable and free oldtable when done */
/* returns TRUE if there was one or more duplicate entries */
{
    FullStringTableBlock        *currblock;
    FullStringTableBlock        *oldblock;
    FullStringTableBlock        *nextblock;

    /* run through the list of block in oldtable */
    oldblock = oldtable->Head;
    while( oldblock != NULL ) {
        /* find oldblock in currtable if it is there */
        nextblock = oldblock->Next;
        currblock = findStringTableBlock( currtable, oldblock->BlockNum );
        if( currblock == NULL ) {
            /* if oldblock in not in currtable move it there from oldtable */
            ResDeleteLLItem( (void **) &(oldtable->Head), (void **) &(oldtable->Tail), oldblock );
            oldblock->Flags = newblockflags;
            ResAddLLItemAtEnd( (void **) &(currtable->Head), (void **) &(currtable->Tail), oldblock );
        } else {
            /* otherwise move the WSemID's to that block */
            mergeStringTableBlocks( currblock, oldblock );
        }
        oldblock = nextblock;
    }

    SemWINFreeStringTable( oldtable );
} /* semMergeStringTables */

static void setStringTableMemFlags( FullStringTable * currtable,
                                    ResMemFlags flags )
/**************************************************************/
{
    FullStringTableBlock *  currblock;

    for( currblock = currtable->Head; currblock != NULL;
                currblock = currblock->Next ) {
        currblock->Flags = flags;
    }
}

static void addTable( FullStringTable **tables, FullStringTable *newtable ) {
/***************************************************************************/

    while( *tables != NULL ) tables = &( ( *tables )->next );
    *tables = newtable;
    newtable->next = NULL;
}

static FullStringTable *findTableFromLang( FullStringTable *tables,
                                       const WResLangType *lang )
/****************************************************************/
{
    FullStringTable     *cur;

    cur = tables;
    while( cur != NULL ) {
        if( cur->lang.lang == lang->lang
            && cur->lang.sublang == lang->sublang ) break;
        cur = cur->next;
    }
    return( cur );
}

void SemWINMergeStrTable( FullStringTable * currtable, ResMemFlags flags )
/************************************************************************/
{
    FullStringTable     *table;
    const WResLangType  *lang;

    lang = SemGetResourceLanguage();
    currtable->lang = *lang;
    table = findTableFromLang( CurrResFile.StringTable, lang );
    if( table == NULL ) {
        setStringTableMemFlags( currtable, flags );
        addTable( &CurrResFile.StringTable, currtable );
    } else {
        semMergeStringTables( table, currtable, flags );
    }
}

void SemWINMergeErrTable( FullStringTable * currtable, ResMemFlags flags )
/************************************************************************/
{
    FullStringTable     *table;
    const WResLangType  *lang;

    lang = SemGetResourceLanguage();
    currtable->lang = *lang;
    table = findTableFromLang( CurrResFile.ErrorTable, lang );

    if( table == NULL ) {
        setStringTableMemFlags( currtable, flags );
        addTable( &CurrResFile.ErrorTable, currtable );
    } else {
        semMergeStringTables( table, currtable, flags );
    }
}

void SemWINWriteStringTable( FullStringTable * currtable, WResID * type )
/***********************************************************************/
/* write the table identified by currtable as a table of type type and then */
/* free the memory that it occupied */
{
    FullStringTableBlock *  currblock;
    FullStringTable         *tofree;
    WResID *                name;
    int                     error;
    ResLocation             loc;

    while( currtable != NULL ) {
        for( currblock = currtable->Head; currblock != NULL;
                    currblock = currblock->Next ) {
            loc.start = SemStartResource();

            error = ResWriteStringTableBlock( &(currblock->Block),
                            currblock->UseUnicode, CurrResFile.handle );
            if( !error && CmdLineParms.MSResFormat &&
                          CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
                error = ResPadDWord( CurrResFile.handle );
            }
            if( error) {
                RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename,
                         LastWresErrStr() );
                ErrorHasOccured = TRUE;
                SemWINFreeStringTable( currtable );
                return;
            }


            loc.len = SemEndResource( loc.start );
            /* +1 because WResID's can't be 0
             * ( see Microsoft Internal Res Docs) */
            name = WResIDFromNum( currblock->BlockNum + 1 );
            SemWINSetResourceLanguage( &currtable->lang, FALSE );
            SemAddResource( name, type, currblock->Flags, loc );
            RCFREE( name );
        }

        tofree = currtable;
        currtable = currtable->next;
        SemWINFreeStringTable( tofree );
    }
    RCFREE( type );
    return;
}
