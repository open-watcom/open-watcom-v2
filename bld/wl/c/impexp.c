/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Utilities for handling imports and exports.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include "walloca.h"
#include "linkstd.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "specials.h"
#include "fileio.h"
#include "alloc.h"
#include "exeos2.h"
#include "exeflat.h"
#include "exepe.h"
#include "exedos.h"
#include "objstrip.h"
#include "ring.h"
#include "carve.h"
#include "strtab.h"
#include "permdata.h"
#include "loados2.h"
#include "loadpe.h"
#include "impexp.h"

#include "clibext.h"


static entry_export *FindPlace( entry_export *exp )
/*************************************************/
// finds the correct place to put exp to keep the export list sorted.
{
    entry_export    *place;
    entry_export    *prev;
    entry_export    *ret;

    ret = NULL;
    prev = NULL;
    place = exp->next;
    if( place != NULL && place->ordinal <= exp->ordinal ) {
        for( ;; ) {
            if( place->ordinal == exp->ordinal ) {
                LnkMsg( WRN + MSG_DUP_EXP_ORDINAL, NULL );
                exp->ordinal = 0;       // if duplicate, assign a new one later
                break;
            }
            if( place->ordinal > exp->ordinal ) {
                ret = exp->next;        // note: this can't happen 1st time
                exp->next = place;
                prev->next = exp;
                break;
            }
            if( place->next == NULL ) { // no more entries, so put on the
                ret = exp->next;        // end, then break the loop.
                place->next = exp;
                exp->next = NULL;
                break;
            }
            prev = place;
            place = place->next;
        }
    }
    return( ret );
}

static entry_export *FreeAnExport( entry_export *exp )
/****************************************************/
{
    entry_export    *next;

    _LnkFree( exp->impname );
    next = exp->next;
    CarveFree( CarveExportInfo, exp );
    return( next );
}

void FreeExportList( void )
/*************************/
{
    entry_export    *exp;

    if( (LinkFlags & LF_INC_LINK_FLAG) == 0 ) {
        for( exp = FmtData.u.os2fam.exports; exp != NULL; ) {
            exp = FreeAnExport( exp );
        }
    }
}

void AddToExportList( entry_export *exp )
/***************************************/
{
    entry_export        **owner;
    entry_export        **place;
    entry_export        *curr;
    size_t              len;
    size_t              currlen;

    place = NULL;
    len = strlen( exp->name.u.ptr );
    for( owner = &FmtData.u.os2fam.exports; (curr = *owner) != NULL; owner = &curr->next ) {
        currlen = strlen( curr->name.u.ptr );
        if( currlen == len && CmpRtn( curr->name.u.ptr, exp->name.u.ptr, len ) == 0 ) {
            if( !IS_FMT_INCREMENTAL( ObjFormat ) ) {
                if( !IS_SYM_COMDAT( exp->sym ) ) {
                    LnkMsg( LOC+WRN+MSG_DUP_EXP_NAME, "s", curr->name.u.ptr );
                }
                FreeAnExport( exp );
            }
            return;
        }
        if( place == NULL ) {
            if( exp->ordinal == 0 ) {
                if( curr->ordinal != 0 ) {
                    place = owner;
                }
            } else if( curr->ordinal == exp->ordinal ) {
                LnkMsg( WRN+MSG_DUP_EXP_ORDINAL, NULL );
                exp->ordinal = 0;    // if duplicate, assign a new one later
                place = &FmtData.u.os2fam.exports;
            } else if( curr->ordinal > exp->ordinal ) {
                place = owner;
            }
        }
    }
    if( IS_SYM_VF_REF( exp->sym ) ) {
        ClearRefInfo( exp->sym );
    }
    exp->sym->e.export = exp;
    exp->sym->info |= SYM_EXPORTED;
    if( place == NULL )
        place = owner;
    DEBUG(( DBG_NEW, "%s", exp->name ));
    exp->next = *place;
    *place = exp;
}

static size_t CheckStdCall( const char *name, size_t len )
/********************************************************/
// check to see if a name is in the stdcall _name@xx format
// this returns the total number of characters to be removed from the name
// including the beginning _
{
    const char  *teststr;
    size_t      chop;

    chop = 0;
    if( len > 3 ) {
        teststr = name + len - 1;
        if( *name == '_' && isdigit( *teststr ) ) {
            teststr--;
            if( *teststr == '@' ) {
                chop = 3;
            } else if( isdigit( *teststr ) ) {
                teststr--;
                if( *teststr == '@' ) {
                    chop = 4;
                }
            }
        }
    }
    return( chop );
}

entry_export *AllocExport( const char *name, size_t len )
/*******************************************************/
{
    entry_export    *exp;
    size_t          chop;

    exp = CarveAlloc( CarveExportInfo );
    exp->isresident = false;
    exp->isexported = true;
    exp->ismovable = false;
    exp->isanonymous = false;
    exp->isfree = false;
    exp->isprivate = false;
    exp->isiopl = false;
    if( name == NULL ) {
        exp->name.u.ptr = NULL;
    } else {
        if( (FmtData.type & MK_PE) && FmtData.u.pe.no_stdcall ) {
            chop = CheckStdCall( name, len );
            if( chop > 0 ) {
                name++;
                len -= chop;
            }
        }
        exp->name.u.ptr = AddSymbolStringTable( &PermStrings, name, len );
    }
    exp->impname = NULL;
    exp->iopl_words = 0;
    exp->ordinal = 0;
    return( exp );
}

#define EXPDEF_ORDINAL  0x80
#define EXPDEF_RESIDENT 0x40
#define EXPDEF_IOPLMASK 0x1F

void MSExportKeyword( const length_name *expname, const length_name *intname, unsigned flags, ordinal_t ordinal )
/***************************************************************************************************************/
// Process the Microsoft Export keyword.
{
    entry_export    *exp;

    exp = AllocExport( expname->name, expname->len );
    exp->iopl_words = flags & EXPDEF_IOPLMASK;
    if( flags & EXPDEF_RESIDENT )
        exp->isresident = true;
    if( intname->len != 0 ) {
        exp->sym = SymOp( ST_CREATE_REFERENCE, intname->name, intname->len );
    } else {
        exp->sym = SymOp( ST_CREATE_REFERENCE, expname->name, expname->len );
    }
    if( LinkFlags & LF_STRIP_CODE ) {
        DataRef( exp->sym );    // make sure it isn't removed.
    }
    if( flags & EXPDEF_ORDINAL ) {
        exp->ordinal = ordinal;
    } else {
        exp->isresident = true; // no ordinal specd == isresident
    }
    AddToExportList( exp );
}

dll_sym_info *AllocDLLInfo( void )
/********************************/
{
    dll_sym_info    *dll;

    dll = CarveAlloc( CarveDLLInfo );
    dll->isfree = false;
    return( dll );
}

void FreeImport( dll_sym_info *dll )
/**********************************/
{
    CarveFree( CarveDLLInfo, dll );
}

static symbol *GetIATSym( symbol *sym )
/*************************************/
{
    char        *iatname;
    size_t      prefixlen;
    size_t      namelen;
    const char  *name;

    name = sym->name.u.ptr;
    if( LinkState & LS_HAVE_PPC_CODE ) {
        DbgAssert(name[0] == '.' && name[1] == '.');
        name += 2;  // skip '..' at the beginning of the name
    }
    prefixlen = sizeof( ImportSymPrefix ) - 1;
    namelen = strlen( name );
    iatname = alloca( namelen + prefixlen + 1 );
    memcpy( iatname, ImportSymPrefix, prefixlen );
    memcpy( iatname + prefixlen, name, namelen );
    prefixlen += namelen;
    iatname[prefixlen] = '\0';
    return( SymOp( ST_CREATE, iatname, prefixlen ) );
}

void MSImportKeyword( symbol *sym, const length_name *modname, const length_name *extname, ordinal_t ordinal )
/************************************************************************************************************/
/* process the MS import keyword definition */
{
    dll_sym_info    *dll;

    if( (sym->info & SYM_DEFINED) == 0 ) {
        sym->info |= SYM_DEFINED | SYM_DCE_REF;
        if( LinkFlags & LF_STRIP_CODE ) {
            DefStripImpSym( sym );
        }
        SET_SYM_TYPE( sym, SYM_IMPORTED );
        dll = AllocDLLInfo();
        sym->p.import = dll;
        if( FmtData.type & MK_PE ) {
            dll->iatsym = GetIATSym( sym );
            dll->iatsym->info |= SYM_DEFINED | SYM_DCE_REF;
            SET_SYM_TYPE( dll->iatsym, SYM_IMPORTED );
            dll->iatsym->p.import = NULL;
        }
        dll->m.modnum = AddNameTable( modname->name, modname->len, true, &FmtData.u.os2fam.mod_ref_list );
        if( ordinal == NOT_IMP_BY_ORDINAL ) {
            dll->isordinal = false;
            dll->u.entry = AddNameTable( extname->name, extname->len, false, &FmtData.u.os2fam.imp_tab_list );
        } else {
            dll->isordinal = true;
            dll->u.ordinal = ordinal;
        }
    }
}

void KillDependantSyms( symbol *sym )
/******************************************/
{
    if( FmtData.type & MK_PE ) {
        sym = GetIATSym( sym );
        sym->info |= SYM_KILL;
    }
}

void CheckExport( const char *name, ordinal_t ordinal, bool cmpcase )
/*******************************************************************/
/* check if the name is exported and hasn't been assigned a value, and if so,
 * give it the specified value */
{
    entry_export    *place;
    entry_export    *prev;
    int             (*rtn)(const char *,const char *);

    DEBUG(( DBG_OLD, "Oldlib export %s ordinal %l", name, ordinal ));
    rtn = ( cmpcase ) ? strcmp : stricmp;
    prev = NULL;
    for( place = FmtData.u.os2fam.exports; place != NULL; place = place->next ) {
        if( rtn( place->name.u.ptr, name ) == 0 ) {
            if( place->ordinal == 0 ) {
                place->ordinal = ordinal;
                place = FindPlace( place );
                if( place != NULL ) {
                    if( prev == NULL ) {
                        FmtData.u.os2fam.exports = place;
                    } else {
                        prev->next = place;
                    }
                }
            }
            break;
        }
        prev = place;
    }
}

ordinal_t FindEntryOrdinal( targ_addr addr, group_entry *grp )
/************************************************************/
{
    ordinal_t       max_ord;
    entry_export    **owner;
    entry_export    *exp;

    max_ord = 0;
    for( owner = &FmtData.u.os2fam.exports; (exp = *owner) != NULL; owner = &exp->next ) {
        if( addr.seg == exp->addr.seg && addr.off == exp->addr.off ) {
            return( exp->ordinal );
        }
        if( exp->ordinal >= max_ord ) {
            max_ord = exp->ordinal;
        }
    }
    exp = AllocExport( NULL, 0 );
    exp->sym = NULL;
    exp->isexported = false;
    exp->ordinal = max_ord + 1;
    if( grp->segflags & SEG_MOVABLE )
        exp->ismovable = true;
    exp->next = NULL;
    exp->addr = addr;
    *owner = exp;
    return( exp->ordinal );
}

char *ImpModuleName( dll_sym_info *dll )
/**************************************/
{
    return( dll->m.modnum->name.u.ptr );
}

bool IsSymElfImported( symbol *sym )
/**********************************/
{
    return( IS_SYM_IMPORTED( sym ) );
}

bool IsSymElfExported( symbol *sym )
/**********************************/
{
    return( FmtData.u.elf.exportallsyms || (sym->info & SYM_EXPORTED) );
}

bool IsSymElfImpExp( symbol *sym )
/********************************/
{
    return( IsSymElfImported( sym ) || IsSymElfExported( sym ) );
}
