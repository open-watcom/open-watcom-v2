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
* Description:  Librarian OMF symbol processing.
*
****************************************************************************/


#include "wlib.h"

struct lname {
    struct lname        *next;
    bool                local;
    unsigned char       len;
    char                name[1];
};

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
static struct lname     *LName_Head;
static struct lname     **LName_Owner;
static unsigned_8       *RecPtr;
static unsigned_8       *RecEnd;
static common_blk       *CurrCommonBlk;
static unsigned short   SegDefCount;
static char             NameBuff[257];

static OmfRecord        *omfRec;
static size_t           omfRecLen;

void InitOmfRec( void )
/*********************/
{
    omfRec = MemAlloc( INIT_OMF_REC_SIZE );
    omfRecLen = INIT_OMF_REC_SIZE;
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
        index = (index << 8) | *RecPtr++;
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

/*
 * throw away a group and segment number and possibly frame number
 */
static void GetIdx( void )
/************************/
{
    unsigned short      grp;

    grp = GetIndex();
    CurrSegRef = GetIndex();
    if( ( grp == 0 ) && ( CurrSegRef == 0 ) ) {
        RecPtr += 2;                    /* skip over frame number */
    }
}

static bool IsCommonRef( void )      /* dedicated routine for FORTRAN 77 common block */
/*****************************/
{
    common_blk          *tmpblk = CurrCommonBlk;

    while( tmpblk != NULL  &&  tmpblk->segindex <= CurrSegRef ) {
        if( tmpblk->segindex == CurrSegRef ) {
            return( true );
        }
        tmpblk = tmpblk->next;
    }
    return( false );
}

static void FreeCommonBlk( void )   /* dedicated routine for FORTRAN 77 common block */
/*******************************/
{
    common_blk          *tmpblk;

    while( CurrCommonBlk != NULL ) {
        tmpblk = CurrCommonBlk->next;
        MemFree( CurrCommonBlk );
        CurrCommonBlk = tmpblk;
    }
}

static void procsegdef( void )      /* dedicated routine for FORTRAN 77 common block */
/****************************/
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

static void AddOMFSymbol( sym_type type )
/***************************************/
{
    if( type == S_COMDEF || type == S_COMDAT || type == S_ALIAS || IsCommonRef() ) {
        AddSym( NameBuff, SYM_WEAK, 0 );
    } else {
        AddSym( NameBuff, SYM_STRONG, 0 );
    }
}

/*
 * from infl, get a intel name: length and name
 */
static void GetName( void )
/*************************/
{
    unsigned_8  len;

    len = *RecPtr++;
    memcpy( NameBuff, RecPtr, len );
    RecPtr += len;
    NameBuff[len] = '\0';
}

/*
 * loop over the publics in the record
 */
static void getpubdef( void )
/***************************/
{
    GetIdx();
    while( RecPtr < RecEnd ) {
        GetName();
        GetOffset();
        GetIndex();
        AddOMFSymbol( S_PUBDEF );
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

/*
 * process a COMDEF record
 */
static void getcomdef( void )
/***************************/
{
    while( RecPtr < RecEnd ) {
        GetName();
        AddOMFSymbol( S_COMDEF );
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

/*
 * process a COMDAT record
 */
static void getcomdat( void )
/***************************/
{
    unsigned            alloc;
    unsigned            idx;
    struct lname        *ln;

    if( *RecPtr & (COMDAT_CONTINUE|COMDAT_LOCAL) )
        return;
    RecPtr++;
    alloc = *RecPtr++ & COMDAT_ALLOC_MASK;
    RecPtr++;
    GetOffset();
    GetIndex();
    if( alloc == COMDAT_EXPLICIT ) {
        GetIdx();
    }
    ln = LName_Head;
    for( idx = GetIndex() - 1; idx > 0; --idx ) {
        ln = ln->next;
    }
    if( ln->local )
        return;
    memcpy( NameBuff, ln->name, ln->len );
    NameBuff[ln->len] = '\0';
    AddOMFSymbol( S_COMDAT );
}

/*
 * process a LNAMES record
 */
static void getlname( bool local )
/********************************/
{
    unsigned_8          len;
    struct lname        *ln;

    while( RecPtr < RecEnd ) {
        len = *RecPtr++;
        ln = MemAlloc( sizeof( struct lname ) - 1 + len );
        ln->local = local;
        ln->len = len;
        memcpy( ln->name, RecPtr, len );
        ln->next = NULL;
        *LName_Owner = ln;
        LName_Owner = &ln->next;
        RecPtr += len;
    }
}

/*
 * process an ALIAS record
 */
static void getalias( void )
/**************************/
{
    while( RecPtr < RecEnd ) {
        GetName();  // alias symbol
        AddOMFSymbol( S_ALIAS );
        GetName();  // substitute symbol
    }
}

static void FreeLNames( void )
/****************************/
{
    struct lname        *next;

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
    for( i = 0; i < GET_LE_16( rec->basic.len ) + 2; ++i ) {
        sum += rec->chkcalc[i];
    }
    rec->chkcalc[i] = -sum;
}

static void SetOmfBuffer( unsigned_16 len )
/*****************************************/
{
    if( len + 3 > omfRecLen ) {
        OmfRecord *new;
        omfRecLen = len + 3;
        new = MemAlloc( omfRecLen );
        new->basic.len = omfRec->basic.len;
        new->basic.type = omfRec->basic.type;
        MemFree( omfRec );
        omfRec = new;
    }
}

static unsigned_8 *SetOmfRecBuffer( unsigned_8 type, unsigned_16 len )
/********************************************************************/
{
    omfRec->basic.type = type;
    omfRec->basic.len = GET_LE_16( len );
    SetOmfBuffer( len );
    return( omfRec->basic.contents );
}

static bool ReadOmfRecord( libfile io )
/*************************************/
{
    unsigned_16 len;

    if( LibRead( io, omfRec, 3 ) != 3 )
        return( false );
    len = GET_LE_16( omfRec->basic.len );
    SetOmfBuffer( len );
    if( LibRead( io, omfRec->basic.contents, len ) != len ) {
        return( false );
    }
    RecPtr = omfRec->basic.contents;
    RecEnd = RecPtr + len - 1;
    return( true );
}

static void trimOmfHeader( void )
/*******************************/
{
    unsigned_16 len;

    len = GET_LE_16( omfRec->basic.len );
    omfRec->basic.contents[len - 1] = '\0';
    len = (unsigned_16)strlen( TrimPath( (char *)omfRec->basic.contents + 1 ) );
    omfRec->basic.contents[0] = len;
    omfRec->basic.len = GET_LE_16( len + 2 );
    CalcOmfRecordCheckSum( omfRec );
}

static void WriteOmfRecord( OmfRecord *rec )
/******************************************/
{
    CalcOmfRecordCheckSum( rec );
    WriteNew( rec, GET_LE_16( rec->basic.len ) + 3 );
}

static void WriteTimeStamp( sym_file *sfile )
/*******************************************/
{
    OmfRecord   rec;

    rec.time.type = CMD_COMENT;
    rec.time.len = GET_LE_16( sizeof( rec.time ) - 3 );
    rec.time.attribute = CMT_TNP | CMT_TNL;
    rec.time.class = CMT_LINKER_DIRECTIVE;
    rec.time.subclass = LDIR_OBJ_TIMESTAMP;
    rec.time.stamp = GET_LE_32( sfile->arch.date );
    WriteOmfRecord( &rec );
}

static file_offset OmfProc( libfile io, sym_file *sfile, omf_oper oper )
/**********************************************************************/
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
        if( !ReadOmfRecord( io ) ) {
            FatalError( ERR_BAD_OBJECT, io->name );
        }
        if( oper == OMF_ISIZE ) {
            size += GET_LE_16( omfRec->basic.len ) + 3;
        } else {
            if( oper == OMF_SYMS ) {
                /* check to see if this is an Easy OMF-386 object file */
                /* Only need to check the first comment record */
                if( ( omfRec->basic.type & ~1 ) == CMD_COMENT ) {
                    if( GET_LE_16( omfRec->basic.len ) == 8 && memcmp( omfRec->basic.contents, "\x80\xAA" "80386", 7 ) == 0 ) {
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
                case CMD_LLNAME:
                    getlname( true );
                    break;
                case CMD_ALIAS:
                    getalias();
                    break;
                }
            }
            switch( omfRec->basic.type ) {
            case CMD_THEADR:
                if( Options.strip_line && !first )
                    continue;
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
                        WriteTimeStamp( sfile );
                    }
                }
                break;
            }
            size += GET_LE_16( omfRec->basic.len ) + 3;
            if( oper == OMF_COPY ) {
                WriteNew( omfRec, GET_LE_16( omfRec->basic.len ) + 3 );
            }
        }
    } while( omfRec->basic.type != CMD_MODEND && omfRec->basic.type != CMD_MODE32 );
    return( size );
}

file_offset OmfCopy( libfile io, sym_file *sfile )
/************************************************/
{
    return( OmfProc( io, sfile, OMF_COPY ) );
}

file_offset OmfSkipObject( libfile io )
/*************************************/
{
    return( OmfProc( io, NULL, OMF_ISIZE ) );
}

void OmfExtract( libfile io, libfile out )
/****************************************/
{
    do {
        ReadOmfRecord( io );
        LibWrite( out, omfRec, GET_LE_16( omfRec->basic.len ) + 3 );
    } while( omfRec->basic.type != CMD_MODEND && omfRec->basic.type != CMD_MODE32 );
}

size_t OmfImportSize( import_sym *import )
/****************************************/
{
    size_t      len;
    size_t      dll_len;
    size_t      sym_len;

    dll_len = strlen( import->DLLName ) + 1;
    sym_len = strlen( import->u.sym.symName ) + 1;
    // THEADR
#ifdef IMP_MODULENAME_DLL
    len = 3 + dll_len + 1;
#else
    len = 3 + sym_len + 1;
#endif
    // Comment DLL Entry
    len += 3 + 2 + 2 + dll_len + sym_len + 1;
    if( import->type == ORDINAL ) {
        len += 2;
    } else if( import->u.sym.exportedName == NULL ) {
        len += 1;
    } else {
        len += strlen( import->u.sym.exportedName ) + 1;
    }
    // Comment timestamp
    len += 3 + 8;
    // MODEND
    len += 3 + 2;
    return( len );
}

void OmfWriteImport( sym_file *sfile )
/************************************/
{
    unsigned_8  sym_len;
    unsigned_8  file_len;
    unsigned_8  exp_len;
    unsigned_16 len;
    unsigned_8  *contents;

    file_len = (unsigned_8)strlen( sfile->import->DLLName );
    sym_len = (unsigned_8)strlen( sfile->import->u.sym.symName );
#ifdef IMP_MODULENAME_DLL
    contents = SetOmfRecBuffer( CMD_THEADR, 1 + file_len + 1 );
    *contents++ = file_len;
    memcpy( contents, sfile->import->DLLName, file_len );
#else
    contents = SetOmfRecBuffer( CMD_THEADR, 1 + sym_len + 1 );
    *contents++ = sym_len;
    memcpy( contents, sfile->import->u.sym.symName, sym_len );
#endif
    WriteOmfRecord( omfRec );
    exp_len = 0;
    len = 4 + 1 + sym_len + 1 + file_len;
    if( sfile->import->type == ORDINAL ) {
        len += 2 + 1;
    } else if( sfile->import->u.sym.exportedName == NULL ) {
        len += 1 + 1;
    } else {
        exp_len = (unsigned_8)strlen( sfile->import->u.sym.exportedName );
        len += 1 + exp_len + 1;
    }
    contents = SetOmfRecBuffer( CMD_COMENT, len );
    *contents++ = CMT_TNP;
    *contents++ = CMT_DLL_ENTRY;
    *contents++ = MOMF_IMPDEF;
    *contents++ = ( sfile->import->type == ORDINAL );
    *contents++ = sym_len;
    memcpy( contents, sfile->import->u.sym.symName, sym_len );
    contents += sym_len;
    *contents++ = file_len;
    memcpy( contents, sfile->import->DLLName, file_len );
    contents += file_len;
    if( sfile->import->type == ORDINAL ) {
        *(unsigned_16 *)contents = (unsigned_16)sfile->import->u.sym.ordinal;
    } else if( sfile->import->u.sym.exportedName == NULL ) {
        *contents = 0;
    } else {
        *contents++ = exp_len;
        memcpy( contents, sfile->import->u.sym.exportedName, exp_len );
    }
    WriteOmfRecord( omfRec );
    WriteTimeStamp( sfile );
    contents = SetOmfRecBuffer( CMD_MODEND, 1 + 1 );
    *contents = 0;
    WriteOmfRecord( omfRec );
}

void OMFWalkSymList( obj_file *ofile, sym_file *sfile )
/*****************************************************/
{
    SegDefCount = 0;    // just for FORTRAN 77 common block
    CurrSegRef = 0;
    LName_Head = NULL;
    LName_Owner = &LName_Head;

    sfile->arch.size = OmfProc( ofile->hdl, sfile, OMF_SYMS );

    FreeCommonBlk();
    FreeLNames();
}
