/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Resource Compiler executable processing utility functions.
*
****************************************************************************/


#include "global.h"
#include "rcrtns.h"
#include "rccore_2.h"
#include "exedos.h"
#include "exeutil.h"

#include "clibext.h"


RcStatus CopyExeData( FILE *src_fp, FILE *dst_fp, uint_32 length )
/*****************************************************************
 * NB When an error occurs the function MUST return without altering errno
 */
{
    size_t          numread;
    size_t          bufflen;

    if( length == 0 ) {
        return( RS_PARAM_ERROR );
    }
    for( bufflen = IO_BUFFER_SIZE; length > 0; length -= bufflen ) {
        if( bufflen > length )
            bufflen = length;
        numread = RESREAD( src_fp, Pass2Info.IoBuffer, bufflen );
        if( numread != bufflen ) {
            return( RESIOERR( src_fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
        }
        if( RESWRITE( dst_fp, Pass2Info.IoBuffer, numread ) != numread ) {
            return( RS_WRITE_ERROR );
        }
    }
    return( RS_OK );
} /* CopyExeData */

RcStatus CopyExeDataTilEOF( FILE *src_fp, FILE *dst_fp )
/*******************************************************
 * NB when an error occurs this function MUST return without altering errno
 */
{
    size_t      numread;

    while( (numread = RESREAD( src_fp, Pass2Info.IoBuffer, IO_BUFFER_SIZE )) != 0 ) {
        if( numread != IO_BUFFER_SIZE && RESIOERR( src_fp, numread ) ) {
            return( RS_READ_ERROR );
        }
        if( RESWRITE( dst_fp, Pass2Info.IoBuffer, numread ) != numread ) {
            return( RS_WRITE_ERROR );
        }
    }

    return( RS_OK );
} /* CopyExeDataTilEOF */

long AlignAmount( long offset, uint_16 shift_count )
/**************************************************/
{
    uint_32     low_bits;       /* low shift_count bits of offset */

    low_bits = offset & (0xffffffffUL >> (32 - shift_count));
    if( low_bits == 0 ) {
        return( 0 );
    } else {
        return( (0x1UL << shift_count) - low_bits );
    }
} /* AlignAmount */

static uint_32 FloorLog2( uint_32 value )
/****************************************
 * This calculates the floor of the log base 2 of value.
 * modified from binary_log function in wlink
 */
{
    uint_32 log;

    if( value == 0 ) {
        return( 0U );
    }
    log = 31;
    while( (value & 0x80000000UL) == 0 ) {  /* done if high bit on */
        value <<= 1;            /* shift left and decrease possible log. */
        log--;
    }
    return( log );
} /* FloorLog2 */

uint_16 FindShiftCount( uint_32 filelen, uint_16 numobjs )
/*********************************************************
 * filelen is the length of the file without any padding, numobjs is the
 * number of objects that must appear on an alignment boundary
 */
{
    uint_16     shift_old;
    uint_16     shift;

    if( filelen < 0x10000L ) {
        return( 0 );
    }

    shift_old = 16;
    shift = (uint_16)( FloorLog2( filelen + numobjs * (1L << shift_old) ) - 15 );
    /*
     * It is possible for the algorithm to blow up so don't check for != use <
     */
    while( shift_old > shift ) {
        shift_old = shift;
        shift = (uint_16)( FloorLog2( filelen + numobjs * (1L << shift_old) ) - 15 );
    }
    /*
     * In event of the rare case that the algorithm blew up take the min
     */
    if( shift > shift_old )
        shift = shift_old;
    return( shift );
} /* FindShiftCount */

RcStatus PadExeData( FILE *fp, long length )
/*******************************************
 * NB When an error occurs the function MUST return without altering errno
 */
{
    size_t  numwrite;

    memset( Pass2Info.IoBuffer, 0, IO_BUFFER_SIZE );
    for( numwrite = IO_BUFFER_SIZE; length > 0; length -= numwrite ) {
        if( numwrite > length )
            numwrite = length;
        if( RESWRITE( fp, Pass2Info.IoBuffer, numwrite ) != numwrite ) {
            return( RS_WRITE_ERROR );
        }
    }
    return( RS_OK );
} /* PadExeData */

void CheckDebugOffset( ExeFileInfo *exe )
/***************************************/
{
    uint_32     curroffset;

    curroffset = RESTELL( exe->fp );
    if( exe->DebugOffset < curroffset ) {
        exe->DebugOffset = curroffset;
    }
} /* CheckDebugOffset */

unsigned_32 OffsetFromRVA( ExeFileInfo *exe, pe_va rva )
/******************************************************/
{
    pe_object           *objects;
    unsigned_16         obj_cnt;
    unsigned            i;
    pe_exe_header       *pehdr;

    pehdr = exe->u.PEInfo.WinHead;
    obj_cnt = pehdr->fheader.num_objects;
    objects = exe->u.PEInfo.Objects;
    for( i = 0; i < obj_cnt; i++ ) {
        if( objects[i].rva == rva )
            break;
        if( objects[i].rva > rva ) {
            if( i != 0 )
                i--;
            break;
        }
    }
    if( i == obj_cnt )
        i--;
    if( objects[i].rva > rva )
        return( 0xFFFFFFFF );
    return( objects[i].physical_offset + rva - objects[i].rva );
}

RcStatus SeekRead( FILE *fp, long newpos, void *buff, size_t size )
/******************************************************************
 * NB When an error occurs the function MUST return without altering errno
 *
 * seek to a specified spot in the file, and read some data
 */
{
    size_t      numread;

    if( RESSEEK( fp, newpos, SEEK_SET ) )
        return( RS_READ_ERROR );
    numread = RESREAD( fp, buff, size );
    if( numread != size ) {
        return( RESIOERR( fp, numread ) ? RS_READ_ERROR : RS_READ_INCMPLT );
    }
    return( RS_OK );

} /* SeekRead */

/*
 * If the value at DOS_RELOC_OFFSET < NE_HEADER_OFFSET then the DOS reloction
 * information starts before the end of the address of the os2_exe_header
 * so this is not a valid windows EXE file.
 */

ExeType FindNEPELXHeader( FILE *fp, unsigned_32 *ne_header_off )
/***************************************************************
 * Determine type of executable
 */
{
    os2_exe_header  nehdr;
    unsigned_16     data;

    if( SeekRead( fp, 0, &data, sizeof( data ) ) != RS_OK
      || data != EXESIGN_DOS ) {
        return( EXE_TYPE_UNKNOWN );
    }
    if( SeekRead( fp, DOS_RELOC_OFFSET, &data, sizeof( data ) ) != RS_OK
      || !NE_HEADER_FOLLOWS( data ) ) {
        return( EXE_TYPE_UNKNOWN );
    }
    if( SeekRead( fp, NE_HEADER_OFFSET, ne_header_off, sizeof( *ne_header_off ) ) != RS_OK
      || *ne_header_off == 0 ) {
        return( EXE_TYPE_UNKNOWN );
    }
    if( SeekRead( fp, *ne_header_off, &data, sizeof( data ) ) != RS_OK )
        return( EXE_TYPE_UNKNOWN );

    switch( data ) {
    case EXESIGN_NE:
        if( SeekRead( fp, *ne_header_off, &nehdr, sizeof( nehdr ) ) != RS_OK )
            break;
        if( nehdr.target == TARGET_OS2 )
            return( EXE_TYPE_NE_OS2 );
        if( nehdr.target == TARGET_WINDOWS || nehdr.target == TARGET_WIN386 )
            return( EXE_TYPE_NE_WIN );
        break;
    case EXESIGN_PE:
        return( EXE_TYPE_PE );
    case EXESIGN_LX:
        return( EXE_TYPE_LX );
    }
    return( EXE_TYPE_UNKNOWN );
} /* FindNEPEHeader */
