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
* Description:  F77 I/O structures & constants definitions
*
****************************************************************************/

#ifndef _F77_FTNIO_H
#define _F77_FTNIO_H 1

#ifdef __MT__
    extern      void    (*_AccessFIO)( void );
    extern      void    (*_ReleaseFIO)( void );
    extern      void    (*_PartialReleaseFIO)( void );
#else
    #define     _AccessFIO()
    #define     _ReleaseFIO()
    #define     _PartialReleaseFIO()
#endif

#if _CPU == 8086
    #define _BadUnit( x )       ( ( x <  0 ) || ( x > 32727 ) )
    #define _BadRecl( x )       ( ( x <= 0 ) || ( x > 65535 ) )
    #define _BadBlockSize( x )  ( ( x <= 0 ) || ( x > 65535 ) )
#else
    #define _BadUnit( x )       ( x <  0 )
    #define _BadRecl( x )       ( x <= 0 )
    #define _BadBlockSize( x )  ( x <= 0 )
#endif
    #define _BadRec( x )        ( x <= 0 )

// In order to avoid hitting every runtime file we do this before prototyping
#if !defined( SetIOCB )
  #if !defined( _M_IX86 ) && defined( __RT__ )
    #define SetIOCB     __RT_SetIOCB
  #endif
#endif

extern  void            SetIOCB( void );
#define _SetIOCB()    SetIOCB();

// UNCOMMENT_TOKEN
// When extended stuff is implemented change EXTENDED_IO_WINDOW and
// XCOMPLEX_IO_WINDOW appropriately (IO_FIELD_BUFF should reflect new maximum)
#define INTEGER_IO_WINDOW       12      // free formatting INTEGER/LOGICAL
#define REAL_IO_WINDOW          16      // free formatting REAL
#define DOUBLE_IO_WINDOW        25      // free formatting DOUBLE PRECISION
#define EXTENDED_IO_WINDOW      25      // free formatting EXTENDED PRECISION
#define COMPLEX_IO_WINDOW       34      // free formatting COMPLEX
#define DCOMPLEX_IO_WINDOW      52      // free formatting DOUBLE COMPLEX
#define XCOMPLEX_IO_WINDOW      52      // free formatting EXTENDED COMPLEX

#define IO_FIELD_BUFF           52      // length of formatting buffer

typedef struct iocb {                   // global iocb structure
    struct ftnfile      *fileinfo;      // pointer to current ftnfile
    // the following fields are set by FIELD= directives
    unsigned_32         set_flags;      // what ptr's have been set
    string              PGM *filename;  // pointer to filename scb
    string              PGM *internal;  // pointer to internal file variable
    union fmt_desc      PGM *fmtptr;    // pointer for fmt=
    intstar4            PGM *iosptr;    // pointer to iostat=
    string              PGM *accptr;    // pointer to access=
    string              PGM *blnkptr;   // pointer to blank=
    string              PGM *formptr;   // pointer to form=
    intstar4            PGM *lenptr;    // pointer to recl=
    string              PGM *statptr;   // pointer to status=
    string              PGM *fmtdptr;   // pointer to formatted=
    string              PGM *nameptr;   // pointer to name=
    string              PGM *dirptr;    // pointer to direct=
    string              PGM *seqptr;    // pointer to sequential=
    string              PGM *cctrlptr;  // pointer to carriagecontrol=
    string              PGM *ufmtdptr;  // pointer to unformatted=
    string              PGM *recfmptr;  // pointer to recordtype=
    string              PGM *actptr;    // pointer to action=
    logstar4            PGM *nmdptr;    // pointer to named=
    intstar4            PGM *nrecptr;   // pointer to nextrec=
    intstar4            PGM *numbptr;   // pointer to unit=
    logstar4            PGM *exstptr;   // pointer to exist=
    logstar4            PGM *openptr;   // pointer to opened=
    intstar4            PGM *bsizeptr;  // pointer to blocksize=
    string              PGM *shareptr;  // pointer to share=
    int                 unitid;         // the unit number
    uint                blocksize;      // block size
    uint                recl;           // record length
    intstar4            recordnum;      // record number
    // the following are for internal use
    uint                rptnum;         // formatting repeat count
    int                 scale;          // formatting scale parameter
    uint                fmtlen;         // formatting length indicator
    int                 status;         // status indicator
    unsigned_32         elmts;          // number of records in internal file
    byte                iostmt;         // indicates the current io operation
    PTYPE               typ;            // type to be formatted
    unsigned_32         flags;          // i/o flags
    char                buffer[IO_FIELD_BUFF+1]; // buffer for formatting
    arr_desc            arr_desc;       // descriptor for array i/o
} iocb;

// i/o flags

#define BAD_RECL                0x00000001L // invalid "RECL=" specifier
#define BAD_REC                 0x00000002L // invalid "REC="  specifier
#define BAD_UNIT                0x00000004L // invalid "UNIT=" specifier
#define UNDCHK_IO               0x00000008L // undefined checking during i/o
#define DBLE_BYTE_CHAR          0x00000010L // 2nd byte of double-byte character
#define NML_DIRECTED            0x00000040L // NAMELIST-directied i/o
#define BAD_BLOCKSIZE           0x00000080L // invalid "BLOCKSIZE=" specifier
#define IOF_ACTIVE              0x00000100L // i/o is active
#define IOF_NOFMT               0x00000200L // UNFORMATTED I/O statement?
#define IOF_PLUS                0x00000400L // formatting
#define IOF_FMTDONE             0x00000800L // formatting complete?
#define IOF_FMTREP              0x00001000L // repeatable edit descriptor?
#define IOF_FMTREALPART         0x00002000L // formatting
#define IOF_OUTPT               0x00004000L // formatting for output?
#define IOF_NOCR                0x00008000L // no carriage return
#define IOF_6_PRECONNECTED      0x00010000L // unit 6 preconnected
#define IOF_5_PRECONNECTED      0x00020000L // unit 5 preconnected
#define IOF_SETIOCB             0x00040000L // IOCB initialized
#define IOF_EXTEND_FORMAT       0x00080000L // extend format code
#define IOF_ARRAY_IO            0x00100000L // array i/o
#define NML_CONTINUE            0x00200000L // continue NAMELIST input after
                                            // an error

// set_flags
#define SET_FILENAME            0x00000001L
#define SET_INTERNAL            0x00000002L
#define SET_ERRSTMT             0x00000004L
#define SET_EOFSTMT             0x00000008L
#define SET_FMTPTR              0x00000010L
#define SET_IOSPTR              0x00000020L
#define SET_ACCPTR              0x00000040L
#define SET_BLNKPTR             0x00000080L
#define SET_FORMPTR             0x00000100L
#define SET_LENPTR              0x00000200L
#define SET_FMTDPTR             0x00000400L
#define SET_NAMEPTR             0x00000800L
#define SET_DIRPTR              0x00001000L
#define SET_SEQPTR              0x00002000L
#define SET_CCTRLPTR            0x00004000L
#define SET_UFMTDPTR            0x00008000L
#define SET_RECFMPTR            0x00010000L
#define SET_ACTPTR              0x00020000L
#define SET_NMDPTR              0x00040000L
#define SET_NRECPTR             0x00080000L
#define SET_NUMBPTR             0x00100000L
#define SET_EXSTPTR             0x00200000L
#define SET_OPENPTR             0x00400000L
#define SET_BSIZEPTR            0x00800000L
#define SET_UNIT                0x01000000L
#define SET_BLOCKSIZE           0x02000000L
#define SET_RECL                0x04000000L
#define SET_RECORDNUM           0x08000000L
#define SET_STATPTR             0x10000000L
#define SET_SHARE               0x20000000L

// type of i/o operation to be performed

enum {
    IO_READ,
    IO_WRITE,
    IO_OPEN,
    IO_CLOSE,
    IO_BKSP,
    IO_EFILE,
    IO_REW,
    IO_INQ,
    IO_DATA
};

#define _LogicalRecordOrganization( fcb ) \
        ( (fcb->accmode != ACCM_DIRECT) && \
          (fcb->formatted == UNFORMATTED_IO) && \
          ((fcb->recfm == RECFM_VARIABLE) || (fcb->recfm == RECFM_DEFAULT)) )

#define _NoRecordOrganization( fcb ) \
        ( (fcb->accmode == ACCM_SEQUENTIAL) && \
          (fcb->formatted == UNFORMATTED_IO) && \
          (fcb->recfm == RECFM_FIXED) )

typedef struct ftnfile {
    struct ftnfile      *link;          // pointer to next ftnfile
    file_handle         fileptr;        // pointer to file
    string              PGM *internal;  // pointer to internal file
    char                *filename;      // pointer to filename string
    char                *buffer;        // buffer location
    unsigned_32         recnum;         // record number
    unsigned_32         eofrecnum;      // endfile record number
    uint                col;            // column number
    uint                len;            // length
    int                 error;          // error indicator
    uint                bufflen;        // record length
    uint                blocksize;      // block size
    int                 unitid;         // unit identifier
    uint                recfm;          // record format
    byte                action;         // read, write, or read/write
    byte                blanks;         // blanks indicator
    byte                status;         // status indicator
    byte                accmode;        // access mode
    byte                formatted;      // format indicator
    byte                cctrl;          // carriage control
    byte                share;          // share access
    byte                device;         // the device type of the file
    unsigned_8          flags;          // flags
} ftnfile;

#define FTN_EOF                 0x01    // eof flag
#define FTN_EXIST               0x02    // does file exist (FORTRAN)
#define FTN_FSEXIST             0x04    // does file exist in the file system
#define FTN_LOG_IO              0x08    // log i/o
#define FTN_LOGICAL_RECORD      0x10    // start logical record

#define ACCM_DEFAULT    0       // default access mode
#define ACCM_SEQUENTIAL 1       // sequential access mode
#define ACCM_DIRECT     2       // direct access mode
#define ACCM_APPEND     3       // append access mode

#define ACT_DEFAULT     0       // default action
#define ACTION_READ     1       // file is read-only
#define ACTION_WRITE    2       // file is write-only
#define ACTION_RW       3       // file is read or write

#define RECFM_DEFAULT   0       // use default record format
#define RECFM_FIXED     1       // record format is fixed
#define RECFM_VARIABLE  2       // record format is variable
#define RECFM_TEXT      3       // record format is text

#define CC_DEFAULT      0       // use default carriage control
#define CC_YES          1       // carriage control
#define CC_NO           2       // no carriage control

#define STATUS_DEFAULT  0       // file status at open is the default
#define STATUS_UNKNOWN  1       // file status at open is unknown
#define STATUS_OLD      2       // file status at open is old
#define STATUS_NEW      3       // file status at open is new
#define STATUS_SCRATCH  4       // file status at open is scratch

#define STATUS_KEEP     2       // file status at close is keep
#define STATUS_DELETE   3       // file status at close is delete

#define FORMATTED_DFLT  0       // i/o operation has default format
#define FORMATTED_IO    1       // i/o operation is formatted (default)
#define UNFORMATTED_IO  2       // i/o operation is unformatted

#define BLANK_DEFAULT   0       // blank default
#define BLANK_NULL      1       // blanks null
#define BLANK_ZERO      2       // blanks as zeros

#define SHARE_DEFAULT   0       // share default
#define SHARE_COMPAT    1       // compatibility mode
#define SHARE_DENYRW    2       // deny read and write access
#define SHARE_DENYWR    3       // deny write access
#define SHARE_DENYRD    4       // deny read access
#define SHARE_DENYNO    5       // allow read and write access

#define PREC_MAX_UNIT 999     // largest allowed unit number

// constants for various specifiers

enum {
    STAT_SPEC,                  // STATUS=
    ACC_SPEC,                   // ACCESS=
    FORM_SPEC,                  // FORM=
    BLNK_SPEC,                  // BLANK=
    CCTRL_SPEC,                 // CARRIAGECONTROL=
    RECFM_SPEC,                 // RECORDTYPE=
    ACTION_SPEC,                // ACTION=
    RECL_SPEC,                  // RECL=
    BLOCKSIZE_SPEC,             // BLOCKSIZE=
    SHARE_SPEC                  // SHARE=
};

#endif
