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
* Description:  OMF file read functions.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>

#include "dmpobj.h"

data_ptr    NamePtr;
byte        NameLen;
unsigned_16 RecLen;
data_ptr    RecBuff;
data_ptr    RecPtr;
unsigned_16 RecMaxLen;
unsigned    RecNum;
unsigned_16 Grpindex;
unsigned_16 Segindex;
unsigned_16 Nameindex;
unsigned_16 Importindex;
unsigned_16 Libindex;
bool        IsPharLap;
bool        IsMS386;
bool        IsIntel;
bool        DumpRaw;
jmp_buf     BailOutJmp;
byte        rec_type[10] = { 0 };
int         rec_count = 0;
Lnamelist   *Lnames;
Lnamelist   *lastLname;
Lnamelist   *Xnames;
Lnamelist   *lastXname;
Grpdeflist  *Grpdefs;
Segdeflist  *Segdefs;

const char *RecNumberToName( byte code )
{
    switch( code & ~1 ) {
    case CMD_RHEADR:
        return( "RHEADR" );
    case CMD_REGINT:
        return( "REGINT" );
    case CMD_REDATA:
        return( "REDATA" );
    case CMD_RIDATA:
        return( "RIDATA" );
    case CMD_OVLDEF:
        return( "OVLDEF" );
    case CMD_ENDREC:
        return( "ENDREC" );
    case CMD_BLKDEF:
        return( "BLKDEF" );
    case CMD_BLKEND:
        return( "BLKEND" );
    case CMD_DEBSYM:
        return( "DEBSYM" );
    case CMD_THEADR:
        return( "THEADR" );
    case CMD_LHEADR:
        return( "LHEADR" );
    case CMD_PEDATA:
        return( "PEDATA" );
    case CMD_PIDATA:
        return( "PIDATA" );
    case CMD_COMENT:
        return( "COMENT" );
    case CMD_MODEND:
        return( "MODEND" );
    case CMD_EXTDEF:
        return( "EXTDEF" );
    case CMD_TYPDEF:
        return( "TYPDEF" );
    case CMD_PUBDEF:
        return( "PUBDEF" );
    case CMD_LOCSYM:
        return( "LOCSYM" );
    case CMD_LINNUM:
        return( "LINNUM" );
    case CMD_LNAMES:
        return( "LNAMES" );
    case CMD_SEGDEF:
        return( "SEGDEF" );
    case CMD_GRPDEF:
        return( "GRPDEF" );
    case CMD_FIXUP:
         return( "FIXUPP" );
    case CMD_LEDATA:
        return( "LEDATA" );
    case CMD_LIDATA:
        return( "LIDATA" );
    case CMD_LIBHED:
        return( "LIBHED" );
    case CMD_LIBNAM:
        return( "LIBNAM" );
    case CMD_LIBLOC:
        return( "LIBLOC" );
    case CMD_LIBDIC:
        return( "LIBDIC" );
    case CMD_COMDEF:
        return( "COMDEF" );
    case CMD_STATIC_EXTDEF:
        return( "static EXTDEF" );
    case CMD_STATIC_PUBDEF:
        return( "static PUBDEF" );
    case CMD_STATIC_COMDEF:
        return( "static COMDEF" );
    case CMD_BAKPAT:
        return( "BAKPAT" );
    case CMD_CEXTDF:
        return( "CEXTDF" );
    case CMD_COMDAT:
        return( "COMDAT" );
    case CMD_LINSYM:
        return( "LINSYM" );
    case CMD_ALIAS:
        return( "ALIAS" );
    case CMD_NBKPAT:
        return( "NBKPAT" );
    case CMD_LLNAME:
        return( "LLNAME" );
    case LIB_HEADER_REC:
        /* this is the oddball in the MS386 format */
        if( IsMS386 )
            IsMS386 = FALSE;
        if( code & 1 ) {
            return( "LIBTAIL" );
        } else {
            return( "LIBHEAD" );
        }
    default:
        return( "**??**" );
    }
}

byte RecNameToNumber( char *name )
{
    if( strnicmp( name, "RHEADR", 6 ) == 0 ) {
        return( CMD_RHEADR );
    } else if( strnicmp( name, "REGINT", 6 ) == 0 ) {
        return( CMD_REGINT );
    } else if( strnicmp( name, "REDATA", 6 ) == 0 ) {
        return( CMD_REDATA );
    } else if( strnicmp( name, "RIDATA", 6 ) == 0 ) {
        return( CMD_RIDATA );
    } else if( strnicmp( name, "OVLDEF", 6 ) == 0 ) {
        return( CMD_OVLDEF );
    } else if( strnicmp( name, "ENDREC", 6 ) == 0 ) {
        return( CMD_ENDREC );
    } else if( strnicmp( name, "BLKDEF", 6 ) == 0 ) {
        return( CMD_BLKDEF );
    } else if( strnicmp( name, "BLKEND", 6 ) == 0 ) {
        return( CMD_BLKEND );
    } else if( strnicmp( name, "DEBSYM", 6 ) == 0 ) {
        return( CMD_DEBSYM );
    } else if( strnicmp( name, "THEADR", 6 ) == 0 ) {
        return( CMD_THEADR );
    } else if( strnicmp( name, "LHEADR", 6 ) == 0 ) {
        return( CMD_LHEADR );
    } else if( strnicmp( name, "PEDATA", 6 ) == 0 ) {
        return( CMD_PEDATA );
    } else if( strnicmp( name, "PIDATA", 6 ) == 0 ) {
        return( CMD_PIDATA );
    } else if( strnicmp( name, "COMENT", 6 ) == 0 ) {
        return( CMD_COMENT );
    } else if( strnicmp( name, "MODEND", 6 ) == 0 ) {
        return( CMD_MODEND );
    } else if( strnicmp( name, "EXTDEF", 6 ) == 0 ) {
        return( CMD_EXTDEF );
    } else if( strnicmp( name, "TYPDEF", 6 ) == 0 ) {
        return( CMD_TYPDEF );
    } else if( strnicmp( name, "PUBDEF", 6 ) == 0 ) {
        return( CMD_PUBDEF );
    } else if( strnicmp( name, "LOCSYM", 6 ) == 0 ) {
        return( CMD_LOCSYM );
    } else if( strnicmp( name, "LINNUM", 6 ) == 0 ) {
        return( CMD_LINNUM );
    } else if( strnicmp( name, "LNAMES", 6 ) == 0 ) {
        return( CMD_LNAMES );
    } else if( strnicmp( name, "SEGDEF", 6 ) == 0 ) {
        return( CMD_SEGDEF );
    } else if( strnicmp( name, "GRPDEF", 6 ) == 0 ) {
        return( CMD_GRPDEF );
    } else if( strnicmp( name, "FIXUPP", 6 ) == 0 ) {
        return( CMD_FIXUP );
    } else if( strnicmp( name, "LEDATA", 6 ) == 0 ) {
        return( CMD_LEDATA );
    } else if( strnicmp( name, "LIDATA", 6 ) == 0 ) {
        return( CMD_LIDATA );
    } else if( strnicmp( name, "LIBHED", 6 ) == 0 ) {
        return( CMD_LIBHED );
    } else if( strnicmp( name, "LIBNAM", 6 ) == 0 ) {
        return( CMD_LIBNAM );
    } else if( strnicmp( name, "LIBLOC", 6 ) == 0 ) {
        return( CMD_LIBLOC );
    } else if( strnicmp( name, "LIBDIC", 6 ) == 0 ) {
        return( CMD_LIBDIC );
    } else if( strnicmp( name, "COMDEF", 6 ) == 0 ) {
        return( CMD_COMDEF );
    } else if( strnicmp( name, "BAKPAT", 6 ) == 0 ) {
        return( CMD_BAKPAT );
    } else if( strnicmp( name, "CEXTDF", 6 ) == 0 ) {
        return( CMD_CEXTDF );
    } else if( strnicmp( name, "COMDAT", 6 ) == 0 ) {
        return( CMD_COMDAT );
    } else if( strnicmp( name, "LINSYM", 6 ) == 0 ) {
        return( CMD_LINSYM );
    } else if( strnicmp( name, "ALIAS", 5 ) == 0 ) {
        return( CMD_ALIAS );
    } else if( strnicmp( name, "NBKPAT", 6 ) == 0 ) {
        return( CMD_NBKPAT );
    } else if( strnicmp( name, "LLNAME", 6 ) == 0 ) {
        return( CMD_LLNAME );
    } else if( strnicmp( name, "STATIC_EXTDEF", 13 ) == 0 ) {
        return( CMD_STATIC_EXTDEF );
    } else if( strnicmp( name, "STATIC_PUBDEF", 13 ) == 0 ) {
        return( CMD_STATIC_PUBDEF );
    } else if( strnicmp( name, "STATIC_COMDEF", 13 ) == 0 ) {
        return( CMD_STATIC_COMDEF );
    } else if( strnicmp( name, "LIBTAIL", 7 ) == 0 ) {
        return( LIB_HEADER_REC );
    } else if( strnicmp( name, "LIBHEAD", 7 ) == 0 ) {
        return( LIB_HEADER_REC );
    } else {
        return( 0 );
    }
}

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

unsigned_16 GetUInt( void )
/*************************/
{
    unsigned_16 word;

    word = *(unsigned_16 *)RecPtr;
    CONV_LE_16( word );
    RecPtr += 2;
    return( word );
}

unsigned_32 GetLInt( void )
/*************************/
{
    unsigned_32 dword;

    dword = *(unsigned_32 *)RecPtr;
    CONV_LE_32( dword );
    RecPtr += 4;
    return( dword );
}

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

byte GetName( void )
/******************/
{
    NameLen = GetByte();
    NamePtr = RecPtr;
    RecPtr += NameLen;
    return( NameLen );
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
        size = lo + ( (unsigned_32)GetByte() << 16 );
    } else if( index == COMDEF_LEAF_4 ) {
        size = GetLInt();
    } else {
        Output( BAILOUT "Unknown COMDEF LEAF size (%b)" CRLF, index );
        longjmp( BailOutJmp, 1 );
    }
    return( size );
}


#if defined(__WATCOMC__)
    #if defined( _M_I86SM ) || defined( _M_I86MM )
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
    #elif defined( _M_I86 )
        extern byte docksum( byte __far *buf, unsigned_16 len );
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

void AddLname( void ) {
/*********************/
    Lnamelist   *entry;

    entry = malloc( sizeof(Lnamelist) + NameLen );
    if( entry == NULL ) {
        OutputSetFH( stdout );
        Output( CRLF "**FATAL** Out of memory!" CRLF );
        leave( 21 );
    }
    if( Lnames == NULL ) {
        Lnames = entry;
    } else {
        lastLname->next = entry;
    }
    entry->next = NULL;
    entry->LnameLen = NameLen;
    memcpy( &(entry->Lname), NamePtr, NameLen );
    *(&(entry->Lname) + NameLen) = '\0';
    lastLname = entry;
}

char *GetLname( unsigned_16 idx ) {
/*********************************/
    Lnamelist   *entry;
    char        *name;
    int         k;

    if( (Lnames == NULL) || (idx == 0) ) {
        return( "" );
    }
    entry = Lnames;
    for( k = 1; k < idx; ++k) {
        entry = entry->next;
        if( entry == NULL ) {
            return( "" ); /* not found */
        }
    }
    name = (char *)&(entry->Lname);
    return( name );
}

static void FreeLnames( void ) {
/*******************************/
    Lnamelist   *entry;

    entry = Lnames;
    for( ;; ) {
        if( entry == NULL ) {
           break;
        }
        Lnames = entry->next;
        free( entry );
        entry = Lnames;
    }
    Lnames = NULL;

}

static void PrintNames( void ) {
/******************************/
    Lnamelist   *entry;
    unsigned_16 k;
    size_t      col;

    if( Lnames != NULL ) {
        k = 0;
        col = Output( CRLF INDENT INDENT "List of Lnames:" CRLF );
        entry = Lnames;
        for( ;; ) {
           if( entry == NULL ) {
               break;
           }
           if( ( col > 40 ) || (entry->LnameLen > 40 ) ) {
               col = Output( CRLF );
           }
           k++;
           col = Output( "%u - '%s' %<", k, &(entry->Lname), 39u );
           entry = entry->next;
       }
       Output( CRLF );
    }

    if( Xnames != NULL ) {
        k = 0;
        col = Output( CRLF INDENT INDENT "List of Xnames:" CRLF );
        entry = Xnames;
        for( ;; ) {
           if( entry == NULL ) {
               break;
           }
           if( ( col > 40 ) || (entry->LnameLen > 40 ) ) {
               col = Output( CRLF );
           }
           k++;
           col = Output( "%u - '%s' %<", k, &(entry->Lname), 39u );
           entry = entry->next;
       }
       Output( CRLF );
    }
}

void AddXname( void ) {
/*********************/
    Lnamelist   *entry;

    entry = malloc( sizeof(Lnamelist) + NameLen );
    if( entry == NULL ) {
        OutputSetFH( stdout );
        Output( CRLF "**FATAL** Out of memory!" CRLF );
        leave( 21 );
    }
    if( Xnames == NULL ) {
        Xnames = entry;
    } else {
        lastXname->next = entry;
    }
    entry->next = NULL;
    entry->LnameLen = NameLen;
    memcpy( &(entry->Lname), NamePtr, NameLen );
    *(&(entry->Lname) + NameLen) = '\0';
    lastXname = entry;
}

char *GetXname( unsigned_16 idx ) {
/*********************************/
    Lnamelist   *entry;
    char        *name;
    int         k;

    if( (Xnames == NULL) || (idx == 0) ) {
        return( "" );
    }
    entry = Xnames;
    for( k = 1; k < idx; ++k) {
        entry = entry->next;
        if( entry == NULL ) {
            return( "" ); /* not found */
        }
    }
    name = (char *)&(entry->Lname);
    return( name );
}

static void FreeXnames( void ) {
/*******************************/
    Lnamelist   *entry;

    entry = Xnames;
    for( ;; ) {
        if( entry == NULL ) {
           break;
        }
        Xnames = entry->next;
        free( entry );
        entry = Xnames;
    }
    Xnames = NULL;

}

void AddSegdef( unsigned_16 idx ) {
/*********************************/
    Segdeflist  *entry;
    Segdeflist  *wkentry;

    entry = malloc( sizeof(Segdeflist) );
    if( entry == NULL ) {
        OutputSetFH( stdout );
        Output( CRLF "**FATAL** Out of memory!" CRLF );
        leave( 21 );
    }
    if( Segdefs == NULL ) {
        Segdefs = entry;
    } else {
        wkentry = Segdefs;
        for ( ;; ) {
            if( wkentry->next == NULL ) {
                break;
            }
            wkentry = wkentry->next;
        }
        wkentry->next = entry;
    }
    entry->next = NULL;
    entry->segind = idx;
}

Segdeflist  *GetSegdef( unsigned_16 idx ) {
/*****************************************/
    Segdeflist  *entry;
    int         k;

    if( (Segdefs == NULL) || (idx == 0) ) {
        return( NULL );
    }
    entry = Segdefs;
    for( k = 1; k < idx; ++k) {
        entry = entry->next;
        if( entry == NULL ) {
            return( NULL ); /* not found */
        }
    }
    return( entry );
}

static void FreeSegdefs( void ) {
/*******************************/
    Segdeflist   *entry;

    entry = Segdefs;
    for( ;; ) {
        if( entry == NULL ) {
           break;
        }
        Segdefs = entry->next;
        free( entry );
        entry = Segdefs;
    }
    Segdefs = NULL;
}


void AddGrpdef( unsigned_16 grpidx, unsigned_16 segidx ) {
/********************************************************/
    Grpdeflist  *entry;
    Grpdeflist  *wkentry;
    int         k;

    if( segidx == 0 ) { /* start new grpdef */
        entry = malloc( sizeof(Grpdeflist) );
        if( entry == NULL ) {
            OutputSetFH( stdout );
            Output( CRLF "**FATAL** Out of memory!" CRLF );
            leave( 21 );
        }
        entry->next = NULL;
        entry->grpind = grpidx;
        for( k = 0; k < MAXGRPSEGS; ++k ) {
            entry->segidx[ k ] = 0; /* no members yet */
        }
        if( Grpdefs == NULL ) {
            Grpdefs = entry;
        } else {
            wkentry = Grpdefs;
            for ( ;; ) {
                if( wkentry->next == NULL ) {
                    break;
                }
                wkentry = wkentry->next;
            }
            wkentry->next = entry;
        }
    } else {               /* add member to grp*/
        if( Grpdefs == NULL ) {
            OutputSetFH( stdout );
            Output( CRLF "**FATAL** No grpdef entry!" CRLF );
            leave( 21 );
        } else {
            wkentry = Grpdefs;
            for ( ;; ) {
                if( wkentry->next == NULL ) {
                    break;
                }
                wkentry = wkentry->next;
            }
            for( k = 0; k < MAXGRPSEGS; ++k ) {
                if( wkentry->segidx[ k ] == 0 ) {
                    break;
                }
            }
            if( k < MAXGRPSEGS ) {
                wkentry->segidx[ k ] = segidx;
            }

        }
    }
}

Grpdeflist *GetGrpdef( unsigned_16 idx ) {
/****************************************/
    Grpdeflist  *entry;
    int         k;

    if( (Grpdefs == NULL) || (idx == 0) ) {
        return( NULL );
    }
    entry = Grpdefs;
    for( k = 1; k < idx; ++k) {
        entry = entry->next;
        if( entry == NULL ) {
            return( NULL ); /* not found */
        }
    }
    return( entry );
}

unsigned_16 GetGrpseg( unsigned_16 idx ) {
/****************************************/
    Segdeflist  *entry;

    if( (Grpdefs == NULL) || (idx == 0) ) {
        return( 0 );
    }
    entry = GetSegdef( idx );
    if( entry == NULL ) {
        return( 0 );
    } else {
       return( entry->segind );
    }
}

static void FreeGrpdefs( void ) {
/*******************************/
    Grpdeflist   *entry;

    entry = Grpdefs;
    for( ;; ) {
        if( entry == NULL ) {
           break;
        }
        Grpdefs = entry->next;
        free( entry );
        entry = Grpdefs;
    }
    Grpdefs = NULL;
}


void ProcFile( FILE *fp, bool is_intel )
/**************************************/
{
    byte        cksum;
    byte        hdr[ 3 ];
    unsigned_16 page_len;
    unsigned_32 offset;
    const char  *recname;
    unsigned_32 total_padding;
    int         raw_dump;
    int         i;
    int         first;

    IsPharLap = FALSE;
    IsMS386 = FALSE;
    IsIntel = is_intel;
    RecNum = 0;
    page_len = 0;
    RecBuff = NULL;
    RecMaxLen = 0;
    total_padding = 0;
    Lnames = NULL;
    Xnames = NULL;
    Segdefs = NULL;
    Grpdefs = NULL;
    first = 1;
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
        no_disp = ( rec_count == 0 ) ? FALSE : TRUE;
        for( i = 0; i < rec_count; i++ ) {
            if( rec_type[ i ] == ( hdr[ 0 ] & ~1 )) {
                no_disp = FALSE;
                break;
            }
        }
        recname = RecNumberToName( hdr[ 0 ] );
        cksum = -( cksum - RecBuff[ RecLen - 1 ] );
        Output( CRLF "%s%s(%2) recnum:%u, offset:%X, len:%x, chksum:%b(%2)" CRLF,
            recname, IsMS386 ? "386" : "", hdr[ 0 ], ++RecNum, offset,
            RecLen, RecBuff[ RecLen - 1 ], cksum );
        RecLen--;
        if( setjmp( BailOutJmp ) == 0 ) {
            switch( hdr[ 0 ] & ~1 ) {
            case CMD_RHEADR:
                ProcRHeadr();
                break;
            case CMD_ENDREC:
                ProcEndRec();
                break;
            case CMD_THEADR:
                ProcTHeadr( first );
                first = 0;
                break;
            case CMD_LHEADR:
                ProcLHeadr();
                break;
            case CMD_COMENT:
                ProcComent();
                break;
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
                first = 1;
                break;
            case CMD_STATIC_EXTDEF:
                /* fall through */
            case CMD_EXTDEF:
                ProcExtNames();
                break;
            case CMD_STATIC_PUBDEF:
                /* fall through */
            case CMD_PUBDEF:
                ProcPubDefs();
                break;
            case CMD_LOCSYM:
                ProcLocSyms();
                break;
            case CMD_LINNUM:
                ProcLinNums();
                break;
            case CMD_LLNAME:
                /* fall through */
            case CMD_LNAMES:
                ProcLNames( &Nameindex );
                break;
            case CMD_SEGDEF:
                ProcSegDefs();
                break;
            case CMD_GRPDEF:
                ProcGrpDef();
                break;
            case CMD_FIXUP:
                ProcFixup();
                break;
            case CMD_LEDATA:
                ProcLedata();
                break;
            case CMD_LIDATA:
                ProcLidata();
                break;
            case CMD_LIBNAM:
                ProcNames( &Libindex );
                break;
            case CMD_STATIC_COMDEF:
                /* fall through */
            case CMD_COMDEF:
                ProcComDef();
                break;
            case CMD_BAKPAT:
                ProcBackPat();
                break;
            case CMD_CEXTDF:
                ProcComExtDef();
                break;
            case CMD_COMDAT:
                ProcComDat();
                break;
            case CMD_LINSYM:
                ProcLineSym();
                break;
            case CMD_ALIAS:
                ProcAlias();
                break;
            case CMD_NBKPAT:
                ProcNameBackPat();
                break;
            case CMD_VERNUM:
                ProcVerNum();
                break;
            case CMD_VENDEXT:
                ProcVendExt();
                break;
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
    if( TranslateIndex) {
        PrintNames();
    }
    FreeGrpdefs();
    FreeSegdefs();
    FreeLnames();
    FreeXnames();
    free( RecBuff );
}
