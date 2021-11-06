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
* Description:  Performs enhanced Unix cp file I/O.
*
****************************************************************************/


#include "cp.h"
#if defined( __WATCOMC__ )
    #include <utime.h>
#else
    #include <sys/utime.h>
#endif
#include "wio.h"

#include "clibext.h"


#if !defined( __NT__ )
#define INVALID_HANDLE_VALUE -1
#endif

/* forward declarations */
static int readABuffer( void );
static void freeCB( ctrl_block *cb, int freecb );

/*
 * GrabFile - read in a specified file, dump it to destination
 */
int GrabFile( char *src, struct stat *stat_s, char *dest, fattrs srcattr )
{
#if defined( __OS2__ ) && defined( __386__ )
    int                 result;
#else
    ctrl_block          *cb;
#endif
    fhandle             handle;
    int                 okay=true;
    timedate            td;
    unsigned            t = 0;
    unsigned            d = 0;

    /*
     * file handle
     */
#ifdef __NT__
    handle = CreateFile( src, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL );
    if( handle == INVALID_HANDLE_VALUE ) {
#else
    if( _dos_open( src, O_RDONLY, &handle ) ) {
#endif
        DropPrintALine( "Error opening file %s",src );
        IOError( errno );
    }

    /*
     * get time/date stamp of file
     */
    if( npflag || todflag ) {
#ifdef __NT__
        FILETIME    local_ft;
        fdt         fd;
        fdt         ft;

        GetFileTime( handle, NULL, NULL, &local_ft );
        FileTimeToDosDateTime( &local_ft, &fd, &ft );
        d = fd;
        t = ft;
#else
        _dos_getftime( handle, &d, &t );
#endif
        if( todflag ) {
            td.yy = (((d & 0xFE00) >> 9) + 1980);
            td.mm = ((d & 0x01E0 ) >> 5);
            td.dd = (d & 0x001F);
            td.hr = ((t & 0xF800) >> 11);
            td.min = ((t & 0x07E0) >> 5);
            td.sec = ((t & 0x001F) << 1);
            /*
             * see if this file passes the date checks
             */
            if( tflag2 ) {
                if( td.hr <= after_t_d.hr ) {
                    if( td.hr < after_t_d.hr ) {
                        return( false );
                    }
                    if( td.min <= after_t_d.min ) {
                        if( td.min < after_t_d.min ) {
                            return( false );
                        }
                        if( td.sec < after_t_d.sec ) {
                            return( false );
                        }
                    }
                }
            }
            if( Tflag1 ) {
                if( td.hr >= before_t_d.hr ) {
                    if( td.hr > before_t_d.hr ) {
                        return( false );
                    }
                    if( td.min >= before_t_d.min ) {
                        if( td.min > before_t_d.min ) {
                            return( false );
                        }
                        if( td.sec > before_t_d.sec ) {
                            return( false );
                        }
                    }
                }
            }
            if( dflag2 ) {
                if( td.yy <= after_t_d.yy ) {
                    if( td.yy < after_t_d.yy ) {
                        return( false );
                    }
                    if( td.mm <= after_t_d.mm ) {
                        if( td.mm < after_t_d.mm ) {
                            return( false );
                        }
                        if( td.dd < after_t_d.dd ) {
                            return( false );
                        }
                    }
                }
            }
            if( Dflag1 ) {
                if( td.yy >= before_t_d.yy ) {
                    if( td.yy > before_t_d.yy ) {
                        return( false );
                    }
                    if( td.mm >= before_t_d.mm ) {
                        if( td.mm > before_t_d.mm ) {
                            return( false );
                        }
                        if( td.dd > before_t_d.dd ) {
                            return( false );
                        }
                    }
                }
            }
        }
    }

#if defined( __OS2__ ) && defined( __386__ )

    /* unused parameters */ (void)srcattr;

    if( !sflag ) {
        PrintALineThenDrop( "Copying file %s to %s", src, dest );
    }
    _dos_close( handle );
    /*
     * perform the copy - OS/2 preserves attributes and time stamp
     */
    result = DosCopy( src, dest, DCPY_EXISTING );
    if( result ) {
        OS2Error( result );
    }
    /*
     * update the time stamp if required
     */
    if( !npflag ) {
        if( !(stat_s->st_mode & S_IWRITE) ) {
            chmod( dest, S_IWRITE | S_IREAD );
        }
        result = utime( dest, NULL );
        if( result < 0 ) {
            DropPrintALine( "Error updating time for %s", dest );
            IOError( errno );
        }
        if( pattrflag && !(stat_s->st_mode & S_IWRITE) ) {
            chmod( dest, S_IREAD );
        }
    } else {
        if( !pattrflag && !(stat_s->st_mode & S_IWRITE) ) {
            chmod( dest, S_IWRITE | S_IREAD );
        }
    }
#else
    /*
     * get memory for control block
     */
    cb = NearAlloc( sizeof( ctrl_block ) );
    cb->inname = NearAlloc( strlen( src ) + 1 );
    cb->outname = NearAlloc( strlen( dest ) + 1 );

    /*
     * set cb info
     */
    strcpy( cb->inname, src );
    strcpy( cb->outname, dest );
    cb->bytes_pending = stat_s->st_size;
    cb->head = cb->curr = NULL;
    cb->outhandle = INVALID_HANDLE_VALUE;
    cb->inhandle = handle;
    cb->prev = cb->next = NULL;
    cb->srcattr = srcattr;
    if( npflag ) {
        cb->t = (unsigned short)t;
        cb->d = (unsigned short)d;
    }
    if( !sflag ) {
        PrintALine( "reading file %s (%ld bytes)", cb->inname, cb->bytes_pending );
    }

    /*
     * chain into list of cb's
     */
    if( CBHead == NULL ) {
        CBHead = CBTail = cb;
    } else {
        CBTail->next = cb;
        cb->prev = CBTail;
        CBTail = cb;
    }

    /*
     * process the file
     */
    while( cb->bytes_pending ) {
        if( !readABuffer() ) {
            freeCB( cb, true );
            okay = false;
            break;
        }
    }
  #ifdef __NT__
    CloseHandle( cb->inhandle );
  #else
    _dos_close( cb->inhandle );
  #endif
#endif
    if( okay ) {
        FileCnt++;
    }
    return( okay );

} /* GrabFile */

#if defined( __OS2__ ) && defined( __386__ )

// copy by OS

#else

/*
 * readABuffer - read a data buffer
 */
static int readABuffer( void )
{
    char        __FAR *buff;
    mem_block   *mb;
    int         flushed = false;
    fsize       buffsize;
    fsize       bytes;

    /*
     * get buffer size
     */
    if( CBTail->bytes_pending > (long) MAXIOBUFFER ) {
        buffsize = MAXIOBUFFER;
    } else {
        buffsize = (unsigned) CBTail->bytes_pending;
    }

    /*
     * get buffer
     */
    while( (buff = FarAlloc( buffsize )) == NULL ) {
        if( flushed ) {
            Die( "Out of FAR memory!\n" );
        }
        FlushMemoryBlocks();
        if( !sflag ) {
            PrintALine( "continuing read of %s (%ld more bytes)", CBTail->inname, CBTail->bytes_pending );
        }
        flushed = true;
    }

    /*
     * read data
     */
#ifdef __NT__
    if( ReadFile( CBTail->inhandle, buff, buffsize, &bytes, NULL ) == 0 || bytes != buffsize ) {
#else
    if( _dos_read( CBTail->inhandle, buff, buffsize, &bytes ) || bytes != buffsize ) {
#endif
        DropPrintALine( "Read error on file %s", CBTail->inname );
        IOError( errno );
    }
    CBTail->bytes_pending -= (long)buffsize;

    /*
     * allocate memory block
     */
    mb = NearAlloc( sizeof( mem_block ) );
    mb->in_memory = true;
    mb->where.buffer = buff;
    mb->next = NULL;
    mb->buffsize = buffsize;
    if( CBTail->head == NULL ) {
        CBTail->head = CBTail->curr = mb;
    } else {
        CBTail->curr->next = mb;
        CBTail->curr = mb;
    }

    return( true );

} /* readABuffer */

#endif

/*
 * freeCB - free a control block
 */
static void freeCB( ctrl_block *cb, int freecb )
{
    mem_block   *mb,*tmb;

    /*
     * free all memory information
     */
    mb = cb->head;
    while( mb != NULL ) {
        tmb = mb->next;
        if( mb->in_memory ) {
            FarFree( mb->where.buffer );
        }
        NearFree( mb );
        mb = tmb;
    }

    /*
     * take ctrl block out of chain
     */
    if( !freecb ) {
        cb->head = NULL;
        return;
    }
    if( cb == CBHead ) {
        CBHead = cb->next;
    }
    if( cb == CBTail ) {
        CBTail = cb->prev;
    }
    if( cb->prev != NULL ) {
        cb->prev->next = cb->next;
    }
    if( cb->next != NULL ) {
        cb->next->prev = cb->prev;
    }
    NearFree( cb->inname );
    NearFree( cb->outname );
    NearFree( cb );

} /* freeCB */

/*
 * FlushMemoryBlocks - write all pending files
 */
void FlushMemoryBlocks()
{
    ctrl_block          *curr,*tmp;
    char                __FAR *buff = NULL;
    mem_block           *mb;
    long                total=0;
    fsize               bytes;
    unsigned long       timetaken;
    unsigned long       secs, hunds;

    curr = CBHead;
    if( curr == NULL ) {
        return;
    }
    while( curr != NULL ) {
        /*
         * open file if we have to
         */
        if( curr->outhandle == INVALID_HANDLE_VALUE ) {
#ifdef __NT__
            curr->outhandle = CreateFile( curr->outname, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
            if( curr->outhandle == INVALID_HANDLE_VALUE ) {
#else
            if( _dos_creat( curr->outname, _A_NORMAL, &curr->outhandle ) ) {
#endif
                DropPrintALine( "Error opening destination file %s",curr->outname );
                IOError( errno );
            }
            if( !sflag ) {
                PrintALine( "writing file %s", curr->outname );
            }
        } else {
            if( !sflag ) {
                PrintALine( "continuing write of %s", curr->outname );
            }
        }

        /*
         * write the buffer
         */
        mb = curr->head;
        while( mb != NULL ) {
            if( mb->in_memory ) {
                buff = mb->where.buffer;
            }
            total += (long) mb->buffsize;
#ifdef __NT__
            if( WriteFile( curr->outhandle, buff, mb->buffsize, &bytes, NULL ) == 0 || bytes != mb->buffsize ) {
#else
            if( _dos_write( curr->outhandle, buff, mb->buffsize, &bytes ) || bytes != mb->buffsize ) {
#endif
                DropPrintALine( "Error writing destination file %s",curr->outname );
                IOError( errno );
            }
            mb = mb->next;
        }
        if( curr->bytes_pending == 0 ) {
#ifdef __NT__
            if( npflag ) {
                FILETIME    local_ft;

                DosDateTimeToFileTime( curr->d, curr->t, &local_ft);
                SetFileTime( curr->outhandle, NULL, NULL, &local_ft );
            }
            CloseHandle( curr->outhandle );
            if( pattrflag ) {
                SetFileAttributes( curr->outname, curr->srcattr );
            }
#else
            if( npflag ) {
                _dos_setftime( curr->outhandle, curr->d, curr->t  );
            }
            _dos_close( curr->outhandle );
            if( pattrflag ) {
                _dos_setfileattr( curr->outname, curr->srcattr );
            }
#endif
        }

        /*
         * now get rid of the info with this block
         */
        tmp = curr;
        curr = curr->next;
        freeCB( tmp, (tmp->bytes_pending == 0L) );
    }

    if( !sflag ) {

        DumpCnt++;

        timetaken = (unsigned long)( clock() - StartTime );
        StartTime = clock();
        secs = ( timetaken / CLOCKS_PER_SEC );
        hunds = timetaken - secs * CLOCKS_PER_SEC;

        if( rflag ) {
            PrintALineThenDrop( " (%ld bytes), %u files written, %u dirs created in %lu.%lu seconds (dump %u)",
                            total, FileCnt, DirCnt, secs, hunds, DumpCnt );
        } else {
            PrintALineThenDrop( " (%ld bytes), %u files written in %lu.%02lu seconds (dump %u)",
                            total, FileCnt, secs, hunds, DumpCnt );
        }
        TotalBytes += total;
        TotalFiles += FileCnt;
        TotalDirs += DirCnt;
        TotalTime += timetaken;
        FileCnt = 0;
        DirCnt = 0;
    }

} /* FlushMemoryBlocks */
