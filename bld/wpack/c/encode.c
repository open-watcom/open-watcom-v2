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


/*
 * ENCODE.C : wpack routines used to encode files.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "wpack.h"
#ifdef UNIX
#include <clibext.h>
#endif

// external function declarations

extern void             EncWriteByte( char );
extern byte             EncReadByte( void );
extern void             DecWriteByte( unsigned char );
extern void             WriteMsg( char * );
extern void             WriteFiller( unsigned );
extern void             FlushRead( void );
extern void             FlushWrite( void );
extern int              QOpenR( char * );
extern unsigned long    QFileLen( int );
extern void *           MemAlloc( unsigned );
extern void             MemFree( void * );
extern unsigned long    QGetDate( int );
extern unsigned_32      GetCRC( void );
extern void             LinkList( void **, void * );
extern void             QClose( int );
extern void             QSeek( int, signed long, int );
extern int              QWrite( int, void *, int );
extern void             WriteNumeric( char *, unsigned long );
extern file_info **     ReadHeader( arccmd *, arc_header * );
extern void             Error( int, char * );
extern void             PackExit( void );
extern int              QOpenM( char * );
extern void             CopyInfo( int, int, unsigned long );
extern int              WriteSeek( unsigned long );
extern int              QOpenW( char * );
extern bool             AssignCodes( int, arccmd * );
extern void             SwitchBuffer( int, bool, void * );
extern void             RestoreBuffer( bool );
extern int              QRead( int, void *, int );
extern int              QWrite( int, void *, int );

extern uchar            text_buf[];
extern int              IOStatus;
extern int              infile, outfile;
extern int              indicies[];
extern byte             len[];

typedef struct runlist {
    struct runlist *        next;
    char                    data[1];
} runlist;

#define MAX_COPYLIST_SIZE (16*1024)

// the code array is also used to keep track of the frequency in the first
// pass through encoding the information

unsigned                code[ NUM_CHARS ];     // the code value for each char
static int              match_position, match_length;
static int              lson[N + 1], rson[N + 257], dad[N + 1];
static runlist *        RunList;
static runlist *        CurrRun;
static int              LastRunLen;
static int              NumSpilled;
static int              RunHandle;
static int              TmpHandle;
static void *           AltBuffer;

unsigned long    codesize;

#if defined( __WATCOMC__ ) && defined( __386__ )
unsigned fastcmp( char *src, char *dst, int *cmp );

#pragma aux fastcmp parm [esi] [edi] [edx] modify exact [eax ebx] value [ecx] = \
        "       xor ecx,ecx" \
        "L1:    mov eax,[esi+ecx]" \
        "       mov ebx,[edi+ecx]" \
        "       lea ecx,4[ecx]" \
        "       xor eax,ebx" \
        "       jne dscan" \
        "       cmp ecx,60" \
        "       jb  L1" \
        "       jmp finished" \
        "dscan: lea ecx,-4[ecx]" \
        "       test ax,ax" \
        "       jne in_low_word" \
        "       lea ecx,2[ecx]" \
        "       shr eax,16" \
        "in_low_word:" \
        "       test al,al" \
        "       jne almost" \
        "       inc ecx" \
        "almost:"\
        "       xor eax,eax"\
        "       xor ebx,ebx"\
        "       mov al, [esi+ecx]" \
        "       mov bl, [edi+ecx]" \
        "       sub eax,ebx"\
        "       mov [edx],eax"\
        "finished:";
#endif

static void InitTree( void )  /* Initializing tree */
/**************************/
{
    int  i;

    for (i = N + 1; i <= N + 256; i++) {
        rson[i] = NIL;            /* root */
    }
    for (i = 0; i < N; i++) {
        dad[i] = NIL;            /* node */
    }
}

static void InsertNode(int r)  /* Inserting node to the tree */
/***************************/
{
    int  i, p, cmp;
    unsigned char  *key;
    unsigned c;

    cmp = 1;
    key = &text_buf[r];
    p = N + 1 + key[0];
    rson[r] = lson[r] = NIL;
    match_length = 0;
    for ( ; ; ) {
        if (cmp >= 0) {
            if (rson[p] != NIL)
                p = rson[p];
            else {
                rson[p] = r;
                dad[r] = p;
                return;
            }
        } else {
            if (lson[p] != NIL)
                p = lson[p];
            else {
                lson[p] = r;
                dad[r] = p;
                return;
            }
        }
        cmp = 0;
#if defined( __WATCOMC__ ) && defined( __386__ )
        i = fastcmp( key + 1, &text_buf[p + 1], &cmp ) + 1;
#else
        for (i = 1; i < F; i++) {
            if( key[i] != text_buf[p + i] ) {
                cmp = key[i] - text_buf[p + i];
                break;
            }
        }
#endif
        if (i > THRESHOLD) {
            if (i > match_length) {
                match_position = ((r - p) & (N - 1)) - 1;
                if ((match_length = i) >= F) {
                    break;
                }
            }
            if (i == match_length) {
                if ((c = ((r - p) & (N - 1)) - 1) < match_position) {
                    match_position = c;
                }
            }
        }
    }
    dad[r] = dad[p];
    lson[r] = lson[p];
    rson[r] = rson[p];
    dad[lson[p]] = r;
    dad[rson[p]] = r;
    if (rson[dad[p]] == p)
        rson[dad[p]] = r;
    else
        lson[dad[p]] = r;
    dad[p] = NIL;  /* remove p */
}

static void DeleteNode(int p)  /* Deleting node from the tree */
/***************************/
{
    int  q;

    if (dad[p] == NIL)
        return;            /* unregistered */
    if (rson[p] == NIL)
        q = lson[p];
    else
    if (lson[p] == NIL)
        q = rson[p];
    else {
        q = lson[p];
        if (rson[q] != NIL) {
            do {
                q = rson[q];
            } while (rson[q] != NIL);
            rson[dad[q]] = lson[q];
            dad[lson[q]] = dad[q];
            lson[q] = lson[p];
            dad[lson[p]] = q;
        }
        rson[q] = rson[p];
        dad[rson[p]] = q;
    }
    dad[q] = dad[p];
    if (rson[dad[p]] == p)
        rson[dad[p]] = q;
    else
        lson[dad[p]] = q;
    dad[p] = NIL;
}

/*
 * Tables for encoding/decoding upper 6 bits of
 * sliding dictionary pointer
 */
/* encoder table */
static uchar p_len[64] = {
    0x03, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08
};

static uchar p_code[64] = {
    0x00, 0x20, 0x30, 0x40, 0x50, 0x58, 0x60, 0x68,
    0x70, 0x78, 0x80, 0x88, 0x90, 0x94, 0x98, 0x9C,
    0xA0, 0xA4, 0xA8, 0xAC, 0xB0, 0xB4, 0xB8, 0xBC,
    0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE,
    0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE,
    0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
    0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

static int CompLen( const int *left, const int *right )
/******************************************/
{
    return( (int)len[ *left ] - (int)len[ *right ] );
}

static void SortLengths( int num )
/********************************/
{
    qsort( indicies, num, sizeof( int ), CompLen );
}

static bool AssignCodes( int num, arccmd *cmd )
/*********************************************/
// this generates the shannon-fano code values in reverse order in the high
// bits of the code array. returns TRUE if codes successfully assigned.
{
    unsigned    codeinc;
    unsigned    lastlen;
    unsigned    codeval;
    int         index;

    SortLengths( num );
    if( len[ indicies[ num - 1 ] ] > MAX_CODE_BITS ) {
        if( !(cmd->flags & BE_QUIET) ) {
            WriteMsg( "Can't do shannon-fano compression: code length too long\n" );
        }
        return( FALSE );
    }
    memset( code, 0, NUM_CHARS * sizeof( unsigned ) );
    codeval = 0;
    codeinc = 0;
    lastlen = 0;
    for( index = num - 1; index >= 0; index -- ) {
        codeval += codeinc;
        if( len[ indicies[ index ] ] != lastlen ) {
            lastlen = len[ indicies[ index ] ];
            codeinc = 1 << (MAX_CODE_BITS - lastlen);
        }
        code[ indicies[ index ] ] = codeval;
    }
    return( TRUE );
}

static unsigned putbuf = 0;
static uchar putlen = 0;

static void Putcode(int l, unsigned c)        /* output c bits */
/************************************/
{
    putbuf |= c >> putlen;
    if ((putlen += l) >= 8) {
        EncWriteByte( putbuf >> 8 );
        if ((putlen -= 8) >= 8) {
            EncWriteByte( putbuf );
            codesize += 2;
            putlen -= 8;
            putbuf = c << (l - putlen);
        } else {
            putbuf <<= 8;
            codesize++;
        }
    }
}

static void EncodePosition( void )
/********************************/
{
    unsigned i;

    /* output upper 6 bits with encoding */
    i = EncReadByte();
    Putcode(p_len[i], (unsigned)p_code[i] << 8);

    i = EncReadByte();
    /* output lower 6 bits directly */
    Putcode(6, i << 10);
}

static void EncodeEnd( void )
/***************************/
{
    if (putlen) {
        EncWriteByte( putbuf >> 8 );
        codesize++;
    }
    putbuf = 0;
    putlen = 0;
}

static void CalcLengths( unsigned long num, int start, int finish, byte tlen )
/****************************************************************************/
{
    unsigned long   subtotal;
    int             index;


// find place that divides frequency as evenly as possible

    subtotal = 0;
    index = start;
    for(;;) {
        subtotal += code[ indicies[ index ] ];
        if( subtotal >= num / 2 ) break;
        if( index >= finish - 1 ) break;
        index++;
    }

// recursively keep subdividing the list until all elements have a length

    tlen++;
    if( index == start ) {
        len[ indicies[ index ] ] = tlen;
    } else {
        CalcLengths( subtotal, start, index, tlen );
    }
    if( index >= finish - 1 ) {
        len[ indicies[ finish ] ] = tlen;
    } else {
        CalcLengths( num - subtotal, index + 1, finish, tlen );
    }
}

static void FreeRunList( void )
/*****************************/
{
    runlist *   next;

    while( RunList != NULL ) {
        next = RunList->next;
        free( RunList );
        RunList = next;
    }
}

static void StartRunList( void )
/******************************/
{
    RunList = MemAlloc( MAX_COPYLIST_SIZE + sizeof( runlist ) );
    RunList->next = NULL;
    CurrRun = RunList;
    NumSpilled = 0;
    LastRunLen = 0;
}

static void NewRunBuffer( void )
/******************************/
{
    runlist *   buf;

    buf = NULL;
    if( NumSpilled == 0 ) {
        buf = malloc( sizeof( runlist ) + MAX_COPYLIST_SIZE );
    }
    if( buf == NULL ) {
        NumSpilled++;
        QWrite( RunHandle, CurrRun->data, MAX_COPYLIST_SIZE );
    } else {
        CurrRun->next = buf;
        buf->next = NULL;
        CurrRun = buf;
    }
    LastRunLen = 0;
}

static bool     WasLiteral;
static byte     CurrRunLen;

static void AddRunEntry( void )
/*****************************/
{
    if( !WasLiteral ) CurrRunLen |= 0x80;
    if( LastRunLen >= MAX_COPYLIST_SIZE ) NewRunBuffer();
    *(CurrRun->data + LastRunLen) = CurrRunLen;
    LastRunLen++;
    CurrRunLen = 0;
}

static void WriteTmpByte( bool inliteral, unsigned c )
/****************************************************/
{
    int     index;

    if( inliteral != WasLiteral || CurrRunLen >= 0x7F ) {
        AddRunEntry();
        WasLiteral = inliteral;
   }
    CurrRunLen++;
    if( code[ c ] == 0xFFFF ) {
        for( index = 0; index < NUM_CHARS; index++ ) {
            code[ index ] = (code[ index ] + 1) / 2;
        }
    }
    code[ c ]++;
}

static void WriteCodes( void )
/****************************/
{
    int     index;
    int     num;
    byte    runlen;
    byte    prevlen;
    byte *  lenptr;

    num = 0;
    prevlen = 0xFF;
    lenptr = len;
    runlen = 0;
    for( index = 0; index < NUM_CHARS; index++ ) {
        if( *lenptr > MAX_CODE_BITS ) {
            WriteMsg( "horrible shannon code overflow\n" );
            exit( 1 );
        }
        if( *lenptr != prevlen || (prevlen == 0 && runlen == 128)
                                    || (prevlen != 0 && runlen == 8)) {
            prevlen = *lenptr;
            num++;
            runlen = 0;
        }
        runlen++;
        lenptr++;
    }
    EncWriteByte( num  - 1 );
    codesize++;
    lenptr = len;
    prevlen = *lenptr;
    num = 0;
    for( index = 0; index < NUM_CHARS; index++ ) {
        if( *lenptr != prevlen || (prevlen != 0 && num == 8)
                               || (prevlen == 0 && num == 128) ) {
            if( prevlen == 0 ) {
                EncWriteByte( 0x80 + num - 1 );
            } else {
                EncWriteByte( ((num - 1) << 4) + prevlen - 1 );
            }
            codesize++;
            prevlen = *lenptr;
            num = 0;
        }
        lenptr++;
        num++;
    }
    if( prevlen == 0 ) {        // write out the last block
        EncWriteByte( 0x80 + num - 1 );
    } else {
        EncWriteByte( ((num - 1) << 4) + prevlen - 1 );
    }
    codesize++;
}

static void DoSecondPass( bool doshannon )
/****************************************/
{
    runlist *       rlptr;
    bool            inliteral;
    char *          data;
    int             index;
    int             num;
    unsigned        c;

    QSeek( RunHandle, 0, SEEK_SET );
    rlptr = RunList;
    while( rlptr != NULL ) {
        num = MAX_COPYLIST_SIZE;
        data = rlptr->data;
        if( rlptr->next == NULL ) {
            if( NumSpilled > 0 ) {
                QRead( RunHandle, AltBuffer, MAX_COPYLIST_SIZE );
                data = AltBuffer;
                NumSpilled--;
            } else {
                num = LastRunLen;
                rlptr = rlptr->next;
            }
        } else {
            rlptr = rlptr->next;
        }
        while( num > 0 ) {
            inliteral = !(*data & 0x80);
            for( index = *data & 0x7F; index > 0; index-- ) {
                c = EncReadByte();
                if( inliteral ) {
                    if( doshannon ) {
                        Putcode( len[ c ], code[ c ] );
                    } else {
                        Putcode( 9, c << 7 );  /* 0 bit, then code word. */
                    }
                } else {
                    if( doshannon ) {
                        c = 255 - THRESHOLD + c;
                        Putcode( len[ c ], code[ c ] );
                    } else {
                        Putcode( 7, (c + 0x40) << 9 ); /* 1, then length */
                    }
                    EncodePosition();
                }
            }
            num--;
            data++;
        }
    }
}

/* Compression */

static int DoEncode( arccmd *cmd )
/********************************/
{
    int             c, num, r, s, last_match_length;
    int             index;
    unsigned        currvalue;
    unsigned *      uptr;
    unsigned long   total;
    bool            inliteral;
    bool            doshannon;

    codesize = 0;
    WasLiteral = TRUE;
    CurrRunLen = 0;
    StartRunList();
    QSeek( TmpHandle, 0, SEEK_SET );
    QSeek( RunHandle, 0, SEEK_SET );
    SwitchBuffer( TmpHandle, TRUE, AltBuffer );
    memset( code, 0, NUM_CHARS * sizeof( unsigned ) );
    InitTree();
    s = 0;
    r = N - F;
    for (index = s; index < r; index++) {
        text_buf[index] = ' ';
    }
    for (num = 0; num < F && (c = EncReadByte(),IOStatus == OK); num++) {
        text_buf[r + num] = c;
    }
    if( IOStatus == IO_PROBLEM ) return( -1 );
    for (index = 1; index <= F; index++) {
        InsertNode(r - index);
    }
    InsertNode(r);
    do {
        if (match_length > num)
            match_length = num;
        if (match_length <= THRESHOLD) {
            match_length = 1;
            inliteral = TRUE;
            currvalue = text_buf[r];
            DecWriteByte( currvalue );
        } else {
            inliteral = FALSE;
            currvalue = 255 - THRESHOLD + match_length;
            DecWriteByte( match_length );
            DecWriteByte( match_position >> 6 );
            DecWriteByte( match_position & 0x3F );
        }
        WriteTmpByte( inliteral, currvalue );
        last_match_length = match_length;
        for (index = 0;
             index < last_match_length && (c=EncReadByte(),IOStatus == OK);
                                                                 index++ ) {
            DeleteNode(s);
            text_buf[s] = c;
            if (s < F - 1)
                text_buf[s + N] = c;
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);
            InsertNode(r);
        }
        if( IOStatus == IO_PROBLEM ) return( -1 );
        while (index++ < last_match_length) {
            DeleteNode(s);
            s = (s + 1) & (N - 1);
            r = (r + 1) & (N - 1);
            if (--num) InsertNode(r);
        }
    } while (num > 0);
    AddRunEntry();
    FlushWrite();
    RestoreBuffer( TRUE );
    SwitchBuffer( TmpHandle, FALSE, NULL );
// now set up the compression stuff & do the second pass.
    num = 0;
    total = 0;
    uptr = code;
    for( index = 0; index < NUM_CHARS; index++ ) {
        len[ index ] = 0;
        if( *uptr != 0 ) {
            total += *uptr;
            indicies[ num ] = index;
            num++;
        }
        uptr++;
    }
    QSeek( infile, 0, SEEK_SET );
    CalcLengths( total, 0, num - 1, 0 );
    doshannon = AssignCodes( num, cmd );
    if( doshannon ) {       // we have shannon codes,
        WriteCodes();       // so write out file using these
    }
    DoSecondPass( doshannon );
    EncodeEnd();
    RestoreBuffer( FALSE );
    FreeRunList();
    return( !doshannon );
}

static void InitHeader( arc_header *header, arccmd *cmd )
/******************************************/
{
    header->signature = SIGNATURE;
    header->major_ver = MAJOR_VERSION;
    header->minor_ver = MINOR_VERSION;
    header->num_files = 0;
    header->info_offset = 0;
    header->info_len = 0;
    if( cmd->flags & SECURE_PACK ) {
        header->major_ver += cmd->internal;// so incompat. with previous vers
        header->internal = cmd->internal;
    }

}

static void WriteHeaders( arc_header *header, info_list *list, int numfiles,
                                                                   int file )
/***************************************************************************/
{
    info_list *     info;           // node of the info_list
    int             entrylen;       // length of the file_info entry

    while( numfiles > 0 ) {     //NYI buffer this stuff.
        entrylen = sizeof(file_info) + (list->i.namelen & NAMELEN_MASK) - 1;
        QWrite( file, &list->i, entrylen );
        header->info_len += entrylen;
        info = list->next;
        MemFree( list );
        list = info;
        numfiles--;
    }
    QSeek( file, 0L, SEEK_SET );
    QWrite( file, header, sizeof( arc_header ) );
}

static void ReplaceExt( char * name, char * new_ext )
/***************************************************/
{
    char p[ _MAX_DIR ];
    char d[ _MAX_DRIVE ];
    char n[ _MAX_FNAME ];

    _splitpath( name, d, p, n, NULL );
    _makepath( name, d, p, n, new_ext );
}

// this is used for keeping track of the different archives while multipacking

typedef struct arc_list {
    struct arc_list *   next;
    info_list *         info;
    unsigned long       length;
    unsigned            info_len;
    int                 handle;
    int                 num_files;
} arc_list;

static void MultiPack( arccmd *cmd, info_list *list )
/***************************************************/
{
    arc_list *      archead;
    arc_list *      currarc;
    int             numarcs;
    unsigned long   compressed;
    unsigned long   currspot;
    unsigned        entrylen;
    unsigned long   limit;
    char            extension[ 4 ];
    char *          arcfname;
    info_list *     nextfile;
    arc_header      header;

    arcfname = alloca( strlen( cmd->arcname ) + 4 );
    strcpy( arcfname, cmd->arcname );
    limit = (unsigned long)cmd->u.limit << 10;
    numarcs = 0;
    archead = NULL;
    QSeek( outfile, sizeof( arc_header ), SEEK_SET );
    currspot = sizeof( arc_header );
    while( list != NULL ) {             // first get length of compressed file
        entrylen = sizeof(file_info) + (list->i.namelen & NAMELEN_MASK) - 1;
        nextfile = list->next;
        if( nextfile == NULL ) {
            compressed = QFileLen( outfile ) - currspot;
        } else {
            compressed = nextfile->i.disk_addr - currspot;
            currspot = nextfile->i.disk_addr;
        }
        currarc = archead;
        while( currarc != NULL ) {      // find an archive to pack file in.
            if( currarc->length + currarc->info_len + compressed
                                                  + entrylen < limit ) break;
            currarc = currarc->next;
        }
        if( currarc == NULL ) {         // need to make a new archive
            currarc = alloca( sizeof( arc_list ) );
            if( currarc == NULL ) Error( -1, "insufficient stack space!" );
            currarc->next = NULL;
            currarc->info = NULL;
            currarc->info_len = 0;
            currarc->length = sizeof( arc_header );
            currarc->num_files = 0;
            numarcs++;
            itoa( numarcs, extension, 36 );
            ReplaceExt( arcfname, extension );
            currarc->handle = QOpenW( arcfname );
            if( currarc->handle < 0 ) PackExit();
            QSeek( currarc->handle, sizeof( arc_header ), SEEK_SET );
            LinkList( &archead, currarc );
        }
        currarc->num_files++;       // update arc info & write data
        list->i.disk_addr = currarc->length;
        currarc->length += compressed;
        currarc->info_len += entrylen;
        LinkList( &currarc->info, list );
        CopyInfo( currarc->handle, outfile, compressed );
        list->next = NULL;
        list = nextfile;
    }
    currarc = archead;          // now fill in header information in archives
    while( currarc != NULL ) {
        InitHeader( &header, cmd );
        header.num_files = currarc->num_files;
        header.info_offset = currarc->length;
        WriteHeaders( &header, currarc->info, currarc->num_files,
                                                             currarc->handle );
        QClose( currarc->handle );
        currarc = currarc->next;
    }
    QClose( outfile );
    remove( cmd->arcname );
}

extern void Encode( arccmd *cmd )
/*******************************/
{
    wpackfile *     currname;       // current file name being processed.
    unsigned long   length;         // unencrypted length of the file.
    unsigned long   amtwrote;       // amount wrote to archive files
    unsigned long   amtread;        // amout read from all files.
    char *          name;           // stored name of the file
    char *          temp;
    unsigned        numfiles;       // number of files stored in archive
    int             namelen;        // length of the filename
    int             result;         // result of encoding the file.
    info_list *     info;           // node of the info_list
    info_list *     liststart;      // beginning of the info_list
    arc_header      header;         // archive main header.
    file_info **    filedata;       // block of file infos from old archive.
    char            tmpfile[ L_tmpnam ];    // pass1 info temp file name;
    char            runfile[ L_tmpnam ];    // run length temp file name;

    if( cmd->files == NULL  ||  cmd->files->filename == NULL ) {
        Error( -1, "No files to pack\n" );
    }
    filedata = ReadHeader( cmd, &header );
    if( filedata != NULL ) {    // there is an old file to add to.
        QClose( infile );
        if( cmd->flags & PACK_LIMIT ) {
            Error( -1, "old archive already exists");
        }
        outfile = QOpenM( cmd->arcname );
        if( outfile == -1 ) PackExit();
        WriteSeek( header.info_offset );
        amtwrote = header.info_offset;
    } else {
        outfile = QOpenW( cmd->arcname );
        WriteFiller( sizeof( arc_header ) );    // reserve space for header.
        amtwrote = sizeof( arc_header );
    }
    tmpnam( tmpfile );
    TmpHandle = QOpenW( tmpfile );
    tmpnam( runfile );
    RunHandle = QOpenW( runfile );
    if( TmpHandle < 0 || RunHandle < 0 ) {
        Error( -1, "problem opening temporary files" );
    }
    AltBuffer = MemAlloc( MAX_COPYLIST_SIZE );      // must be >= WRITE_SIZE
    liststart = NULL;
    amtread = 0;
    numfiles = 0;
    for( currname = cmd->files; currname->filename != NULL; currname++ ) {
        FlushRead();
        if( !(cmd->flags & PRESERVE_FNAME_CASE) ) {
            strlwr( currname->filename );
        }
        infile = QOpenR( currname->filename );
        if( infile == -1 ) {
            WriteMsg( "could not open file: " );
            WriteMsg( currname->filename );
            WriteMsg( "\n" );
        } else {
            numfiles++;
            if( !(cmd->flags & BE_QUIET) ) {
                WriteMsg( "packing file '" );
                WriteMsg( currname->filename );
                WriteMsg( "'\n" );
            }
            result = DoEncode( cmd );
            if( result == -1 ) continue;      // don't archive if error.
            if( currname->packname != NULL ) {
                name = currname->packname;
            } else if( !(cmd->flags & KEEP_PATHNAME) ) { // search for actual file name
                temp = currname->filename;
                name = temp;
                while( *temp != '\0' ) {
                    if( *temp == '\\' || *temp == ':' ) {
                        name = temp + 1;
                    }
                    temp++;
                }
            } else {
                name = currname->filename;
            }
            length = QFileLen( infile );
            amtread += length;
            namelen = strlen( name );
            info = MemAlloc( sizeof( info_list ) + namelen - 1);
            info->i.length = length;
            info->i.disk_addr = amtwrote;
            amtwrote += codesize;
            if( cmd->flags & USE_DATE_TIME ) {
                info->i.stamp = cmd->time;
            } else {
                info->i.stamp = QGetDate( infile );
            }
            memcpy( info->i.name, name, namelen );
            if( result ) {
                namelen |= NO_SHANNON_CODE;
            }
            info->i.namelen = namelen;
            info->i.crc = GetCRC();
            info->next = NULL;
            LinkList( &liststart, info );
            QClose( infile );
        }  // end if
    } // end for
    QClose( TmpHandle );
    QClose( RunHandle );
    remove( tmpfile );
    remove( runfile );
    if( numfiles > 0 ) {
        FlushWrite();
        if( cmd->flags & PACK_LIMIT ) {
            MultiPack( cmd, liststart );
        } else {    // add files to current archive
            if( filedata == NULL ) {
                InitHeader( &header, cmd );
            } else {        // write stuff in old header.
                QWrite( outfile, *filedata, header.info_len );
            }
            header.num_files += numfiles;
            header.info_offset = amtwrote;
            WriteHeaders( &header, liststart, numfiles, outfile );
            if( filedata == NULL ) {
                amtwrote += header.info_len;
                if( !(cmd->flags & BE_QUIET ) ) {
                    WriteNumeric( "uncompressed size: ", amtread );
                    WriteNumeric( "compressed size: ", amtwrote );
                }
            }
            QClose( outfile );      // close the archive file.
        }
    }
}


