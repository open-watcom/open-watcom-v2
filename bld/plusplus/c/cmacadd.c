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


#include "plusplus.h"
#include "memmgr.h"
#include "preproc.h"
#include "ring.h"
#include "pcheader.h"
#include "stats.h"
#include "name.h"
#include "brinfo.h"


#define MACRO_HASH_SIZE         NAME_HASH
#define MAC_SEGMENT_SIZE        (0x8000 - sizeof( char * ))
#define MAC_SEGMENT_LIMIT       (MAC_SEGMENT_SIZE - 2)
#define HASH_TABLE_SIZE         (MACRO_HASH_SIZE * sizeof( macroHashTable ))

typedef struct macro_seg_list MACRO_SEG_LIST;
struct macro_seg_list {
    MACRO_SEG_LIST  *next;
    char            segment[ MAC_SEGMENT_SIZE ];
};

typedef struct pch_delmac PCH_DELMAC;
struct pch_delmac {
    PCH_DELMAC          *next;
    unsigned            hash;
    char                name[1];
};

static unsigned         macroSegmentLimit;  // last free byte in MacroSegment
static MEPTR            *macroHashTable;    // hash table [ MACRO_HASH_SIZE ]
static MEPTR            beforeIncludeChecks;// #undef macros defined before first #include
static MACRO_SEG_LIST   *macroSegmentList;  // pointer to list of macro segments
static PCH_DELMAC       *macroPCHDeletes;   // macros to delete after PCH load
static unsigned         undefCount;         // # macros #undef'd

ExtraRptCtr( macro_segments );
ExtraRptCtr( macros_defined );
ExtraRptCtr( macros_defined_with_parms );
ExtraRptCtr( macros_redefined );
ExtraRptSpace( macro_space );

#define macroSizeAlign( s )     _RoundUp((s), sizeof( int ))

static void *macroAllocateInSeg( // ALLOCATE WITHIN A SEGMENT
    unsigned size )             // - size
{
    void *retn;                 // - return location

    ExtraRptSpaceAdd( macro_space, size );
    retn = MacroOffset;
    size = macroSizeAlign( size );
    MacroOffset += size;
    macroSegmentLimit -= size;
    return( retn );
}


static void macroAllocSegment(   // ALLOCATE MACRO SEGMENT
    unsigned minimum )          // - minimum size req'd
{
    MACRO_SEG_LIST  *macroSegment;

    if( minimum > MAC_SEGMENT_LIMIT ) {
        CErr1( ERR_OUT_OF_MACRO_MEMORY );
        CSuicide();
    }
    macroSegment = RingAlloc( &macroSegmentList, sizeof( MACRO_SEG_LIST ) );
    MacroOffset =  macroSegment->segment;
    macroSegmentLimit = MAC_SEGMENT_LIMIT;
    ExtraRptIncrementCtr( macro_segments );
}

static void macroStorageAlloc( void )
{
    macroHashTable = CMemAlloc( HASH_TABLE_SIZE );
    memset( macroHashTable, 0, HASH_TABLE_SIZE );
    macroSegmentList = NULL;
    beforeIncludeChecks = NULL;
    undefCount = 0;
    MacroOffset = NULL;
    macroSegmentLimit = 0;
}

static void macroStorageFree( MACRO_SEG_LIST **seglist, MEPTR **hashtab )
{
    CMemFreePtr( hashtab );
    RingFree( seglist );
}

static void macroStorageRestart( MACRO_SEG_LIST **old_seglist, MEPTR **old_hashtab )
{
    *old_seglist = macroSegmentList;
    *old_hashtab = macroHashTable;
    macroStorageAlloc();
    ExtraRptZeroCtr( macro_segments );
    ExtraRptZeroCtr( macros_defined );
    ExtraRptZeroCtr( macros_defined_with_parms );
    ExtraRptZeroCtr( macros_redefined );
    ExtraRptZeroSpace( macro_space );
}

void MacroStorageInit(          // INITIALIZE FOR MACRO STORAGE
    void )
{
    macroStorageAlloc();
    ExtraRptRegisterCtr( &macro_segments, "macro segments" );
    ExtraRptRegisterCtr( &macros_defined, "macros defined" );
    ExtraRptRegisterCtr( &macros_defined_with_parms, "macros defined with parms" );
    ExtraRptRegisterCtr( &macros_redefined, "macros redefined" );
    ExtraRptRegisterSpace( &macro_space, "macros space" );
    macroPCHDeletes = NULL;
}

static int macroCompare(        // COMPARE TWO MACROS TO SEE IF IDENTICAL
    MEPTR m1,                   // - macro #1
    MEPTR m2 )                  // - macro #2
{
    if( m1->macro_len  != m2->macro_len )   return( -1 );
    if( m1->macro_defn != m2->macro_defn )  return( -1 );
    if( m1->parm_count != m2->parm_count )  return( -1 );
    return( memcmp( m1->macro_name, m2->macro_name, m1->macro_len - offsetof( MEDEFN, macro_name ) ) );
}

void MacroStorageFini( void )
/***************************/
{
    macroStorageFree( &macroSegmentList, &macroHashTable );
}

#ifdef OPT_BR
void MacroWriteBrinf            // WRITE MACROS TO BRINF
    ( void )
{
    unsigned i;
    MEPTR curr;

    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        RingIterBeg( macroHashTable[ i ], curr ) {
            BrinfDeclMacro( curr );
        } RingIterEnd( curr )
    }
}
#endif

pch_status PCHWriteMacros( void )
{
    unsigned hashval;
    unsigned wlen;
    MEPTR curr;
    MEPTR next;
    SRCFILE save_defn_src_file;

    for( hashval = 0; hashval < MACRO_HASH_SIZE; ++hashval ) {
        RingIterBeg( macroHashTable[hashval], curr ) {
            next = curr->next_macro;
            save_defn_src_file = curr->defn.src_file;
            curr->macro_flags &= ~MFLAG_PCH_TEMPORARY_FLAGS;
            curr->next_macro = PCHSetUInt( hashval );
            curr->defn.src_file = SrcFileGetIndex( save_defn_src_file );
            wlen = curr->macro_len;
            PCHWriteUInt( wlen );
            PCHWrite( curr, wlen );
            curr->defn.src_file = save_defn_src_file;
            curr->next_macro = next;
        } RingIterEnd( curr )
    }
    PCHWriteUInt( 0 );
    return( PCHCB_OK );
}

pch_status PCHInitMacros( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniMacros( bool writing )
{
    writing = writing;
    return( PCHCB_OK );
}

static void findMaxLength( MEPTR curr, void *data )
{
    unsigned *pmax = data;
    unsigned wlen;

    wlen = curr->macro_len;
    if( wlen > *pmax ) {
        *pmax = wlen;
    }
}

static void writeMacroCheck( MEPTR curr, void *data )
{
    unsigned *phash = data;
    unsigned wlen;
    MEPTR next;

    next = curr->next_macro;
    curr->next_macro = PCHSetUInt( *phash );
    wlen = curr->macro_len;
    PCHWriteUInt( wlen );
    PCHWrite( curr, wlen );
    curr->next_macro = next;
}

static void forAllMacrosDefinedBeforeFirstInclude( void (*rtn)( MEPTR, void * ), void *data )
{
    size_t len;
    unsigned i;
    unsigned hash;
    MEPTR curr;

    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        hash = i;
        RingIterBeg( macroHashTable[ i ], curr ) {
            if( curr->macro_flags & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE ) {
                if( data == NULL ) {
                    (*rtn)( curr, &hash );
                } else {
                    (*rtn)( curr, data );
                }
            }
        } RingIterEnd( curr )
    }
    RingIterBeg( beforeIncludeChecks, curr ) {
        if( data == NULL ) {
            len = strlen( curr->macro_name );
            hash = NameCalcHashLen( curr->macro_name, len );
            (*rtn)( curr, &hash );
        } else {
            (*rtn)( curr, data );
        }
    } RingIterEnd( curr )
}

void PCHDumpMacroCheck(         // DUMP MACRO CHECK INFO INTO PCHDR
    void )
{
    unsigned max_wlen;

    // find largest macro definition size
    max_wlen = 1;
    forAllMacrosDefinedBeforeFirstInclude( findMaxLength, &max_wlen );
    PCHWriteUInt( max_wlen );
    // write macros out
    forAllMacrosDefinedBeforeFirstInclude( writeMacroCheck, NULL );
    PCHWriteUInt( 0 );
}

bool PCHVerifyMacroCheck(       // READ AND VERIFY MACRO CHECK INFO FROM PCHDR
    void )
{
    bool ret;
    int macros_different;
    unsigned max_rlen;
    unsigned rlen;
    unsigned pch_hash;
    unsigned i;
    size_t del_len;
    MEPTR cmdln_macro;
    MEPTR pch_macro;
    MEPTR new_macro;
    MEPTR matched_macro;
    PCH_DELMAC *macro_delete;

    /*
      This check is performed before we load the PCH so we have all macros
      that are defined before the first #include in our current macro table.

      Checks we perform are:

        if a macro was defined before the PCH was created
            if it was referenced during #include processing
                (1) a definition must be present
                (2) the definition must match exactly
            else
                (3) keep the current definition
                    (may mean deleting macro after PCH load)
            endif
        else
            if the current compilation has a new user-defined macro
                (4) macro could have potentially triggered a #ifdef/#ifndef/#if
                    so we can't trust the PCH
            endif
        endif
    */
    ret = TRUE;
    max_rlen = PCHReadUInt();
    pch_macro = CMemAlloc( max_rlen );
    for( ; (rlen = PCHReadUInt()) != 0; ) {
        PCHRead( pch_macro, rlen );
        pch_hash = PCHGetUInt( pch_macro->next_macro );
        //printf( "pch mac: %s\n", pch_macro->macro_name );
        matched_macro = NULL;
        RingIterBeg( macroHashTable[pch_hash], new_macro ) {
            if( strcmp( new_macro->macro_name, pch_macro->macro_name ) == 0 ) {
                matched_macro = new_macro;
                new_macro->macro_flags |= MFLAG_PCH_CHECKED;
                macros_different = macroCompare( new_macro, pch_macro );
                if( pch_macro->macro_flags & MFLAG_REFERENCED ) {
                    // (2) original macro was referenced during first #include
                    if( ! macros_different ) {
                        // OK; defns are identical in both compilation units
                        break;
                    }
                    PCHWarn2p( WARN_PCH_CONTENTS_MACRO_DIFFERENT, pch_macro->macro_name );
                    ret = FALSE;
                    break;
                }
                if( macros_different ) {
                    // (3) macro is different but never referenced in PCH build
                    // action: keep this macro definition
                    new_macro->macro_flags |= MFLAG_PCH_OVERRIDE;
                }
                break;
            }
        } RingIterEnd( new_macro )
        if( matched_macro != NULL ) {
            /* macro is in current compilation */
            if( ret == FALSE ) {
                /* problem was detected */
                break;
            }
        } else {
            /* macro not found in current compile */
            if( pch_macro->macro_flags & MFLAG_REFERENCED ) {
                // (1) original macro was referenced during first #include
                // but no definition in current compilation
                PCHWarn2p( WARN_PCH_CONTENTS_MACRO_NOT_PRESENT, pch_macro->macro_name );
                ret = FALSE;
                break;
            }
            // (3) queue macro to be deleted when PCH is loaded
            del_len = offsetof( PCH_DELMAC, name ) + strlen( pch_macro->macro_name ) + 1;
            macro_delete = CMemAlloc( del_len );
            macro_delete->hash = pch_hash;
            strcpy( macro_delete->name, pch_macro->macro_name );
            RingAppend( &macroPCHDeletes, macro_delete );
        }
    }
    CMemFree( pch_macro );
    if( ret ) {
        for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
            RingIterBeg( macroHashTable[i], cmdln_macro ) {
                if(( cmdln_macro->macro_flags & MFLAG_PCH_CHECKED ) == 0 ) {
                    // (4) macro was not defined when pch was created
                    if( cmdln_macro->macro_flags & MFLAG_USER_DEFINED ) {
                        PCHWarn2p( WARN_PCH_CONTENTS_MACRO_DIFFERENT, cmdln_macro->macro_name );
                        ret = FALSE;
                        break;
                    }
                }
            } RingIterEnd( cmdln_macro )
        }
    }
    if( ret == FALSE ) {
        RingFree( &macroPCHDeletes );
    }
    return( ret );
}

pch_status PCHReadMacros( void )
{
    int i;
    MEPTR new_mac;
    MEPTR curr;
    MEPTR prev;
    MEPTR pch_prev;
    MEPTR pch_curr;
    unsigned mlen;
    unsigned hash;
    MACRO_SEG_LIST *old_seglist;
    MEPTR *old_hashtab;
    PCH_DELMAC *del_name;

    macroStorageRestart( &old_seglist, &old_hashtab );
    for( ; (mlen = PCHReadUInt()) != 0; ) {
        MacroOverflow( mlen, 0 );
        new_mac = macroAllocateInSeg( mlen );
        PCHRead( new_mac, mlen );
        hash = PCHGetUInt( new_mac->next_macro );
        new_mac->next_macro = NULL;
        new_mac->defn.src_file = SrcFileMapIndex( new_mac->defn.src_file );
        RingAppend( &macroHashTable[ hash ], new_mac );
    }
    // add macros from current compilation that should override the PCH macros
    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        RingIterBeg( old_hashtab[ i ], curr ) {
            if( curr->macro_flags & MFLAG_PCH_OVERRIDE ) {
                // macro is different but doesn't affect PCH contents
                // action: replace with current compilation unit's defn
                pch_prev = NULL;
                RingIterBeg( macroHashTable[ i ], pch_curr ) {
                    if( strcmp( curr->macro_name, pch_curr->macro_name ) == 0 ) {
                        RingPruneWithPrev( &macroHashTable[i], pch_curr, pch_prev );
                        mlen = curr->macro_len;
                        new_mac = macroAllocateInSeg( mlen );
                        memcpy( new_mac, curr, mlen );
                        RingAppend( &macroHashTable[i], new_mac );
                        break;
                    }
                    pch_prev = pch_curr;
                } RingIterEnd( pch_curr )
                continue;
            }
        } RingIterEnd( curr )
    }
    // delete unreferenced macros that existed in PCH but don't exist now
    RingIterBeg( macroPCHDeletes, del_name ) {
        prev = NULL;
        RingIterBeg( macroHashTable[ del_name->hash ], curr ) {
            if( strcmp( curr->macro_name, del_name->name ) == 0 ) {
                RingPruneWithPrev( &macroHashTable[del_name->hash], curr, prev );
                break;
            }
            prev = curr;
        } RingIterEnd( curr )
    } RingIterEnd( del_name )
    RingFree( &macroPCHDeletes );
    macroStorageFree( &old_seglist, &old_hashtab );
    return( PCHCB_OK );
}

#define magicPredefined( n )    ( strcmp( "defined", (n) ) == 0 )

static MEPTR macroFind(         // LOOK UP A HASHED MACRO
    const char *name,           // - macro name
    unsigned len,               // - length of macro name
    unsigned *phash )           // - returned hash value
{
    char *id;                   // - current macro name
    MEPTR curr;                 // - current macro name being checked
    unsigned hash;              // - hash value for current macro name
    unsigned mask;              // - mask for quick comparison

    hash = NameCalcHashLen( name, len );
    *phash = hash;
    ++len;
    if( len > NAME_MAX_MASK_INDEX ) {
        mask = NameCmpMask[ NAME_MAX_MASK_INDEX ];
    } else {
        mask = NameCmpMask[ len ];
    }
    RingIterBeg( macroHashTable[ hash ], curr ) {
        id = curr->macro_name;
        if(( *((unsigned*)id) ^ *((unsigned*)name) ) & mask ) {
            continue;
        }
        if( NameMemCmp( id, name, len ) == 0 ) {
            curr = BrinfReferenceMacro( curr );
            return curr;
        }
    } RingIterEnd( curr )
    return NULL;
}


void MacroOverflow(             // OVERFLOW SEGMENT IF REQUIRED
    unsigned amount_needed,     // - amount for macro
    unsigned amount_used )      // - amount used in segment
{
    char    *old_offset;

    amount_needed = macroSizeAlign( amount_needed );
    DbgAssert( amount_used <= macroSegmentLimit );
    if( macroSegmentLimit < amount_needed ) {
        old_offset = MacroOffset;
        macroAllocSegment( amount_needed );
        if( amount_used != 0 ) {
            ExtraRptSpaceSubtract( macro_space, amount_used );
            memcpy( MacroOffset, old_offset, amount_used );
        }
    }
}

static void unlinkMacroFromTable( MEPTR fmentry, unsigned hash )
{
    ++undefCount;
    RingPrune( &macroHashTable[ hash ], fmentry );
    if(( InitialMacroFlag & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE ) == 0 ) {
        // make sure we only do this *after* the first include has started
        // processing otherwise the PCH is created in such a way that
        // the #undef'd macro must be defined before the #include 98/07/13
        if( fmentry->macro_flags & MFLAG_DEFINED_BEFORE_FIRST_INCLUDE ) {
            RingAppend( &beforeIncludeChecks, fmentry );
        }
    }
}

MEPTR MacroDefine(              // DEFINE A NEW MACRO
    MEPTR mentry,               // - scanned macro
    unsigned len,               // - length of entry
    unsigned name_len )         // - name of macro name
{
    MEPTR mptr;                 // - new entry for macro
    MEPTR old_mentry;           // - old entry for macro
    char *mac_name;             // - name for macro
    unsigned hash;              // - hash bucket for macro
    msg_status_t msg_st;        // - error message status

    DbgAssert( mentry == (MEPTR)MacroOffset );
    mptr = NULL;
    mac_name = mentry->macro_name;
    if( magicPredefined( mac_name ) ) {
        CErr2p( ERR_DEFINE_IMPOSSIBLE, mac_name );
    } else {
        old_mentry = macroFind( mac_name, name_len, &hash );
        if( old_mentry != NULL ) {
            if( old_mentry->macro_flags & MFLAG_CAN_BE_REDEFINED ) {
                unlinkMacroFromTable( old_mentry, hash );
                old_mentry = NULL;
            } else {
                if( macroCompare( mentry, old_mentry ) != 0 ) {
                    if( old_mentry->macro_defn == 0 ) {
                        CErr2p( ERR_DEFINE_IMPOSSIBLE, mac_name );
                        InfMacroDecl( old_mentry );
                    } else {
                        msg_st = CErr2p( ANSI_MACRO_DEFN_NOT_IDENTICAL, mac_name );
                        if( msg_st & MS_PRINTED ) {
                            InfMacroDecl( old_mentry );
                        }
                        if( msg_st & MS_WARNING ) {
                            /* message turned out to be a warning */
                            unlinkMacroFromTable( old_mentry, hash );
                            old_mentry = NULL;
                        }
                    }
                }
            }
        }
        if( old_mentry == NULL ) {
            mentry->macro_flags = InitialMacroFlag;
            mptr = macroAllocateInSeg( len );
            DbgAssert( mptr == mentry );
            RingAppend( &macroHashTable[ hash ], mptr );
            ExtraRptIncrementCtr( macros_defined );
#ifdef XTRA_RPT
            if( mentry->parm_count != 0 ) {
                ExtraRptIncrementCtr( macros_defined_with_parms );
            }
#endif
        }
    }
    return( mptr );
}


MEPTR MacroSpecialAdd(          // ADD A SPECIAL MACRO
    char *name,                 // - macro name
    unsigned value,             // - value for special macro
    macro_flags flags )         // - macro flags
{
    size_t len;
    size_t reqd;
    MEPTR mentry;

    len = strlen( name );
    reqd = offsetof( MEDEFN, macro_name ) + 1 + len;
    MacroOverflow( reqd, 0 );
    mentry = (MEPTR)MacroOffset;
    TokenLocnClear( mentry->defn );
    mentry->macro_defn = 0;     /* indicate special macro */
    mentry->macro_len = reqd;
    mentry->parm_count = value;
    memcpy( mentry->macro_name, name, len + 1 );

    mentry = MacroDefine( mentry, reqd, len );
    if( mentry != NULL ) {
        mentry->macro_flags |= flags;
    }
    return mentry;
}


MEPTR MacroLookup(          // LOOKUP CURRENT NAME AS A MACRO
    char const *name,       // - name
    unsigned len )          // - length of name
{
    unsigned hash;

    return macroFind( name, len, &hash );
}


bool MacroExists(           // TEST IF MACRO EXISTS
    const char *macname,    // - macro name
    unsigned len )          // - length of name
{
    unsigned hash;
    bool exists;
    MEPTR mac;

    mac = macroFind( macname, len, &hash );
    if( mac != NULL ) {
        mac->macro_flags |= MFLAG_REFERENCED;
        exists = ( (mac->macro_flags & MFLAG_SPECIAL ) == 0 );
    } else {
        exists = FALSE;
    }
    return exists;
}


bool MacroDependsDefined    // MACRO DEPENDENCY: DEFINED OR NOT
    ( void )
{
    bool retn = MacroExists( Buffer, TokenLen );
#ifdef OPT_BR
    retn = BrinfDependsMacroDefined( retn, Buffer, TokenLen );
#endif
    return retn;
}


static void doMacroUndef( char *name, unsigned len, bool quiet )
{
    MEPTR fmentry;          // - current macro entry
    unsigned hash;          // - current macro hash

    if( magicPredefined( name ) ) {
        if( ! quiet ) {
            CErr2p( ERR_UNDEF_IMPOSSIBLE, name );
        }
    } else {
        fmentry = macroFind( name, len, &hash );
        if( fmentry != NULL ) {
            if( fmentry->macro_defn == 0 ) {
                if( ! quiet ) {
                    CErr2p( ERR_UNDEF_IMPOSSIBLE, name );
                }
            } else {
                BrinfUndefMacro( fmentry );
                unlinkMacroFromTable( fmentry, hash );
            }
        }
    }
}

void MacroUndefine(             // UNDEFINE CURRENT NAME AS MACRO
    unsigned len )              // - length of macro name
{
    doMacroUndef( Buffer, len, FALSE );
}

void MacroCmdLnUndef(           // -U<macro-name>
    char *name,                 // - macro name
    unsigned len )              // - length of macro name
{
    doMacroUndef( name, len, TRUE );
}

void MacroCanBeRedefined(       // SET MACRO SO THAT USE CAN REDEFINE IN SOURCE
    MEPTR mptr )                // - the macro entry
{
    if( mptr != NULL ) {
        mptr->macro_flags |= MFLAG_CAN_BE_REDEFINED;
    }
}

void MacroStateClear( MACRO_STATE *ms )
/*************************************/
{
    ms->curr_offset = NULL;
    ms->undef_count = 0;
}

void MacroStateGet( MACRO_STATE *ms )
/***********************************/
{
    ms->curr_offset = MacroOffset;
    ms->undef_count = undefCount;
}

bool MacroStateMatchesCurrent( MACRO_STATE *ms )
/**********************************************/
{
    if( ms->curr_offset != MacroOffset ) {
        return( FALSE );
    }
    if( ms->undef_count != undefCount ) {
        return( FALSE );
    }
    return( TRUE );
}
