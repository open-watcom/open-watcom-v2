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


#include <stdlib.h>

#include "dmpobj.h"

data_ptr    NamePtr;
byte        NameLen;
unsigned_16 RecLen;
data_ptr    RecBuff;
data_ptr    RecPtr;
unsigned_16 RecMaxLen;
unsigned    RecNum;
unsigned_16 Segindex;
unsigned_16 Nameindex;
unsigned_16 Importindex;
unsigned_16 Libindex;
bool        IsPharLap;
bool        IsMS386;
bool        IsIntel;
bool        DumpRaw;
jmp_buf     BailOutJmp;

bool EndRec( void )
/*****************/
{
    return( RecPtr >= (RecBuff + RecLen) );
}

unsigned_16 RecOffset( void )
/***************************/
{
    return( (unsigned_16)( RecPtr - RecBuff ) );
}

byte GetByte( void )
/******************/
{
    byte        ret;

    ret = *RecPtr;
    RecPtr++;
    return( ret );
}

void BackupByte( void )
/*********************/
{
    --RecPtr;
}

#if defined( M_I86 ) || defined( __386__ )
unsigned_16 GetUInt( void )
/*************************/
{
    unsigned_16 word;

    word = *(unsigned_16 *)RecPtr;
    RecPtr += 2;
    return( word );
}

unsigned_32 GetLInt( void )
/*************************/
{
    unsigned_32 dword;

    dword = *(unsigned_32 *)RecPtr;
    RecPtr += 4;
    return( dword );
}
#else
unsigned_16 GetUInt( void )
/*************************/
{
    unsigned_16 lo;
    unsigned_16 hi;

    lo = GetByte();
    hi = GetByte();
    return( lo + (hi << 8) );
}

unsigned_32 GetLInt( void )
/*************************/
{
    unsigned_32 lo;
    unsigned_32 hi;

    lo = GetUInt();
    hi = GetUInt();
    return( lo + (hi << 16 ) );
}
#endif

unsigned_32 GetEither( void )
/***************************/
{
    unsigned_32 value;

    if( IsPharLap || IsMS386 ) {
        value = GetLInt();
    } else {
        value = GetUInt();
    }
    return( value );
}

void GetName( void )
/******************/
{
    NameLen = GetByte();
    NamePtr = RecPtr;
    RecPtr += NameLen;
}

unsigned_16 GetIndex( void )
/**************************/
{
    unsigned_16 index;

    index = GetByte();
    if( index & 0x80 ) {
      index = ( (index & 0x7f) << 8 ) + GetByte();
    }
    return( index );
}

unsigned_32 GetVariable( void )
/*****************************/
{
    byte        index;
    unsigned_32 size;
    unsigned_16 lo;

    index = GetByte();
    if( index <= COMDEF_LEAF_SIZE ) {
        size = index;
    } else if( index == COMDEF_LEAF_2 ) {
        size = GetUInt();
    } else if( index == COMDEF_LEAF_3 ) {
        lo = GetUInt();
        size = lo + ( GetByte() << 16 );
    } else if( index == COMDEF_LEAF_4 ) {
        size = GetLInt();
    } else {
        Output( BAILOUT "Unknown COMDEF LEAF size (%b)" CRLF, index );
        longjmp( BailOutJmp, 1 );
    }
    return( size );
}


#if defined(__WATCOMC__)
    #if defined( M_I86SM ) || defined( M_I86MM )
        extern byte docksum( byte *buf, unsigned_16 len );
        #pragma aux docksum = \
            0x30 0xd2       /* xor dl,dl */ \
            0xac            /* L1: lodsb */ \
            0x00 0xc2       /* add dl,al */ \
            0xe2 0xfb       /* loop L1 */ \
            parm            [si] [cx] \
            value           [dl] \
            modify          [ax dl];
        #define SPECIAL_CHKSUM
    #elif defined( M_I86 )
        extern byte docksum( byte far *buf, unsigned_16 len );
        #pragma aux docksum = \
            0x1e            /* push ds */ \
            0x8e 0xda       /* mov ds,dx */ \
            0x30 0xd2       /* xor dl,dl */ \
            0xac            /* L1: lodsb */ \
            0x00 0xc2       /* add dl,al */ \
            0xe2 0xfb       /* loop L1 */ \
            0x1f            /* pop ds */ \
            parm            [dx si] [cx] \
            value           [dl] \
            modify          [ax];
        #define SPECIAL_CHKSUM
    #endif
#endif

static byte checkSumBuff( void )
{
#if defined( SPECIAL_CHKSUM )
    return( docksum( RecBuff, RecLen ) );
#else
    byte        cksum;
    data_ptr    p;

    cksum = 0;
    p = RecBuff + RecLen;
    while( p > RecBuff ) {
        --p;
        cksum += *p;
    }
    return( cksum );
#endif
}

void ResizeBuff( unsigned_16 reqd_len ) {

    if( reqd_len > RecMaxLen ) {
        RecMaxLen = reqd_len;
        if( RecBuff != NULL ) {
            free( RecBuff );
        }
        RecBuff = malloc( RecMaxLen );
        if( RecBuff == NULL ) {
            OutputSetFH( stdout );
            Output( CRLF "**FATAL** Out of memory!" CRLF );
            leave( 20 );
        }
    }
}

void ProcFile( FILE *fp, bool is_intel )
/**************************************/
{
    byte        cksum;
    byte        hdr[ 3 ];
    unsigned_16 page_len;
    unsigned_32 offset;
    char        *recname;
    unsigned_32 total_padding;
    int         raw_dump;

    IsPharLap = FALSE;
    IsMS386 = FALSE;
    IsIntel = is_intel;
    RecNum = 0;
    page_len = 0;
    RecBuff = NULL;
    RecMaxLen = 0;
    total_padding = 0;
    for(;;) {
        raw_dump = DumpRaw;
        offset = ftell( fp );
        if( fread( hdr, 1, 3, fp ) != 3 ) break;
        cksum  = hdr[ 0 ];
        cksum += hdr[ 1 ];
        cksum += hdr[ 2 ];
        RecLen = hdr[ 1 ] | ( hdr[ 2 ] << 8 );
        ResizeBuff( RecLen );
        RecPtr = RecBuff;
        if( fread( RecBuff, RecLen, 1, fp ) == 0 ) {
            break;
        }
        cksum += checkSumBuff();
        IsMS386 = hdr[ 0 ] & 1;
        if( IsMS386 ) {
            IsIntel = FALSE;
        }
        switch( hdr[ 0 ] & ~1 ) {
        case CMD_RHEADR:        recname = "RHEADR";             break;
        case CMD_REGINT:        recname = "REGINT";             break;
        case CMD_REDATA:        recname = "REDATA";             break;
        case CMD_RIDATA:        recname = "RIDATA";             break;
        case CMD_OVLDEF:        recname = "OVLDEF";             break;
        case CMD_ENDREC:        recname = "ENDREC";             break;
        case CMD_BLKDEF:        recname = "BLKDEF";             break;
        case CMD_BLKEND:        recname = "BLKEND";             break;
        case CMD_DEBSYM:        recname = "DEBSYM";             break;
        case CMD_THEADR:        recname = "THEADR";             break;
        case CMD_LHEADR:        recname = "LHEADR";             break;
        case CMD_PEDATA:        recname = "PEDATA";             break;
        case CMD_PIDATA:        recname = "PIDATA";             break;
        case CMD_COMENT:        recname = "COMENT";             break;
        case CMD_MODEND:        recname = "MODEND";             break;
        case CMD_EXTDEF:        recname = "EXTDEF";             break;
        case CMD_TYPDEF:        recname = "TYPDEF";             break;
        case CMD_PUBDEF:        recname = "PUBDEF";             break;
        case CMD_LOCSYM:        recname = "LOCSYM";             break;
        case CMD_LINNUM:        recname = "LINNUM";             break;
        case CMD_LNAMES:        recname = "LNAMES";             break;
        case CMD_SEGDEF:        recname = "SEGDEF";             break;
        case CMD_GRPDEF:        recname = "GRPDEF";             break;
        case CMD_FIXUP:         recname = "FIXUPP";             break;
        case CMD_LEDATA:        recname = "LEDATA";             break;
        case CMD_LIDATA:        recname = "LIDATA";             break;
        case CMD_LIBHED:        recname = "LIBHED";             break;
        case CMD_LIBNAM:        recname = "LIBNAM";             break;
        case CMD_LIBLOC:        recname = "LIBLOC";             break;
        case CMD_LIBDIC:        recname = "LIBDIC";             break;
        case CMD_COMDEF:        recname = "COMDEF";             break;
        case CMD_STATIC_EXTDEF: recname = "static EXTDEF";      break;
        case CMD_STATIC_PUBDEF: recname = "static PUBDEF";      break;
        case CMD_STATIC_COMDEF: recname = "static COMDEF";      break;
        case CMD_BAKPAT:        recname = "BAKPAT";             break;
        case CMD_CEXTDF:        recname = "CEXTDF";             break;
        case CMD_COMDAT:        recname = "COMDAT";             break;
        case CMD_LINSYM:        recname = "LINSYM";             break;
        case CMD_ALIAS:         recname = "ALIAS";              break;
        case CMD_NBKPAT:        recname = "NBKPAT";             break;
        case CMD_LLNAME:        recname = "LLNAME";             break;
        case LIB_HEADER_REC:
                /* this is the oddball in the MS386 format */
            if( IsMS386 ) {
                IsMS386 = 0;
                recname = "LIBTAIL";
            } else {
                recname = "LIBHEAD";
            }
            break;
        default:                recname = "**??**";             break;
        }
        cksum = -( cksum - RecBuff[ RecLen - 1 ] );
        Output( CRLF "%s%s(%2) recnum:%u, offset:%X, len:%x, chksum:%b(%2)" CRLF,
            recname, IsMS386 ? "386" : "", hdr[ 0 ], ++RecNum, offset,
            RecLen, RecBuff[ RecLen - 1 ], cksum );
        RecLen--;
        if( setjmp( BailOutJmp ) == 0 ) {
            switch( hdr[ 0 ] & ~1 ) {
            case CMD_RHEADR:    ProcRHeadr();                   break;
            case CMD_ENDREC:    ProcEndRec();                   break;
            case CMD_THEADR:    ProcTHeadr();                   break;
            case CMD_LHEADR:    ProcLHeadr();                   break;
            case CMD_COMENT:    ProcComent();                   break;
            case CMD_MODEND:
                ProcModEnd();
                if( page_len != 0 ) {
                    offset = ftell( fp );
                    offset = page_len - offset % page_len;
                    if( offset != page_len ) {
                        total_padding += offset;
                        fseek( fp, offset, SEEK_CUR );
                    }
                }
                break;
            case CMD_STATIC_EXTDEF: /* fall through */
            case CMD_EXTDEF:    ProcExtNames();                 break;
            case CMD_STATIC_PUBDEF: /* fall through */
            case CMD_PUBDEF:    ProcPubDefs();                  break;
            case CMD_LOCSYM:    ProcLocSyms();                  break;
            case CMD_LINNUM:    ProcLinNums();                  break;
            case CMD_LLNAME:        /* fall through */
            case CMD_LNAMES:    ProcNames( &Nameindex );        break;
            case CMD_SEGDEF:    ProcSegDefs();                  break;
            case CMD_GRPDEF:    ProcGrpDef();                   break;
            case CMD_FIXUP:     ProcFixup();                    break;
            case CMD_LEDATA:    ProcLedata();                   break;
            case CMD_LIDATA:    ProcLidata();                   break;
            case CMD_LIBNAM:    ProcNames( &Libindex );         break;
            case CMD_STATIC_COMDEF: /* fall through */
            case CMD_COMDEF:    ProcComDef();                   break;
            case CMD_BAKPAT:    ProcBackPat();                  break;
            case CMD_CEXTDF:    ProcComExtDef();                break;
            case CMD_COMDAT:    ProcComDat();                   break;
            case CMD_LINSYM:    ProcLineSym();                  break;
            case CMD_ALIAS:     ProcAlias();                    break;
            case CMD_NBKPAT:    ProcNameBackPat();              break;
            case CMD_VERNUM:    ProcVerNum();                   break;
            case CMD_VENDEXT:   ProcVendExt();                  break;
            case LIB_HEADER_REC:
                if( hdr[ 0 ] & 1 ) {
                    /* LIB_TRAILER_REC */
                    ProcLibTrailer( fp );
                    fseek( fp, 0L, SEEK_END );
                    page_len = 0;
                } else {
                    page_len = RecLen + 4;
                    ProcLibHeader();
                }
                break;
            default:
                if( !raw_dump ) {
                    OutputData( 0L, 0L );
                }
                break;
            }
        } else {
            /* something bailed out... */
            if( raw_dump ) {
                Output( INDENT "Error at offset %x" CRLF,
                        (unsigned_32)RecOffset );
            } else if( !EndRec() ) {
                Output( INDENT "Remainder of record follows:" CRLF );
                OutputData( (unsigned_32)RecOffset(), 0L );
            } else {
                Output( INDENT "End of record" CRLF );
            }
        }
        if( raw_dump ) {
            RecPtr = RecBuff;
            Output(
"====================RAW DUMP==============================================="
                CRLF );
            OutputData( 0L, 0L );
            Output(
"====================RAW DUMP==============================================="
                CRLF );
        }
    }
    if( total_padding > 0 ) {
        Output( CRLF "total padding=%X" CRLF, total_padding );
    }
    free( RecBuff );
}
