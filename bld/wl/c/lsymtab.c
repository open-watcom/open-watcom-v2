/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Symbol table routines for wlink.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "walloca.h"
#include "linkstd.h"
#include "pcobj.h"
#include "objio.h"
#include "newmem.h"
#include "objnode.h"
#include "wcomdef.h"
#include "mapio.h"
#include "impexp.h"
#include "objstrip.h"
#include "objpass1.h"
#include "ring.h"
#include "strtab.h"
#include "carve.h"
#include "permdata.h"
#include "symtab.h"
#include "symmem.h"
#include "symtrace.h"

#include "clibext.h"


#define STATIC_TABSIZE  241     /* should be prime */
#define GLOBAL_TABSIZE  1789    /* should be prime */

#define STATIC_TABALLOC (256 * sizeof( symbol * ))  // 1st power of 128 > TABSIZE
#define GLOBAL_TABALLOC (1792 * sizeof( symbol * )) // 1st power of 128 > TABSIZE

int             (*CmpRtn)( const char *, const char *, size_t );
size_t          NameLen;
symbol          *LastSym;

static  symbol  *SymList = NULL;

// not a static array to save space for watfor

static symbol   **GlobalSymPtrs;
static symbol   **StaticSymPtrs;

static const unsigned ScatterTable[] = {
#ifdef _M_I86   // it is a 16-bit hosted linker
0x1DE8,
0x2EE1,
0x6E21,
0x134B,
0x0F40,
0x6D5B,
0x0767,
0x1454,
0x3FE5,
0x30A8,
0x05D0,
0x3ED8,
0x6306,
0x0B8C,
0x02B9,
0x3A2E,
0x065A,
0x15F3,
0x177C,
0x1A12,
0x1100,
0x789C,
0x3DB0,
0x2031,
0x5416,
0x0785,
0x7725,
0x0A74,
0x091B,
0x1715,
0x799E,
0x7A0C,
0x5A05,
0x780C,
0x5A4D,
0x3F42,
0x4F21,
0x462B,
0x750F,
0x15FC,
0x3EAF,
0x75A2,
0x0B6C,
0x1F0E,
0x1A39,
0x4B3E,
0x574C,
0x2246,
0x35B7,
0x2B4F,
0x0D7A,
0x6536,
0x6464,
0x6B5A,
0x0BDA,
0x22BF,
0x075E,
0x1283,
0x2F6A,
0x6F62,
0x6A00,
0x34B9,
0x0DFB,
0x1646,
0x0800,
0x549F,
0x0FAB,
0x3B09,
0x1947,
0x783E,
0x2429,
0x4C47,
0x7F93,
0x5C6B,
0x0FA8,
0x12EB,
0x71CF,
0x72F9,
0x0BA0,
0x6A38,
0x012E,
0x179F,
0x3747,
0x6C96,
0x5409,
0x27A8,
0x13D3,
0x211D,
0x327E,
0x685D,
0x086C,
0x75E3,
0x40C4,
0x6832,
0x29F3,
0x1306,
0x0350,
0x46B3,
0x4275,
0x5278,
0x08A9,
0x0F2C,
0x546D,
0x688D,
0x550C,
0x121B,
0x15BD,
0x0946,
0x6BBF,
0x6956,
0x1E6B,
0x165B,
0x1235,
0x12FC,
0x471C,
0x220A,
0x28E7,
0x4F1D,
0x7351,
0x52A2,
0x35ED,
0x2C29,
0x4363,
0x72CE,
0x7D5E,
0x6E19,
0x6A3F,
0x3AA2,
0x436D,
0x3C5F,
0x22E2,
0x1D67,
0x1440,
0x428E,
0x0194,
0x2826,
0x2D8F,
0x2FCA,
0x1CE4,
0x3CF7,
0x6602,
0x40ED,
0x4A66,
0x3707,
0x2E47,
0x417E,
0x6B2F,
0x436A,
0x07F5,
0x2F53,
0x040D,
0x7AA6,
0x0E23,
0x6D00,
0x1D87,
0x06FB,
0x6BC7,
0x2F06,
0x2798,
0x6374,
0x5BD0,
0x0FBC,
0x5D2A,
0x7FAE,
0x0F03,
0x75FC,
0x6354,
0x546B,
0x1397,
0x3A8F,
0x2053,
0x34D6,
0x1A8F,
0x3856,
0x064A,
0x6894,
0x36D9,
0x333D,
0x4DB9,
0x5A8F,
0x722B,
0x41DF,
0x5BBF,
0x6881,
0x5399,
0x25F9,
0x500F,
0x1F42,
0x33F6,
0x3F91,
0x76B4,
0x6FD2,
0x7BF1,
0x06E2,
0x1984,
0x2926,
0x27EB,
0x390E,
0x6D65,
0x42B3,
0x2D99,
0x2382,
0x1743,
0x43BB,
0x1F5E,
0x3A28,
0x04CD,
0x535A,
0x2966,
0x6450,
0x14F2,
0x3D4F,
0x4482,
0x2C5C,
0x4C1F,
0x778B,
0x3AC6,
0x3E2D,
0x1035,
0x747C,
0x59E4,
0x6051,
0x284C,
0x4E16,
0x6F47,
0x53E9,
0x7C2A,
0x15A6,
0x69EF,
0x475B,
0x4F81,
0x5456,
0x3E0F,
0x47BD,
0x74EC,
0x087F,
0x6668,
0x5CFC,
0x34A8,
0x2BB2,
0x1F63,
0x3CD0,
0x6312,
0x0D84,
0x37F1,
0x4060,
0x62E4,
0x0F1D,
0x1423,
0x08B4,
0x0F2F,
0x0B7F,
0x3D88,
0x7DDF,
0x4918,
0x7897
#else   /* 32-bit */
0x1DE82EE1,
0x6E21134B,
0x0F406D5B,
0x07671454,
0x3FE530A8,
0x05D03ED8,
0x63060B8C,
0x02B93A2E,
0x065A15F3,
0x177C1A12,
0x1100789C,
0x3DB02031,
0x54160785,
0x77250A74,
0x091B1715,
0x799E7A0C,
0x5A05780C,
0x5A4D3F42,
0x4F21462B,
0x750F15FC,
0x3EAF75A2,
0x0B6C1F0E,
0x1A394B3E,
0x574C2246,
0x35B72B4F,
0x0D7A6536,
0x64646B5A,
0x0BDA22BF,
0x075E1283,
0x2F6A6F62,
0x6A0034B9,
0x0DFB1646,
0x0800549F,
0x0FAB3B09,
0x1947783E,
0x24294C47,
0x7F935C6B,
0x0FA812EB,
0x71CF72F9,
0x0BA06A38,
0x012E179F,
0x37476C96,
0x540927A8,
0x13D3211D,
0x327E685D,
0x086C75E3,
0x40C46832,
0x29F31306,
0x035046B3,
0x42755278,
0x08A90F2C,
0x546D688D,
0x550C121B,
0x15BD0946,
0x6BBF6956,
0x1E6B165B,
0x123512FC,
0x471C220A,
0x28E74F1D,
0x735152A2,
0x35ED2C29,
0x436372CE,
0x7D5E6E19,
0x6A3F3AA2,
0x436D3C5F,
0x22E21D67,
0x1440428E,
0x01942826,
0x2D8F2FCA,
0x1CE43CF7,
0x660240ED,
0x4A663707,
0x2E47417E,
0x6B2F436A,
0x07F52F53,
0x040D7AA6,
0x0E236D00,
0x1D8706FB,
0x6BC72F06,
0x27986374,
0x5BD00FBC,
0x5D2A7FAE,
0x0F0375FC,
0x6354546B,
0x13973A8F,
0x205334D6,
0x1A8F3856,
0x064A6894,
0x36D9333D,
0x4DB95A8F,
0x722B41DF,
0x5BBF6881,
0x539925F9,
0x500F1F42,
0x33F63F91,
0x76B46FD2,
0x7BF106E2,
0x19842926,
0x27EB390E,
0x6D6542B3,
0x2D992382,
0x174343BB,
0x1F5E3A28,
0x04CD535A,
0x29666450,
0x14F23D4F,
0x44822C5C,
0x4C1F778B,
0x3AC63E2D,
0x1035747C,
0x59E46051,
0x284C4E16,
0x6F4753E9,
0x7C2A15A6,
0x69EF475B,
0x4F815456,
0x3E0F47BD,
0x74EC087F,
0x66685CFC,
0x34A82BB2,
0x1F633CD0,
0x63120D84,
0x37F14060,
0x62E40F1D,
0x142308B4,
0x0F2F0B7F,
0x3D887DDF,
0x49187897,
0x1D4A14E8,
0x25520D06,
0x3C07087C,
0x755F76AC,
0x23707056,
0x28866DF9,
0x3DA4636A,
0x409331F2,
0x4E4A10D5,
0x56513905,
0x61717B84,
0x69C8228E,
0x4C2744A6,
0x7A373525,
0x1ADB30D2,
0x21D075AE,
0x097353F8,
0x3136231E,
0x612C1009,
0x06B7230D,
0x58355266,
0x1D492B01,
0x4F0B3C0A,
0x0F463274,
0x07932076,
0x08E779AB,
0x31F9075F,
0x66823136,
0x6B4B1D1A,
0x7A830E45,
0x09D43DC3,
0x372C57B1,
0x53390731,
0x380C37C6,
0x78551D9A,
0x67415ED2,
0x72D60F05,
0x3A6D426F,
0x20940173,
0x4379058F,
0x38B517CC,
0x110C554C,
0x7681398B,
0x1ACA466D,
0x16062929,
0x354D7FB8,
0x7E6B154A,
0x49E43CFA,
0x361510AA,
0x4E0E76D7,
0x7C7B1CC6,
0x36B43B52,
0x25C9334A,
0x632A031B,
0x14387A3D,
0x3BE40F9C,
0x6B280EEE,
0x00F81DC2,
0x5802139E,
0x0457798B,
0x0CD06BEA,
0x4BCB3E54,
0x489953FF,
0x56B04FE0,
0x097E3E7C,
0x03725827,
0x44952525,
0x50CA55E5,
0x3F89384E,
0x76B707DE,
0x67EE1902,
0x133F40F1,
0x446509F6,
0x66E370E3,
0x5B744330,
0x5CE36DE9,
0x2C225477,
0x5B364AF3,
0x14554279,
0x164746BB,
0x40EC4ABF,
0x64711F90,
0x039C7A51,
0x4D3957E3,
0x2A8B0328,
0x504E378E,
0x35AE7C58,
0x204115E8,
0x05E378FA,
0x4D043889,
0x61CD21DB,
0x1A2452DE,
0x6C733DE6,
0x7CC6726B,
0x69985551,
0x7D6363E3,
0x6BD85B8A,
0x79435CEA,
0x48870BE0,
0x7FBB76A4,
0x6D4964F2,
0x47264505,
0x73716ED8,
0x75A416D8,
0x0D1B3813,
0x39952A8C,
0x28083535,
0x6DD463C1,
0x623C6F4F,
0x45B34D96,
0x2C592D1F,
0x3CBB75B1,
0x35B902F6,
0x46206C0B,
0x7E4E756B,
0x78067381,
0x2A3B1AC2,
0x2E762F1A,
0x732E6719,
0x621F5819,
0x53840F58,
0x2ECC48C2,
0x471A2ED6,
0x45A967ED,
0x3DF41BCC,
0x4735414C,
0x1C90167A,
0x4106567B
#endif
};

void ResetSym( void )
/**************************/
{
    NameLen = 0;
    SymList = NULL;
    HeadSym = NULL;
    LastSym = NULL;
    CmpRtn = strnicmp;
    ResetPermBlocks();
    ClearHashPointers();
}

void InitSym( void )
/*************************/
{
    _ChkAlloc( GlobalSymPtrs, GLOBAL_TABALLOC );
    _ChkAlloc( StaticSymPtrs, STATIC_TABALLOC );
}

#ifdef DEVBUILD

static void DumpTable( symbol **table, unsigned tabsize )
/*******************************************************/
{
    unsigned    mask;
    unsigned    min;
    unsigned    max;
    unsigned    val;
    unsigned_32 numsyms;
    unsigned    index;
    symbol      *sym;

    numsyms = 0;
    min = 0xFFFF;
    max = 0;
    for( index = 0; index < tabsize; index++ ) {
        mask = DBG_ALWAYS;
        if( (index % 15) != 14 ) {
            mask |= DBG_NOCRLF;
        }
        val = 0;
        for( sym = table[index]; sym != NULL; sym = sym->hash ) {
            val++;
        }
        DEBUG(( mask, "%x ", val ));
        if( val > max ) {
            max = val;
        }
        if( val < min ) {
            min = val;
        }
        numsyms += val;
    }
    DEBUG(( DBG_ALWAYS, "" ));  // put out a blank line.
    DEBUG(( DBG_ALWAYS, "# of syms: %l, max height %d, min height %d",
            numsyms, max, min ));
}

static void DumpHashTable( void )
/*******************************/
{
    if( LinkState & LS_INTERNAL_DEBUG ) {
        DEBUG(( DBG_ALWAYS, "symbol table load" ));
        DumpTable( GlobalSymPtrs, GLOBAL_TABSIZE );
        DumpTable( StaticSymPtrs, STATIC_TABSIZE );
    }
}
#endif

static void WipeSym( symbol *sym )
/********************************/
{
    if( IS_SYM_IMPORTED( sym )
      && (FmtData.type & MK_ELF) == 0 ) {
        if( FmtData.type & MK_NOVELL ) {
            if( sym->p.import != DUMMY_IMPORT_PTR ) {
                _LnkFree( sym->p.import );
            }
        } else {
            FreeImport( sym->p.import );
        }
        sym->p.import = NULL;
    } else if( IS_SYM_ALIAS( sym ) ) {
        if( sym->info & SYM_FREE_ALIAS ) {
            _LnkFree( sym->p.alias.u.ptr );
        }
        sym->u.aliaslen = 0;    // make sure this is nulled again
    }
}

static void FreeSymbol( symbol *sym )
/***********************************/
{
    WipeSym( sym );
    CarveFree( CarveSymbol, sym );
}

void CleanSym( void )
/*************************/
{
    symbol *    sym;
    symbol *    next;

#ifdef DEVBUILD
    DumpHashTable();
#endif
    if( (LinkFlags & LF_INC_LINK_FLAG) == 0 ) {
        for( sym = HeadSym; sym != NULL; sym = next ) {
            next = sym->link;
            FreeSymbol( sym );
        }
    }
    ReleasePermBlocks();
    ReleasePass1Blocks();
}

void FiniSym( void )
/*************************/
{
    _LnkFree( GlobalSymPtrs );
    _LnkFree( StaticSymPtrs );
}

static void PrepHashTable( symbol **table, unsigned size )
/********************************************************/
{
    for( ; size > 0; --size ) {
        *table = CarveGetIndex( CarveSymbol, *table );
        table++;
    }
}

void WriteHashPointers( void *cookie )
/*******************************************/
{
    PrepHashTable( StaticSymPtrs, STATIC_TABSIZE );
    WritePermFile( cookie, StaticSymPtrs, STATIC_TABALLOC );
    PrepHashTable( GlobalSymPtrs, GLOBAL_TABSIZE );
    WritePermFile( cookie, GlobalSymPtrs, GLOBAL_TABALLOC );
}

static void RebuildHashTable( symbol **table, unsigned size )
/***********************************************************/
{
    for( ; size > 0; --size ) {
        *table = CarveMapIndex( CarveSymbol, *table );
        table++;
    }
}

void ReadHashPointers( void *cookie )
/******************************************/
{
    ReadPermFile( cookie, StaticSymPtrs, STATIC_TABALLOC );
    ReadPermFile( cookie, GlobalSymPtrs, GLOBAL_TABALLOC );
    RebuildHashTable( StaticSymPtrs, STATIC_TABSIZE );
    RebuildHashTable( GlobalSymPtrs, GLOBAL_TABSIZE );
}

void ClearHashPointers( void )
/***********************************/
{
    memset( GlobalSymPtrs, 0, GLOBAL_TABSIZE * sizeof( symbol * ) );
    memset( StaticSymPtrs, 0, STATIC_TABSIZE * sizeof( symbol * ) );
}

void SetSymCase( void )
/****************************/
{
    if( LinkFlags & LF_CASE_FLAG ) {
        CmpRtn = strncmp;
    } else {
        CmpRtn = strnicmp;
    }
}

void SymModStart( void )
/*****************************
 * do necessary symbol table processing before the start of a module in pass1
 */
{
    SymList = LastSym;
}

void SymModEnd( void )
/***************************
 * go through the list of symbols generated in this module, and find all
 * references to symbols which are in this module only.
 */
{
    symbol *    sym;

    if( SymList == NULL ) {
        sym = HeadSym;
    } else {
        sym = SymList;
        if( sym != NULL ) {     // symlist points to previous one
            sym = sym->link;
        }
    }
    for( ; sym != NULL; sym = sym->link ) {
        sym->info &= ~SYM_IN_CURRENT;
        sym->info |= SYM_OLDHAT;
        if( (sym->info & SYM_REFERENCED)
          && (sym->info & SYM_DEFINED) ) {
            sym->info &= ~SYM_REFERENCED;
            sym->info |= SYM_LOCAL_REF;
        }
    }
    SymList = NULL;
}

void ClearRefInfo( symbol *sym )
/*************************************/
{
    symbol *    save;

    if( (sym->info & SYM_EXPORTED) == 0 ) {
        save = *(sym->e.vfdata);
        _LnkFree( sym->e.vfdata );
        sym->e.def = save;
    }
}

void ClearSymUnion( symbol * sym )
/***************************************
 * clear the symbol unions of any possible allocated data
 */
{
    sym->info &= ~SYM_LINK_GEN;
    if( IS_SYM_VF_REF( sym ) ) {
        ClearRefInfo( sym );
    } else {
        WipeSym( sym );
    }
}

symbol *RefISymbol( const char *name )
/************************************/
{
    return( SymOp( ST_CREATE_REFERENCE, name, strlen( name ) ) );
}

symbol *DefISymbol( const char *name )
/************************************/
{
    symbol *sym;

    sym = RefISymbol( name );
    if( (sym->info & SYM_DEFINED)
      && (sym->info & SYM_LINK_GEN) == 0 ) {
        LnkMsg( ERR+MSG_RESERVED_SYM_DEFINED, "s", name );
    }
    sym->info |= SYM_DEFINED | SYM_LINK_GEN;
    return( sym );
}

symbol *FindISymbol( const char *name )
/*************************************/
{
    return( SymOp( ST_FIND | ST_REFERENCE, name, strlen( name ) ) );
}

symbol *SymOpNWPfx( sym_flags symop, const char *name, size_t length, const char *prefix, size_t prefixlen )
/**********************************************************************************************************/
{
    symbol  *sym;

    if( (sym = SymOp( symop, name, length )) == NULL )
        return( NULL );

    if( ( prefix != NULL ) && prefixlen ) {
        if( prefixlen > 254 ) {
            LnkMsg( ERR+MSG_SYMBOL_NAME_TOO_LONG, "s", prefix );
            return( NULL );
        }
        if( (sym->prefix = AddSymbolStringTable( &PrefixStrings, prefix, prefixlen )) == NULL ) {
            LnkMsg( ERR+MSG_INTERNAL, "s", "no memory for prefix symbol");
            return( NULL );
        }
    }
    return( sym );
}

static void SetSymAlias( symbol *sym, const char *target, size_t targetlen )
/**************************************************************************/
{
    SET_SYM_TYPE( sym, SYM_ALIAS );
    sym->p.alias.u.ptr = ChkToString( target, targetlen );
    sym->u.aliaslen = targetlen;
    sym->info |= SYM_DEFINED;           /* an alias can't be undefined */
    sym->info &= ~SYM_WAS_LAZY;
}

void MakeSymAlias( const char *name, size_t namelen, const char *target, size_t targetlen )
/******************************************************************************************
 * make a symbol table alias
 */
{
    symbol      *sym;
    symbol      *targ;

    if( namelen == targetlen
      && (*CmpRtn)( name, target, namelen ) == 0 ) {
        char    *buff;

        DUPSTR_STACK( buff, target, targetlen );
        LnkMsg( WRN+MSG_CIRCULAR_ALIAS_FOUND, "s", buff );
        return;                 // <--------- NOTE: premature return!!!!
    }
    sym = SymOp( ST_CREATE | ST_NOALIAS, name, namelen );
    if( IS_SYM_ALIAS( sym ) ) {
        LnkMsg( WRN+MSG_MULTIPLE_ALIASES, "S", sym );
        if( sym->info & SYM_FREE_ALIAS ) {
            _LnkFree( sym->p.alias.u.ptr );
        }
    } else if( sym->info & SYM_DEFINED ) {
        return;                 // <--------- NOTE: premature return!!!!
    }
    sym->info |= SYM_FREE_ALIAS;
    /*
     * need to defined aliased symbol as well, so aliases in .obj files can be
     * hauled in from libraries
     */
    targ = SymOp( ST_CREATE, target, targetlen );
    targ->info |= SYM_ALIAS_TARGET;
    SetSymAlias( sym, targ->name.u.ptr, targetlen );
}

void WeldSyms( symbol *src, symbol *targ )
/*****************************************
 * make all references to src refer to targ. (alias src to targ)
 */
{
    if( targ != NULL ) {
        SetSymAlias( src, targ->name.u.ptr, strlen( targ->name.u.ptr ) );
    }
}

static symbol *GlobalSearchSym( const char *symname, unsigned hash, size_t len )
/*******************************************************************************
 * search through the given chain for the given name
 */
{
    symbol      *sym;

    for( sym = GlobalSymPtrs[hash]; sym != NULL; sym = sym->hash ) {
        if( len == sym->namelen_cmp
          && (*CmpRtn)( symname, sym->name.u.ptr, len ) == 0 ) {
            break;
        }
    }
    return( sym );
}

static symbol *StaticSearchSym( const char *symname, unsigned hash, size_t len )
/*******************************************************************************
 * search through the given chain for the given name
 */
{
    symbol      *sym;

    for( sym = StaticSymPtrs[hash]; sym != NULL; sym = sym->hash ) {
        if( sym->info & SYM_IN_CURRENT ) {
            if( len == sym->namelen_cmp
              && memcmp( symname, sym->name.u.ptr, len ) == 0 ) {
                break;
            }
        }
    }
    return( sym );
}

static unsigned StaticHashFn( const char *name, size_t len )
/**********************************************************/
{
    unsigned    value;
    unsigned    modval;

    modval = (unsigned)CurrMod->modtime;
    value = ScatterTable[modval & 0xff];
    modval >>= 8;
    value = value ^ ScatterTable[modval & 0xff];
    while( len-- > 0 ) {
        value = (value << 1) ^ ScatterTable[*(unsigned char *)name];
        ++name;
    }
    return( value % STATIC_TABSIZE );
}

static unsigned GlobalHashFn( const char *name, size_t len )
/**********************************************************/
{
    unsigned    value;

    value = 0;
    while( len-- > 0 ) {
        value = (value << 1) ^ ScatterTable[*(unsigned char *)name | 0x20];
        ++name;
    }
    return( value % GLOBAL_TABSIZE );
}

static symbol *DoSymOp( sym_flags symop, const char *symname, size_t length )
/***************************************************************************/
{
    unsigned    hash;
    symbol      *sym;
    size_t      searchlen;
#ifdef DEVBUILD
    char        *symname_dbg;

    DUPSTR_STACK( symname_dbg, symname, length );
#endif
    DEBUG(( DBG_OLD, "SymOp( %d, %s, %d )", symop, symname_dbg, length ));
    if( NameLen
      && NameLen < length ) {
        searchlen = NameLen;
    } else {
        searchlen = length;
    }
    if( symop & ST_STATIC ) {
        hash = StaticHashFn( symname, searchlen );
        /* If symbol isn't unique, don't look for duplicates. */
        if( (symop & (ST_CREATE | ST_STATIC | ST_NONUNIQUE)) ==
                  (ST_CREATE | ST_STATIC | ST_NONUNIQUE) ) {
            sym = NULL;
        } else {
            sym = StaticSearchSym( symname, hash, searchlen );
        }
    } else {
        hash = GlobalHashFn( symname, searchlen );
        sym = GlobalSearchSym( symname, hash, searchlen );
    }
    DEBUG(( DBG_OLD, "- hash %d", hash ));
    if( sym != NULL ) {
        DEBUG(( DBG_OLD, "found symbol %s", symname_dbg ));
        DEBUG(( DBG_OLD, " - handle = %h", sym ));
        return( sym );
    }
    if( (symop & ST_FIND) == 0 ) {
        sym = AddSym();
        sym->name.u.ptr = AddSymbolStringTable( &PermStrings, symname, length );
        sym->namelen_cmp = searchlen;

        if( symop & ST_STATIC ) {
            sym->info |= SYM_STATIC;
            sym->hash = StaticSymPtrs[hash];
            StaticSymPtrs[hash] = sym;
        } else {
            sym->hash = GlobalSymPtrs[hash];
            GlobalSymPtrs[hash] = sym;
            if( FindSymTrace( symname ) ) {
                sym->info |= SYM_TRACE;
            }
        }
        DEBUG((DBG_OLD, "added symbol %s", symname_dbg ));
    }
    return( sym );
}

symbol *UnaliasSym( sym_flags symop, symbol *sym )
/************************************************/
{
    symbol *orig_sym = sym;
    while( sym != NULL
      && IS_SYM_ALIAS( sym ) ) {
        sym = DoSymOp( symop, sym->p.alias.u.ptr, sym->u.aliaslen );
        /* circular ref, may be a weak symbol ! */
        if( sym == orig_sym ) {
            break;
        }
    }
    return( sym );
}

symbol *SymOp( sym_flags symop, const char *symname, size_t length )
/******************************************************************/
/* search for symbols, handling aliases */
{
    symbol *    sym;

    sym = DoSymOp( symop, symname, length );
    if( (symop & ST_NOALIAS) == 0 ) {
        sym = UnaliasSym( symop, sym );
    }
    if( sym != NULL ) {
        if( symop & ST_DEFINE ) {
            if( IS_SYM_ALIAS( sym )
              && (sym->info & SYM_FREE_ALIAS) ) {
                _LnkFree( sym->p.alias.u.ptr );
                sym->info &= ~SYM_FREE_ALIAS;
            }
            sym->info |= SYM_DEFINED;
        }
        if( symop & ST_REFERENCE ) {
            sym->info |= SYM_REFERENCED;
        }
    }
    return( sym );
}

void ReportMultiple( symbol *sym, const char *name, size_t len )
/**************************************************************/
/* report a multiply-defined symbol */
{
    unsigned    lev;

    if( LinkFlags & LF_REDEFS_OK ) {
        lev = WRN;
    } else {
        lev = MILD_ERR;
        LinkState |= LS_UNDEFED_SYM_ERROR;
    }
    if( CmpRtn( sym->name.u.ptr, name, len + 1 ) == 0 ) {
        LnkMsg( LOC+lev+MSG_MULT_DEF, "S", sym );
    } else {
        LnkMsg( LOC+lev+MSG_MULT_DEF_BY, "12", sym->name.u.ptr, name );
    }
}

void ReportUndefined( void )
/*********************************/
/* tell user about any undefined symbols */
{
    symbol *    sym;
    unsigned    level;

    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        sym->info &= ~SYM_CLEAR_ON_P2;  // reset also floatin-point patch flags

        /* if alias target, not referenced and not defined, ignore */
        if( (sym->info & SYM_ALIAS_TARGET) && (sym->info & (SYM_DEFINED | SYM_REFERENCED | SYM_LOCAL_REF | SYM_EXPORTED)) == 0 )
            continue;

        if( (sym->info & (SYM_DEFINED | SYM_IS_ALTDEF)) == 0 )  {
            if( LinkFlags & LF_UNDEFS_ARE_OK ) {
                level = WRN;
            } else {
                level = MILD_ERR;
                LinkState |= LS_UNDEFED_SYM_ERROR;
            }
            LnkMsg( level+MSG_UNDEF_REF, "S", sym );
        }
    }
}

symbol *AddAltDef( symbol *sym, sym_info sym_type )
/*************************************************/
{
    symbol *    altsym;

    if( (LinkFlags & LF_INC_LINK_FLAG) == 0 )
        return( sym );
    altsym = AddSym();
    SET_SYM_TYPE( altsym, sym_type );
    altsym->info |= SYM_DEAD | SYM_IS_ALTDEF;
    altsym->mod = CurrMod;
    altsym->name = sym->name;
    altsym->e.mainsym = sym;
    Ring2Append( &CurrMod->publist, altsym );
    RingAppend( &sym->u.altdefs, altsym );
    return( altsym );
}

symbol *HashReplace( symbol *sym )
/********************************/
{
    symbol *    newsym;

    if( sym->mod == NULL )
        return( sym );
    Ring2Prune( &sym->mod->publist, sym );
    if( IS_SYM_COMMUNAL( sym ) ) {
        sym->p.seg->isdead = true;
    }
    if( (LinkFlags & LF_INC_LINK_FLAG) == 0 )
        return( sym );
    newsym = AddSym();
    newsym->e.mainsym = sym;
    newsym->name = sym->name;
    newsym->namelen_cmp = sym->namelen_cmp;
    newsym->info = sym->info | SYM_DEAD | SYM_IS_ALTDEF;
    Ring2Append( &sym->mod->publist, newsym );
    RingAppend( &sym->u.altdefs, newsym );
    if( IS_SYM_NICOMDEF( sym ) ) {
        sym->p.cdefsize = sym->p.seg->length;
    }
    return( sym );
}

static void SetDataSymInfo( symbol *sym, symbol *old )
/****************************************************/
{
    sym->u.datasym = NULL;
    sym->p.seg->u1.vm_ptr = old->p.seg->u1.vm_ptr;
    sym->info |= SYM_HAS_DATA;
    old->info &= ~SYM_HAS_DATA;
    old->u.datasym = sym;
}

static bool SetNewDataSym( void *_dead, void *_sym )
/**************************************************/
{
    symbol *deadsym = _dead;
    symbol *sym = _sym;

    if( deadsym == sym->u.datasym ) {
        if( deadsym->u.datasym == NULL ) {
            SetDataSymInfo( sym, deadsym );
        } else {
            sym->u.datasym = deadsym->u.datasym;
        }
        return( true );
    }
    return( false );
}

static void CleanAltDefs( symbol *sym )
/*************************************/
{
    symbol *    testring;
    symbol *    altsym;

    if( IS_SYM_ALIAS( sym )
      || (sym->info & SYM_DEAD)
      || sym->u.altdefs == NULL )
        return;
    testring = NULL;
    while( (altsym = RingPop( &sym->u.altdefs )) != NULL ) {
        if( altsym->info & SYM_KILL ) {
            if( altsym->info & SYM_HAS_DATA ) {
                altsym->u.datasym = NULL;
                RingAppend( &testring, altsym );
            }
        } else if( (altsym->info & SYM_HAS_DATA) == 0 ) {
            RingLookup( testring, SetNewDataSym, altsym );
        }
    }
}

static void UndefSymbol( symbol *sym )
/************************************/
{
    ClearSymUnion( sym );
    SET_ADDR_UNDEFINED( sym->addr );
    if( (sym->info & SYM_EXPORTED) == 0
      && sym->e.def != NULL ) {
        sym->info = SYM_LAZY_REF | SYM_REFERENCED;
    } else {
        sym->info = SYM_REGULAR | SYM_REFERENCED;
    }
    sym->mod = NULL;
}

static void PurgeHashTable( symbol **curr )
/*****************************************/
{
    while( *curr != NULL ) {
        CleanAltDefs( *curr );
        if( (*curr)->info & SYM_KILL ) {
            if( (*curr)->info & SYM_RELOC_REFD ) {
                UndefSymbol( *curr );
                curr = &(*curr)->hash;
            } else {
                *curr = (*curr)->hash;
            }
        } else {
            curr = &(*curr)->hash;
        }
    }
}

static void CleanupOldAltdefs( symbol **curr )
/********************************************/
/* some symbols may have been marked SYM_KILL during PurgeHashTable. Make
 * sure these are gone */
{
    while( *curr != NULL ) {
        if( (*curr)->info & SYM_KILL ) {
            *curr = (*curr)->hash;
        } else {
            curr = &(*curr)->hash;
        }
    }
}

static void WalkAHashTable( void (*fn)(symbol **), symbol **symtab,
                                unsigned size )
/*****************************************************************/
{
    for( ; size > 0; --size ) {
        fn( symtab );
        symtab++;
    }
}

static void WalkHashTables( void (*fn)(symbol **) )
/***************************************************/
{
    WalkAHashTable( fn, GlobalSymPtrs, GLOBAL_TABSIZE );
    WalkAHashTable( fn, StaticSymPtrs, STATIC_TABSIZE );
}

void PurgeSymbols( void )
/******************************/
{
    symbol **   list;
    symbol *    sym;

    WalkHashTables( PurgeHashTable );
    WalkHashTables( CleanupOldAltdefs );
    for( list = &HeadSym; *list != NULL; ) {
        sym = *list;
        if( sym->info & SYM_KILL ) {
            *list = sym->link;
            FreeSymbol( sym );
        } else if( sym->info & SYM_IS_ALTDEF ) {
            *list = sym->link;          // gonna get rid of these later
        } else {
            if( IS_SYM_ALIAS( sym )
              && (sym->info & SYM_WAS_LAZY) ) {
                WipeSym( sym );
                sym->info = SYM_WEAK_REF | SYM_REFERENCED;
            }
            LastSym = sym;
            list = &(*list)->link;
        }
    }
}

void ConvertLazyRefs( void )
/*********************************/
/* go through all symbols, & turn lazy refs to aliases to default sym. */
{
    symbol *    defsym;
    symbol *    sym;

    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        if( IS_SYM_A_REF( sym ) ) {
            if( IS_SYM_VF_REF( sym ) ) {
                defsym = *(sym->e.vfdata);
                _LnkFree( sym->e.vfdata );
            } else {
                defsym = sym->e.def;
                if( sym->info & SYM_VF_MARKED ) {
                    DefStripSym( sym, AllocSegData() ); // see note 1 below.
                }
            }
            WeldSyms( sym, defsym );
            sym->info |= SYM_WAS_LAZY;
            if( LinkFlags & LF_SHOW_DEAD ) {
                if( MapFile != NULL ) {
                    WriteMapLnkMsg( MSG_SYMBOL_DEAD, "S", sym );
                }
            }
        }
    }
}

/*
 * NOTE 1 for above function:
 * this is done because of pure virtual functions.  A pure virtual function
 * can be referenced with a vf reference coment, and never be defined, yet we
 * still want this vf reference to have an effect on the vf elimination call
 * graph.  So... we look at all undefed lazy refs that have been referenced
 * by a vf reference coment, and add a bogus segdata to the graph to register
 * the effects of this references
*/

static void MarkSymTraced( void *sym )
/************************************/
{
    ((symbol *)sym)->info |= SYM_TRACE;
}

void TraceSymList( symbol * sym )
/**************************************/
{
    Ring2Walk( sym, MarkSymTraced );
}

symbol *MakeWeakExtdef( const char *name, symbol *def )
/*****************************************************/
/* make a weak extdef */
{
    symbol * sym;

    sym = SymOp( ST_CREATE, name, strlen( name ) );
    sym->e.def = def;
    SET_SYM_TYPE( sym, SYM_LINK_WEAK_REF );
    return( sym );
}

void ConvertVFSym( symbol * sym )
/**************************************/
/* convert the symbol from a virtual function def. record to either a extdef
 * or a lazy reference*/
{
    symbol *    defsym;

    defsym = *(sym->e.vfdata);
    _LnkFree( sym->e.vfdata );
    sym->e.def = defsym;
    if( IS_SYM_PURE_REF( sym ) ) {
        SET_SYM_TYPE( sym, SYM_LAZY_REF );
    } else {
        SET_SYM_TYPE( sym, SYM_REGULAR );
    }
    sym->info &= ~SYM_CHECKED;
}

offset SymbolAbsAddr( symbol *sym )
/****************************************/
{
    offset      addr;
    group_entry *gr;

    gr = SymbolGroup( sym );
    addr = sym->addr.off;
    if( gr != NULL ) {
        addr += gr->linear + FmtData.base;
    } else {
        // it is an imported symbol; addr is already absolute
    }
    return( addr );
}

group_entry *SymbolGroup( symbol *sym )
/********************************************/
{
    group_entry *group;

    if( IS_SYM_ALIAS( sym ) ) {
        group = NULL;
    } else if( IS_SYM_GROUP( sym ) ) {
        for( group = Groups; group != NULL; group = group->next_group ) {
            if( sym == group->sym ) {
                break;
            }
        }
    } else {
        if( sym->p.seg != NULL ) {
            group = sym->p.seg->u.leader->group;
        } else {
            group = NULL;
        }
    }
    return( group );
}
