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
* Description:  wpack routines used to decode files.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef __WATCOMC__
#include "clibext.h"
#endif
#ifndef __UNIX__
#include <dos.h>
#endif
#include "wpack.h"
#include "txttable.h"

// external function declarations
extern unsigned char    DecReadByte( void );
extern void             DecWriteByte( unsigned char );
extern void             FlushRead( void );
extern void             FlushWrite( void );
extern int              BufSeek( unsigned long );
extern void             QSetDate( char *, unsigned long );
extern int              QWrite( int, void *, int );
extern bool             CheckCRC( unsigned_32 );
extern void             QClose( int );
extern int              QOpenW( char * );
extern file_info **     ReadHeader( arccmd *, arc_header * );
extern void             FreeHeader( file_info ** );
extern void             AssignCodes( int );
extern void             ModifyCRC( unsigned long *, byte );
extern void             UnReadByte( byte );
extern void             DecWriteBuf( void *, int );
extern int              OK_ToReplace( char * );
extern int              OK_ReplaceRDOnly( char * );
extern void             LogUnPacking( char * );
extern int              UnPackHook( char * );

#ifdef _M_IX86
#pragma aux DecReadByte parm nomemory modify nomemory;
#pragma aux DecWriteByte parm nomemory modify nomemory;
#endif

extern int          infile, outfile;
extern uchar        text_buf[];
extern int          indicies[];
extern byte         len[];

unsigned short   MinCodeLen;
unsigned short   MinVal[ MAX_CODE_BITS + 1];
unsigned short   MapOffset[ MAX_CODE_BITS + 1];
unsigned short   CharMap[ NUM_CHARS ];

/* decoder table */
static uchar d_code[256] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
    0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
    0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
    0x0C, 0x0C, 0x0C, 0x0C, 0x0D, 0x0D, 0x0D, 0x0D,
    0x0E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F,
    0x10, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11,
    0x12, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x13,
    0x14, 0x14, 0x14, 0x14, 0x15, 0x15, 0x15, 0x15,
    0x16, 0x16, 0x16, 0x16, 0x17, 0x17, 0x17, 0x17,
    0x18, 0x18, 0x19, 0x19, 0x1A, 0x1A, 0x1B, 0x1B,
    0x1C, 0x1C, 0x1D, 0x1D, 0x1E, 0x1E, 0x1F, 0x1F,
    0x20, 0x20, 0x21, 0x21, 0x22, 0x22, 0x23, 0x23,
    0x24, 0x24, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27,
    0x28, 0x28, 0x29, 0x29, 0x2A, 0x2A, 0x2B, 0x2B,
    0x2C, 0x2C, 0x2D, 0x2D, 0x2E, 0x2E, 0x2F, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};

static uchar d_len[256] = {
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
};

static unsigned short getbuf;
static uchar          getlen;
static uchar          secondbuf;

// this not needed anymore
#if 0
GLOBAL int GetBit( void )
/***********************/
/* get one bit */
{
    int i;

    if( getlen == 0 ) {
        getbuf = secondbuf << 8;
        getlen = 8;
        secondbuf = DecReadByte();
    }
    i = (int) getbuf;
    getbuf <<= 1;
    getlen--;
    return ( i < 0 );
}
#endif

GLOBAL unsigned short GetByte(void)
/*********************************/
/* get a byte */
{
    unsigned short i;

    i = getbuf >> 8;
    if( getlen >= 8 ) {
        getbuf <<= 8;
        getlen -= 8;
    } else {
        getbuf = secondbuf;
        i |= getbuf >> getlen;
        getbuf <<= (16 - getlen);
        secondbuf = DecReadByte();
    }
    return( i );
}

static byte Mask[] = { 0, 1, 3, 7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF };

GLOBAL int DecodePosition( void )
/*******************************/
{
    unsigned short i, j, c;

    /* decode upper 6 bits from given table */
    i = GetByte();
    c = (unsigned short)d_code[i] << 6;
    j = d_len[i];

    /* input lower 6 bits directly */
    j -= 2;
    if( j > getlen ) {
        getbuf |= (unsigned short)secondbuf << (8 - getlen);
        getlen += 8;
        secondbuf = DecReadByte();
    }
    i = (i << j) | (getbuf >> (16 - j));
    getbuf <<= j;
    getlen -= j;
    return c | i & 0x3f;
}

static int  Location;

static void MakeShannonTrie( void )
/*********************************/
{
    unsigned short   numcoded;
    unsigned short   num;
    unsigned short   entry;
    unsigned short   curr;
    byte        entrylen;
    byte        entrynum;

/*
 * first read in and unpack table
*/
    curr = 0;
    num = 0;
    Location = 0;
    numcoded = DecReadByte() + 1;
    while( numcoded > 0 ) {
        entry = DecReadByte();
        if( entry & 0x80 ) {        // no codes assigned, so skip some
            curr += (entry & 0x7F) + 1;
        } else {
            entrynum = (entry >> 4) + 1;
            entrylen = (entry & 0xF) + 1;
            while( entrynum > 0 ) {
                indicies[ num ] = curr;
                len[ curr ] = entrylen;
                num++;
                curr++;
                entrynum--;
            }
        }
        numcoded--;
    }
    AssignCodes( num );         // assign codes to the lengths
}                               // & build decompression structures

static int CompLen( const int *left, const int *right )
/*****************************************************/
{
    return( (int)len[ *left ] - (int)len[ *right ] );
}


static void SortLengths( int num )
/********************************/
{
    extern int wpack_qsort( void *, int, int,
                            int (*)(const void *, const void *) );

    wpack_qsort( indicies, num, sizeof( int ),
                 (int (*)(const void *, const void *))CompLen );
}

extern void AssignCodes( int num )
/********************************/
// this builds the decompression structures.
{
    unsigned short   codeinc;
    unsigned short   lastlen;
    unsigned short   codeval;
    int         index;
    unsigned short   curroffset;

    SortLengths( num );
    for( index = 0; index <= MAX_CODE_BITS; index++ ) { // so lengths with no
        MinVal[ index ] = 0xFFFF;                   // codes are never decoded
    }
    codeval = 0;
    codeinc = 0;
    lastlen = 0;
    curroffset = 0;
    MinCodeLen = len[ indicies[ 0 ] ];
    for( index = num - 1; index >= 0; index-- ) {
        codeval += codeinc;
        if( len[ indicies[ index ] ] != lastlen ) {
            lastlen = len[ indicies[ index ] ];
            codeinc = 1 << (16 - lastlen);
            MinVal[ lastlen ] = codeval;
            MapOffset[ lastlen ] = curroffset;
        }
        CharMap[ curroffset ] = indicies[ index ];
        curroffset++;
    }
}

extern void NoShannonDecode( unsigned long textsize )
/***************************************************/
/* Decoding/Uncompressing */
{
    int             i, j, k, r, c;
    unsigned long   count;

    if (textsize == 0) return;
    getlen = 0;
    getbuf = 0;
    secondbuf = DecReadByte();
    for (i = 0; i < N - F; i++) {
        text_buf[i] = ' ';
    }
    r = N - F;
    for (count = 0; count < textsize; ) {
        if( getlen == 0 ) {
            getbuf = ((unsigned short)secondbuf << 8) | DecReadByte();
            getlen = 16;
            secondbuf = DecReadByte();
        } else if( getlen <= 8 ) {
            getbuf |= (unsigned short)secondbuf << ( 8 - getlen );
            getlen += 8;
            secondbuf = DecReadByte();
        }
        if( (short int) getbuf < 0 ) {    // it is a copy command.
            j = (getbuf >> 9) & 0x3F;
            getlen -= 7;
            getbuf <<= 7;
            i = (r - DecodePosition() - 1) & (N - 1);
            for (k = 0; k < j; k++) {
                c = text_buf[(i + k) & (N - 1)];
                text_buf[r++] = c;
                DecWriteByte( c );
                r &= (N - 1);
            }
            count += j;
        } else {
            c = getbuf >> 7;
            getbuf <<= 9;
            getlen -= 9;
            DecWriteByte( c);
            text_buf[r++] = c;
            r &= (N - 1);
            count++;
        }
    }
    FlushWrite();
}

extern void DoDecode( unsigned long textsize )
/********************************************/
/* Decoding/Uncompressing */
{
    int             i, j, k, r, c;
    int             spare;
    int             codelen;
    unsigned long   count;

    if (textsize == 0) return;
    MakeShannonTrie();
    getlen = 0;
    getbuf = 0;
    secondbuf = DecReadByte();
    for (i = 0; i < N - F; i++) {
        text_buf[i] = ' ';
    }
    r = N - F;
    for (count = 0; count < textsize; ) {
        if( getlen < 8 ) {
            getbuf |= (unsigned short)secondbuf << ( 8 - getlen );
            getlen += 8;
            secondbuf = DecReadByte();
        }
        spare = getlen - 8;
        getlen = 16;
        getbuf |= secondbuf >> spare;    // fill getbuf to 16 bits.
        codelen = MinCodeLen;
        for(;;) {
            if( getbuf >= MinVal[ codelen ] )break;
            codelen++;
        }
        c = CharMap[ MapOffset[ codelen ] +
                            ((getbuf - MinVal[ codelen ]) >> (16 - codelen)) ];
        getbuf <<= codelen;
        getlen -= codelen;
        if( spare > codelen ) {
            getlen -= 8 - spare;
        } else {
            getbuf |= (secondbuf & Mask[ spare ]) << (codelen - spare);
            getlen += spare;
            secondbuf = DecReadByte();
        }
        if (c < 256) {
            DecWriteByte( c);
            text_buf[r++] = c;
            r &= (N - 1);
            count++;
        } else {
            i = (r - DecodePosition() - 1) & (N - 1);
            j = c - 255 + THRESHOLD;
            for (k = 0; k < j; k++) {
                c = text_buf[(i + k) & (N - 1)];
                text_buf[r++] = c;
                DecWriteByte( c );
                r &= (N - 1);
            }
            count += j;
        }
    }
    FlushWrite();
}

static bool CompareCRC( unsigned long crcvalue )
/**********************************************/
// this modifies the old CRC for the bit lookahead, checks the CRC, and then
// it adds the lookahead to the CRC for the next file.
// note this assumes intel byte ordering
{
    bool    retval;

    if( getlen == 16 ) {
        ModifyCRC( &crcvalue, getbuf >> 8 );
        ModifyCRC( &crcvalue, getbuf & 0xFF );
        ModifyCRC( &crcvalue, secondbuf );
        retval = CheckCRC( crcvalue );
        UnReadByte( secondbuf );
        UnReadByte( getbuf & 0xFF );
        UnReadByte( getbuf >> 8 );
    } else if( getlen >= 8 ) {
        getbuf >>= 16 - getlen;
        ModifyCRC( &crcvalue, getbuf & 0xFF );
        ModifyCRC( &crcvalue, secondbuf );
        retval = CheckCRC( crcvalue );
        UnReadByte( secondbuf );
        UnReadByte( getbuf );
    } else {
        ModifyCRC( &crcvalue, secondbuf );
        retval = CheckCRC( crcvalue );
        UnReadByte( secondbuf );
    }
    return( retval );
}

static int FileExists( char *name, file_info *info );           // FileExists defined further down in this file
extern bool DecodeFile( file_info *info, arccmd *cmd )
/****************************************************/
{
    char           drive[_MAX_DRIVE];
    char           directory[_MAX_DIR];
    char           fname[_MAX_FNAME];
    char           extin[_MAX_EXT];
    char *         name;
    char *         thename;      // filename terminated with a nullchar.
    int            pathlen;
    unsigned short namelen;

    namelen = info->namelen & NAMELEN_MASK;
    thename = alloca( namelen + 1 );
    memcpy( thename, info->name, namelen );
    *(thename + namelen) = '\0';
    if( (cmd->flags & KEEP_PATHNAME) ) {
        name = cmd->u.path;
    } else if( !(cmd->flags & (REPLACE_PATH | PREPEND_PATH)) ) {
        name = thename;
    } else {
        pathlen = strlen( cmd->u.path );
        name = alloca( namelen + pathlen  + 1 );
        if( cmd->flags & REPLACE_PATH ) {
            _splitpath( cmd->u.path, drive, directory, NULL, NULL );
            _splitpath( thename, NULL, NULL, fname, extin );
            _makepath( name, drive, directory, fname, extin );
        } else {
            memcpy( name, cmd->u.path, pathlen );
            memcpy( name + pathlen, info->name, namelen );
            *(name + pathlen + namelen) = '\0';
        }
    }
    if( ! FileExists( name, info ) ) {
        outfile = QOpenW( name );
        if( outfile == -1 )  {
            return( FALSE );
        } else {
            LogUnPacking( name );
            if( info->namelen & NO_SHANNON_CODE ) {
                NoShannonDecode( info->length );
            } else {
                DoDecode( info->length );
            }
            QClose( outfile );
            QSetDate( name, info->stamp );
            if( !CompareCRC( info->crc ) && info->length > 0 ) {
                char msg[ 100 ];
                strcpy( msg, LookupText( NULL, TXT_WARN_FILE ) );
                strcat( msg, " \'" );
                strcat( msg, name );
                strcat( msg, "\' " );
                strcat( msg, LookupText( NULL, TXT_INC_CRC ) );
                Error( TXT_INC_CRC, msg );
                return( FALSE );
            }
        }
    }
    return( TRUE );
}

static int FileExists( char *name, file_info *info )            /* 26-may-90 */
{
    auto struct stat            statblk;
#if !defined( __UNIX__ )
    unsigned                    attribute;
#endif
    int                         rc;

    rc = stat( name, &statblk );
    if( rc == 0 ) {
#if defined( __UNIX__ )
        if( !( statblk.st_mode & S_IWRITE ) ) {
#else
        _dos_getfileattr( name, &attribute );
        if( attribute & ( _A_RDONLY | _A_HIDDEN ) ) {
#endif
            /* file is read-only */
            if( !OK_ReplaceRDOnly( name ) ) {
                return( 1 );
            }
#if defined( __UNIX__ )
            chmod( name, 0777 );
#else
            _dos_setfileattr( name, _A_NORMAL );
#endif
        }
        if( UnPackHook( name ) ) return( 0 ); // this is allowed to modify name
        if( statblk.st_mtime == info->stamp ) {
            return( 1 );
        }
        if( statblk.st_mtime > info->stamp ) {                  /* 14-sep-91 */
            /* file already exists with newer date */
            if( !OK_ToReplace( name ) ) {
                return( 1 );
            }
        }
    } else {
        UnPackHook( name );
    }
    return( 0 );    /* file does not exist, or it has different date or size */
}

extern int Decode( arccmd *cmd )
/*******************************/
{
    file_info **    currfile;
    file_info **    filedata;
    wpackfile *     currname;
    arc_header      header;
    unsigned short  namelen;
    char            *msg;

    filedata = ReadHeader( cmd, &header );
    if( filedata == NULL ) {
        msg = LookupText( NULL, TXT_ARC_NOT_EXIST );
        Error( TXT_ARC_NOT_EXIST, msg );
        return FALSE;
    }
    if( cmd->files == NULL  ||  cmd->files->filename == NULL ) {
//      BufSeek( sizeof( arc_header ) );    // skip header.
        for( currfile = filedata; *currfile != NULL; currfile++ ) {
            if( BufSeek( (*currfile)->disk_addr ) != -1 ) {
                if( !DecodeFile( *currfile, cmd ) ) {
                    return FALSE;
                }
            }
        }
    } else {
        for( currname = cmd->files; currname->filename != NULL; currname++ ) {
            for( currfile = filedata; *currfile != NULL; currfile++ ) {
                namelen = (*currfile)->namelen & NAMELEN_MASK;
                if( strlen( currname->filename ) == namelen &&
                    memicmp(currname->filename, (*currfile)->name, namelen) == 0 ) {
                    if( BufSeek( (*currfile)->disk_addr ) != -1 ) {
                        if( !DecodeFile( *currfile, cmd ) ) {
                            return FALSE;
                        }
                    }
                    break;
                }
            }
            if( *currfile == NULL ) {
                char msg[ 50 ];
                strcpy( msg, LookupText( NULL, TXT_NOT_IN_ARC ) );
                Log( LookupText( NULL, TXT_WARN_FILE ), "\"", currname->filename, "\"",
                     msg, NULL );
            }
        }  // end for
    } // end if
    QClose( infile );       // close the archive file.
    FreeHeader( filedata );
    return TRUE;
}
