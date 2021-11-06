/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  File I/O read processing
*
****************************************************************************/


#include "ftnstd.h"
#include <string.h>
#if defined( __WINDOWS__ )
    #include <conio.h>
#endif
#include "posio.h"
#include "sysbuff.h"
#include "posget.h"
#include "posseek.h"
#include "poserr.h"
#include "posflush.h"


size_t readbytes( b_file *io, char *buff, size_t len )
//====================================================
{
    size_t      bytes_read;
    size_t      total;
    size_t      amt;

    total = 0;
    amt = MAX_SYSIO_SIZE;
    while( len > 0 ) {
        if( amt > len )
            amt = len;
        bytes_read = read( io->handle, buff, amt );
        if( bytes_read == READ_ERROR ) {
            FSetSysErr( io );
            return( READ_ERROR );
        } else if( bytes_read == 0 ) {
            if( total != 0 )
                break;
            FSetEof( io );
            return( READ_ERROR );
        }
        io->phys_offset += (unsigned long)bytes_read;
        total += bytes_read;
        buff += bytes_read;
        len -= bytes_read;
        if( bytes_read < amt ) {
            break;
        }
    }
    return( total );
}


static  int     FillBuffer( b_file *io )
//======================================
{
    size_t      bytes_read;

    if( FlushBuffer( io ) < 0 )
        return( -1 );
    bytes_read = readbytes( io, io->buffer, io->buff_size );
    if( bytes_read == READ_ERROR )
        return( -1 );
    io->attrs |= READ_AHEAD;
    io->high_water = 0;
    io->read_len = bytes_read;
    return( 0 );
}


size_t SysRead( b_file *io, char *b, size_t len )
//===============================================
{
    size_t      bytes_read;
    size_t      amt;
    size_t      offs_in_b;
    size_t      max_valid;

    if( io->attrs & BUFFERED ) {
        // determine the maximum valid position in the buffer
        max_valid = io->read_len;
        if( max_valid < io->high_water ) {
            max_valid = io->high_water;
        }
        // if we're beyond that position then we must fill the buffer
        if( io->b_curs >= max_valid ) {
            if( FillBuffer( io ) < 0 )
                return( READ_ERROR );
            max_valid = io->read_len;
        }
        amt = max_valid - io->b_curs;
        if( amt > len ) {
            amt = len;
        }
        memcpy( b, &io->buffer[io->b_curs], amt );
        offs_in_b = amt;
        io->b_curs += amt;
        len -= amt;
        if( len ) {
            // flush the buffer
            if( FlushBuffer( io ) < 0 )
                return( READ_ERROR );
            if( len > io->buff_size ) {
                // read a multiple of io->buff_size bytes
                amt = len - len % io->buff_size;
                bytes_read = readbytes( io, b + offs_in_b, amt );
                if( bytes_read == READ_ERROR )
                    return( READ_ERROR );
                offs_in_b += bytes_read;
                if( bytes_read < amt )
                    return( offs_in_b );
                len -= amt;
            }
            if( len ) {
                // first fill the buffer
                bytes_read = readbytes( io, io->buffer, io->buff_size );
                if( bytes_read == READ_ERROR )
                    return( READ_ERROR );
                io->attrs |= READ_AHEAD;
                io->read_len = bytes_read;
                // then grab our bytes from it
                if( len > bytes_read ) {
                    len = bytes_read;
                }
                memcpy( b + offs_in_b, io->buffer, len );
                io->b_curs = len;
                offs_in_b += len;
            }
        }
        return( offs_in_b );
    } else {
        return( readbytes( io, b, len ) );
    }
}


static size_t GetTextRec( b_file *io, char *b, size_t len )
//=========================================================
// Get a record from a TEXT file.
{
    char        ch;
    size_t      read;
    char        rs[2];

    if( io->attrs & SEEK ) { // direct access
        if( SysRead( io, b, len ) == READ_ERROR )
            return( 0 );
        if( SysRead( io, rs, sizeof( char ) ) == READ_ERROR )
            return( 0 );
        if( rs[0] == CHAR_LF )
            return( len );
#if ! defined( __UNIX__ )
        if( rs[0] == CHAR_CR ) {
            if( SysRead( io, &rs[1], sizeof( char ) ) == READ_ERROR ) {
                return( 0 );
            }
            if( rs[1] == CHAR_LF )
                return( len );
            if( ( io->attrs & CARRIAGE_CONTROL ) && ( rs[1] == CHAR_FF ) ) {
                return( len );
            }
        }
#endif
        FSetErr( POSIO_BAD_RECORD, io );
        return( 0 );
    } else if( io->attrs & BUFFERED ) {
        char            *ptr;
        char            *stop;
        bool            seen_cr;
        bool            trunc;
        size_t          max_valid;

        // determine maximum valid position in the buffer
        max_valid = io->read_len;
        if( max_valid < io->high_water ) {
            max_valid = io->high_water;
        }
        stop = io->buffer + max_valid;
        ptr = io->buffer + io->b_curs;
        read = 0;
        seen_cr = false;
        trunc = false;
        for( ;; ) {
            if( ptr >= stop ) {
                io->b_curs = ptr - io->buffer;
                if( FillBuffer( io ) < 0 ) {
                    // we have to do this so that io->b_curs is set properly
                    // on end of file
                    ptr = io->buffer + io->b_curs;
                    if( read > 0 && io->stat == POSIO_EOF ) {
                        FSetIOOk( io );
                    }
                    break;
                }
                stop = io->buffer + io->read_len;
                ptr = io->buffer + io->b_curs;
            }
            ch = *ptr;
            ++ptr;
            if( ch == CHAR_LF )
                break;
            if( !seen_cr ) {
                if( ch == CHAR_CTRL_Z ) {
                    --ptr; // give back char so we don't read past EOF
                    if( read == 0 )
                        FSetEof( io );
                    break;
                }
                if( ch == CHAR_CR ) {
                    seen_cr = true;
                } else if( read < len ) {
                    b[read] = ch;
                    ++read;
                } else {
                    trunc = true;
                }
            } else {
                if( ch == CHAR_FF && (io->attrs & CARRIAGE_CONTROL) )
                    break;
                --ptr;  // give back the char
                seen_cr = false;
                if( read < len ) {
                    b[read] = CHAR_CR;
                    ++read;
                } else {
                    trunc = true;
                }
            }
        }
        io->b_curs = ptr - io->buffer;
        if( trunc ) {
            FSetTrunc( io );
        }
        return( read );
    } else {    // device (CON)
        read = 0;
        len = readbytes( io, b, len );
        if( len == READ_ERROR )
            return( 0 );
        for( ;; ) {
            if( read == len )
                break;
#if defined( __UNIX__ ) || defined( __NETWARE__ )
            if( *b == CHAR_LF )
                return( read );
#else
            if( *b == CHAR_CR ) {
                ++b;
                if( read == len - 1 )
                    break;
                if( *b == CHAR_LF )
                    return( read );
                --b;
            } else if( *b == CHAR_CTRL_Z ) {
                FSetEof( io );
                return( read );
            }
#endif
            ++b;
            ++read;
        }
        FSetTrunc( io );
        return( read );
    }
}


static size_t GetVariableRec( b_file *io, char *b, size_t len )
//=============================================================
// Get a record from a file with "variable" records.
{
    size_t      tag_len;
    unsigned_32 tag;
    unsigned_32 save_tag;

    if( SysRead( io, (char *)(&tag), sizeof( tag ) ) == READ_ERROR ) {
        return( 0 );
    }
    save_tag = tag;
    tag_len = tag & 0x7fffffff;
    if( tag_len > len ) {
        FSetTrunc( io );
        if( SysRead( io, b, len ) == READ_ERROR )
            return( 0 );
        if( SysSeek( io, (long)tag_len - (long)len, SEEK_CUR ) < 0 ) {
            FSetSysErr( io );
            return( 0 );
        }
    } else {
        if( SysRead( io, b, tag_len ) == READ_ERROR )
            return( 0 );
        len = tag_len;
    }
    if( SysRead( io, (char *)(&tag), sizeof( tag ) ) == READ_ERROR )
        return( 0 );
    if( tag != save_tag ) {
        FSetErr( POSIO_BAD_RECORD, io );
        return( 0 );
    }
    return( len );
}


static size_t GetFixedRec( b_file *io, char *b, size_t len )
//==========================================================
// Get a record from a file with "fixed" records.
{
    len = SysRead( io, b, len );
    if( len == READ_ERROR )
        return( 0 );
    return( len );
}


size_t FGetRec( b_file *io, char *b, size_t len )
//===============================================
// Get a record from a file.
{
    FSetIOOk( io );
    if( io->attrs & REC_TEXT )
        return( GetTextRec( io, b, len ) );
    if( io->attrs & REC_VARIABLE )
        return( GetVariableRec( io, b, len ) );
    return( GetFixedRec( io, b, len ) );
}

#if 0
size_t  GetRec( char *b, size_t len )
//===================================
// Get a record from standard input device.
{
    return( FGetRec( FStdIn, b, len ) );
}
#endif

char    GetStdChar( void )
//========================
// Get a character from standard input.
{
    char        ch;

#if defined( __WINDOWS__ )
    ch = getche();
    if( ch == CHAR_CR )
        return( CHAR_LF );
#else
    if( read( STDIN_FILENO, &ch, 1 ) < 0 )
        return( NULLCHAR );
  #if ! defined( __UNIX__ )
    if( ch == CHAR_CR ) {
        if( read( STDIN_FILENO, &ch, 1 ) < 0 ) {
            return( NULLCHAR );
        }
    }
  #endif
#endif
    return( ch );
}


int     FCheckLogical( b_file *io )
//=================================
{
    unsigned_32 tag;
    size_t      rc;

    rc = SysRead( io, (char *)(&tag), sizeof( tag ) );
    if( rc == READ_ERROR ) {
        if( io->stat != POSIO_EOF )
            return( -1 );
        // if an EOF occurs we've skipped the record
        FSetIOOk( io );
        return( 0 );
    }
    rc = 0;
    if( tag & 0x80000000 ) {
        rc = 1;
    }
    if( SysSeek( io, -(long)sizeof( unsigned_32 ), SEEK_CUR ) < 0 )
        return( -1 );
    return( rc );
}


int     FSkipLogical( b_file *io )
//================================
{
    unsigned_32 tag;
    unsigned_32 save_tag;
    size_t      rc;

    for(;;) {
        rc = SysRead( io, (char *)(&tag), sizeof( tag ) );
        if( rc == READ_ERROR ) {
            if( io->stat != POSIO_EOF )
                return( -1 );
            // if an EOF occurs we've skipped the record
            FSetIOOk( io );
            return( 0 );
        }
        if( (tag & 0x80000000) == 0 )
            break;
        save_tag = tag;
        tag &= 0x7fffffff;
        if( SysSeek( io, tag, SEEK_CUR ) < 0 )
            return( -1 );
        rc = SysRead( io, (char *)(&tag), sizeof( tag ) );
        if( rc == READ_ERROR )
            return( -1 );
        if( tag != save_tag ) {
            FSetErr( POSIO_BAD_RECORD, io );
            return( -1 );
        }
    }
    if( SysSeek( io, -(long)sizeof( unsigned_32 ), SEEK_CUR ) < 0 )
        return( -1 );
    return( 0 );
}


signed_32       FGetVarRecLen( b_file *io )
//=========================================
{
    unsigned_32 tag;
    unsigned_32 save_tag;
    int         rc;
    long        pos;
    unsigned_32 size = 0;

    pos = FGetFilePos( io );
    for( ;; ) {
        rc = SysRead( io, (char *)(&tag), sizeof( tag ) );
        if( rc == READ_ERROR ) {
            if( io->stat != POSIO_EOF )
                return( -1 );
            // if an EOF occurs we've skipped the record
            FSetIOOk( io );
            break;
        }
        save_tag = tag;
        if( !size ) {
            if( tag & 0x80000000 ) {
                FSetErr( POSIO_BAD_RECORD, io );
                return( -1 );
            }
        } else {
            if( tag & 0x80000000 ) {
                tag &= 0x7fffffff;
            } else {
                break;
            }
        }
        size += tag;
        if( SysSeek( io, tag, SEEK_CUR ) < 0 )
            return( -1 );
        rc = SysRead( io, (char *)(&tag), sizeof( tag ) );
        if( rc == READ_ERROR )
            return( -1 );
        if( tag != save_tag ) {
            FSetErr( POSIO_BAD_RECORD, io );
            return( -1 );
        }
    }
    if( SysSeek( io, pos, SEEK_SET ) < 0 )
        return( -1 );
    return( size );
}
