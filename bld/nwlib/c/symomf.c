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


#include <wlib.h>

static unsigned short CurrSegRef = 0;
static char                     Rec32Bit;

struct lname {
    struct lname        *next;
    char                local;
    char                len;
    char                name[1];
};

typedef struct COMMON_BLK {
    struct COMMON_BLK       *next;
    unsigned short          segindex;
}   common_blk;


typedef enum {
    S_COMDEF,
    S_COMDAT,
    S_PUBDEF
} sym_type;

static struct lname     *LName_Head;
static struct lname     **LName_Owner;
static char             Typ;
static unsigned short   Len, MaxLen;
static char *           CurrRec;
static char *           RecPtr;
static common_blk *     CurrCommonBlk;
static unsigned short   SegDefCount;
static bool             EasyOMF;
static char             NameBuff[257];

static bool GetLen( libfile io )
{
    char        ch1, ch2;

    if( LibRead( io, &ch1, sizeof( ch1 ) ) != sizeof( ch1 ) ) return( FALSE );
    if( LibRead( io, &ch2, sizeof( ch2 ) ) != sizeof( ch1 ) ) return( FALSE );
    Len = ( ( unsigned ) ch2 << 8 ) + ch1;
    return( TRUE );
}

bool GetRec( libfile io )
/***********************/
{
    if( LibRead( io, &Typ, sizeof( Typ ) ) != sizeof( Typ ) ) return( FALSE );
    if( !GetLen( io ) ) return( FALSE );
    if( Len > MaxLen ) {
        MemFree( CurrRec );
        CurrRec = MemAlloc( Len );
        MaxLen = Len;
    }
    if( LibRead( io, CurrRec, Len ) != Len ) return( FALSE );
    RecPtr = CurrRec;
    /* check to see if this is an Easy OMF-386 object file */
    /* Only need to check the first comment record */
    if( ( Typ & ~1 ) == CMD_COMENT  &&  Len == 8  &&
        memcmp( CurrRec, "\x80\xAA" "80386", 7 ) == 0 ) {
        EasyOMF = TRUE;
    }
    return( TRUE );
}


void OMFWalkSymList( obj_file *ofile, sym_file *sfile, void (*rtn)(char*name,symbol_strength,unsigned char) )
{
    EasyOMF = FALSE;
    SegDefCount = 0;    // just for FORTRAN 77 common block
    CurrSegRef = 0;
    LName_Head = NULL;
    LName_Owner = &LName_Head;

    do {
        if( !GetRec( ofile->hdl ) ) {
            FatalError( ERR_BAD_OBJECT, ofile->hdl->name );
        }

        if( Typ & 1 ) {
            Rec32Bit = TRUE;
        } else {
            Rec32Bit = EasyOMF;
        }

        if (sfile)
        {
            switch (Typ)
            {
                case CMD_SEGDEF:
                case CMD_SEGD32:
                    procsegdef();
                    break;
                case CMD_PUBDEF:
                case CMD_PUBD32:
                    getpubdef();
                    break;
                case CMD_COMENT:
                    GetComent( sfile );
                    break;
                case CMD_COMDEF:
                    getcomdef();
                    break;
                case CMD_COMDAT:
                case CMD_COMD32:
                    getcomdat();
                    break;
                case CMD_LNAMES:
                    getlname( FALSE );
                    break;
                case CMD_LLNAME:
                    getlname( TRUE );
                    break;
            } /* switch */
        } /* if */
    } while( Typ != CMD_MODEND && Typ != CMD_MODE32 );

    if (sfile)
    {
        FreeCommonBlk();
        FreeLNames();
        MemFree( CurrRec );
        CurrRec = NULL;
        Len = MaxLen = 0;
        sfile->arch.size = LibTell( ofile->hdl ) - sfile->inlib_offset;
    }
}


void OMFLibWalk( libfile io, char *name, void *(rtn)( arch_header *arch, libfile io ) )
{
    long                pagelen, offset, end_offset;
    arch_header         arch;
    char                buff[MAX_IMPORT_STRING];
    int                 len;

    if( LibRead( io, &Typ, sizeof( Typ ) ) != sizeof( Typ ) ) return; // nyi - FALSE?
    if( !GetLen( io ) ) return;
    pagelen = Len + sizeof( Typ ) + sizeof( Len );
    if( Options.page_size == 0 ) {
        Options.page_size = pagelen;
    }
    LibSeek( io, Len, SEEK_CUR );
    NewArchHeader( &arch, name );
    CurrRec = NULL;
    for( ;; ) {
        offset = LibTell( io );
        if( !GetRec( io ) ) break;
        if( Typ != CMD_THEADR ) break;
        len = *RecPtr++;
        memcpy( buff, RecPtr, len );
        buff[len] = '\0';
        arch.name = buff;
        LibSeek( io, offset, SEEK_SET );
        rtn( &arch, io );
        end_offset = LibTell( io );
        if( end_offset == offset ) {
            do {
                if( LibRead( io, &Typ, sizeof( Typ ) ) != sizeof( Typ ) ) {
                    FatalError( ERR_BAD_OBJECT, io->name );
                }
                if( LibRead( io, &Len, sizeof( Len ) ) != sizeof( Len ) ) {
                    FatalError( ERR_BAD_OBJECT, io->name );
                }
                LibSeek( io, Len, SEEK_CUR );
            } while( Typ != CMD_MODEND && Typ != CMD_MODE32 );
        }
        offset = pagelen - end_offset % pagelen;
        if( offset == pagelen ) offset = 0;
        LibSeek( io, offset, SEEK_CUR );
        if( LibRead( io, &Typ, sizeof( Typ ) ) != sizeof( Typ ) ) break;
        if( Typ == LIB_TRAILER_REC ) break;
        LibSeek( io, -1, SEEK_CUR );
    }
    MemFree( CurrRec );
    Len = MaxLen = 0;
    CurrRec = NULL;
}



static void AddOMFSymbol( sym_type type )
{
    if( type == S_COMDEF || type == S_COMDAT || IsCommonRef() ) {
        AddSym( NameBuff, SYM_WEAK, 0 );
    } else {
        AddSym( NameBuff, SYM_STRONG, 0 );
    }
}

static unsigned short GetIndex()
/******************************/
{
    unsigned short index;

    index = *RecPtr++;
    if( index > 0x7F ) {
        index &= 0x7f;
        index = (index << 8) | *RecPtr++;
    }
    return( index );
}


static void GetOffset()
/*********************/
{
    RecPtr += 2;
    if( Rec32Bit ) { /* if 386, must skip over 4 bytes */
        RecPtr += 2;
    }
}

/*
 * throw away a group and segment number and possibly frame number
 */
static void GetIdx()
/******************/
{
    register unsigned short         grp;

    grp = GetIndex();
    CurrSegRef = GetIndex();
    if( ( grp == 0 ) && ( CurrSegRef == 0 ) ) {
        RecPtr += 2;                    /* skip over frame number */
    }
}

int IsCommonRef()       /* dedicated routine for FORTRAN 77 common block */
/***************/
{
    common_blk * tmpblk = CurrCommonBlk;

    while( tmpblk != NULL  &&  tmpblk->segindex <= CurrSegRef ) {
        if( tmpblk->segindex == CurrSegRef ) {
            return( TRUE );
        }
        tmpblk = tmpblk->next;
    }
    return( FALSE );
}


void FreeCommonBlk() /* dedicated routine for FORTRAN 77 common block */
/******************/
{
    common_blk *    tmpblk;

    while( CurrCommonBlk != NULL ) {
        tmpblk = CurrCommonBlk->next;
        MemFree( CurrCommonBlk );
        CurrCommonBlk = tmpblk;
    }
}

static void procsegdef()  /* dedicated routine for FORTRAN 77 common block */
/***********************/
{
    common_blk   *      cmn;
    common_blk  **      owner;

    uint_8  tmpbyte = ( *RecPtr & 0x1f ) >> 2; /* get "COMBINE" bits */

    SegDefCount++;
    if( tmpbyte == 6 ) {    /* COMBINE == COMMON, record it */

        owner = &CurrCommonBlk;

        for( ;; ) {
            cmn = *owner;
            if( cmn == NULL ) break;
            owner = &cmn->next;
        }
        cmn = MemAlloc( sizeof( common_blk ) );
        cmn->segindex = SegDefCount;
        cmn->next = NULL;
        *owner = cmn;
    }
}


/*
 * from infl, get a intel name: length and name
 */
void GetName()
{
    char        num_char;

    num_char = *RecPtr++;
    memcpy( NameBuff, RecPtr, num_char );
    RecPtr += num_char;
    NameBuff[ num_char ] = '\0';
}
/*
 * loop over the publics in the record
 */
static void getpubdef()
{
    GetIdx();
    while( (int)( RecPtr - CurrRec ) < ( Len - 1 ) ) {
        GetName();
        GetOffset();
        GetIndex();
        AddOMFSymbol( S_PUBDEF );
    }
}

/*
 * get the public definition out of the coment record
 */
static void GetComent( sym_file *sfile )
{
    RecPtr++;   // skip attribute byte of comment rec
    switch( *RecPtr++ ){
    case CMT_DLL_ENTRY:
        if( *RecPtr++ == DLL_IMPDEF ){
            RecPtr++;       // skip the ordinal flag
            GetName();
            AddOMFSymbol( S_PUBDEF );
        }
        break;
    case CMT_LINKER_DIRECTIVE:
        if( *RecPtr++ == LDIR_OBJ_TIMESTAMP ) {
            sfile->arch.date = *(unsigned_32 *)RecPtr;
        }
        break;
    }
}

static void GetComLen()
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
static void getcomdef()
{
    while( (int)( RecPtr - CurrRec ) < ( Len - 1 ) ) {
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
static void getcomdat()
{
    unsigned            alloc;
    unsigned            idx;
    struct lname                *ln;

    if( *RecPtr & (COMDAT_CONTINUE|COMDAT_LOCAL) ) return;
    RecPtr++;
    alloc = *RecPtr++ & COMDAT_ALLOC_MASK;
    RecPtr++;
    GetOffset();
    GetIndex();
    if( alloc == COMDAT_EXPLICIT ) {
        GetIdx();
    }
    idx = GetIndex() - 1;
    for( ln = LName_Head; idx != 0; --idx, ln = ln->next ) {
        /* nothing to do */
    }
    if( ln->local ) return;
    memcpy( NameBuff, ln->name, ln->len );
    NameBuff[ ln->len ] = '\0';
    AddOMFSymbol( S_COMDAT );
}

static void     getlname( int local )
{
    unsigned    len;
    struct lname        *ln;

    while( (int)( RecPtr - CurrRec ) < ( Len - 1 ) ) {
        len = *RecPtr++;
        ln = MemAlloc( (sizeof( struct lname ) - 1) + len );
        ln->local = local;
        ln->len = len;
        memcpy( ln->name, RecPtr, len );
        ln->next = NULL;
        *LName_Owner = ln;
        LName_Owner = &ln->next;
        RecPtr += len;
    }
}


static void FreeLNames()
{
    struct lname        *next;

    while( LName_Head != NULL ) {
        next = LName_Head->next;
        MemFree( LName_Head );
        LName_Head = next;
    }
}


/*
 * Skip to the end of the current object -- used in the code to delete
 * omf objects.
 */
void OMFSkipThisObject(arch_header *arch, libfile io)
{
    obj_file *file;

    file = OpenLibFile(arch->name, io);
    OMFWalkSymList(file, NULL, NULL);
    CloseLibFile(file);
} /* OMFSkipThisObject() */
