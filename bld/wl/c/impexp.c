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

static void ReadOldLib( void );
static void ReadNameTable( f_handle the_file );

static entry_export * FindPlace( entry_export *exp )
/**************************************************/
// finds the correct place to put exp to keep the export list sorted.
{
    entry_export *  place;
    entry_export *  prev;
    entry_export *  ret;

    ret = NULL;
    prev = NULL;
    place = exp->next;
    if( place != NULL && place->ordinal <= exp->ordinal ) {
        for(;;) {
            if( place->ordinal == exp->ordinal ) {
                LnkMsg( WRN + MSG_DUP_EXP_ORDINAL, NULL );
                exp->ordinal = 0;    // if duplicate, assign a new one later
                break;
            } else if ( place->ordinal > exp->ordinal ) {
                ret = exp->next;      //note: this can't happen 1st time
                exp->next = place;
                prev->next = exp;
                break;
            }
            if( place->next == NULL ) {  // no more entries, so put on the
                ret = exp->next;     // end, then break the loop.
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
    entry_export *  next;

    _LnkFree( exp->impname );
    next = exp->next;
    CarveFree( CarveExportInfo, exp );
    return( next );
}

void FreeExportList( void )
/********************************/
{
    entry_export *  exp;

    if( LinkFlags & INC_LINK_FLAG ) return;
    for( exp = FmtData.u.os2.exports; exp != NULL; ) {
        exp = FreeAnExport( exp );
    }
}

void AddToExportList( entry_export *exp )
/**********************************************/
{
    entry_export **     owner;
    entry_export **     place;
    entry_export *      curr;
    size_t              len;
    size_t              currlen;

    place = NULL;
    owner = &FmtData.u.os2.exports;
    len = strlen( exp->name );
    for( ;; ) {
        curr = *owner;
        if( curr == NULL ) break;
        currlen = strlen( curr->name );
        if( currlen == len && CmpRtn( curr->name, exp->name, len ) == 0 ) {
            if( !IS_FMT_INCREMENTAL(ObjFormat) ) {
                if( !IS_SYM_COMDAT(exp->sym) ) {
                    LnkMsg( LOC+WRN+MSG_DUP_EXP_NAME, "s", curr->name );
                }
                FreeAnExport( exp );
            }
            return;
        }
        if( place == NULL ) {
            if( exp->ordinal == 0 ) {
                if( curr->ordinal != 0 ) place = owner;
            } else if( curr->ordinal == exp->ordinal ) {
                LnkMsg( WRN+MSG_DUP_EXP_ORDINAL, NULL );
                exp->ordinal = 0;    // if duplicate, assign a new one later
                place = &FmtData.u.os2.exports;
            } else if ( curr->ordinal > exp->ordinal ) {
                place = owner;
            }
        }
        owner = &curr->next;
    }
    if( IS_SYM_VF_REF( exp->sym ) ) {
        ClearRefInfo( exp->sym );
    }
    exp->sym->e.export = exp;
    exp->sym->info |= SYM_EXPORTED;
    if( place == NULL ) place = owner;
    DEBUG(( DBG_NEW, "%s", exp->name ));
    exp->next = *place;
    *place = exp;
}

static unsigned CheckStdCall( const char *name, unsigned len )
/************************************************************/
// check to see if a name is in the stdcall _name@xx format
// this returns the total number of characters to be removed from the name
// including the beginning _
{
    const char  *teststr;
    unsigned    chop;

    if( len <= 3 )
        return 0;
    chop = 0;
    teststr = name + len - 1;
    if( *name == '_' && isdigit(*teststr) ) {
        teststr--;
        if( *teststr == '@' ) {
            chop = 3;
        } else if( isdigit(*teststr) ) {
            teststr--;
            if( *teststr == '@' ) {
                chop = 4;
            }
        }
    }
    return chop;
}

entry_export *AllocExport( const char *name, size_t len )
/*******************************************************/
{
    entry_export    *exp;
    size_t          chop;

    exp = CarveAlloc( CarveExportInfo );
    exp->isexported = true;
    exp->isprivate = false;
    exp->ismovable = false;
    exp->isresident = false;
    exp->isfree = false;
    if( name == NULL ) {
        exp->name = NULL;
    } else {
        if( (FmtData.type & MK_PE) && FmtData.u.pe.no_stdcall ) {
            chop = CheckStdCall( name, len );
            if( chop > 0 ) {
                name++;
                len -= chop;
            }
        }
        exp->name = AddSymbolStringTable( &PermStrings, name, len );
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
    entry_export *  exp;

    exp = AllocExport( expname->name, expname->len );
    exp->isanonymous = false;
    exp->iopl_words = flags & EXPDEF_IOPLMASK;
    exp->isresident = (flags & EXPDEF_RESIDENT) != 0;
    if( intname->len != 0 ) {
        exp->sym = SymOp( ST_CREATE | ST_REFERENCE, intname->name, intname->len);
    } else {
        exp->sym = SymOp( ST_CREATE | ST_REFERENCE, expname->name, expname->len);
    }
    if( LinkFlags & STRIP_CODE ) {
        DataRef( exp->sym );    // make sure it isn't removed.
    }
    if( flags & EXPDEF_ORDINAL ) {
        exp->ordinal = ordinal;
    } else {
        exp->isresident = true; // no ordinal specd == isresident
    }
    AddToExportList( exp );
}

dll_sym_info * AllocDLLInfo( void )
/****************************************/
{
    dll_sym_info * dll;

    dll = CarveAlloc( CarveDLLInfo );
    dll->isfree = false;
    return dll;
}

void FreeImport( dll_sym_info * dll )
/******************************************/
{
    CarveFree( CarveDLLInfo, dll );
}

static symbol * GetIATSym( symbol *sym )
/**************************************/
{
    char *      iatname;
    size_t      prefixlen;
    size_t      namelen;
    char *      name;

    name = sym->name;
    if( LinkState & HAVE_PPC_CODE) {
        DbgAssert(name[0] == '.' && name[1] == '.');
        name += 2;  // skip '..' at the beginning of the name
    }
    prefixlen = sizeof(ImportSymPrefix) - 1;
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
    dll_sym_info *      dll;

    if( (sym->info & SYM_DEFINED) == 0 ) {
        sym->info |= SYM_DEFINED | SYM_DCE_REF;
        if( LinkFlags & STRIP_CODE ) {
            DefStripImpSym(sym);
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
        dll->m.modnum = AddNameTable( modname->name, modname->len, true, &FmtData.u.os2.mod_ref_list );
        dll->isordinal = ordinal != NOT_IMP_BY_ORDINAL;
        if( !dll->isordinal ) {
            dll->u.entry = AddNameTable( extname->name, extname->len, false, &FmtData.u.os2.imp_tab_list );
        } else {
            dll->u.ordinal = ordinal;
        }
    }
}

void KillDependantSyms( symbol *sym )
/******************************************/
{
    if( (FmtData.type & MK_PE) == 0 )
        return;
    sym = GetIATSym( sym );
    sym->info |= SYM_KILL;
}

void AssignOrdinals( void )
/********************************/
/* assign ordinal values to entries in the export list */
{
    entry_export *      exp;
    entry_export *      place;
    entry_export *      prev;
    bool                isspace;

    if( FmtData.u.os2.exports != NULL ) {
        if( FmtData.u.os2.old_lib_name != NULL ) {
            ReadOldLib();
        }
        prev = FmtData.u.os2.exports;
        place = prev->next;
        isspace = false;
        for( exp = FmtData.u.os2.exports; exp->ordinal == 0; exp = FmtData.u.os2.exports ) {
            // while still unassigned values
            for(;;) {                 // search for an unassigned value
                if( place != NULL ) {
                    isspace = ( place->ordinal - prev->ordinal > 1 );
                }
                if( place == NULL || isspace ) {
                    if( FmtData.u.os2.exports != prev ) {
                        FmtData.u.os2.exports = exp->next;
                        prev->next = exp;
                        exp->next = place;
                    }
                    exp->ordinal = prev->ordinal + 1;
                    prev = exp;      // now exp is 'previous' to place
                    break;
                } else {
                    prev = place;
                    place = place->next;
                }
            }
        }
    }
}

static void ReadOldLib( void )
/****************************/
// Read an old DLL & match ordinals of exports in it with exports in this.
{
    f_handle    the_file;
    long        filepos;
    union {
        dos_exe_header  dos;
        os2_exe_header  os2;
        os2_flat_header os2f;
        exe_pe_header   pe;
    }           head;
    char *      fname;
    pe_object * objects;
    pe_object * currobj;

    fname = FmtData.u.os2.old_lib_name;
    the_file = QOpenR( fname );
    QRead( the_file, &head, sizeof(dos_exe_header), fname );
    if( head.dos.signature != DOS_SIGNATURE || head.dos.reloc_offset != 0x40 ) {
        LnkMsg( WRN + MSG_INV_OLD_DLL, NULL );
    } else {
        QSeek( the_file, 0x3c, fname );
        QRead( the_file, &filepos, sizeof( long ), fname );
        QSeek( the_file, filepos, fname );
        QRead( the_file, &head, sizeof(head), fname );
        if( head.os2.signature == OS2_SIGNATURE_WORD ) {
            QSeek( the_file, filepos + head.os2.resident_off, fname );
            ReadNameTable( the_file );
            QSeek( the_file, head.os2.nonres_off, fname );
            ReadNameTable( the_file );
        } else if( head.os2f.signature == OSF_FLAT_SIGNATURE || head.os2f.signature == OSF_FLAT_LX_SIGNATURE ) {
            if( head.os2f.resname_off != 0 ) {
                QSeek( the_file, filepos + head.os2f.resname_off, fname );
                ReadNameTable( the_file );
            }
            if( head.os2f.nonres_off != 0 ) {
                QSeek( the_file, head.os2f.nonres_off, fname );
                ReadNameTable( the_file );
            }
        } else if( head.pe.pe32.signature == PE_SIGNATURE ) {
            int                 num_objects;
            pe_hdr_table_entry  *table;

            if( IS_PE64( head.pe ) ) {
                num_objects = PE64( head.pe ).num_objects;
                table = PE64( head.pe ).table;
            } else {
                num_objects = PE32( head.pe ).num_objects;
                table = PE32( head.pe ).table;
            }
            _ChkAlloc( objects, num_objects * sizeof( pe_object ) );
            QRead( the_file, objects, num_objects * sizeof( pe_object ), fname );
            currobj = objects;
            for( ; num_objects > 0; --num_objects ) {
                if( currobj->rva == table[PE_TBL_EXPORT].rva ) {
                    QSeek( the_file, currobj->physical_offset, fname );
                    table[PE_TBL_EXPORT].rva -= currobj->physical_offset;
                    ReadPEExportTable( the_file, &table[PE_TBL_EXPORT]);
                    break;
                }
                currobj++;
            }
            _LnkFree( objects );
            if( num_objects == 0 ) {
                LnkMsg( WRN + MSG_INV_OLD_DLL, NULL );
            }
        } else {
            LnkMsg( WRN+MSG_INV_OLD_DLL, NULL );
        }
    }
    QClose( the_file, fname );
    _LnkFree( fname );
    FmtData.u.os2.old_lib_name = NULL;
}

void CheckExport( char * name, ordinal_t ordinal, exportcompare_fn *rtn )
/***********************************************************************/
/* check if the name is exported and hasn't been assigned a value, and if so,
 * give it the specified value */
{
    entry_export *  place;
    entry_export *  prev;

    DEBUG(( DBG_OLD, "Oldlib export %s ordinal %l", name, ordinal ));
    prev = NULL;
    for( place = FmtData.u.os2.exports; place != NULL; place = place->next ) {
        if( rtn( place->name, name ) == 0 ) {
            if( place->ordinal == 0 ) {
                place->ordinal = ordinal;
                place = FindPlace( place );
                if( place != NULL ) {
                    if( prev == NULL ) {
                        FmtData.u.os2.exports = place;
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

static void ReadNameTable( f_handle the_file )
/********************************************/
// Read a name table & set export ordinal value accordingly.
{
    unsigned_8          len_u8;
    unsigned_16         ordinal;
    exportcompare_fn    *rtn;
    char                *fname;

    fname = FmtData.u.os2.old_lib_name;
    if( LinkFlags & CASE_FLAG ) {
        rtn = strcmp;
    } else {
        rtn = stricmp;
    }                             // skip the module name & ordinal.
    for( ;; ) {
        QRead( the_file, &len_u8, sizeof( len_u8 ), fname );
        if( len_u8 == 0 )
            break;
        QRead( the_file, TokBuff, len_u8, fname );
        QRead( the_file, &ordinal, sizeof( unsigned_16 ), fname );
        if( ordinal == 0 )
            continue;
        TokBuff[len_u8] = '\0';
        CheckExport( TokBuff, ordinal, rtn );
    }
}

ordinal_t FindEntryOrdinal( targ_addr addr, group_entry *grp )
/************************************************************/
{
    ordinal_t       max_ord;
    entry_export    **owner;
    entry_export    *exp;

    max_ord = 0;
    owner = &FmtData.u.os2.exports;
    for( ;; ) {
        exp = *owner;
        if( exp == NULL )
            break;
        if( addr.seg == exp->addr.seg && addr.off == exp->addr.off ) {
            return( exp->ordinal );
        }
        if( exp->ordinal >= max_ord )
            max_ord = exp->ordinal;
        owner = &exp->next;
    }
    exp = AllocExport( NULL, 0 );
    exp->sym = NULL;
    exp->isexported = false;
    exp->isanonymous = false;
    exp->ordinal = max_ord + 1;
    exp->ismovable = ( (grp->segflags & SEG_MOVABLE) != 0 );
    exp->next = NULL;
    exp->addr = addr;
    *owner = exp;
    return( exp->ordinal );
}

char * ImpModuleName( dll_sym_info *dll )
/**********************************************/
{
    return dll->m.modnum->name;
}

bool IsSymElfImported( symbol *s )
/***************************************/
{
    return IS_SYM_IMPORTED(s);
}

bool IsSymElfExported( symbol *s )
/***************************************/
{
    return FmtData.u.elf.exportallsyms || (s->info & SYM_EXPORTED);
}

bool IsSymElfImpExp( symbol *s )
/*************************************/
{
    return IsSymElfImported(s) || IsSymElfExported(s);
}
