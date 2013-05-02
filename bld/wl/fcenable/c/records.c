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
* Description:  Object file record management routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wio.h"
#include "fcenable.h"

extern void *   MemAlloc( unsigned );
extern void     MemFree( void * );
extern void     Warning( char * );
extern long     QSeek( int, long, int );
extern void     IOError( char * );
extern void     Error( char * );
extern void     LinkList( void **, void * );
extern int      QRead( int, void *, int );
extern int      QWrite( int, void *, int );
extern void     FreeList( void * );
extern void     BuildRecord( void *, unsigned );
extern int      GetIndex( char ** );
extern void     IndexRecord( unsigned );

#define VARIABLE_SIZE 1
#define MAJOR_OBJ_VERSION 1
#define MINOR_OBJ_VERSION 1

typedef struct {
        unsigned char class;
        unsigned short int  length;
} HEAD;

typedef struct {
        char    rest[ VARIABLE_SIZE ];
} ANYOBJ_REC;

typedef struct {
        BYTE    attrib;
        BYTE    class;
        char    coment[ VARIABLE_SIZE ];
} COMENT_REC;

#define MAX_COMENT_DATA (MAX_OBJECT_REC_SIZE - sizeof(COMENT_REC) - 1)

typedef struct {
        BYTE    name_len;
        char    name[ VARIABLE_SIZE ];
} THEADR_REC;

typedef struct {
        HEAD            head;
        union {
            COMENT_REC  coment;
            ANYOBJ_REC  anyobj;
            THEADR_REC  theadr;
        } u;
} OBJECT_REC;

#define         LIBRARY_HEADER  0xf0
#define         LIBRARY_TRAILER 0xf1

#define         RHEADR          0x6e
#define         REGINT          0x70
#define         REDATA          0x72
#define         RIDATA          0x74
#define         OVLDEF          0x76
#define         ENDREC          0x78
#define         BLKDEF          0x7a
#define         BLKEND          0x7c
#define         DEBSYM          0x7e
#define         THEADR          0x80
#define         LHEADR          0x82
#define         PEDATA          0x84
#define         PIDATA          0x86
#define         COMENT          0x88
#define         MODEND          0x8a
#define         MODE32          0x8b     // 32-bit end of module record
#define         EXTDEF          0x8c
#define         TYPDEF          0x8e
#define         PUBDEF          0x90
#define         PUBD32          0x91     // 32-bit export names record
#define         LOCSYM          0x92
#define         LINNUM          0x94
#define         LINN32          0x95     // 32-bit line number record.
#define         LNAMES          0x96
#define         SEGDEF          0x98
#define         SEGD32          0x99     // 32-bit segment definition
#define         GRPDEF          0x9a
#define         FIXUPP          0x9c
#define         FIXU32          0x9d     // 32-bit relocation record
#define         LEDATA          0xa0
#define         LEDA32          0xa1     // 32-bit object record
#define         LIDATA          0xa2
#define         LIDA32          0xa3     // 32-bit repeated data record
#define         LIBHED          0xa4
#define         LIBNAM          0xa6
#define         LIBLOC          0xa8
#define         LIBDIC          0xaa
#define         WEIRDEF         0xb0
#define         NEW_DBG_1       0xb4
#define         NEW_DBG_2       0xb6

extern int              InFile;
extern int              OutFile;
extern name_list *      ClassList;
extern name_list *      SegList;
extern exclude_list *   ExcludeList;

int                 PageLen;
OBJECT_REC *        Rec1;

static bool     Easy32;
static bool     MakeUnsafe;
static int      NameIndex;
static int      SegIndex;
static BYTE     SubTotal = 0;
static char *   OutputBuffer;
static int      InBuffer = 0;

// forward declarations
extern char *   SaveRecord( void * );


extern void InitRecStuff( void )
/******************************/
{
    Rec1 = MemAlloc( MAX_OBJECT_REC_SIZE );
    OutputBuffer = MemAlloc( MAX_OBJECT_REC_SIZE );
    PageLen = 0;
}

extern void FileCleanup( void )
/*****************************/
{
    PageLen = 0;
}

static void ZeroIndicies( name_list *list )
/*****************************************/
{
    while( list != NULL ) {
        list->lnameidx = 0;
        list = list->next;
    }
}

extern void CleanRecStuff( void )
/*******************************/
{
    ZeroIndicies( ClassList );
    ZeroIndicies( SegList );
    Easy32 = FALSE;
    NameIndex = 0;
    SegIndex = 0;
}

extern void FinalCleanup( void )
/******************************/
{
    MemFree( Rec1 );
    MemFree( OutputBuffer );
    FreeList( ClassList );
    FreeList( SegList );
    FreeList( ExcludeList );
}

static void WriteRecord( void )
/*****************************/
{
    BuildRecord( Rec1, Rec1->head.length + sizeof( HEAD ) );
    SubTotal = 0;       // reset checksum calculations.
}

static void proccoment( void )
/****************************/
// need to detect EASY_OMF 32-bit .obj files.
{
    BYTE class;

    class = Rec1->u.coment.class;
    if( class == 0xAA ) {
        if( memcmp( Rec1->u.coment.coment, "80386", 5 ) == 0 ) {
            Easy32 = TRUE;
        }
    }
    WriteRecord();
}

static bool FindName( name_list *list, char *name, int name_len )
/***************************************************************/
{
    while( list != NULL ) {
        if( memicmp( list->name, name, name_len ) == 0 ) {
            list->lnameidx = NameIndex;
            return( TRUE );
        }
        list = list->next;
    }
    return( FALSE );
}

static void SetExLnames( char *name, int name_len )
/*************************************************/
{
    exclude_list *  list;

    list = ExcludeList;
    while( list != NULL ) {
        if( memicmp( list->name, name, name_len ) == 0 ) {
            list->lnameidx = NameIndex;
        }
        list = list->next;
    }
}

static void proclnames( void )
/****************************/
{
    unsigned        rec_len;
    BYTE            name_len;
    char            *name;

    rec_len = Rec1->head.length;
    name = Rec1->u.anyobj.rest;
    while( rec_len > 1 ) {
        ++NameIndex;
        name_len = *name;
        name++;
        if( !FindName( ClassList, name, name_len ) ) {
            FindName( SegList, name, name_len );
        }
        SetExLnames( name, name_len );
        name += name_len;
        rec_len -= name_len + 1;
    }
    WriteRecord();
}

static bool MatchIndex( name_list *list, int index )
/**************************************************/
{
    while( list != NULL ) {
        if( list->lnameidx == index ) {
            return( TRUE );
        }
        list = list->next;
    }
    return( FALSE );
}

static void CheckSum( void )
/**************************/
{
    BYTE    cksum;

    cksum = -SubTotal;
    BuildRecord( &cksum, 1 );
    SubTotal = 0;
}

static void procsegdef( bool is386 )
/**********************************/
{
    BYTE            acbp;
    int             segidx;
    int             classidx;
    char *          dataloc;
    BYTE            onebyte;
    unsigned long   fourbytes;
    exclude_list *  exclude;

    WriteRecord();
    ++SegIndex;
    dataloc = Rec1->u.anyobj.rest;
    acbp = *dataloc;
    dataloc++;
    if( ( acbp & 0xE0 ) == 0 ) {
        dataloc += 3;           /* adjust for frame_number and offset */
    }
    dataloc += 2;                       // skip the length;
    if( is386 || Easy32 ) {         // if a 386 .obj file.......
        dataloc += 2;               // got more offset to skip.
    }
    segidx = GetIndex( &dataloc );
    classidx = GetIndex( &dataloc );
    GetIndex( &dataloc );                   // skip the ovl_name
    if( MatchIndex( ClassList, classidx ) || MatchIndex( SegList, segidx ) ) {
        onebyte = COMENT;
        BuildRecord( &onebyte, 1 );
        if( SegIndex >= 128 ) {       // then indicies are 2 bytes.
            onebyte = 6;        // includes checksum
        } else {
            onebyte = 5;
        }
        BuildRecord( &onebyte, 1 );
        fourbytes = 0x4FFE8000;       // 00 80 FE 4F
        BuildRecord( &fourbytes, 4 ); //  |  |  |  ^-- 'O' optimize command
        IndexRecord( SegIndex );      //  |  |  +----- linker directive class
        CheckSum();                   //  |  +-------- attribute (nopurge)
    }                                 //  +----------- high-order length
    exclude = ExcludeList;
    while( exclude != NULL ) {          // set all seg indicies in the exclude
        if( exclude->lnameidx == segidx ) {                     // list.
            exclude->segidx = SegIndex;
        }
        exclude = exclude->next;
    }
}

static void ProcDataRec( bool is386 )
/***********************************/
{
    unsigned        segidx;
    char *          dataloc;
    exclude_list *  exclude;
    unsigned long   offset;
    unsigned long   endoffset;

    WriteRecord();
    MakeUnsafe = FALSE;
    dataloc = Rec1->u.anyobj.rest;
    segidx = GetIndex( &dataloc );
    if( is386 || Easy32 ) {
        offset = *((unsigned_32 *)dataloc);
    } else {
        offset = *((unsigned_16 *)dataloc);
    }
    endoffset = offset + Rec1->head.length;
    exclude = ExcludeList;
    while( exclude != NULL ) {
        if( segidx == exclude->segidx ) {
            if( offset < exclude->start_off ) {
                if( endoffset >= exclude->start_off ) {
                    MakeUnsafe = TRUE;
                    break;
                }
            } else {
                if( exclude->end_off >= offset ) {
                    MakeUnsafe = TRUE;
                    break;
                }
            }
        }
        exclude = exclude->next;
    }
}

// this shuts off optimization for the following fixupp. record is of the form
// Comment, low order length, high order length, attribute (nopurge),
// class (linker directive), unsafe fixupp command, checksum.
static BYTE UnsafeRec[] = { COMENT, 4, 0, 0x80, 0xFE, 'U', 0xA1 };

static void procfixupp( void )
/****************************/
{
    if( MakeUnsafe ) {
        BuildRecord( UnsafeRec, sizeof( UnsafeRec ) );
    }
    WriteRecord();
}

extern void ProcessRec( void )
/****************************/
{
    bool    is386;

    is386 = FALSE;
    switch( Rec1->head.class ) {
    case SEGD32: is386 = TRUE;      // note the fall through
    case SEGDEF: procsegdef( is386 ); break;
    case COMENT: proccoment(); break;
    case LNAMES: proclnames(); break;
    case LEDA32: is386 = TRUE;      //  even more fall through here.
    case LIDA32: is386 = TRUE;
    case LEDATA:
    case LIDATA: ProcDataRec( is386 ); break;
    case FIXU32:
    case FIXUPP: procfixupp(); break;
    default:
        WriteRecord();
        break;
    }
}

extern int GetIndex( char **rec )
/*******************************/
{
    char *  ptr;
    int     index;

    ptr = *rec;
    index = *ptr;
    ptr++;
    if( index & 0x80 ) {
        index &= 0x7f;
        index <<= 8;
        index += *ptr;
        ptr++;
    }
    *rec = ptr;
    return( index );
}

extern int ReadRec( void )
/************************/
{
    int     len;
    int     to_read;
    long    offset;

    len = QRead( InFile, Rec1, sizeof( HEAD ) );
    if( len == 0 ) {
        return( ENDFILE );
    }
    if( len != sizeof( HEAD ) ) {
        Error( "premature end of file" );
    }
    if( Rec1->head.class == LIBRARY_HEADER ) {
        PageLen = Rec1->head.length + sizeof( HEAD );
        QSeek( InFile, PageLen, SEEK_SET );
        ReadRec();
        return( LIBRARY );
    } else if( Rec1->head.class == LIBRARY_TRAILER ) {
        return( ENDLIBRARY );
    } else {
        to_read = Rec1->head.length;
        if( to_read >= MAX_OBJECT_REC_SIZE ) {
            Error( "object file record too long" );
        }
        len = QRead( InFile, &(Rec1->u.anyobj.rest), to_read );
        if( len != to_read ) {
            Error( "premature end of file encountered" );
        }
        if( Rec1->head.class == THEADR ) {
            return( OBJECT );
        } else if( Rec1->head.class == MODEND
                   || Rec1->head.class == MODE32 ) {
            if( PageLen != 0 ) {            // skip padding in the library.
                offset = tell( InFile );
                offset = PageLen - offset % PageLen;
                if( offset == PageLen ) offset = 0;
                QSeek( InFile, offset, SEEK_CUR );
            }
            return( ENDMODULE );
        }
    }
    return( OK );
}

static void AddToSubTotal( void * buff, int len )
/***********************************************/
{
    char *  data;

    data = (char *) buff;
    while( --len >= 0 ) {
        SubTotal += *data++;
    }
}

extern void BuildRecord( void *info, unsigned len )
/*************************************************/
{
    int     overlap;
    char *  data;

    data = (char *) info;
    AddToSubTotal( data, len );
    overlap = len + InBuffer - MAX_OBJECT_REC_SIZE;
    if( overlap > 0 ) {
        memcpy( OutputBuffer + InBuffer, data, len - overlap );
        QWrite( OutFile, OutputBuffer, MAX_OBJECT_REC_SIZE );
        data += len - overlap;
        InBuffer = 0;
        len = overlap;
    }
    memcpy( OutputBuffer + InBuffer, data, len );
    InBuffer += len;
}

extern void FlushBuffer( void )
/*****************************/
{
    if( InBuffer > 0 ) {
        QWrite( OutFile, OutputBuffer, InBuffer );
        InBuffer = 0;
    }
}

extern void IndexRecord( unsigned index )
/***************************************/
// note this assumes the intel byte ordering
{
    BYTE    onebyte;

    if( index >= 128 ) {
        index = (index << 8) | (index >> 8) | 0x80;
        AddToSubTotal( &index, 2 );
        BuildRecord( &index, 2 );
    } else {
        onebyte = index;
        AddToSubTotal( &onebyte, 1 );
        BuildRecord( &onebyte, 1 );
    }
}
