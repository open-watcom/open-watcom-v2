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


// WATLIBIO -- definitions for I/O routines supporting locate-mode input
//             and output
//
// 90/05/17 -- J.W.Welch        -- defined
// 90/09/04 -- J.W.Welch        -- added system-independent codes
// 91/02/26 -- J.W.Welch        -- added IO_DA definitions


typedef struct                      // FILE_DEFN (file control)
{   char _FARD *record_ptr;         // - current record
    unsigned record_size;           // - sizeof( current record )
    int      record_offset;         // - offset of rec. in block
    unsigned block_size;            // - size of block read
    unsigned block_capacity;        // - max. block size
    int file_handle;                // - file handle
    long block_posn;                // - position in file
    unsigned access_read   :1;      // - read-access permitted
    unsigned access_write  :1;      // - write-access permitted
    unsigned buffer_read   :1;      // - buffer in memory read
    unsigned buffer_wrote  :1;      // - buffer in memory written
    unsigned recs_fixed    :1;      // - fixed records (set by application)
    char file_name[ _MAX_PATH ];    // - file name
} FILE_DEFN;

typedef struct                      // IODA (direct-access control block)
{   unsigned index_size;            // - size of index
    unsigned record_count;          // - number records in file
    unsigned record_max;            // - max. size of record in file
    unsigned bucket_size;           // - bucket size (last segment of index)
    unsigned bound;                 // - bound for bucket size
    FILE_DEFN _FARD *file_defn;     // - file definition for file
    long index[1];                  // - index
} IODA;



// SYSTEM-INDEPENDENT OPEN MODES

enum                                // SYSTEM-INDEPENDENT OPEN INFORMATION
{   _SD_OPEN_READ                   // - read access
,   _SD_OPEN_WRITE                  // - write access
,   _SD_OPEN_APPEND                 // - append with write
,   _SD_BINARY                      // - binary
};


// FUNCTIONAL INTERFACE


FILE_DEFN _FARD *io_alloc_file_defn(        // ALLOCATE A FILE_DEFN
            unsigned size );                // - size of buffer or 0
int     io_close_read(                      // CLOSE INPUT FILE
            FILE_DEFN _FARD *ctl );         // - control information
int     io_close_read_file(                 // CLOSE INPUT FILE, DEALLOCATE
            FILE_DEFN _FARD *ctl );         // - control information
int     io_close_write(                     // CLOSE OUTPUT FILE
            FILE_DEFN _FARD *ctl );         // - control information
int     io_close_write_file(                // CLOSE OUTPUT FILE, DEALLOCATE
            FILE_DEFN _FARD *ctl );         // - control information
int     ioda_build_index(                   // BUILD IODA INDEX
            IODA _FARD *ioda_ctl,           // - ioda control block
            FILE_DEFN _FARD *ctl );         // - file control block
int     ioda_read(                          // READ USING IODA INDEX
            IODA _FARD *ioda_ctl,           // - ioda control block
            unsigned record_number );       // - record number
void    io_dealloc_file_defn(               // DE-ALLOCATE A FILE_DEFN
            FILE_DEFN _FARD *ctl );         // - control information
int     io_eof(                             // TEST FOR END-OF-FILE
            FILE_DEFN _FARD *ctl );         // - file definition
long    io_file_size(                       // GET FILE SIZE
            FILE_DEFN _FARD *ctl );         // - file definition
int     io_getrec(                          // GET NEXT RECORD
            FILE_DEFN _FARD *ctl );         // - control information
int     io_getrec_fixed(                    // GET NEXT RECORD (FIXED SIZE)
            FILE_DEFN _FARD *ctl,           // - file control information
            unsigned recsize );             // - record size
int     io_open_append(                     // OPEN OUTPUT FILE (APPEND)
            FILE_DEFN _FARD *ctl,           // - control information
            const char _FARD *filename );   // - file name
FILE_DEFN _FARD *io_open_append_file(       // OPEN APPEND FILE, ALLOC DEFN
            const char _FARD *filename );   // - file name
int     io_open_read(                       // OPEN INPUT FILE
            FILE_DEFN _FARD *ctl,           // - control information
            const char _FARD *filename );   // - file name
FILE_DEFN _FARD *io_open_read_file(         // OPEN INPUT FILE, ALLOC DEFN
            const char _FARD *filename );   // - file name
int     io_open_update(                     // OPEN UPDATE FILE
            FILE_DEFN _FARD *ctl,           // - control information
            const char _FARD *filename );   // - file name
FILE_DEFN _FARD *io_open_update_file(       // OPEN UPDATE FILE, ALLOC DEFN
            const char _FARD *filename );   // - file name
int     io_open_write(                      // OPEN OUTPUT FILE
            FILE_DEFN _FARD *ctl,           // - control information
            const char _FARD *filename );   // - file name
FILE_DEFN _FARD *io_open_write_file(        // OPEN OUTPUT FILE, ALLOC DEFN
            const char _FARD *filename );   // - file name
int     io_point(                           // POINT INPUT FILE
            FILE_DEFN _FARD *ctl,           // - control information
            long position );                // - seek address (in bytes)
int     io_read_blk(                        // READ NEXT FILE BLOCK
            FILE_DEFN _FARD *ctl );         // - control information
long    io_record_position(                 // GET RECORD SEEK POSITION
            FILE_DEFN _FARD *ctl );         // - control information
int     io_write_char(                      // WRITE CHARACTER
            FILE_DEFN _FARD *ctl,           // - control information
            char character );               // - the character
int     io_write_eor(                       // WRITE END OF RECORD
            FILE_DEFN _FARD *ctl );         // - control information
void _FARD *io_write_reserve(               // RESERVE SPACE TO BE WRITTEN
            FILE_DEFN _FARD *ctl,           // - file control information
            unsigned size );                // - size to reserve
int     io_write_string(                    // WRITE STRING
            FILE_DEFN _FARD *ctl,           // - control information
            const char _FARD *string );     // - the string
int     io_write_vector(                    // WRITE VECTOR
            FILE_DEFN _FARD *ctl,           // - control information
            const char _FARD *vector,       // - the character
            unsigned size );                // - vector's size

// FUNCTIONAL INTERFACE -- internal

int     io_check_update(                    // WRITE BUFFER IF UPDATED
            FILE_DEFN _FARD *ctl );         // - control information
int     io_check_read(                      // CHECK BUFFER HAS BEEN READ
            FILE_DEFN _FARD *ctl );         // - control information
int     io_close(                           // CLOSE FILE
            FILE_DEFN _FARD *ctl );         // - control information
int     io_open_init(                       // INITIALIZATION AFTER OPENING
            FILE_DEFN _FARD *ctl,           // - control information
            const char _FARD *filename,     // - filename
            unsigned open_mode,             // - open mode
            unsigned access_mode );         // - access mode
int     io_read_next(                       // READ NEXT BUFFER SEGMENT
            FILE_DEFN _FARD *ctl,           // - control information
            unsigned offset );              // - offset for read
int     io_seek(                            // LOW-LEVEL SEEK IN FILE
            FILE_DEFN _FARD *ctl,           // - control information
            long position );                // - seek address (in bytes)
int     io_shift_buffer(                    // SHIFT BUFFER, READ REMAINDER
            FILE_DEFN _FARD *fd,            // - file definition
            unsigned size_left );           // - size remaining
int     io_write_next(                      // WRITE NEXT BUFFER
            FILE_DEFN _FARD *ctl );         // - control information
