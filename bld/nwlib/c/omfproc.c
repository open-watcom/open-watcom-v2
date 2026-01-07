/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024-2025 The Open Watcom Contributors. All Rights Reserved.
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
static lname            *LNames;
static lname            **LNamesEnd;
static unsigned_8       *RecPtr;
static unsigned_8       *RecEnd;
static common_blk       *CurrCommonBlk;
static unsigned short   SegDefCount;

static OmfRecord        *omfRec;
static size_t           omfRecLen;


static void SetOmfBuffer( unsigned_16 len )
/*****************************************/
{
    if( len > omfRecLen ) {
        if( omfRec != NULL ) {
            MemFree( omfRec );
        }
        omfRec = MemAlloc( sizeof( OmfRecord ) + len );
        omfRecLen = len;
    }
}

static unsigned_8 *SetOmfRecBuffer( unsigned_8 type, unsigned_16 len )
/********************************************************************/
{
    SetOmfBuffer( len );
    omfRec->type = type;
    omfRec->len = len;
    return( omfRec->contents );
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

    while( (tmpblk = CurrCommonBlk) != NULL ) {
        CurrCommonBlk = tmpblk->next;
        MemFree( tmpblk );
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
    ln = LNames;
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
        *LNamesEnd = ln;
        LNamesEnd = &ln->next;
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
    lname           *tmp;

    while( (tmp = LNames) != NULL ) {
        LNames = tmp->next;
        MemFree( tmp );
    }
}

static void CalcOmfRecordCheckSum( OmfRecord *rec )
/*************************************************/
{
    unsigned_8      sum;
    unsigned_16     len;
    unsigned_16     i;

    len = rec->len;
    sum = rec->type + len + ( len >> 8 );
    for( i = 0; i < len - OMFSUMLEN; ++i ) {
        sum += rec->contents[i];
    }
    rec->contents[i] = -sum;
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
    CONV_LE_16( len );
    SetOmfRecBuffer( type, len );
    if( LibRead( io, omfRec->contents, len ) != len ) {
        return( false );
    }
    RecPtr = omfRec->contents;
    RecEnd = RecPtr + len - OMFSUMLEN;
    return( true );
}

static void trimOmfHeader( void )
/*******************************/
{
    unsigned_16 len;

    len = omfRec->len;
    omfRec->contents[len - OMFSUMLEN] = '\0';
    TrimPathInPlace( (char *)omfRec->contents + 1 );
    len = (unsigned_8)strlen( (char *)omfRec->contents + 1 );
    omfRec->contents[0] = (unsigned_8)len;
    omfRec->len = 1 + len + OMFSUMLEN;
}

static void WriteOmfRecord( libfile io, OmfRecord *rec )
/******************************************************/
{
    CalcOmfRecordCheckSum( rec );
    WriteOmfRecHeader( io, rec->type, rec->len );
    LibWrite( io, rec->contents, rec->len );
}

static void WriteOmfTimeStamp( libfile io, sym_file *sfile )
/***********************************************************
 * output OMF timestamp record
 *
 * struct {
 *     // OMF record header
 *     unsigned_8  type;
 *     unsigned_16 len;
 *     // OMF timestamp data
 *     unsigned_8  attribute;
 *     unsigned_8  class;
 *     unsigned_8  subclass;
 *     unsigned_32 stamp;
 *     // OMF record checksum
 *     unsigned_8  chksum;
 * }
 *
 * it does not use omfRec because it is processed in another OMF record
 * processing and omfRec keeps the data of this record
 */
{
    OMFRECORD( 7 + OMFSUMLEN )  rec;

    rec.type = CMD_COMENT;
    rec.len = 7 + OMFSUMLEN;
    rec.contents[0] = CMT_TNP | CMT_TNL;
    rec.contents[1] = CMT_LINKER_DIRECTIVE;
    rec.contents[2] = LDIR_OBJ_TIMESTAMP;
    mset_U32LE( rec.contents + 3, sfile->arch.date );
    WriteOmfRecord( io, (OmfRecord *)&rec );
}

static file_offset OmfProc( libfile src, libfile dst, sym_file *sfile, omf_oper oper )
/************************************************************************************/
{
    bool        time_stamp;
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
            size += OMFHDRLEN + omfRec->len;
        } else {
            if( oper == OMF_SYMS ) {
                /*
                 * check to see if this is an Easy OMF-386 object file
                 * Only need to check the first comment record
                 */
                if( ( omfRec->type & ~1 ) == CMD_COMENT ) {
                    if( omfRec->len == 7 + OMFSUMLEN
                      && omfRec->contents[0] == CMT_TNP
                      && omfRec->contents[1] == CMT_EASY_OMF
                      && memcmp( omfRec->contents + 2, "80386", 5 ) == 0 ) {
                        EasyOMF = true;
                    }
                }
                if( omfRec->type & 1 ) {
                    Rec32Bit = true;
                } else {
                    Rec32Bit = EasyOMF;
                }
                switch( omfRec->type ) {
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
            switch( omfRec->type ) {
            case CMD_THEADR:
                /*
                 * TODO!
                 *   Open Watcom tools now generate first THEADR record with "true"
                 *   module name and second THEADR record with source file name
                 *   this second record can ommit (debug info)
                 *   other THEADR records can be generated in case of source file change
                 */
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
                switch( omfRec->contents[1] ) {
                case CMT_LINKER_DIRECTIVE:
                    if( omfRec->contents[2] == LDIR_OBJ_TIMESTAMP ) {
                        time_stamp = true;
                        if( oper == OMF_SYMS ) {
                            sfile->arch.date = mget_U32LE( omfRec->contents + 3 );
                        }
                    }
                    break;
                case CMT_DLL_ENTRY:
                    if( omfRec->contents[2] == DLL_EXPDEF ) {
                        if( oper == OMF_COPY ) {
                            if( ExportListFile != NULL ) {
                                LibWrite( ExportListFile, omfRec->contents + 5, omfRec->contents[4] );
                                LibWriteU8( ExportListFile, '\n' );
                            }
                        }
                        if( Options.strip_expdef ) {
                            continue;
                        }
                    } else if( omfRec->contents[2] == DLL_IMPDEF ) {
                        if( oper == OMF_SYMS ) {
                            /*
                             * the content of OMF record may be destroyed because
                             * it is no longer in use (only for symbols list)
                             */
                            omfRec->contents[5 + omfRec->contents[4]] = '\0';
                            AddSym( (char *)( omfRec->contents + 5 ), SYM_STRONG, 0 );
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
                    size += OMFHDRLEN + 7 + OMFSUMLEN;
                    if( oper == OMF_COPY ) {
                        WriteOmfTimeStamp( dst, sfile );
                    }
                }
                break;
            }
            size += OMFHDRLEN + omfRec->len;
            if( oper == OMF_COPY ) {
                WriteOmfRecord( dst, omfRec );
            }
        }
    } while( omfRec->type != CMD_MODEND && omfRec->type != CMD_MODE32 );
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
    } while( omfRec->type != CMD_MODEND && omfRec->type != CMD_MODE32 );
}

size_t OmfImportSize( import_sym *impsym )
/****************************************/
{
    size_t      size;
    size_t      dll_len;
    size_t      sym_len;

    dll_len = 1 + impsym->dllName.len;
    sym_len = 1 + strlen( impsym->u.omf_coff.symName );
    /*
     * THEADR OMF record
     */
#ifdef IMP_MODULENAME_DLL
    size = OMFHDRLEN + dll_len + OMFSUMLEN;
#else
    size = OMFHDRLEN + sym_len + OMFSUMLEN;
#endif
    /*
     * Comment DLL Entry OMF record
     */
    size += OMFHDRLEN + 2 + 2 + dll_len + sym_len;
    if( impsym->type == ORDINAL ) {
        size += 2;
    } else if( impsym->u.omf_coff.exportedName == NULL ) {
        size += 1;
    } else {
        size += 1 + strlen( impsym->u.omf_coff.exportedName );
    }
    size += OMFSUMLEN;
    /*
     * Comment timestamp OMF record
     */
    size += OMFHDRLEN + 7 + OMFSUMLEN;
    /*
     * MODEND OMF record
     */
    size += OMFHDRLEN + 1 + OMFSUMLEN;
    return( size );
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
    contents = SetOmfRecBuffer( CMD_THEADR, 1 + file_len + OMFSUMLEN );
    *contents++ = file_len;
    memcpy( contents, sfile->impsym->dllName.name, file_len );
#else
    contents = SetOmfRecBuffer( CMD_THEADR, 1 + sym_len + OMFSUMLEN );
    *contents++ = sym_len;
    memcpy( contents, sfile->impsym->u.omf_coff.symName, sym_len );
#endif
    WriteOmfRecord( io, omfRec );
    exp_len = 0;
    len = 2 + 2 + 1 + sym_len + 1 + file_len;
    if( sfile->impsym->type == ORDINAL ) {
        len += 2;
    } else if( sfile->impsym->u.omf_coff.exportedName == NULL ) {
        len += 1;
    } else {
        exp_len = (unsigned_8)strlen( sfile->impsym->u.omf_coff.exportedName );
        len += 1 + exp_len;
    }
    contents = SetOmfRecBuffer( CMD_COMENT, len + OMFSUMLEN );
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
        mset_U16LE( contents, sfile->impsym->u.omf_coff.ordinal );
    } else if( sfile->impsym->u.omf_coff.exportedName == NULL ) {
        *contents = 0;
    } else {
        *contents++ = exp_len;
        memcpy( contents, sfile->impsym->u.omf_coff.exportedName, exp_len );
    }
    WriteOmfRecord( io, omfRec );
    WriteOmfTimeStamp( io, sfile );
    contents = SetOmfRecBuffer( CMD_MODEND, 1 + OMFSUMLEN );
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
    LNames = NULL;
    LNamesEnd = &LNames;

    sfile->arch.size = OmfProc( ofile->io, NULL, sfile, OMF_SYMS );

    FreeCommonBlk();
    FreeLNames();
}
