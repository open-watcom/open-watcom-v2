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


#include "watcom.h"
#include <time.h>

#include "bool.h"
#include "align.h"
#ifdef __WATCOMC__
#pragma pack( 1 );
#endif

enum {
    OK = 0,
    AT_EOF = 1,
    IO_PROBLEM = 2
};

#define EXIT_OK 0
#define EXIT_FAILED -1

typedef unsigned char   uchar;
typedef unsigned char   byte;

/* LZSS Parameters */

#define N           4096    /* Size of string buffer */
#define F           60    /* Size of look-ahead buffer */
#define THRESHOLD   2
#define NIL         N    /* End of tree's node  */

/* Huffman coding parameters */

#define NUM_CHARS   (256 - THRESHOLD + F)
#define ROOT        0
#define MAX_CODE_BITS   16

// format of the archive header.

#define MAJOR_VERSION 1
#define MINOR_VERSION 1
#define SIGNATURE 0x2403

typedef struct {
    unsigned_16     signature   _ALIGN(1);
    byte            major_ver   _ALIGN(1);
    byte            minor_ver   _ALIGN(1);
    unsigned_16     num_files   _ALIGN(1); // number of files in the archive.
    unsigned_16     info_len    _ALIGN(1); // length of file information
    unsigned_32     info_offset _ALIGN(1); // offset of file information in file.
    unsigned_32     internal    _ALIGN(1); // deter access by old version
} arc_header;

typedef struct {
    unsigned_32     length      _ALIGN(1); // uncompressed length of the file
    unsigned_32     disk_addr   _ALIGN(1); // offset of information on disk
    unsigned_32     stamp       _ALIGN(1); // DOS date/time stamp
    unsigned_32     crc         _ALIGN(1); // 32-bit cyclic redundancy check.
    unsigned_8      namelen     _ALIGN(1); // length of the filename
    char            name[1]     _ALIGN(1); // file name (variable-length)
} file_info;

// the high order bit of the namelen field is used to indicate that there
// isn't any shannon-fano compression in the file.

#define NO_SHANNON_CODE 0x80
#define NAMELEN_MASK 0x7F

typedef struct info_list {
    struct info_list *  next;
    file_info           i;
} info_list;

// structure used to tell the encoder/decoder what to do.

typedef enum {
    KEEP_PATHNAME = 0x01,       // keep pathnames when archiving
    PACK_LIMIT    = 0x02,       // limit archives to specified size
    SECURE_PACK   = 0x04,       // use -i#### for weak security of archive
    BE_QUIET      = 0x08,       // don't print out information messages
    PREPEND_PATH  = 0x10,       // prepend path to output files
    REPLACE_PATH  = 0x20,       // replace path with specified one.
    PRESERVE_FNAME_CASE = 0x40, // preserve the file name case in the archive
    USE_DATE_TIME = 0x80        // use provided date & time.
} arcflags;

typedef struct {
    char        *filename;
    char        *packname;
} wpackfile;

typedef struct {
    wpackfile * files;      // list of files to be processed
    char *      arcname;    // name of archive
    arcflags    flags;      // what to do when packing/unpacking
    union {
        unsigned    limit;      // used in packing: max. size of indiv. archive
        char *      path;       // used when unpacking.
    } u;
    time_t      time;           // used when packing.
    unsigned_32 internal;       // used when -i option is implemented
} arccmd;

#define GLOBAL extern

#define _WPACK 1

#define _WINSTALL 0

extern void             SetupTextTable();
extern int              InitIO();
extern int              Decode( arccmd * );

#ifdef __WATCOMC__
#pragma pack();
#endif
