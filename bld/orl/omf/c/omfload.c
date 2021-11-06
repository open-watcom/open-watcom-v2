/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OMF file parsing routines.
*
****************************************************************************/


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "watcom.h"
#include "omfload.h"
#include "omfmunge.h"
#include "omforl.h"


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
    ofh->flags = ORL_FILE_FLAG_LITTLE_ENDIAN | ORL_FILE_FLAG_16BIT_MACHINE;
    ofh->debug_style = OMF_DBG_STYLE_CODEVIEW;
}


omf_rec_size    OmfGetWordSize( bool is32 )
{
    if( is32 ) {
        return( 4 );
    } else {
        return( 2 );
    }
}


omf_sec_offset  OmfGetUWord( omf_bytes buffer, int wordsize )
{
    omf_sec_offset  result;

    assert( buffer );

    result = 0;
    switch( wordsize ) {
    case 4:
        result |= buffer[3] << 24;
        result |= buffer[2] << 16;
        /* fall through */
    case 2:
        result |= buffer[1] << 8;
        /* fall through */
    default:
        result |= buffer[0];
        break;
    }
    return( result );
}


unsigned_16 OmfGetU16( omf_bytes buffer )
{
    assert( buffer );

    return( ( buffer[1] << 8 ) | buffer[0] );
}


omf_sec_addend    OmfGetSWord( omf_bytes buffer, int wordsize )
{
    omf_sec_offset  result;

    result = OmfGetUWord( buffer, wordsize );
    switch( wordsize ) {
    case 2:
        if( result & 0x8000 )
            result |= 0xFFFF0000;
        break;
    default:
        if( result & 0x80 )
            result |= 0xFFFFFF00;
        break;
    case 4:
        break;
    }
    return( (omf_sec_addend)result );
}


static bool check32Bit( omf_file_handle ofh, omf_rectyp typ )
{
    return( _Is32BitRec( typ ) || (ofh->status & OMF_STATUS_EASY_OMF) );
}


static orl_sec_alignment getAlignment( int val )
{
    switch( val ) {
    case ALIGN_ABS:             /* absolute segment - no alignment      */
    case ALIGN_BYTE:            /* relocatable seg - byte aligned       */
        return( 0 );
    case ALIGN_WORD:            /* relocatable seg - word aligned       */
        return( 1 );
    case ALIGN_PARA:            /* relocatable seg - para aligned       */
        return( 4 );
    case ALIGN_PAGE:            /* relocatable seg - page aligned       */
        return( 8 );
    case ALIGN_DWORD:           /* relocatable seg - dword aligned      */
                                /* 32-bit ALIGN_UNABS unnamed absolute  *
                                 * segment, this is currently not       *
                                 * supported by the linker.             */
        return( 2 );
    case ALIGN_LTRELOC:         /* load-time relocatable segment        */
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
    unsigned short      len;
    omf_bytes           buff;
    unsigned_8          chksum;

    assert( ofh );

    buff = _ClientRead( ofh, 2 );
    if( buff == NULL )
        return( ORL_ERROR );
    len = OmfGetU16( buff );
    if( len == 0 )
        return( ORL_ERROR );
    ofh->parsebuf = _ClientRead( ofh, len );
    if( ofh->parsebuf == NULL )
        return( ORL_ERROR );
    ofh->parselen = len - 1;

    // some slob compilers put out 0 for all chksum's or wrong value
    // so only setup file flag ORL_FILE_CHKSUM_ERROR
    // if any check sum is incorrect
    chksum = ofh->last_rec + buff[0] + buff[1];
    buff = ofh->parsebuf;
    while( len-- ) {
        chksum += *buff++;
    }
    if( chksum ) {
        ofh->flags |= ORL_FILE_CHKSUM_ERROR;
    }
    return( ORL_OKAY );
}


static omf_idx loadIndex( omf_bytes *buffer, omf_rec_size *len )
{
    omf_bytes           deref;
    omf_rec_size        pos;
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

static omf_frame loadFrameNumber( omf_bytes *buffer, omf_rec_size *len )
{
    omf_frame       frame_num;

    frame_num = OmfGetU16( *buffer );
    *buffer += 2;
    *len -= 2;
    return( frame_num );
}

static void GetFrame( unsigned char method, ORL_STRUCT( omf_thread_fixup ) *thread, omf_bytes *buffer, omf_rec_size *len )
{
    thread->method = method;
    switch( method ) {
    case FRAME_SEG:                 /* segment index                */
    case FRAME_GRP:                 /* group index                  */
    case FRAME_EXT:                 /* external index               */
        thread->idx = loadIndex( buffer, len );
        break;
    case FRAME_ABS:                 /* absolute frame number        */
        thread->idx = loadFrameNumber( buffer, len );
        break;
    default:
        thread->idx = 0;
        break;
    }
}

static void GetTarget( unsigned char method, ORL_STRUCT( omf_thread_fixup ) *thread, omf_bytes *buffer, omf_rec_size *len )
{
    thread->method = method;
    switch( method ) {
    case TARGET_SEG:                /* segment index                */
    case TARGET_GRP:                /* group index                  */
    case TARGET_EXT:                /* external index               */
        thread->idx = loadIndex( buffer, len );
        break;
    case TARGET_ABS:                /* absolute frame number        */
        thread->idx = loadFrameNumber( buffer, len );
        break;
    default:
        thread->idx = 0;
        break;
    }
}

static orl_return       processExplicitFixup( omf_file_handle ofh, bool is32, omf_bytes *buffer, omf_rec_size *cur )
{
    omf_bytes           buf;
    omf_rec_size        len;
    unsigned_8          m;
    omf_fix_loc         fix_loc;
    int                 offset;
    ORL_STRUCT( omf_thread_fixup ) fthread;
    ORL_STRUCT( omf_thread_fixup ) tthread;
    unsigned_8          datum;
    omf_rec_size        wordsize;
    omf_sec_addend      disp;

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
    m = ( (datum & FIXUPP_MBIT) != 0 );
    fix_loc = (omf_fix_loc)(( datum >> 2 ) & 0x0f);
    offset = ( (datum & 0x03) << 8 ) | buf[1];
    datum = buf[2];
    buf += 3;
    len -= 3;
    /*
     * frame processing
     */
    if( datum & FIXDAT_FTHREAD ) {
        fthread = ofh->frame_thread[( datum >> 4 ) & 3];
    } else {
        GetFrame( ( datum >> 4 ) & 7, &fthread, &buf, &len );
    }
    /*
     * target processing
     */
    if( datum & FIXDAT_TTHREAD ) {
        tthread = ofh->target_thread[datum & 3];
    } else {
        GetTarget( datum & 3, &tthread, &buf, &len );
        if( fthread.method == FRAME_TARG ) {
            /* fmethod becomes the same as tmethod (1 of first 3)
             */
            fthread = tthread;
        }
    }
    /*
     * target displacement processing
     */
    if( datum & FIXDAT_PBIT ) {
        disp = 0;
    } else {
        disp = OmfGetSWord( buf, wordsize );
        buf += wordsize;
        len -= wordsize;
    }

    *buffer = buf;
    *cur = len;
    return( OmfAddFixupp( ofh, is32, m, fix_loc, offset, &fthread, &tthread, disp ) );
}


static orl_return       processThreadFixup( omf_file_handle ofh, omf_bytes *buffer, omf_rec_size *cur )
{
    omf_bytes           buf;
    omf_rec_size        len;
    unsigned_8          datum;

    assert( ofh );
    assert( buffer );
    assert( *buffer );
    assert( cur );
    assert( *cur > 0 );
    assert( (**buffer & 0x80) == 0 );

    buf = *buffer;
    len = *cur;

    datum = buf[0];
    buf++;
    len--;

    if( datum & 0x40 ) {
        /*
         * frame thread processing
         */
        GetFrame( ( datum >> 2 ) & 7, &ofh->frame_thread[datum & 3], &buf, &len );
    } else {
        /*
         * target thread processing
         */
        GetTarget( ( datum >> 2 ) & 3, &ofh->target_thread[datum & 3], &buf, &len );
    }

    *buffer = buf;
    *cur = len;
    return( ORL_OKAY );
}


static orl_return       doTHEADR( omf_file_handle ofh )
{
    orl_return          return_val;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    return( OmfTheadr( ofh ) );
}


static orl_return       doCOMENT( omf_file_handle ofh )
{
    orl_return          return_val;
    unsigned_8          class;
    unsigned_8          flags;
    omf_bytes           buffer;
    omf_rec_size        len;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );

    len = ofh->parselen;
    if( len < 2 )
        return( ORL_ERROR );
    buffer = ofh->parsebuf;
    flags = buffer[0];
    class = buffer[1];
    buffer += 2;
    len -= 2;

    return_val = OmfAddComment( ofh, class, flags, buffer, len );
    if( return_val != ORL_OKAY )
        return( return_val );

    switch( class ) {
    case CMT_DISASM_DIRECTIVE:
        ofh->status |= OMF_STATUS_WATCOM;
        break;
    case CMT_WAT_PROC_MODEL:
        ofh->status |= OMF_STATUS_WATCOM;
        /* fall through */
    case CMT_MS_PROC_MODEL:
        /* Determine CPU
         */
        if( len == 0 )
            break;
        switch( *buffer ) {
        case '2':
        case '0':
            /* 16 bit code
             */
            ofh->machine_type = ORL_MACHINE_TYPE_I8086;
            _SetWordSize( ofh->flags, ORL_FILE_FLAG_16BIT_MACHINE );
            break;
        case '3':
        default:
            ofh->machine_type = ORL_MACHINE_TYPE_I386;
            _SetWordSize( ofh->flags, ORL_FILE_FLAG_32BIT_MACHINE );
            break;
        }

        /* we do not use any of the other info, so we don't look at it.
         * see dmpobj source code for format of this coment record
         */
        break;
    case CMT_EASY_OMF:
        /* Pharlap Sucks!!!!
         * Their weird object files forces us to do this crap
         */
        if( ( flags == CMT_TNP ) && memcmp( buffer, EASY_OMF_SIGNATURE, 5 ) == 0 ) {
            ofh->status |= OMF_STATUS_EASY_OMF;
            ofh->machine_type = ORL_MACHINE_TYPE_I386;
            _SetWordSize( ofh->flags, ORL_FILE_FLAG_32BIT_MACHINE );
            ofh->status |= OMF_STATUS_ARCH_SET;
        }
        break;
    case CMT_MS_OMF:
        /* If we see the "New OMF" COMENT record, we need to see if it
         * contains the debug info style information. If it isn't CodeView,
         * we don't want to parse LINNUM records as they will be in different
         * format and could confuse us. This happens with .obj files generated
         * by IBM's compilers. Note that we default to CodeView (MS) style
         * but we might encounter a COMENT record setting some unknown style.
         */
         if( *buffer )
             buffer++;
         if( ( len == 0 ) || memcmp( buffer, "CV", 2 ) == 0 ) {
             ofh->debug_style = OMF_DBG_STYLE_CODEVIEW;
         } else if( memcmp( buffer, "HL", 2 ) == 0 ) {
             ofh->debug_style = OMF_DBG_STYLE_HLL;
         } else {
             ofh->debug_style = OMF_DBG_STYLE_UNKNOWN;
         }
         break;
    }
    return( return_val );
}


static orl_return       doMODEND( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return  return_val;

    /* unused parameters */ (void)typ;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    ofh->status |= OMF_STATUS_FILE_LOADED;
    return( OmfModEnd( ofh ) );
}


static orl_return       doEXTDEF( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    omf_bytes           buffer;
    omf_rec_size        len;
    omf_string_len      slen;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    len = ofh->parselen;
    buffer = ofh->parsebuf;

    while( len ) {
        slen = *buffer++;
        len--;
        if( slen > len )
            return( ORL_ERROR );
        if( slen > 0 ) {
            return_val = OmfAddExtDef( ofh, (char *)buffer, slen, typ );
            if( return_val != ORL_OKAY ) {
                break;
            }
        }
        len -= slen;
        buffer += slen;
        loadIndex( &buffer, &len );
    }

    return( return_val );
}


static orl_return       doCEXTDEF( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    omf_bytes           buffer;
    omf_rec_size        len;
    omf_string          extname;
    omf_idx             idx;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    len = ofh->parselen;
    buffer = ofh->parsebuf;

    while( len ) {
        if( len < 2 )
            return( ORL_ERROR );
        idx = loadIndex( &buffer, &len );
        loadIndex( &buffer, &len );
        extname = OmfGetLName( ofh->lnames, idx );
        if( extname == NULL )
            return( ORL_ERROR );
        return_val = OmfAddExtDef( ofh, extname->string, extname->len, typ );
        if( return_val != ORL_OKAY ) {
            break;
        }
    }
    return( return_val );
}


static orl_return       doCOMDEF( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    omf_bytes           buffer;
    omf_rec_size        len;
    omf_string_len      slen;
    int                 dec;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    len = ofh->parselen;
    buffer = ofh->parsebuf;

    while( len ) {
        slen = *buffer++;
        len--;
        if( slen > len )
            return( ORL_ERROR );
        if( slen > 0 ) {
            return_val = OmfAddExtDef( ofh, (char *)buffer, slen, typ );
            if( return_val != ORL_OKAY ) {
                break;
            }
        }
        len -= slen;
        buffer += slen;
        loadIndex( &buffer, &len );
        switch( buffer[0] ) {
        case COMDEF_NEAR:
            slen = 1;
            break;
        case COMDEF_FAR:
            slen = 2;
            break;
        default:
            return( ORL_ERROR );
        }
        buffer++;
        len--;
        while( slen > 0 ) {
            dec = 1;
            if( len < 1 )
                return( ORL_ERROR );
            if( buffer[0] & COMDEF_LEAF_SIZE ) {
                switch( buffer[0] ) {
                case COMDEF_LEAF_4:  dec++;
                    /* fall through */
                case COMDEF_LEAF_3:  dec++;
                    /* fall through */
                case COMDEF_LEAF_2:  dec += 2;
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

    return( return_val );
}

static orl_return       doLINNUM( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    omf_bytes           buffer;
    omf_rec_size        len;
    omf_idx             seg = 0;
    omf_idx             name = 0;
    unsigned_16         line;
    unsigned_32         offset;
    int                 wordsize;
    omf_sec_handle      sh;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    len = ofh->parselen;
    buffer = ofh->parsebuf;

    switch( typ ) {
    case CMD_LINSYM:
    case CMD_LINSYM32:
        buffer++;
        len--;
        name = loadIndex( &buffer, &len );
        if( name == 0 )
            return( ORL_ERROR );
        seg = 0;
        break;
    case CMD_LINNUM:
    case CMD_LINNUM32:
        switch( ofh->debug_style ) {
        case OMF_DBG_STYLE_CODEVIEW:
            // We have MS style line numbers.
            loadIndex( &buffer, &len );
            seg = loadIndex( &buffer, &len );
            if( seg == 0 )
                return( ORL_OKAY );
            break;
        case OMF_DBG_STYLE_HLL:
            // We have IBM HLL style line numbers.
            // TODO
            return( ORL_OKAY );
        case OMF_DBG_STYLE_UNKNOWN:
            // We don't know what this is. Do not attempt to parse.
            return( ORL_OKAY );
        }
        break;
    default:
        assert( 0 );
    }

    sh = OmfFindSegOrComdat( ofh, seg, name );
    if( sh == NULL )
        return( ORL_ERROR );

    wordsize = OmfGetWordSize( check32Bit( ofh, typ ) );

    while( len ) {
        if( len < ( wordsize + 2 ) )
            return( ORL_ERROR );
        line = OmfGetU16( buffer );
        buffer += 2;
        len -= 2;
        offset = OmfGetUWord( buffer, wordsize );
        buffer += wordsize;
        len -= wordsize;
        return_val = OmfAddLineNum( sh, line, offset );
        if( return_val != ORL_OKAY ) {
            break;
        }
    }
    return( return_val );
}


static orl_return       doPUBDEF( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    omf_bytes           buffer;
    omf_rec_size        len;
    omf_string_len      slen;
    omf_idx             seg;
    omf_idx             group;
    omf_frame           frame;
    char                *pubname;
    omf_sec_offset      offset;
    bool                is32;
    int                 wordsize;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );

    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < 2 )
        return( ORL_ERROR );

    group = loadIndex( &buffer, &len );
    seg = loadIndex( &buffer, &len );

    frame = 0;
    if( seg == 0 ) {
        if( len < 2 )
            return( ORL_ERROR );
        frame = loadFrameNumber( &buffer, &len );
    }

    while( len ) {
        slen = *buffer++;
        len--;
        if( ( slen + 1 + wordsize ) > len )
            return( ORL_ERROR );
        pubname = (char *)buffer;
        buffer += slen;
        len -= slen;
        offset = OmfGetUWord( buffer, wordsize );
        buffer += wordsize;
        len -= wordsize;
        loadIndex( &buffer, &len );
        return_val = OmfAddPubDef( ofh, is32, group, seg, frame, pubname, slen, offset, typ );
        if( return_val != ORL_OKAY ) {
            break;
        }
    }
    return( return_val );
}


static orl_return       doLNAMES( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    omf_bytes           buffer;
    omf_rec_size        len;
    omf_string_len      slen;

    /* unused parameters */ (void)typ;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    len = ofh->parselen;
    buffer = ofh->parsebuf;
    while( len ) {
        slen = *buffer++;
        len--;
        if( slen > len )
            return( ORL_ERROR );
        return_val = OmfAddLName( ofh, (char *)buffer, slen );
        if( return_val != ORL_OKAY )
            break;
        len -= slen;
        buffer += slen;
    }
    return( return_val );
}




static orl_return       doSEGDEF( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    omf_bytes           buffer;
    omf_rec_size        len;
    omf_idx             name;
    omf_idx             class;
    unsigned_8          datum;
    bool                is32;
    int                 wordsize;
    orl_sec_alignment   align;
    omf_sec_size        size;
    int                 combine;
    bool                max_size;
    bool                use32;
    omf_frame           frame;
    bool                abs_frame;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );

    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < ( 4 + wordsize ) )
        return( ORL_ERROR );
    datum = *buffer++;
    len--;

    frame = 0;
    abs_frame = false;
    align = getAlignment( datum >> 5 );
    combine = ( datum >> 2 ) & 7;
    if( ( datum >> 5 ) == ALIGN_ABS ) {
        abs_frame = true;
        if( ofh->status & OMF_STATUS_EASY_OMF ) {
            // FIXME !!! it looks bugy, frame should be 16-bit and offset ?
            // I can not found any information about it
            frame = loadFrameNumber( &buffer, &len );
        } else {
            frame = loadFrameNumber( &buffer, &len );
        }
        /*
         * skip one byte
         */
        buffer += 1;
        len -= 1;
        if( len < ( wordsize + 3 ) ) {
            return( ORL_ERROR );
        }
    }

    size = (omf_sec_size)OmfGetUWord( buffer, wordsize );
    buffer += wordsize;
    len -= wordsize;

    name = loadIndex( &buffer, &len );
    class = loadIndex( &buffer, &len );
    loadIndex( &buffer, &len );

    max_size = ( (datum & 0x02) != 0 );
    use32 = ( (datum & 0x01) != 0 );
    if( ofh->status & OMF_STATUS_EASY_OMF ) {
        use32 = true;
        if( len >= 1 ) {
            datum = buffer[0];
            if( (datum & EASY_USE32_FIELD) == 0 ) {
                use32 = false;
            }
        }
    }

    if( use32 && (ofh->status & OMF_STATUS_ARCH_SET) == 0 ) {
        ofh->machine_type = ORL_MACHINE_TYPE_I386;
        _SetWordSize( ofh->flags, ORL_FILE_FLAG_32BIT_MACHINE );
        ofh->status |= OMF_STATUS_ARCH_SET;
    }

    return( OmfAddSegDef( ofh, is32, align, combine, use32, max_size, abs_frame, frame, size, name, class ) );
}


static orl_return       doGRPDEF( omf_file_handle ofh )
{
    orl_return          return_val;
    omf_bytes           buffer;
    omf_rec_size        len;
    omf_idx             name;
    omf_idx             *segs;
    size_t              size;
    unsigned            num_segs;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( ( len < 1 ) || (len & 1) == 0 )
        return( ORL_ERROR );
    name = loadIndex( &buffer, &len );

    /* assume maximum possible group size of len / 2, max number of
     * segs is about 32768.
     */
    if( len > 65536 )
        return( ORL_ERROR );
    size = ( ( len / 2 ) + 1 ) * sizeof( omf_idx );
    segs = _ClientAlloc( ofh, size );
    if( segs == NULL )
        return( ORL_OUT_OF_MEMORY );
    memset( segs, 0, size );

    num_segs = 0;
    while( len ) {
        if( buffer[0] != 0xff )
            return( ORL_ERROR );
        buffer++;
        len--;
        segs[num_segs] = loadIndex( &buffer, &len );
        num_segs++;
    }

    return( OmfAddGrpDef( ofh, name, segs, num_segs ) );
}


static orl_return       doFIXUPP( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    omf_bytes           buffer;
    omf_rec_size        len;
    unsigned_8          datum;
    bool                is32;
//    int                 wordsize;

    assert( ofh );

    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );

    is32 = check32Bit( ofh, typ );
//    wordsize = OmfGetWordSize( is32 );

    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < 0 )
        return( ORL_ERROR );

    while( len ) {
        /* determine if it is a thread or fixupp subrecord
         * and act upon it
         */
        datum = buffer[0];
        if( datum & FIXUPP_FIXUP ) {
            return_val = processExplicitFixup( ofh, is32, &buffer, &len );
        } else {
            return_val = processThreadFixup( ofh, &buffer, &len );
        }
        if( return_val != ORL_OKAY ) {
            break;
        }
    }
    return( return_val );
}


static orl_return       doBAKPAT( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    omf_bytes           buffer;
    omf_rec_size        len;
    unsigned_8          loctype;
    bool                is32;
    int                 wordsize;
    omf_sec_addend      disp;
    omf_sec_offset      offset;
    omf_idx             segidx;
    omf_idx             symidx;

    assert( ofh );

    /* BAKPAT records are created by Microsoft QuickC, as well as by 16-bit
     * Microsoft C/C++ 7.0 and later when compiling without optimizations.
     * NBKPAT records are probably specific to MS C++ 7.0 and later only.
     */
    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );

    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < 0 )
        return( ORL_ERROR );

    segidx = symidx = 0;

    if( typ == CMD_BAKPAT || typ == CMD_BAKPAT32 ) {
        /* Segment index first, then location type. */
        segidx = loadIndex( &buffer, &len );
        if( segidx == 0 )
            return( ORL_ERROR );

        loctype = *buffer++;
        --len;
    } else {
        assert( typ == CMD_NBKPAT || typ == CMD_NBKPAT32 );
        /* Location first, then symbol index. */
        loctype = *buffer++;
        --len;
        symidx = loadIndex( &buffer, &len );
        if( symidx == 0 ) {
            return( ORL_ERROR );
        }
    }

    while( len ) {
        /* Read the offset and displacement (always the same size). */
        offset = OmfGetUWord( buffer, wordsize );
        buffer += wordsize;
        len -= wordsize;
        disp = OmfGetSWord( buffer, wordsize );
        buffer += wordsize;
        len -= wordsize;

        return_val = OmfAddBakpat( ofh, loctype, offset, segidx, symidx, disp );
        if( return_val != ORL_OKAY ) {
            break;
        }
    }
    return( return_val );
}


static orl_return       doLEDATA( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    bool                is32;
    omf_bytes           buffer;
    omf_rec_size        len;
    int                 wordsize;
    omf_idx             seg;
    omf_sec_offset      offset;

    assert( ofh );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );
    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < ( wordsize + 1 ) )
        return( ORL_ERROR );

    seg = loadIndex( &buffer, &len );
    if( seg == 0 )
        return( ORL_ERROR );
    offset = OmfGetUWord( buffer, wordsize );
    buffer += wordsize;
    len -= wordsize;
    if( len < 0 )
        return( ORL_ERROR );

    return( OmfAddLEData( ofh, is32, seg, offset, buffer, len, false ) );
}


static orl_return       doLIDATA( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    bool                is32;
    omf_bytes           buffer;
    omf_rec_size        len;
    int                 wordsize;
    omf_idx             seg;
    omf_sec_offset      offset;

    assert( ofh );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );
    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < ( wordsize + 1 ) )
        return( ORL_ERROR );

    seg = loadIndex( &buffer, &len );
    if( seg == 0 )
        return( ORL_ERROR );
    offset = OmfGetUWord( buffer, wordsize );
    buffer += wordsize;
    len -= wordsize;
    if( len < 0 )
        return( ORL_ERROR );

    /* LIData must be processed after the fixups have been read in,  in order
     * to clone them appropriately, evil but necessary, we have no choice in
     * the matter, the conversion will create an LEData which will then be
     * added to the section.
     */
    return( OmfAddLIData( ofh, is32, seg, offset, buffer, len, false ) );
}


static orl_return       doCOMDAT( omf_file_handle ofh, omf_rectyp typ )
{
    orl_return          return_val;
    bool                is32;
    omf_bytes           buffer;
    omf_rec_size        len;
    int                 wordsize;
    omf_idx             seg;
    omf_idx             group;
    omf_frame           frame;
    omf_idx             name;
    unsigned_8          attr;
    int                 align;
    unsigned_8          flags;
    omf_sec_offset      offset;

    assert( ofh );

    is32 = check32Bit( ofh, typ );
    wordsize = OmfGetWordSize( is32 );
    return_val = loadRecord( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );
    len = ofh->parselen;
    buffer = ofh->parsebuf;
    if( len < ( wordsize + 3 ) )
        return( ORL_ERROR );

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

    offset = OmfGetUWord( buffer, wordsize );
    buffer += wordsize;
    len -= wordsize;

    loadIndex( &buffer, &len );

    group = 0;
    seg = 0;
    frame = 0;
    if( (attr & COMDAT_ALLOC_MASK) == COMDAT_EXPLICIT ) {
        group = loadIndex( &buffer, &len );
        seg = loadIndex( &buffer, &len );
        if( seg == 0 && group == 0 ) {
            frame = loadFrameNumber( &buffer, &len );
        }
    }

    name = loadIndex( &buffer, &len );

    return( OmfAddComDat( ofh, is32, flags, attr, align, offset, seg, group, frame, name, buffer, len, typ ) );
}


static orl_return   procRecord( omf_file_handle ofh, omf_rectyp typ )
{
    assert( ofh );

    switch( typ ) {
    case CMD_COMENT:            /* comment record                       */
        return( doCOMENT( ofh ) );

    case CMD_MODEND:            /* end of module record                 */
    case CMD_MODEND32:          /* 32-bit end of module record          */
        return( doMODEND( ofh, typ ) );

    case CMD_EXTDEF:            /* import names record                  */
    case CMD_LEXTDEF:           /* local import names record            */
    case CMD_LEXTDEF32:         /* 32-bit local import names record     */
        return( doEXTDEF( ofh, typ ) );

    case CMD_CEXTDEF:           /* external reference to a COMDAT       */
        return( doCEXTDEF( ofh, typ ) );

    case CMD_PUBDEF:            /* export names record                  */
    case CMD_PUBDEF32:          /* 32-bit export names record           */
    case CMD_LPUBDEF:           /* static export names record           */
    case CMD_LPUBDEF32:         /* static export names record           */
        return( doPUBDEF( ofh, typ ) );

    case CMD_LNAMES:            /* list of names record                 */
    case CMD_LLNAMES:           /* a "local" lnames                     */
        return( doLNAMES( ofh, typ ) );

    case CMD_SEGDEF:            /* segment definition record            */
    case CMD_SEGDEF32:          /* 32-bit segment definition            */
        return( doSEGDEF( ofh, typ ) );

    case CMD_GRPDEF:            /* group definition record              */
        return( doGRPDEF( ofh ) );

    case CMD_FIXUPP:            /* relocation record                    */
    case CMD_FIXUPP32:          /* 32-bit relocation record             */
        return( doFIXUPP( ofh, typ ) );

    case CMD_LEDATA:            /* object record                        */
    case CMD_LEDATA32:          /* 32-bit object record                 */
        return( doLEDATA( ofh, typ ) );

    case CMD_LIDATA:            /* repeated data record                 */
    case CMD_LIDATA32:          /* 32-bit repeated data record          */
        return( doLIDATA( ofh, typ ) );

    case CMD_COMDAT:            /* initialized communal data record     */
    case CMD_COMDAT32:          /* initialized 32-bit communal data rec */
        return( doCOMDAT( ofh, typ ) );

    case CMD_COMDEF:            /* communal definition                  */
    case CMD_LCOMDEF:           /* local comdev                         */
        return( doCOMDEF( ofh, typ ) );

    case CMD_THEADR:            /* additonal header record              */
        return( doTHEADR( ofh ) );

    case CMD_LINNUM:            /* line number record                   */
    case CMD_LINNUM32:          /* 32-bit line number record.           */
    case CMD_LINSYM:            /* LINNUM for a COMDAT                  */
    case CMD_LINSYM32:          /* 32-bit LINNUM for a COMDAT           */
        return( doLINNUM( ofh, typ ) );

    case CMD_BAKPAT:            /* backpatch record (for Quick C)       */
    case CMD_BAKPAT32:          /* 32-bit backpatch record              */
    case CMD_NBKPAT:            /* named backpatch record (MS C++)      */
    case CMD_NBKPAT32:          /* 32-bit named backpatch record        */
        return( doBAKPAT( ofh, typ ) );

    case CMD_RHEADR:            /* These records are simply ignored     */
    case CMD_REGINT:            /****************************************/
    case CMD_REDATA:
    case CMD_RIDATA:
    case CMD_OVLDEF:
    case CMD_ENDREC:
    case CMD_BLKDEF:            /* block definition record              */
    case CMD_BLKDEF32:          /* weird extension for QNX MAX assembler*/
    case CMD_BLKEND:            /* block end record                     */
    case CMD_BLKEND32:          /* _might_ be used by QNX MAX assembler */
    case CMD_DEBSYM:
    case CMD_LHEADR:
    case CMD_PEDATA:
    case CMD_PIDATA:
    case CMD_LOCSYM:
    case CMD_LIBHED:
    case CMD_LIBNAM:
    case CMD_LIBLOC:
    case CMD_LIBDIC:
    case CMD_ALIAS:             /* alias definition record              */
    case CMD_VERNUM:            /* TIS version number record            */
    case CMD_VENDEXT:           /* TIS vendor extension record          */
    case CMD_TYPDEF:            /* type definition record               */
        return( loadRecord( ofh ) );

    default:
        return( ORL_ERROR );
    }
}

orl_return OmfLoadFileStructure( omf_file_handle ofh )
{
    orl_return  return_val;
    omf_rectyp  *typ;

    assert( ofh );

    setInitialData( ofh );
    typ = _ClientRead( ofh, 1 );
    if( typ == NULL || ( *typ != CMD_THEADR ) )
        return( ORL_ERROR );
    ofh->last_rec = *typ;
    return_val = doTHEADR( ofh );
    if( return_val != ORL_OKAY )
        return( return_val );

    for( ;; ) {
        typ = _ClientRead( ofh, 1 );
        if( typ == NULL ) {
            return_val = ORL_ERROR;
            break;
        }
        ofh->last_rec = *typ;
        return_val = procRecord( ofh, *typ );
        if( return_val != ORL_OKAY )
            break;
        if( ( *typ == CMD_MODEND ) || ( *typ == CMD_MODEND32 ) ) {
            break;
        }
    }

    return( return_val );
}

orl_return      OmfParseScanTab( omf_bytes buffer, omf_rec_size len, omf_scan_tab st_entry )
{
    int         wordsize;

    assert( buffer );
    assert( st_entry );

    switch( buffer[0] ) {
    case DDIR_SCAN_TABLE:
        wordsize = 2;
        break;
    case DDIR_SCAN_TABLE_32:
        wordsize = 4;
        break;
    default:
        return( ORL_ERROR );
    }
    len--;
    buffer++;

    if( len < ( 2 * wordsize + 1 ) )
        return( ORL_ERROR );
    st_entry->seg = loadIndex( &buffer, &len );
    if( st_entry->seg == 0 ) {
        if( len < ( 2 * wordsize + 1 ) )
            return( ORL_ERROR );
        st_entry->lname = loadIndex( &buffer, &len );
        if( st_entry->lname == 0 ) {
            return( ORL_ERROR );
        }
    }

    if( len < ( 2 * wordsize ) )
        return( ORL_ERROR );
    st_entry->start = OmfGetUWord( buffer, wordsize );
    buffer += wordsize;
    st_entry->end = OmfGetUWord( buffer, wordsize );

    return( ORL_OKAY );
}
