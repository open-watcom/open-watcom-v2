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


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pcobj.h"
#include "omfload.h"
#include "omfmunge.h"

/* Local definitions
 */
#define MAX_REC_SIZE    65536
#define TWO_BYTE_MASK   0x80


static void             setInitialData( omf_file_handle ofh )
{
    assert( ofh );

    /* Default will be 16-bit code
     */
    ofh->machine_type = ORL_MACHINE_TYPE_I8086;
    ofh->type = ORL_FILE_TYPE_OBJECT;
    ofh->flags = 0;
    ofh->flags |= ORL_FILE_FLAG_LITTLE_ENDIAN;
    ofh->flags |= ORL_FILE_FLAG_16BIT_MACHINE;
}


int                     OmfGetWordSize( int is32 )
{
    if( is32 ) {
        return( 4 );
    } else {
        return( 2 );
    }
}


static orl_sec_offset   getUWord( omf_bytes buffer, int wordsize )
{
    orl_sec_offset      result = 0;

    assert( buffer );

    switch( wordsize ) {
    case( 4 ):
        result |= buffer[3] << 24;
        result |= buffer[2] << 16;
    case( 2 ):
        result |= buffer[1] << 8;
    default:
        result |= buffer[0];
    }
    return( result );
}


static int              check32Bit( omf_file_handle ofh, omf_rectyp typ )
{
    int is32;

    is32 = _Is32BitRec( typ ) || ( ofh->status & OMF_STATUS_EASY_OMF );
    return( is32 );
}


static orl_sec_alignment getAlignment( int val )
{
    switch( val ) {
    case( ALIGN_ABS ):          /* absolute segment - no alignment      */
    case( ALIGN_BYTE ):         /* relocatable seg - byte aligned       */
        return( 0 );
    case( ALIGN_WORD ):         /* relocatable seg - word aligned       */
        return( 1 );
    case( ALIGN_PARA ):         /* relocatable seg - para aligned       */
        return( 4 );
    case( ALIGN_PAGE ):         /* relocatable seg - page aligned       */
        return( 8 );
    case( ALIGN_DWORD ):        /* relocatable seg - dword aligned      */
                                /* 32-bit ALIGN_UNABS unnamed absolute  *
                                 * segment, this is currently not       *
                                 * supported by the linker.             */
        return( 2 );
    case( ALIGN_LTRELOC ):      /* load-time relocatable segment        */
                                /* 32-bit ALIGN_4KPAGE relocatable      *
                                 * seg - 4k page aligned                */
        return( 12 );
    default:
        assert( 0 );
        return( 0 );
    }
}


static orl_return       loadRecord( omf_file_handle ofh )
{
    long                len;
    omf_bytes           buff;
    uint_8              record_chksum;
    uint_8              chksum;

    assert( ofh );

    buff = _ClientRead( ofh, 2 );
    if( !buff ) return( ORL_ERROR );
    len = getUWord( buff, 2 );
    if( len <= 0 ) return( ORL_ERROR );
    ofh->parsebuf = _ClientRead( ofh, len );
    if( !ofh->parsebuf ) return( ORL_ERROR );
    ofh->parselen = len - 1;

    record_chksum = ofh->parsebuf[ ofh->parselen ];
    // some slob compilers put out 0 for all chksum's, so
    // only check if it is non-zero
    if( record_chksum != 0 ) {
        chksum = ofh->last_rec + buff[0] + buff[1];
        while( len ) {
            len--;
            chksum += ofh->parsebuf[len];
        }

        if( chksum ) return( ORL_ERROR );
    }
    return( ORL_OKAY );
}


omf_idx loadIndex( omf_bytes *buffer, long *len )
{
    omf_bytes           deref;
    long                pos;
    omf_idx             idx;

    assert( buffer );
    assert( *buffer );
    assert( len );

    deref = *buffer;
    pos = *len;

    if( deref[0] & TWO_BYTE_MASK ) {
        idx = ( ( deref[0] & 0x7f ) << 8 ) | deref[1];
        deref += 2;
        pos -= 2;
    } else {
        idx = deref[0];
        deref++;
        pos--;
    }
    *len = pos;
    *buffer = deref;
    return( idx );
}


static orl_return       processExplicitFixup( omf_file_handle ofh, int is32,
                                            omf_bytes *buffer, long *cur )
{
    omf_bytes           buf;
    long                len;
    int                 m;
    int                 location;
    int                 offset;
    int                 fmethod;
    omf_idx             fidx;
    int                 thred;
    int                 tmethod;
    omf_idx             tidx;
    uint_8              datum;
    int                 wordsize;
    orl_sec_offset      displacement;

    assert( ofh );
    assert( buffer );
    assert( *buffer );
    assert( cur );
    assert( *cur > 0 );
    assert( **buffer & 0x80 );

    wordsize = OmfGetWordSize( is32 );

    buf = *buffer;
    len = *cur;

    datum = buf[0];
    m = ( datum & 0x40 ) != 0;
    location = ( datum >> 2 ) & 0x0f;
    offset = ( ( datum & 0x03 ) << 8 ) | buf[1];
    datum = buf[2];
    buf += 3;
    len -= 3;

    if( 0x80 & datum ) {
        thred = ( datum >> 4 ) & 0x03;
        fmethod = ofh->frame_thred[thred].method;
        fidx = ofh->frame_thred[thred].idx;
    } else {
        fmethod = ( datum >> 4 ) & 0x07;
        switch( fmethod ) {
        case( F_SEG ):                  /* segment index                */
        case( F_GRP ):                  /* group index                  */
        case( F_EXT ):                  /* external index               */
            fidx = loadIndex( &buf, &len );
            break;
        case( F_LOC ):                  /* frame containing location    */
        case( F_ABS ):                  /* absolute frame number        */
        case( F_TARG ):                 /* frame same as target         */
        case( F_NONE ):                 /* no frame                     */
            fidx = 0;
            break;
        }
    }

    if( 0x08 & datum ) {
        thred = datum & 0x03;
        tmethod = ofh->target_thred[thred].method;
        tidx = ofh->target_thred[thred].idx;
    } else {
        tmethod = datum & 0x07;
        tidx = loadIndex( &buf, &len );
        if( fmethod == F_TARG ) {
            /* fmethod becomes the same as tmethod (1 of first 3)
             */
            fmethod = tmethod & 0x03;
            fidx = tidx;
        }
    }

    if( datum & 0x04 ) {
        displacement = 0;
    } else {
        displacement = getUWord( buf, wordsize );
        buf += wordsize;
        len -= wordsize;
    }

    *buffer = buf;
    *cur = len;
    return( OmfAddFixupp( ofh, is32, m, location, offset, fmethod, fidx,
                          tmethod, tidx, displacement ) );
}


static orl_return       processThreadFixup( omf_file_handle ofh,
                                            omf_bytes *buffer, long *cur )
{
    omf_bytes           buf;
    long                len;
    int                 d;
    int                 method;
    int                 thred;
    uint_8              datum;
    omf_thred_fixup     *thredp;

    assert( ofh );
    assert( buffer );
    assert( *buffer );
    assert( cur );
    assert( *cur > 0 );
    assert( !( **buffer & 0x80 ) );

    buf = *buffer;
    len = *cur;

    datum = buf[0];
    d = ( datum & 0x40 ) != 0;
    method = ( datum >> 2 ) & 0x07;
    thred = datum & 0x03;
    buf++;
    len--;

    if( d ) {
        thredp = &ofh->frame_thred[thred];
    } else {
        thredp = &ofh->target_thred[thred];
    }

    thredp->method = method;

    /* If Index expected
     */
    if( !d || ( method < 3 ) ) {
        thredp->idx = loadIndex( &buf, &len );
    }

    *buffer = buf;
    *cur = len;
    return( ORL_OKAY );
}


static orl_return       doTHEADR( omf_file_handle ofh )
{
    orl_return  err;
    int         len;

    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );
    if( ofh->modnamelen ) return( ORL_ERROR );
    len = ofh->parsebuf[0];
    if( len > ofh->parselen ) return( ORL_ERROR );
    memcpy( ofh->modname, ofh->parsebuf + 1, len );
    ofh->modname[len] = 0;
    ofh->modnamelen = len;
    return( ORL_OKAY );
}


static orl_return       doCOMENT( omf_file_handle ofh )
{
    orl_return          err;
    uint_8              class;
    uint_8              flags;
    omf_bytes           buffer;
    unsigned int        len;

    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );

    buffer = ofh->parsebuf;
    len = ofh->parselen;
    flags = buffer[0];
    class = buffer[1];
    buffer += 2;
    if( len < 2 ) return( ORL_ERROR );
    len -= 2;

    err = OmfAddComment( ofh, class, flags, buffer, len );
    if( err != ORL_OKAY ) return( err );

    switch( class ) {
    case( CMT_WAT_PROC_MODEL ):
    case( CMT_MS_PROC_MODEL ):
        /* Determine CPU
         */
        if( !len ) break;
        switch( *buffer ) {
        case( '2' ):
        case( '0' ):
            /* 16 bit code
             */
            ofh->machine_type = ORL_MACHINE_TYPE_I8086;
            _SetWordSize( ofh->flags, ORL_FILE_FLAG_16BIT_MACHINE );
            break;
        case( '3' ):
        default:
            ofh->machine_type = ORL_MACHINE_TYPE_I386;
            _SetWordSize( ofh->flags, ORL_FILE_FLAG_32BIT_MACHINE );
            break;
        }

        /* we do not use any of the other info, so we don't look at it.
         * see dmpobj source code for format of this coment record
         */
        break;
    case( CMT_EASY_OMF ):
        /* Pharlap Sucks!!!!
         * Their weird object files forces us to do this crap
         */
        if( ( flags == CMT_TNP ) && !memcmp( buffer, EASY_OMF_SIGNATURE, 5 ) ) {
            ofh->status |= OMF_STATUS_EASY_OMF;
            ofh->machine_type = ORL_MACHINE_TYPE_I386;
            _SetWordSize( ofh->flags, ORL_FILE_FLAG_32BIT_MACHINE );
            ofh->status |= OMF_STATUS_ARCH_SET;
        }
        break;
    }
    return( err );
}


static orl_return       doMODEND( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return  err;

    typ = typ;
    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );
    ofh->status |= OMF_STATUS_FILE_LOADED;
    return( OmfModEnd( ofh ) );
}


static orl_return       doEXTDEF( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          err;
    omf_bytes           buffer;
    long                len;
    int                 slen;

    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );
    len = ofh->parselen;
    buffer = ofh->parsebuf;

    while( len ) {
        slen = buffer[0];
        buffer++;
        len--;
        if( slen > len ) return( ORL_ERROR );
        if( slen > 0 ) {
            err = OmfAddExtDef( ofh, buffer, slen, typ );
            if( err != ORL_OKAY ) break;
        }
        len -= slen;
        buffer += slen;
        loadIndex( &buffer, &len );
    }

    return( err );
}


static orl_return       doCEXTDEF( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          err;
    omf_bytes           buffer;
    long                len;
    int                 slen;
    omf_idx             idx;
    char                name[257];

    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );
    len = ofh->parselen;
    buffer = ofh->parsebuf;

    while( len ) {
        if( len < 2 ) return( ORL_ERROR );
        idx = loadIndex( &buffer, &len );
        loadIndex( &buffer, &len );
        slen = OmfGetLName( ofh->lnames, idx, name );
        if( slen < 0 ) return( ORL_ERROR );
        err = OmfAddExtDef( ofh, name, slen, typ );
        if( err != ORL_OKAY ) break;
    }
    return( err );
}


static orl_return       doCOMDEF( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          err;
    omf_bytes           buffer;
    long                len;
    int                 slen;
    int                 dec;

    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );
    len = ofh->parselen;
    buffer = ofh->parsebuf;

    while( len ) {
        slen = buffer[0];
        buffer++;
        len--;
        if( slen > len ) return( ORL_ERROR );
        if( slen > 0 ) {
            err = OmfAddExtDef( ofh, buffer, slen, typ );
            if( err != ORL_OKAY ) break;
        }
        len -= slen;
        buffer += slen;
        loadIndex( &buffer, &len );
        switch( buffer[0] ) {
        case( COMDEF_NEAR ):
            slen = 1;
            break;
        case( COMDEF_FAR ):
            slen = 2;
            break;
        default:
            return( ORL_ERROR );
        }
        buffer++;
        len--;
        while( slen > 0 ) {
            dec = 1;
            if( len < 1 ) return( ORL_ERROR );
            if( buffer[0] & COMDEF_LEAF_SIZE ) {
                switch( buffer[0] ) {
                case( COMDEF_LEAF_4 ):  dec++;
                case( COMDEF_LEAF_3 ):  dec++;
                case( COMDEF_LEAF_2 ):  dec += 2;
                    break;
                default:
                    return( ORL_ERROR );
                }
            }
            len -= dec;
            buffer += dec;
            slen--;
        }
    }

    return( err );
}


static orl_return       doLINNUM( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          err;
    omf_bytes           buffer;
    long                len;
    omf_idx             seg;
    omf_idx             name;
    unsigned_16         line;
    unsigned_32         offset;
    int                 wordsize;

    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );
    len = ofh->parselen;
    buffer = ofh->parsebuf;

    switch( typ ) {
    case( CMD_LINSYM ):
    case( CMD_LINSYM32 ):
        buffer++;
        len--;
        name = loadIndex( &buffer, &len );
        if( !name ) return( ORL_ERROR );
        seg = 0;
        break;
    case( CMD_LINNUM ):
    case( CMD_LINNUM32 ):
        loadIndex( &buffer, &len );
        seg = loadIndex( &buffer, &len );
        if( !seg ) return( ORL_OKAY );
        name = 0;
        break;
    default:
        assert( 0 );
    }

    wordsize = OmfGetWordSize( check32Bit( ofh, typ ) );

    while( len ) {
        if( len < ( wordsize + 2 ) ) return( ORL_ERROR );
        line = getUWord( buffer, 2 );
        buffer += 2;
        len -= 2;
        offset = getUWord( buffer, wordsize );
        buffer += wordsize;
        len -= wordsize;
        err = OmfAddLineNum( ofh, seg, name, line, offset );
        if( err != ORL_OKAY ) break;
    }
    return( err );
}


static orl_return       doPUBDEF( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          err;
    omf_bytes           buffer;
    long                len;
    int                 slen;
    omf_idx             seg;
    omf_idx             group;
    omf_frame           frame;
    omf_bytes           name;
    orl_sec_offset      offset;
    int                 is32;
    int                 wordsize;

    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );

    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < 2 ) return( ORL_ERROR );

    group = loadIndex( &buffer, &len );
    seg = loadIndex( &buffer, &len );

    if( !seg ) {
        if( len < 2 ) return( ORL_ERROR );
        frame = getUWord( buffer, 2 );
        buffer += 2;
        len -= 2;
    }

    while( len ) {
        slen = buffer[0];
        buffer++;
        len--;
        if( ( slen + 1 + wordsize ) > len ) return( ORL_ERROR );
        name = buffer;
        buffer += slen;
        len -= slen;
        offset = getUWord( buffer, wordsize );
        buffer += wordsize;
        len -= wordsize;
        loadIndex( &buffer, &len );
        err = OmfAddPubDef( ofh, is32, group, seg, frame, name, slen, offset,
                            typ );
        if( err != ORL_OKAY ) break;
    }
    return( err );
}


static orl_return       doLNAMES( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          err;
    omf_bytes           buffer;
    long                len;
    int                 slen;

    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );
    len = ofh->parselen;
    buffer = ofh->parsebuf;

    while( len ) {
        slen = buffer[0];
        buffer++;
        len--;
        if( slen > len ) return( ORL_ERROR );
        err = OmfAddLName( ofh, buffer, slen, typ );
        if( err != ORL_OKAY ) break;
        len -= slen;
        buffer += slen;
    }
    return( err );
}


static orl_return       doSEGDEF( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          err;
    omf_bytes           buffer;
    long                len;
    omf_idx             name;
    omf_idx             class;
    uint_8              datum;
    int                 is32;
    int                 wordsize;
    orl_sec_alignment   align;
    orl_sec_size        size;
    int                 combine;
    int                 max = 0;
    int                 use32 = 0;
    orl_sec_frame       frame;

    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );

    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < ( 4 + wordsize ) ) return( ORL_ERROR );
    datum = buffer[0];

    align = getAlignment( datum >> 5 );
    combine = ( datum >> 2 ) & 7;
    buffer++;
    len--;

    if( ( datum >> 5 ) == ALIGN_ABS ) {
        if( ofh->status & OMF_STATUS_EASY_OMF ) {
            frame = getUWord( buffer, 4 );
        } else {
            frame = getUWord( buffer, 2 );
        }
        buffer += 3;
        len -= 3;
        if( len < ( wordsize + 3 ) ) return( ORL_ERROR );
    } else {
        frame = ORL_SEC_NO_ABS_FRAME;
    }

    if( datum & 0x02 ) {
        max = 1;
    } else {
        size = getUWord( buffer, wordsize );
    }
    buffer += wordsize;
    len -= wordsize;

    if( datum & 0x01 ) {
        use32 = 1;
    } else {
        use32 = 0;
    }

    name = loadIndex( &buffer, &len );
    class = loadIndex( &buffer, &len );
    loadIndex( &buffer, &len );

    if( ofh->status & OMF_STATUS_EASY_OMF ) {
        use32 = 1;
        if( len >= 1 ) {
            datum = buffer[0];
            if( !(datum & EASY_USE32_FIELD ) ) {
                use32 = 0;
            }
        }
    }

    if( use32 && !( ofh->status & OMF_STATUS_ARCH_SET ) ) {
        ofh->machine_type = ORL_MACHINE_TYPE_I386;
        _SetWordSize( ofh->flags, ORL_FILE_FLAG_32BIT_MACHINE );
        ofh->status |= OMF_STATUS_ARCH_SET;
    }

    return( OmfAddSegDef( ofh, is32, align, combine, use32, max, frame, size,
                          name, class ) );
}


static orl_return       doGRPDEF( omf_file_handle ofh )
{
    orl_return          err;
    omf_bytes           buffer;
    long                len;
    omf_idx             name;
    omf_idx             *segs;
    orl_sec_size        size;

    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );
    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( ( len < 1 ) || !( len & 1 ) ) return( ORL_ERROR );
    name = loadIndex( &buffer, &len );

    /* assume maximum possible group size of len / 2, max number of
     * segs is about 32768.
     */
    if( len > 65536 ) return( ORL_ERROR );
    size = ( ( len / 2 ) + 1 ) * sizeof( omf_idx );
    segs = _ClientAlloc( ofh, size );
    if( !segs ) return( ORL_OUT_OF_MEMORY );
    memset( segs, 0, size );

    size = 0;
    while( len ) {
        if( buffer[0] != 0xff ) return( ORL_ERROR );
        buffer++;
        len--;
        segs[size] = loadIndex( &buffer, &len );
        size++;
    }

    return( OmfAddGrpDef( ofh, name, segs, size ) );
}


static orl_return       doFIXUPP( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          err;
    omf_bytes           buffer;
    long                len;
    uint_8              datum;
    int                 is32;
    int                 wordsize;

    assert( ofh );

    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );

    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < 0 ) return( ORL_ERROR );

    while( len ) {
        /* determine if it is a thread or fixupp subrecord
         * and act upon it
         */
        datum = buffer[0];
        if( datum & 0x80 ) {
            err = processExplicitFixup( ofh, is32, &buffer, &len );
        } else {
            err = processThreadFixup( ofh, &buffer, &len );
        }
        if( err != ORL_OKAY ) break;
    }
    return( err );
}


static orl_return       doLEDATA( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          err;
    int                 is32;
    omf_bytes           buffer;
    long                len;
    int                 wordsize;
    omf_idx             seg;
    orl_sec_offset      offset;

    assert( ofh );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );
    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );
    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < ( wordsize + 1 ) ) return( ORL_ERROR );

    seg = loadIndex( &buffer, &len );
    if( !seg ) return( ORL_ERROR );
    offset = getUWord( buffer, wordsize );
    buffer += wordsize;
    len -= wordsize;
    if( len < 0 ) return( ORL_ERROR );

    return( OmfAddLEData( ofh, is32, seg, offset, buffer, len, 0 ) );
}


static orl_return       doLIDATA( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          err;
    int                 is32;
    omf_bytes           buffer;
    long                len;
    int                 wordsize;
    omf_idx             seg;
    orl_sec_offset      offset;

    assert( ofh );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );
    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );
    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < ( wordsize + 1 ) ) return( ORL_ERROR );

    seg = loadIndex( &buffer, &len );
    if( !seg ) return( ORL_ERROR );
    offset = getUWord( buffer, wordsize );
    buffer += wordsize;
    len -= wordsize;
    if( len < 0 ) return( ORL_ERROR );

    /* LIData must be processed after the fixups have been read in,  in order
     * to clone them appropriately, evil but necessary, we have no choice in
     * the matter, the conversion will create an LEData which will then be
     * added to the section.
     */
    return( OmfAddLIData( ofh, is32, seg, offset, buffer, len, 0 ) );
}


static orl_return       doCOMDAT( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          err;
    int                 is32;
    omf_bytes           buffer;
    long                len;
    int                 wordsize;
    omf_idx             seg;
    omf_idx             group;
    omf_frame           frame;
    omf_idx             name;
    uint_8              attr;
    int                 align;
    uint_8              flags;
    orl_sec_offset      offset;

    assert( ofh );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );
    err = loadRecord( ofh );
    if( err != ORL_OKAY ) return( err );
    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < ( wordsize + 3 ) ) return( ORL_ERROR );

    flags = buffer[0];
    attr = buffer[1];
    align = buffer[2];
    if( align == COMDAT_ALIGN_SEG ) {
        align = -1;
    } else {
        align = getAlignment( align );
    }
    buffer += 3;
    len -= 3;

    offset = getUWord( buffer, wordsize );
    buffer += wordsize;
    len -= wordsize;

    loadIndex( &buffer, &len );

    if( ( attr & COMDAT_ALLOC_MASK ) == COMDAT_EXPLICIT ) {
        group = loadIndex( &buffer, &len );
        seg = loadIndex( &buffer, &len );
        if( !seg && !group ) {
            frame = getUWord( buffer, 2 );
            buffer += 2;
            len -= 2;
        }
    }

    name = loadIndex( &buffer, &len );

    return( OmfAddComDat( ofh, is32, flags, attr, align, offset, seg, group,
                        frame, name, buffer, len, typ ) );
}


static orl_return       procRecord( omf_file_handle ofh, omf_rectyp typ )
{
    assert( ofh );

    switch( typ ) {
    case( CMD_COMENT ):         /* comment record                       */
        return( doCOMENT( ofh ) );

    case( CMD_MODEND ):         /* end of module record                 */
    case( CMD_MODEND32 ):       /* 32-bit end of module record          */
        return( doMODEND( ofh, typ ) );

    case( CMD_EXTDEF ):         /* import names record                  */
    case( CMD_LEXTDEF ):        /* local import names record            */
    case( CMD_LEXTDEF32 ):      /* 32-bit local import names record     */
        return( doEXTDEF( ofh, typ ) );

    case( CMD_CEXTDF ):         /* external reference to a COMDAT       */
        return( doCEXTDEF( ofh, typ ) );

    case( CMD_PUBDEF ):         /* export names record                  */
    case( CMD_PUBDEF32 ):       /* 32-bit export names record           */
    case( CMD_LPUBDEF ):        /* static export names record           */
    case( CMD_LPUBDEF32 ):      /* static export names record           */
        return( doPUBDEF( ofh, typ ) );

    case( CMD_LNAMES ):         /* list of names record                 */
    case( CMD_LLNAME ):         /* a "local" lnames                     */
        return( doLNAMES( ofh, typ ) );

    case( CMD_SEGDEF ):         /* segment definition record            */
    case( CMD_SEGDEF32 ):       /* 32-bit segment definition            */
        return( doSEGDEF( ofh, typ ) );

    case( CMD_GRPDEF ):         /* group definition record              */
        return( doGRPDEF( ofh ) );

    case( CMD_FIXUPP ):         /* relocation record                    */
    case( CMD_FIXUPP32 ):       /* 32-bit relocation record             */
        return( doFIXUPP( ofh, typ ) );

    case( CMD_LEDATA ):         /* object record                        */
    case( CMD_LEDATA32 ):       /* 32-bit object record                 */
        return( doLEDATA( ofh, typ ) );

    case( CMD_LIDATA ):         /* repeated data record                 */
    case( CMD_LIDATA32 ):       /* 32-bit repeated data record          */
        return( doLIDATA( ofh, typ ) );

    case( CMD_COMDAT ):         /* initialized communal data record     */
    case( CMD_COMDAT32 ):       /* initialized 32-bit communal data rec */
        return( doCOMDAT( ofh, typ ) );

    case( CMD_COMDEF ):         /* communal definition                  */
    case( CMD_LCOMDEF ):        /* local comdev                         */
        return( doCOMDEF( ofh, typ ) );

    case( CMD_LINNUM ):         /* line number record                   */
    case( CMD_LINNUM32 ):       /* 32-bit line number record.           */
    case( CMD_LINSYM ):         /* LINNUM for a COMDAT                  */
    case( CMD_LINSYM32 ):       /* 32-bit LINNUM for a COMDAT           */
        return( doLINNUM( ofh, typ ) );

                                /* No idea what to do with these yet    */
    case( CMD_BAKPAT ):         /* backpatch record (for Quick C)       */
    case( CMD_BAKPAT32 ):
    case( CMD_NBKPAT ):         /* named backpatch record (quick c?)    */
    case( CMD_NBKPAT32 ):       /* 32-bit named backpatch record        */
        return( loadRecord( ofh ) );

    case( CMD_RHEADR ):         /* These records are simply ignored     */
    case( CMD_REGINT ):         /****************************************/
    case( CMD_REDATA ):
    case( CMD_RIDATA ):
    case( CMD_OVLDEF ):
    case( CMD_ENDREC ):
    case( CMD_BLKDEF ):         /* block definition record              */
    case( CMD_BLKDEF32 ):       /* weird extension for QNX MAX assembler*/
    case( CMD_BLKEND ):         /* block end record                     */
    case( CMD_BLKEND32 ):       /* _might_ be used by QNX MAX assembler */
    case( CMD_DEBSYM ):
    case( CMD_LHEADR ):
    case( CMD_PEDATA ):
    case( CMD_PIDATA ):
    case( CMD_LOCSYM ):
    case( CMD_LIBHED ):
    case( CMD_LIBNAM ):
    case( CMD_LIBLOC ):
    case( CMD_LIBDIC ):
    case( CMD_ALIAS ):          /* alias definition record              */
    case( CMD_VERNUM ):         /* TIS version number record            */
    case( CMD_VENDEXT ):        /* TIS vendor extension record          */
    case( CMD_TYPDEF ):         /* type definition record               */
    case( CMD_THEADR ):         /* additonal header record              */
        return( loadRecord( ofh ) );

    default:
        return( ORL_ERROR );
    }
}

orl_return OmfLoadFileStructure( omf_file_handle ofh )
{
    orl_return  err = ORL_OKAY;
    omf_rectyp  *typ;

    assert( ofh );

    setInitialData( ofh );
    typ = _ClientRead( ofh, 1 );
    if( !typ || ( *typ != CMD_THEADR ) ) return( ORL_ERROR );
    ofh->last_rec = *typ;
    err = doTHEADR( ofh );
    if( err != ORL_OKAY ) return( err );

    for( ;; ) {
        typ = _ClientRead( ofh, 1 );
        if( !typ ) {
            err = ORL_ERROR;
            break;
        }
        ofh->last_rec = *typ;
        err = procRecord( ofh, *typ );
        if( err != ORL_OKAY ) break;
        if( ( *typ == CMD_MODEND ) || ( *typ == CMD_MODEND32 ) ) break;
    }

    return( err );
}

orl_return      OmfParseScanTab( omf_bytes buffer, long len,
                                 omf_scan_tab_struct *entry )
{
    int         wordsize;

    assert( buffer );
    assert( entry );

    switch( buffer[0] ) {
    case( DDIR_SCAN_TABLE ):
        wordsize = 2;
        break;
    case( DDIR_SCAN_TABLE_32 ):
        wordsize = 4;
        break;
    default:
        return( ORL_ERROR );
    }
    len--;
    buffer++;

    if( len < ( 2 * wordsize + 1 ) ) return( ORL_ERROR );
    entry->seg = loadIndex( &buffer, &len );
    if( !entry->seg ) {
        if( len < ( 2 * wordsize + 1 ) ) return( ORL_ERROR );
        entry->lname = loadIndex( &buffer, &len );
        if( !entry->lname ) return( ORL_ERROR );
    }

    if( len < ( 2 * wordsize ) ) return( ORL_ERROR );
    entry->start = getUWord( buffer, wordsize );
    buffer += wordsize;
    entry->end = getUWord( buffer, wordsize );

    return( ORL_OKAY );
}
