/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Librarian OMF symbol processing.
*
****************************************************************************/


#include "wlib.h"

typedef struct lname {
    struct lname        *next;
    bool                local;
    unsigned char       len;
    char                name[1];
} lname;

typedef struct COMMON_BLK {
    struct COMMON_BLK       *next;
    unsigned short          segindex;
}   common_blk;


typedef enum {
    S_COMDEF,
    S_COMDAT,
    S_PUBDEF,
    S_ALIAS
} sym_type;

typedef enum omf_oper {
    OMF_ISIZE,
    OMF_OSIZE,
    OMF_COPY,
    OMF_SYMS
} omf_oper;

static unsigned short   CurrSegRef = 0;
static char             Rec32Bit;
static lname            *LName_Head;
static lname            **LName_Owner;
static unsigned_8       *RecPtr;
static unsigned_8       *RecEnd;
static common_blk       *CurrCommonBlk;
static unsigned short   SegDefCount;

static OmfRecord        *omfRec;
static size_t           omfRecLen;


static void SetOmfBuffer( unsigned_16 len )
/*****************************************/
{
    if( len + 3 > omfRecLen ) {
        OmfRecord *new;

        omfRecLen = len + 3;
        new = MemAlloc( omfRecLen );
        MemFree( omfRec );
        omfRec = new;
    }
}

static unsigned_8 *SetOmfRecBuffer( unsigned_8 type, unsigned_16 len )
/********************************************************************/
{
    SetOmfBuffer( len );
    omfRec->basic.type = type;
    omfRec->basic.len = len;
    return( omfRec->basic.contents );
}

void InitOmfRec( void )
/*********************/
{
    omfRecLen = 0;
    SetOmfBuffer( INIT_OMF_REC_SIZE );
}

void FiniOmfRec( void )
/*********************/
{
    MemFree( omfRec );
    omfRecLen = 0;
}

static unsigned short GetIndex( void )
/************************************/
{
    unsigned short index;

    index = *RecPtr++;
    if( index > 0x7F ) {
        index &= 0x7f;
        index = ( index << 8 ) | *RecPtr++;
    }
    return( index );
}

static void GetOffset( void )
/***************************/
{
    RecPtr += 2;
    if( Rec32Bit ) { /* if 386, must skip over 4 bytes */
        RecPtr += 2;
    }
}

static void GetIdx( void )
/*************************
 * throw away a group and segment number and possibly frame number
 */
{
    unsigned short      grp;

    grp = GetIndex();
    CurrSegRef = GetIndex();
    if( ( grp == 0 )
      && ( CurrSegRef == 0 ) ) {
        RecPtr += 2;                    /* skip over frame number */
    }
}

static bool IsCommonRef( void )
/******************************
 * dedicated routine for FORTRAN 77 common block
 */
{
    common_blk      *tmpblk;

    for( tmpblk = CurrCommonBlk; tmpblk != NULL; tmpblk = tmpblk->next ) {
        if( tmpblk->segindex < CurrSegRef )
            continue;
        if( tmpblk->segindex == CurrSegRef ) {
            return( true );
        }
        break;
    }
    return( false );
}

static void FreeCommonBlk( void )
/********************************
 * dedicated routine for FORTRAN 77 common block
 */
{
    common_blk          *tmpblk;

    while( CurrCommonBlk != NULL ) {
        tmpblk = CurrCommonBlk->next;
        MemFree( CurrCommonBlk );
        CurrCommonBlk = tmpblk;
    }
}

static void procsegdef( void )
/*****************************
 * dedicated routine for FORTRAN 77 common block
 */
{
    common_blk          *cmn;
    common_blk          **owner;

    uint_8  tmpbyte = ( *RecPtr & 0x1f ) >> 2; /* get "COMBINE" bits */

    SegDefCount++;
    if( tmpbyte == 6 ) {    /* COMBINE == COMMON, record it */
        owner = &CurrCommonBlk;
        for( ;; ) {
            cmn = *owner;
            if( cmn == NULL )
                break;
            owner = &cmn->next;
        }
        cmn = MemAlloc( sizeof( common_blk ) );
        cmn->segindex = SegDefCount;
        cmn->next = NULL;
        *owner = cmn;
    }
}

static void AddOMFSymbol( sym_type type, const char *symname )
/************************************************************/
{
    if( type == S_COMDEF
      || type == S_COMDAT
      || type == S_ALIAS
      || IsCommonRef() ) {
        AddSym( symname, SYM_WEAK, 0 );
    } else {
        AddSym( symname, SYM_STRONG, 0 );
    }
}

static unsigned_8 GetOMFName( char *name )
/*****************************************
 * from infl, get a intel name: length and name
 */
{
    unsigned_8  len;

    len = *RecPtr++;
    strncpy( name, (char *)RecPtr, len );
    RecPtr += len;
    name[len] = '\0';
    return( len );
}

static void getpubdef( void )
/****************************
 * loop over the publics in the record
 */
{
    char        name[256];

    GetIdx();
    while( RecPtr < RecEnd ) {
        GetOMFName( name );
        GetOffset();
        GetIndex();
        AddOMFSymbol( S_PUBDEF, name );
    }
}

static void GetComLen( void )
/***************************/
{
    switch( *RecPtr++ ) {
    case COMDEF_LEAF_4:
        RecPtr += 4;
        break;
    case COMDEF_LEAF_3:
        RecPtr += 3;
        break;
    case COMDEF_LEAF_2:
        RecPtr += 2;
        break;
    default:
        break;
    }
}

static void getcomdef( void )
/****************************
 * process a COMDEF record
 */
{
    char        name[256];

    while( RecPtr < RecEnd ) {
        GetOMFName( name );
        AddOMFSymbol( S_COMDEF, name );
        GetIndex();
        switch( *RecPtr++ ) {
        case COMDEF_FAR:
            GetComLen();
            /* fall through */
        case COMDEF_NEAR:
            GetComLen();
            break;
        }
    }
}

static lname *getIdxLName( void )
{
    unsigned            idx;
    lname               *ln;

    idx = GetIndex();
    if( idx == 0 )      /* no LNAME */
        return( NULL );
    /*
     * change from 1-based index to O-based
     */
    idx--;
    /*
     * get LNAME from linked list
     */
    ln = LName_Head;
    while( idx-- > 0 ) {
        ln = ln->next;
    }
    return( ln );
}

static void getcomdat( void )
/****************************
 * process a COMDAT record
 */
{
    unsigned            alloc;
    lname               *ln;

    if( *RecPtr & (COMDAT_CONTINUE | COMDAT_LOCAL) )
        return;
    RecPtr++;
    alloc = *RecPtr++ & COMDAT_ALLOC_MASK;
    RecPtr++;
    GetOffset();
    GetIndex();
    if( alloc == COMDAT_EXPLICIT ) {
        GetIdx();
    }
    ln = getIdxLName();
    if( ln == NULL
      || ln->local ) {
        return;
    }
    AddOMFSymbol( S_COMDAT, ln->name );
}

static void getlname( bool local )
/*********************************
 * process a LNAMES record
 */
{
    unsigned_8          len;
    char                name[256];
    lname               *ln;

    while( RecPtr < RecEnd ) {
        len = GetOMFName( name );
        ln = MemAlloc( sizeof( lname ) + len );
        strcpy( ln->name, name );
        ln->len = len;
        ln->local = local;
        ln->next = NULL;
        *LName_Owner = ln;
        LName_Owner = &ln->next;
    }
}

static void getalias( void )
/***************************
 * process an ALIAS record
 */
{
    char        name[256];

    while( RecPtr < RecEnd ) {
        /*
         * alias symbol
         */
        GetOMFName( name );
        AddOMFSymbol( S_ALIAS, name );
        /*
         * substitute symbol
         */
        GetOMFName( name );
    }
}

static void FreeLNames( void )
/****************************/
{
    lname           *next;

    while( LName_Head != NULL ) {
        next = LName_Head->next;
        MemFree( LName_Head );
        LName_Head = next;
    }
}

static void CalcOmfRecordCheckSum( OmfRecord *rec )
/*************************************************/
{
    unsigned_8      sum;
    unsigned_16     i;

    sum = 0;
    for( i = 0; i < rec->basic.len + 2; ++i ) {
        sum += rec->chkcalc[i];
    }
    rec->chkcalc[i] = -sum;
}

static bool ReadOmfRecord( libfile io )
/*************************************/
{
    unsigned_8  type;
    unsigned_16 len;

    if( LibRead( io, &type, sizeof( type ) ) != sizeof( type ) )
        return( false );
    if( LibRead( io, &len, sizeof( len ) ) != sizeof( len ) )
        return( false );
    SetOmfRecBuffer( type, GET_LE_16( len ) );
    if( LibRead( io, omfRec->basic.contents, omfRec->basic.len ) != omfRec->basic.len ) {
        return( false );
    }
    RecPtr = omfRec->basic.contents;
    RecEnd = RecPtr + omfRec->basic.len - 1;
    return( true );
}

static void trimOmfHeader( void )
/*******************************/
{
    unsigned_16 len;

    len = omfRec->basic.len;
    omfRec->basic.contents[len - 1] = '\0';
    TrimPathInPlace( (char *)omfRec->basic.contents + 1 );
    len = (unsigned_8)strlen( (char *)omfRec->basic.contents + 1 );
    omfRec->basic.contents[0] = (unsigned_8)len;
    omfRec->basic.len = len + 2;
}

static void WriteOmfRecord( libfile io, OmfRecord *rec )
/******************************************************/
{
    unsigned_16     len;

    len = rec->basic.len;
    CalcOmfRecordCheckSum( rec );
    WriteOmfRecHdr( io, rec->basic.type, GET_LE_16( len ) );
    LibWrite( io, rec->basic.contents, len );
}

static void WriteTimeStamp( libfile io, sym_file *sfile )
/*******************************************************/
{
    OmfTimeStamp    rec;

    rec.type = CMD_COMENT;
    rec.len = sizeof( rec ) - 3;
    rec.attribute = CMT_TNP | CMT_TNL;
    rec.class = CMT_LINKER_DIRECTIVE;
    rec.subclass = LDIR_OBJ_TIMESTAMP;
    rec.stamp = GET_LE_32( sfile->arch.date );
    WriteOmfRecord( io, (OmfRecord *)&rec );
}

static file_offset OmfProc( libfile src, libfile dst, sym_file *sfile, omf_oper oper )
/************************************************************************************/
{
    bool        time_stamp;
    char        new_line[] = { '\n' };
    bool        first;
    file_offset size;
    bool        EasyOMF;

    EasyOMF = false;
    time_stamp = false;
    size = 0;
    first = true;
    do {
        if( !ReadOmfRecord( src ) ) {
            FatalError( ERR_BAD_OBJECT, src->name );
        }
        if( oper == OMF_ISIZE ) {
            size += omfRec->basic.len + 3;
        } else {
            if( oper == OMF_SYMS ) {
                /*
                 * check to see if this is an Easy OMF-386 object file
                 * Only need to check the first comment record
                 */
                if( ( omfRec->basic.type & ~1 ) == CMD_COMENT ) {
                    if( omfRec->basic.len == 8
                      && omfRec->basic.contents[0] == CMT_TNP
                      && omfRec->basic.contents[1] == CMT_EASY_OMF
                      && memcmp( omfRec->basic.contents + 2, "80386", 5 ) == 0 ) {
                        EasyOMF = true;
                    }
                }
                if( omfRec->basic.type & 1 ) {
                    Rec32Bit = true;
                } else {
                    Rec32Bit = EasyOMF;
                }
                switch( omfRec->basic.type ) {
                case CMD_SEGDEF:
                case CMD_SEGD32:
                    procsegdef();
                    break;
                case CMD_PUBDEF:
                case CMD_PUBD32:
                    getpubdef();
                    break;
                case CMD_COMDEF:
                    getcomdef();
                    break;
                case CMD_COMDAT:
                case CMD_COMD32:
                    getcomdat();
                    break;
                case CMD_LNAMES:
                    getlname( false );
                    break;
                case CMD_LLNAMES:
                    getlname( true );
                    break;
                case CMD_ALIAS:
                    getalias();
                    break;
                }
            }
            switch( omfRec->basic.type ) {
            case CMD_THEADR:
                if( Options.strip_line
                  && !first ) {
                    continue;
                }
                if( Options.trim_path )
                    trimOmfHeader();
                first = false;
                break;
            case CMD_LINSYM:
            case CMD_LINS32:
            case CMD_LINNUM:
            case CMD_LINN32:
                if( Options.strip_line ) {
                    continue;
                }
                break;
            case CMD_COMENT:
                switch( omfRec->basic.contents[1] ) {
                case CMT_LINKER_DIRECTIVE:
                    if( omfRec->time.subclass == LDIR_OBJ_TIMESTAMP ) {
                        time_stamp = true;
                        if( oper == OMF_SYMS ) {
                            sfile->arch.date = GET_LE_32( omfRec->time.stamp );
                        }
                    }
                    break;
                case CMT_DLL_ENTRY:
                    if( omfRec->basic.contents[2] == DLL_EXPDEF ) {
                        if( oper == OMF_COPY ) {
                            if( ExportListFile != NULL ) {
                                LibWrite( ExportListFile, omfRec->basic.contents + 5, omfRec->basic.contents[4] );
                                LibWrite( ExportListFile, new_line, sizeof( new_line ) );
                            }
                        }
                        if( Options.strip_expdef ) {
                            continue;
                        }
                    } else if( omfRec->basic.contents[2] == DLL_IMPDEF ) {
                        if( oper == OMF_SYMS ) {
                            omfRec->basic.contents[5 + omfRec->basic.contents[4]] = '\0';
                            AddSym( (char *)( omfRec->basic.contents + 5 ), SYM_STRONG, 0 );
                        }
                    }
                    break;
                case CMT_DEPENDENCY:
                    if( Options.strip_dependency )
                        continue;
                    break;
                case CMT_DEFAULT_LIBRARY:
                    if( Options.strip_library )
                        continue;
                    break;
                }
                break;
            case CMD_MODEND:
            case CMD_MODE32:
                first = true;
                if( !time_stamp ) {
                    size += sizeof( OmfTimeStamp );
                    if( oper == OMF_COPY ) {
                        WriteTimeStamp( dst, sfile );
                    }
                }
                break;
            }
            size += omfRec->basic.len + 3;
            if( oper == OMF_COPY ) {
                WriteOmfRecord( dst, omfRec );
            }
        }
    } while( omfRec->basic.type != CMD_MODEND && omfRec->basic.type != CMD_MODE32 );
    return( size );
}

file_offset OmfCopy( libfile src, libfile dst, sym_file *sfile )
/**************************************************************/
{
    return( OmfProc( src, dst, sfile, OMF_COPY ) );
}

file_offset OmfSkipObject( libfile io )
/*************************************/
{
    return( OmfProc( io, NULL, NULL, OMF_ISIZE ) );
}

void OmfExtract( libfile src, libfile dst )
/*****************************************/
{
    do {
        ReadOmfRecord( src );
        WriteOmfRecord( dst, omfRec );
    } while( omfRec->basic.type != CMD_MODEND && omfRec->basic.type != CMD_MODE32 );
}

size_t OmfImportSize( import_sym *impsym )
/****************************************/
{
    size_t      len;
    size_t      dll_len;
    size_t      sym_len;

    dll_len = impsym->dllName.len + 1;
    sym_len = strlen( impsym->u.omf_coff.symName ) + 1;
    /*
     * THEADR OMF record
     */
#ifdef IMP_MODULENAME_DLL
    len = 3 + dll_len + 1;
#else
    len = 3 + sym_len + 1;
#endif
    /*
     * Comment DLL Entry OMF record
     */
    len += 3 + 2 + 2 + dll_len + sym_len + 1;
    if( impsym->type == ORDINAL ) {
        len += 2;
    } else if( impsym->u.omf_coff.exportedName == NULL ) {
        len += 1;
    } else {
        len += strlen( impsym->u.omf_coff.exportedName ) + 1;
    }
    /*
     * Comment timestamp OMF record
     */
    len += 3 + 8;
    /*
     * MODEND OMF record
     */
    len += 3 + 2;
    return( len );
}

void OmfWriteImport( libfile io, sym_file *sfile )
/************************************************/
{
    unsigned_8  sym_len;
    unsigned_8  file_len;
    unsigned_8  exp_len;
    unsigned_16 len;
    unsigned_8  *contents;

    file_len = (unsigned_8)sfile->impsym->dllName.len;
    sym_len = (unsigned_8)strlen( sfile->impsym->u.omf_coff.symName );
#ifdef IMP_MODULENAME_DLL
    contents = SetOmfRecBuffer( CMD_THEADR, 1 + file_len + 1 );
    *contents++ = file_len;
    memcpy( contents, sfile->impsym->dllName.name, file_len );
#else
    contents = SetOmfRecBuffer( CMD_THEADR, 1 + sym_len + 1 );
    *contents++ = sym_len;
    memcpy( contents, sfile->impsym->u.omf_coff.symName, sym_len );
#endif
    WriteOmfRecord( io, omfRec );
    exp_len = 0;
    len = 4 + 1 + sym_len + 1 + file_len;
    if( sfile->impsym->type == ORDINAL ) {
        len += 2 + 1;
    } else if( sfile->impsym->u.omf_coff.exportedName == NULL ) {
        len += 1 + 1;
    } else {
        exp_len = (unsigned_8)strlen( sfile->impsym->u.omf_coff.exportedName );
        len += 1 + exp_len + 1;
    }
    contents = SetOmfRecBuffer( CMD_COMENT, len );
    *contents++ = CMT_TNP;
    *contents++ = CMT_DLL_ENTRY;
    *contents++ = MOMF_IMPDEF;
    *contents++ = ( sfile->impsym->type == ORDINAL );
    *contents++ = sym_len;
    memcpy( contents, sfile->impsym->u.omf_coff.symName, sym_len );
    contents += sym_len;
    *contents++ = file_len;
    memcpy( contents, sfile->impsym->dllName.name, file_len );
    contents += file_len;
    if( sfile->impsym->type == ORDINAL ) {
        MPUT_LE_16( contents, sfile->impsym->u.omf_coff.ordinal );
    } else if( sfile->impsym->u.omf_coff.exportedName == NULL ) {
        *contents = 0;
    } else {
        *contents++ = exp_len;
        memcpy( contents, sfile->impsym->u.omf_coff.exportedName, exp_len );
    }
    WriteOmfRecord( io, omfRec );
    WriteTimeStamp( io, sfile );
    contents = SetOmfRecBuffer( CMD_MODEND, 1 + 1 );
    *contents = 0;
    WriteOmfRecord( io, omfRec );
}

void OMFWalkSymList( obj_file *ofile, sym_file *sfile )
/*****************************************************/
{
    /*
     * just for FORTRAN 77 common block
     */
    SegDefCount = 0;

    CurrSegRef = 0;
    LName_Head = NULL;
    LName_Owner = &LName_Head;

    sfile->arch.size = OmfProc( ofile->io, NULL, sfile, OMF_SYMS );

    FreeCommonBlk();
    FreeLNames();
}
