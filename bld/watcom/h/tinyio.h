/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS API (int 21h) in-line assembly wrappers.
*
****************************************************************************/


#ifndef _TINYIO_H_INCLUDED
#define _TINYIO_H_INCLUDED

#include "dosfuncx.h"
#include "watcom.h"
#include "descript.h"


/*
 * miscellaneous definitions
 */

/*
  open access mask

  bits  use
  ====  ===
  7     inheritance flag
  4-6   sharing mode
  3     reserved (=0)
  0-2   read/write access
*/
typedef enum {
    TIO_READ                    = 0x00,
    TIO_WRITE                   = 0x01,
    TIO_READ_WRITE              = 0x02,
    TIO_DENY_COMPATIBILITY      = 0x00,
    TIO_DENY_READ_WRITE         = 0x10,
    TIO_DENY_WRITE              = 0x20,
    TIO_DENY_READ               = 0x30,
    TIO_DENY_NONE               = 0x40,
    TIO_INHERITANCE             = 0x80,
    TIO_READ_DENY_WRITE         = TIO_READ | TIO_DENY_WRITE,
    TIO_NULL_ATTR               = 0x00
} open_attr;

typedef enum {
    TIO_SEEK_SET                = 0,
    TIO_SEEK_CUR                = 1,
    TIO_SEEK_END                = 2,
} seek_info;

typedef enum {
    TIO_STDIN_FILENO            = 0,
    TIO_STDOUT_FILENO           = 1,
    TIO_STDERR_FILENO           = 2,
    TIO_STDAUX_FILENO           = 3,
    TIO_STDPRN_FILENO           = 4
} tio_file_handles;

typedef enum {
    TIO_NORMAL              = 0x00,
    TIO_READ_ONLY           = 0x01,
    TIO_HIDDEN              = 0x02,
    TIO_SYSTEM              = 0x04,
    TIO_VOLUME_LABEL        = 0x08,
    TIO_SUBDIRECTORY        = 0x10,
    TIO_ARCHIVE             = 0x20,
} create_attr;

typedef enum {
    TIO_CREATE              = 0x01, /* from pg 3-112 of The Programmers */
    TIO_OPEN                = 0x10, /* PC Source Book */
    TIO_TRUNCATE            = 0x20,
} create_action;

typedef enum {
    TIO_F_OK                = 0x00,
    TIO_X_OK                = 0x01,
    TIO_W_OK                = 0x02,
    TIO_R_OK                = 0x04,
} access_mode;

#define TINY_IN     0
#define TINY_OUT    1
#define TINY_ERR    2

/*
 * return value from TinyGetDeviceInfo
 */
enum {
    TIO_CTL_CONSOLE_IN      = 0x0001,
    TIO_CTL_CONSOLE_OUT     = 0x0002,
    TIO_CTL_NULL            = 0x0004,
    TIO_CTL_CLOCK           = 0x0008,
    TIO_CTL_SPECIAL         = 0x0010, /* won't be supported in the future */
    TIO_CTL_RAW             = 0x0020,
    TIO_CTL_EOF             = 0x0040,
    TIO_CTL_DEVICE          = 0x0080,

    TIO_CTL_DISK_DRIVE_MASK = 0x001f, /* if ret & CTL_DEVICE == 0 */
};

/*
 * return values from calls (same names as OS/2 ERROR_* macros)
 */
enum {
    TIO_INVALID_FUNCTION    = 1,
    TIO_FILE_NOT_FOUND,
    TIO_PATH_NOT_FOUND,
    TIO_TOO_MANY_OPEN_FILES,
    TIO_ACCESS_DENIED,
    TIO_INVALID_HANDLE,
    TIO_ARENA_TRASHED,
    TIO_NOT_ENOUGH_MEMORY,
    TIO_INVALID_BLOCK,
    TIO_BAD_ENVIRONMENT,
    TIO_BAD_FORMAT,
    TIO_INVALID_ACCESS,
    TIO_INVALID_DATA,

    TIO_INVALID_DRIVE       = 15,
    TIO_CURRENT_DIRECTORY,
    TIO_NOT_SAME_DEVICE,
    TIO_NO_MORE_FILES,
    TIO_WRITE_PROTECT,
    TIO_BAD_UNIT,
    TIO_NOT_READY,
    TIO_BAD_COMMAND,
    TIO_CRC,
    TIO_BAD_LENGTH,
    TIO_SEEK,
    TIO_NOT_DOS_DISK,
    TIO_SECTOR_NOT_FOUND,
    TIO_OUT_OF_PAPER,
    TIO_WRITE_FAULT,
    TIO_READ_FAULT,
    TIO_GEN_FAILURE,
    TIO_SHARING_VIOLATION,
    TIO_LOCK_VIOLATION,
    TIO_WRONG_DISK,
    TIO_FCB_UNAVAILABLE,

    TIO_FILE_EXISTS         = 80,

    TIO_CANNOT_MAKE         = 82,
    TIO_FAIL_I24,

    TIO_FIND_ERROR          = TIO_FILE_NOT_FOUND,
    TIO_FIND_NO_MORE_FILES  = TIO_NO_MORE_FILES,
};

/*
 * return from TinyGetCountry
 */
typedef enum {
    TDATE_M_D_Y         = 0,
    TDATE_D_M_Y         = 1,
    TDATE_Y_M_D         = 2,
} date_format;

typedef enum {
    TTIME_12_HOUR       = 0,
    TTIME_24_HOUR       = 1,
} time_format;

enum {                          /* mask values for 'currency_symbol_position' */
    TPOSN_FOLLOWS_VALUE = 0x01, /* currency symbol follows value */
    TPOSN_ONE_SPACE     = 0x02, /* currency symbol is one space from value */
};

/*
 *  DOS FCB structure related definitions for TinyFCB... functions
 */
enum {
    TIO_PRSFN_IGN_SEPARATORS    = 0x01, /* if (separators) present ignore them*/
    TIO_PRSFN_DONT_OVW_DRIVE    = 0x02, /* leave drive in FCB unaltered if not*/
    TIO_PRSFN_DONT_OVW_FNAME    = 0x04, /* present in parsed string.  Same for*/
    TIO_PRSFN_DONT_OVW_EXT      = 0x08  /* fname and ext                      */
};


#pragma pack( __push, 1 )

/*
 * stuff for TinyGetFileStamp & TinySetFileStamp
 */

typedef struct {
    uint_16             twosecs : 5;    /* seconds / 2 */
    uint_16             minutes : 6;
    uint_16             hours   : 5;
} tiny_ftime_t;

typedef struct {
    uint_16             day     : 5;
    uint_16             month   : 4;
    uint_16             year    : 7;
} tiny_fdate_t;

typedef struct {
    tiny_ftime_t        time;
    tiny_fdate_t        date;
} tiny_file_stamp_t;

/*
 * format of DTA for TinyFindFirst/Next
 */
#define TIO_NAME_MAX    13  /* filename.ext\0 */
typedef struct {
    char                reserved[21];   /* dos uses this area */
    uint_8              attr;           /* attribute of file */
    tiny_ftime_t        time;
    tiny_fdate_t        date;
    uint_32             size;
    char                name[TIO_NAME_MAX];
} tiny_find_t;

/*
 * return from TinyGetDate and TinyGetTime
 */
typedef struct tiny_date_t {
    uint_8              day_of_month;   /* 1 - 31 */
    uint_8              month;          /* 1 - 12 */
    uint_8              year;           /* year minus 1900 */
    uint_8              day_of_week;    /* 0 - Sun, ..., 6 - Sat */
} tiny_date_t;

typedef struct tiny_time_t {
    uint_8              hundredths;     /* 0 - 99 */
    uint_8              seconds;        /* 0 - 59 */
    uint_8              minutes;        /* 0 - 59 */
    uint_8              hour;           /* 0 - 23 */
} tiny_time_t;

/*
 * return from TinyGetDOSVersion
 */
typedef struct tiny_dos_version {
    uint_8              major;
    uint_8              minor;
} tiny_dos_version;

typedef struct tiny_country_info_dos2 {
    uint_16             date_format;
    char                currency_symbol[2];
    char                thousands_separator[2];
    char                decimal_separator[2];
    char                reserved[24];
} tiny_country_info_dos2;

typedef struct tiny_country_info_dos3 {
    uint_16             date_format;
    char                currency_symbol[5];
    char                thousands_separator[2];
    char                decimal_separator[2];
    char                date_separator[2];
    char                time_separator[2];
    uint_8              currency_symbol_position;
    uint_8              decimal_places;
    uint_8              time_format;
    char                (__far *map_to_upper)( char );
    char                data_separator[2];
    char                reserved[10];
} tiny_country_info_dos3;

typedef union {
    tiny_country_info_dos2      ms2;
    tiny_country_info_dos3      ms3;
} tiny_country_info;

/*
 *  DOS FCB structure definitions for TinyFCB... functions
 */
#define TIO_EXTFCB_FLAG    0xff
typedef union {
    uint_8  extended_fcb_flag;  /* == TIO_EXTFCB_FLAG when extended */

    /* from MSDOS Encyclopedia pg 1473 */
    struct {
        uint_8          drive_identifier;   /* != TIO_EXTFCB_FLAG */
        char            filename[8];
        char            file_extension[3];
        uint_16         current_block_num;
        uint_16         record_size;
        uint_32         file_size;
        tiny_fdate_t    date_stamp;
        tiny_ftime_t    time_stamp;
        char            reserved[8];
        uint_8          current_record_num;
        uint_8          random_record_number[4];
    } normal;

    /* from MSDOS Encyclopedia pg 1476 */
    struct {
        uint_8          extended_fcb_flag;  /* == TIO_EXTFCB_FLAG */
        char            reserved1[5];
        uint_8          file_attribute;
        uint_8          drive_identifier;
        char            filename[8];
        char            file_extension[3];
        uint_16         current_block_num;
        uint_16         record_size;
        uint_32         file_size;
        tiny_fdate_t    date_stamp;
        tiny_ftime_t    time_stamp;
        char            reserved2[8];
        uint_8          current_record_num;
        uint_8          random_record_number[4];
    } extended;
} tiny_fcb_t;

/*
 * type definitions
 */
typedef int             tiny_handle_t;
typedef int_32          tiny_ret_t;

/*
 * pointer to data on the stack, used with TinyLSeek
 */
typedef uint_32 __based( __segname( "_STACK" ) )    *u32_stk_ptr;

#pragma pack( __pop )


/*
 * macro defintions
 */
#define TINY_ERROR( h )         ((int_32)(h)<0)
#define TINY_OK( h )            ((int_32)(h)>=0)
#define TINY_INFO( h )          ((uint_16)(h))
#define TINY_LINFO( h )         ((uint_32)(h))
/* 5-nov-90 AFS TinySeek returns a 31-bit offset that must be sign extended */
#define TINY_INFO_SEEK( h )     (((int_32)(h)^0xc0000000L)-0xc0000000L)


/*********************************************************
 * DOS functions related pragmas (INT 21h)
 ********************************************************/

/*
 * match up functions with proper pragma for memory model
 */
#define TinyFarOpen             _fTinyOpen
#define TinyFarCreate           _fTinyCreate
#define TinyFarCreateEx         _fTinyCreateEx
#define TinyFarCreateNew        _fTinyCreateNew
#define TinyFarCreateTemp       _fTinyCreateTemp
#define TinyClose               _TinyClose
#define TinyCommitFile          _TinyCommitFile
#define TinyFarWrite            _fTinyWrite
#define TinyFarRead             _fTinyRead
#define TinyFarDelete           _fTinyDelete
#define TinyFarRename           _fTinyRename
#define TinyFarMakeDir          _fTinyMakeDir
#define TinyFarRemoveDir        _fTinyRemoveDir
#define TinyFarChangeDir        _fTinyChangeDir
#define TinyDup                 _TinyDup
#define TinyDup2                _TinyDup2
#define TinyAllocBlock          _TinyAllocBlock
#define TinyTestAllocBlock      _TinyTestAllocBlock
#define TinyMaxAlloc            _TinyMaxAlloc
#define TinyFreeBlock           _TinyFreeBlock
#define TinySetBlock            _TinySetBlock
#define TinyMaxSet              _TinyMaxSet
#define TinyFarGetCWDir         _fTinyGetCWDir
#define TinyGetDeviceInfo       _TinyGetDeviceInfo
#define TinySetDeviceInfo       _TinySetDeviceInfo
#define TinyGetCtrlBreak        _TinyGetCtrlBreak
#define TinySetCtrlBreak        _TinySetCtrlBreak
#define TinyGetDate             _TinyGetDate
#define TinyGetTime             _TinyGetTime
#define TinyGetCurrDrive        _TinyGetCurrDrive
#define TinySetCurrDrive        _TinySetCurrDrive
#define TinyFarGetDTA           _TinyGetDTA
#define TinyFarChangeDTA        _TinyChangeDTA
#define TinyFarSetDTA           _fTinySetDTA
#define TinyFarFindFirst        _fTinyFindFirst
#define TinyFindNext            _TinyFindNext
#define TinyFindNextDTA         _TinyFindNextDTA
#define TinyFindCloseDTA        _TinyFindCloseDTA
#define TinyGetFileStamp        _TinyGetFileStamp
#define TinySetFileStamp        _TinySetFileStamp
#define TinyGetVect             _TinyGetVect
#define TinySetVect             _TinySetVect
#define TinyDOSVersion          _TinyDOSVersion
#define TinyGetCH               _TinyGetCH
#define TinyGetCHE              _TinyGetCHE
#define TinyGetSwitchChar       _TinyGetSwitchChar
#define TinySetSwitchChar       _TinySetSwitchChar
#define TinyFreeSpace           _TinyFreeSpace
#define TinySetCountry          _TinySetCountry
#define TinySetIntr             _TinySetIntr
#define TinyLock                _TinyLock
#define TinyUnlock              _TinyUnlock
#define TinyGetPSP              _TinyGetPSP
#define TinySetPSP              _TinySetPSP
#define TinyCreatePSP           _TinyCreatePSP
#define TinyFarGetFileAttr      _fTinyGetFileAttr
#define TinySeek                _TinySeek
#define TinyLSeek               _TinyLSeek
#define TinyTerminateProcess    _TinyTerminateProcess
#define TinyExit                _TinyTerminateProcess
#define TinySetMaxHandleCount   _TinySetMaxHandleCount

/* handle small/large data models */
#if defined( _M_I86SM ) || defined( _M_I86MM ) || defined( __386__ )

#define TinyBufferedInput       _nTinyBufferedInput
#define TinyOpen                _nTinyOpen
#define TinyCreate              _nTinyCreate
#define TinyCreateEx            _nTinyCreateEx
#define TinyCreateNew           _nTinyCreateNew
#define TinyCreateTemp          _nTinyCreateTemp
#define TinyWrite               _nTinyWrite
#define TinyRead                _nTinyRead
#define TinyDelete              _nTinyDelete
#define TinyRename              _nTinyRename
#define TinyMakeDir             _nTinyMakeDir
#define TinyRemoveDir           _nTinyRemoveDir
#define TinyChangeDir           _nTinyChangeDir
#define TinyGetCWDir            _nTinyGetCWDir
#define TinySetDTA              _nTinySetDTA
#define TinyFindFirst           _nTinyFindFirst
#define TinyFindFirstDTA        _nTinyFindFirstDTA
#define TinyGetFileAttr         _nTinyGetFileAttr
#define TinySetFileAttr         _nTinySetFileAttr
#define TinyGetCountry          _nTinyGetCountry
#define TinyFCBPrsFname         _nTinyFCBPrsFname
#define TinyFCBDeleteFile       _nTinyFCBDeleteFile

#else

#define TinyBufferedInput       _fTinyBufferedInput
#define TinyOpen                _fTinyOpen
#define TinyCreate              _fTinyCreate
#define TinyCreateEx            _fTinyCreateEx
#define TinyCreateNew           _fTinyCreateNew
#define TinyCreateTemp          _fTinyCreateTemp
#define TinyWrite               _fTinyWrite
#define TinyRead                _fTinyRead
#define TinyDelete              _fTinyDelete
#define TinyRename              _fTinyRename
#define TinyMakeDir             _fTinyMakeDir
#define TinyRemoveDir           _fTinyRemoveDir
#define TinyChangeDir           _fTinyChangeDir
#define TinyGetCWDir            _fTinyGetCWDir
#define TinySetDTA              _fTinySetDTA
#define TinyFindFirst           _fTinyFindFirst
#define TinyGetFileAttr         _fTinyGetFileAttr
#define TinySetFileAttr         _fTinySetFileAttr
#define TinyGetCountry          _fTinyGetCountry
#define TinyFCBPrsFname         _fTinyFCBPrsFname
#define TinyFCBDeleteFile       _fTinyFCBDeleteFile

#endif


/*********************************************************
 * BIOS absolute read/write related pragmas (INT 25h/26h)
 ********************************************************/

#define TinyFarAbsWrite         _fTinyAbsWrite
#define TinyFarAbsRead          _fTinyAbsRead
#if defined( _M_I86SM ) || defined( _M_I86MM ) || defined( __386__ )
#define TinyAbsWrite            _nTinyAbsWrite
#define TinyAbsRead             _nTinyAbsRead
#else
#define TinyAbsWrite            _fTinyAbsWrite
#define TinyAbsRead             _fTinyAbsRead
#endif


#define tiny_call


/*********************************************************
 * DOS functions related pragmas (INT 21h)
 ********************************************************/

/*
 *  Function prototypes (_f functions not supported under 386)
 *
 *  WARNING! Don't change a _n or _ prototype without verifying that it
 *      won't break the WINDOWS 386 library code!! DJG
 */
void                    _fTinyBufferedInput( char __far *__n );
void        tiny_call   _nTinyBufferedInput( char __near *__n );
tiny_ret_t              _fTinyOpen( const char __far *__n, open_attr __ax );
tiny_ret_t  tiny_call   _nTinyOpen( const char __near *__n, open_attr __ax );
tiny_ret_t              _fTinyCreate( const char __far *__n, create_attr __a );
tiny_ret_t  tiny_call   _nTinyCreate( const char __far *__n, create_attr __a );
tiny_ret_t              _fTinyCreateEx( const char __far *__n, open_attr __oa, create_attr __ca, create_action __act );
tiny_ret_t  tiny_call   _nTinyCreateEx( const char __far *__n, open_attr __oa, create_attr __ca, create_action __act );
tiny_ret_t              _fTinyCreateNew( const char __far *__n, create_attr __a );
tiny_ret_t  tiny_call   _nTinyCreateNew( const char __far *__n, create_attr __a );
tiny_ret_t              _fTinyCreateTemp( const char __far *__n, create_attr __a );
tiny_ret_t  tiny_call   _nTinyCreateTemp( const char __far *__n, create_attr __a );
tiny_ret_t  tiny_call   _TinyClose( tiny_handle_t );
tiny_ret_t  tiny_call   _TinyCommitFile( tiny_handle_t );
tiny_ret_t              _fTinyWrite( tiny_handle_t, const void __far *, uint );
tiny_ret_t  tiny_call   _nTinyWrite( tiny_handle_t, const void __near *, uint );
tiny_ret_t              _fTinyRead( tiny_handle_t, void __far *, uint );
tiny_ret_t  tiny_call   _nTinyRead( tiny_handle_t, void __near *, uint );
tiny_ret_t  tiny_call   _TinySeek( tiny_handle_t, uint_32, int_8 __where );
tiny_ret_t  tiny_call   _TinyLSeek( tiny_handle_t, uint_32, int_8 __where, u32_stk_ptr );
tiny_ret_t              _fTinyDelete( const char __far * );
tiny_ret_t  tiny_call   _nTinyDelete( const char __near * );
tiny_ret_t              _fTinyRename( const char __far *__o, const char __far *__n );
tiny_ret_t  tiny_call   _nTinyRename( const char __near *__o, const char __near *__n);
tiny_ret_t              _fTinyMakeDir( const char __far *__name );
tiny_ret_t  tiny_call   _nTinyMakeDir( const char __near *__name );
tiny_ret_t              _fTinyRemoveDir( const char __far *__name );
tiny_ret_t  tiny_call   _nTinyRemoveDir( const char __near *__name );
tiny_ret_t              _fTinyChangeDir( const char __far *__name );
tiny_ret_t  tiny_call   _nTinyChangeDir( const char __near *__name );
tiny_ret_t  tiny_call   _TinyDup( tiny_handle_t );
tiny_ret_t  tiny_call   _TinyDup2( tiny_handle_t __exist,tiny_handle_t __alias);
tiny_ret_t  tiny_call   _TinyAllocBlock( uint __paras );
tiny_ret_t  tiny_call   _TinyTestAllocBlock( uint __paras );
uint        tiny_call   _TinyMaxAlloc( void );
tiny_ret_t  tiny_call   _TinyFreeBlock( uint __seg );
tiny_ret_t  tiny_call   _TinySetBlock( uint __paras, uint __seg );
uint        tiny_call   _TinyMaxSet( uint __seg );
tiny_ret_t              _fTinyGetCWDir( char __far *__buff, uint_8 __drive );
tiny_ret_t  tiny_call   _nTinyGetCWDir( char __near *__buff, uint_8 __drive );
tiny_ret_t  tiny_call   _TinyGetDeviceInfo( tiny_handle_t __dev );
tiny_ret_t  tiny_call   _TinySetDeviceInfo( tiny_handle_t __dev, uint_8 __info);
uint_8      tiny_call   _TinyGetCtrlBreak( void );
void        tiny_call   _TinySetCtrlBreak( uint_8 __new_setting );
void        tiny_call   _TinyTerminateProcess( int_8 );
tiny_date_t tiny_call   _TinyGetDate( void );
tiny_time_t tiny_call   _TinyGetTime( void );
uint_8      tiny_call   _TinyGetCurrDrive( void );
void        tiny_call   _TinySetCurrDrive( uint_8 );
void          __far *   _TinyGetDTA( void );
void          __far *   _TinyChangeDTA( void __far * );
void                    _fTinySetDTA( void __far * );
void        tiny_call   _nTinySetDTA( void __near * );
tiny_ret_t              _fTinyFindFirst( const char __far *__pattern, create_attr __attr);
tiny_ret_t  tiny_call   _nTinyFindFirst( const char __near *, create_attr );
tiny_ret_t  tiny_call   _nTinyFindFirstDTA( const char __near *, create_attr, void * );
tiny_ret_t  tiny_call   _TinyFindNext( void );
tiny_ret_t  tiny_call   _TinyFindNextDTA( void * );
tiny_ret_t  tiny_call   _TinyFindCloseDTA( void * );
tiny_ret_t  tiny_call   _TinyGetFileStamp( tiny_handle_t );
tiny_ret_t  tiny_call   _TinySetFileStamp( tiny_handle_t, tiny_ftime_t __hms, tiny_fdate_t __ymd );
tiny_ret_t  tiny_call   _nTinyGetFileAttr( const char __near *__file );
tiny_ret_t              _fTinyGetFileAttr( const char __far *__file );
tiny_ret_t  tiny_call   _nTinySetFileAttr( const char __near *__file, create_attr );
tiny_ret_t              _fTinySetFileAttr( const char __far *__file, create_attr );
void        tiny_call   _TinySetIntr( uint_8, uint );
#ifdef __WINDOWS_386__
void        tiny_call   _TinySetVect( uint_8, void __near * );
void *      tiny_call   _TinyGetVect( uint_8 );
#else
void        tiny_call   _TinySetVect(uint_8, void (__far __interrupt *__f)());
void  (__far __interrupt * tiny_call _TinyGetVect( uint_8 ))();
#endif
tiny_dos_version  tiny_call _TinyDOSVersion( void );
uint_8      tiny_call   _TinyGetCH( void );
uint_8      tiny_call   _TinyGetCHE( void );
uint_8      tiny_call   _TinyGetSwitchChar( void );
void        tiny_call   _TinySetSwitchChar( char );
uint_32     tiny_call   _TinyFreeSpace( uint_8 );
tiny_ret_t  tiny_call   _nTinyGetCountry( const tiny_country_info __near * );
tiny_ret_t              _fTinyGetCountry( const tiny_country_info __far * );
tiny_ret_t  tiny_call   _TinySetCountry( uint_16 );
tiny_ret_t  tiny_call   _nTinyFCBPrsFname( const char __near *__str, tiny_fcb_t __far *__fcb, uint_8 __ctrl );
tiny_ret_t  tiny_call   _fTinyFCBPrsFname( const char __far *__str, tiny_fcb_t __far *__fcb, uint_8 __ctrl );
tiny_ret_t  tiny_call   _nTinyFCBDeleteFile( tiny_fcb_t __near *__fcb );
tiny_ret_t              _fTinyFCBDeleteFile( tiny_fcb_t __far *__fcb );
tiny_ret_t  tiny_call   _TinyLock(tiny_handle_t,uint_32 __start,uint_32 __l);
tiny_ret_t  tiny_call   _TinyUnlock(tiny_handle_t,uint_32 __start,uint_32 __l);
uint        tiny_call   _TinyGetPSP( void );
void        tiny_call   _TinySetPSP( uint_16 __seg );
void        tiny_call   _TinyCreatePSP( uint_16 __seg );
tiny_ret_t  tiny_call   _TinySetMaxHandleCount( uint_16 );

/*********************************************************
 * BIOS absolute read/write related pragmas (INT 25h/26h)
 ********************************************************/

tiny_ret_t  _fTinyAbsWrite( uint_8 __drive, uint __sector, uint __sectorcount, const void __far *__buff );
tiny_ret_t  _nTinyAbsWrite( uint_8 __drive, uint __sector, uint __sectorcount, const void __near *__buff );
tiny_ret_t  _fTinyAbsRead( uint_8 __drive, uint __sector, uint __sectorcount, const void __far *__buff );
tiny_ret_t  _nTinyAbsRead( uint_8 __drive, uint __sector, uint __sectorcount, const void __near *__buff );


/*********************************************************
 * in-line assembly instruction bytes definition
 ********************************************************/

#include "asmbytes.h"

#if defined( __WINDOWS_386__ )
 extern  void   __Int21( void );
 #pragma aux __Int21 "*"
 #define __INT_21       "call __Int21"
#else
 #define __INT_21       _INT 0x21
#endif


/*********************************************************
 * DOS functions related pragmas (INT 21h)
 ********************************************************/

#if defined( __386__ )

/***************************
 * 80386 versions of pragmas
 ***************************/

#pragma aux _TinyCreatePSP = \
        "pushfd"        \
        _MOV_AH DOS_CREATE_PSP \
        __INT_21        \
        "popfd"         \
    __parm __caller     [__dx] \
    __value             \
    __modify __exact    [__ax]

#pragma aux _TinySetPSP = \
        "pushfd"        \
        _MOV_AH DOS_SET_PSP \
        __INT_21        \
        "popfd"         \
    __parm __caller     [__bx] \
    __value             \
    __modify __exact    [__ah]

#pragma aux _TinyGetPSP = \
        _PUSHF          \
        "xor  eax,eax"  \
        _MOV_AH DOS_GET_PSP \
        __INT_21        \
        "mov  ax,bx"    \
        _POPF           \
    __parm __caller     [] \
    __value             [__eax] \
    __modify __exact    [__eax __ebx]

#pragma aux _TinySetMaxHandleCount = \
        _MOV_AH DOS_SET_HCOUNT \
        __INT_21        \
        _SBB_BX_BX      \
        _USE16 _AND_BX_AX \
    __parm __caller     [__bx] \
    __value             [__ebx] \
    __modify __exact    [__eax]

#pragma aux _nTinyBufferedInput = \
        _MOV_AH DOS_BUFF_INPUT \
        __INT_21        \
    __parm __caller     [__edx] \
    __value             \
    __modify __exact    [__ah]

#pragma aux _nTinyOpen = \
        _MOV_AH DOS_OPEN \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] [__al] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _nTinyCreate = \
        _MOV_AH DOS_CREAT \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] [__ecx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _nTinyCreateEx = \
        _MOV_AX_W 0 DOS_EXT_CREATE \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__esi] [__ebx] [__ecx] [__edx] \
    __value             [__eax] \
    __modify __exact    [__eax __ecx]

#pragma aux _nTinyCreateNew = \
        _MOV_AH DOS_CREATE_NEW \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] [__ecx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _nTinyCreateTemp = \
        _MOV_AH DOS_CREATE_TMP \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] [__ecx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyClose = \
        _MOV_AH DOS_CLOSE \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__bx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyCommitFile = \
        _MOV_AH DOS_COMMIT_FILE \
        "clc"           \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__bx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _nTinyWrite = \
        _MOV_AH DOS_WRITE \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__bx] [__edx] [__ecx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _fTinyWrite = \
        "push ds"       \
        "xchg edx,eax"  \
        "mov  ds,eax"   \
        _MOV_AH DOS_WRITE \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
        "pop  ds"       \
    __parm __caller     [__bx] [__dx __eax] [__ecx] \
    __value             [__eax] \
    __modify __exact    [__eax __edx]

#pragma aux _nTinyRead = \
        _MOV_AH DOS_READ \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__bx] [__edx] [__ecx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _fTinyRead = \
        "push ds"       \
        "xchg edx,eax"  \
        "mov  ds,eax"   \
        _MOV_AH DOS_READ \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
        "pop  ds"       \
    __parm __caller     [__bx] [__dx __eax] [__ecx] \
    __value             [__eax] \
    __modify __exact    [__eax __edx]

#pragma aux _TinyLSeek = \
        _MOV_AH DOS_LSEEK \
        "mov  ecx,edx"  \
        "shr  ecx,16"   \
        __INT_21        \
        "mov  ss:[edi],ax" \
        "mov  ss:2[edi],dx" \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__bx] [__edx] [__al] [__edi] \
    __value             [__eax] \
    __modify __exact    [__eax __ecx __edx]

#pragma aux _TinySeek = \
        _MOV_AH DOS_LSEEK \
        "mov  ecx,edx"  \
        "shr  ecx,16"   \
        __INT_21        \
        "rcl  dx,1"     \
        "ror  dx,1"     \
        "shl  edx,16"   \
        "mov  dx,ax"    \
    __parm __caller     [__bx] [__edx] [__al] \
    __value             [__edx] \
    __modify __exact    [__eax __ecx __edx]

#pragma aux _nTinyDelete = \
        _MOV_AH DOS_UNLINK \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] \
    __value             [__eax] \
    __modify __exact    [__eax]

// 06/22/95 T. Schiller
//
// The only reason we save/restore ebx is that in win386 the high word of
// ebx sometimes (one known cause is that the File Manager is running) gets
// trashed.
#pragma aux _nTinyRename = \
        "push ebx"      \
        "push es"       \
        "mov es,ecx"    \
        _MOV_AH DOS_RENAME \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
        "pop es"        \
        "pop ebx"       \
    __parm __caller     [__edx] [__cx __edi] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _nTinyMakeDir = \
        _MOV_AH DOS_MKDIR \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _nTinyRemoveDir = \
        _MOV_AH DOS_RMDIR \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _nTinyChangeDir = \
        _MOV_AH DOS_CHDIR \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _nTinyGetCWDir = \
        _MOV_AH DOS_GETCWD \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__esi] [__dl] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _fTinyGetCWDir = \
        "push ds"       \
        "mov  ds,ecx"   \
        _MOV_AH DOS_GETCWD \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
        "pop  ds"       \
    __parm __caller     [__cx __esi] [__dl] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyDup = \
        _MOV_AH DOS_DUP \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__bx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyDup2 = \
        _MOV_AH DOS_DUP2 \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__bx] [__cx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyAllocBlock = \
        _MOV_AH DOS_ALLOC_SEG \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__ebx] \
    __value             [__eax] \
    __modify __exact    [__eax __ebx]

#pragma aux _TinyTestAllocBlock = \
        _MOV_AH DOS_ALLOC_SEG \
        __INT_21        \
        "jnc short finish" \
        "mov  eax,ebx"   \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    "finish:"           \
    __parm __caller     [__ebx] \
    __value             [__eax] \
    __modify __exact    [__eax __ebx]

#pragma aux _TinyMaxAlloc = \
        "xor  ebx,ebx"   \
        "dec  ebx"       \
        _MOV_AH DOS_ALLOC_SEG \
        __INT_21        \
    __parm __caller     [] \
    __value             [__ebx] \
    __modify __exact    [__eax __ebx]

#pragma aux _TinyFreeBlock = \
        "push es"       \
        "mov  es,eax"    \
        _MOV_AH DOS_FREE_SEG \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
        "pop  es"        \
    __parm __caller     [__eax] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinySetBlock = \
        "push es"       \
        "mov  es,eax"   \
        _MOV_AH DOS_MODIFY_SEG \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
        "pop  es"       \
    __parm __caller     [__ebx] [__eax] \
    __value             [__eax] \
    __modify __exact    [__eax __ebx]

#pragma aux _TinyMaxSet = \
        "push es"       \
        "mov  es,eax"   \
        "xor  ebx,ebx"  \
        "dec  ebx"      \
        _MOV_AH DOS_MODIFY_SEG \
        __INT_21        \
        "pop  es"       \
    __parm __caller     [__eax] \
    __value             [__ebx] \
    __modify __exact    [__eax __ebx]

#pragma aux _TinyGetDeviceInfo = \
        _MOV_AX_W _GET_ DOS_IOCTL \
        __INT_21        \
        "rcl  edx,1"    \
        "ror  edx,1"    \
    __parm __caller     [__bx] \
    __value             [__edx] \
    __modify __exact    [__eax __edx]

#pragma aux _TinySetDeviceInfo = \
        "xor  dh,dh"    \
        _MOV_AX_W _SET_ DOS_IOCTL \
        __INT_21        \
        "rcl  edx,1"    \
        "ror  edx,1"    \
    __parm __caller     [__bx] [__dl] \
    __value             [__edx] \
    __modify __exact    [__eax __edx]

#pragma aux _TinyGetCtrlBreak = \
        _MOV_AX_W _GET_ DOS_CTRL_BREAK \
        __INT_21        \
    __parm __caller     [] \
    __value             [__dl] \
    __modify __exact    [__eax __dl]

#pragma aux _TinySetCtrlBreak = \
        _MOV_AX_W _SET_ DOS_CTRL_BREAK \
        __INT_21        \
    __parm __caller     [__dl] \
    __value             \
    __modify __exact    [__eax __dl]

#pragma aux _TinyTerminateProcess = \
        _MOV_AH DOS_EXIT \
        __INT_21        \
    __parm __caller     [__al] \
    __aborts

#pragma aux _TinyGetDate = \
        _MOV_AH DOS_GET_DATE \
        __INT_21        \
        "sub  cx,1900"  \
        "mov  ch,al"    \
        "shl  ecx,16"   \
        "mov  cx,dx"    \
    __parm __caller     [] \
    __value             [__ecx] \
    __modify __exact    [__eax __ecx __edx]

#pragma aux _TinyGetTime = \
        _MOV_AH DOS_GET_TIME \
        __INT_21        \
        "shl  ecx,16"   \
        "mov  cx,dx"    \
    __parm __caller     [] \
    __value             [__ecx] \
    __modify __exact    [__eax __ecx __edx]

#pragma aux _TinyGetCurrDrive = \
        _MOV_AH DOS_CUR_DISK \
        __INT_21        \
    __parm __caller     [] \
    __value             [__al] \
    __modify __exact    [__ax]

#pragma aux _TinySetCurrDrive = \
        _MOV_AH DOS_SET_DRIVE \
        __INT_21        \
    __parm __caller     [__dl] \
    __value             \
    __modify __exact    [__ax]

#pragma aux _TinyGetDTA = \
        "push es"       \
        _MOV_AH DOS_GET_DTA \
        __INT_21        \
        "mov  ecx,es"   \
        "pop  es"       \
    __parm __caller     [] \
    __value             [__cx __ebx] \
    __modify __exact    [__ah __ebx __ecx]

#pragma aux _TinyChangeDTA = \
        "push es"       \
        _MOV_AH DOS_GET_DTA \
        __INT_21        \
        "push ds"       \
        "mov  ds,ecx"   \
        _MOV_AH DOS_SET_DTA \
        __INT_21        \
        "pop  ds"       \
        "mov  ecx,es"   \
        "pop  es"       \
    __parm __caller     [__cx __edx] \
    __value             [__cx __ebx] \
    __modify __exact    [__ah __ebx __ecx]

#pragma aux _nTinySetDTA = \
        _MOV_AH DOS_SET_DTA \
        __INT_21        \
    __parm __caller     [__edx] \
    __value             \
    __modify __exact    [__ah]

#pragma aux _fTinySetDTA = \
        "push ds"       \
        "mov  ds,ecx"   \
        _MOV_AH DOS_SET_DTA \
        __INT_21        \
        "pop  ds"       \
    __parm __caller     [__cx __edx] \
    __value             \
    __modify __exact    [__ah]

#pragma aux _nTinyFindFirst = \
        _MOV_AH DOS_FIND_FIRST \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] [__cx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _nTinyFindFirstDTA = \
        _MOV_AH DOS_FIND_FIRST \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] [__cx] [__ebx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyFindNext = \
        _MOV_AH DOS_FIND_NEXT \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyFindNextDTA = \
        _MOV_AX_W 0 DOS_FIND_NEXT \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyFindCloseDTA = \
        _MOV_AX_W 1 DOS_FIND_NEXT \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__edx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinyGetFileStamp = \
        _MOV_AX_W _GET_ DOS_FILE_DATE \
        __INT_21        \
        "rcl  dx,1"      \
        "ror  dx,1"      \
        "shl  edx,16"    \
        "mov  dx,cx"     \
    __parm __caller     [__bx] \
    __value             [__edx] \
    __modify __exact    [__eax __ecx __edx]

#pragma aux _TinySetFileStamp = \
        _MOV_AX_W _SET_ DOS_FILE_DATE \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__bx] [__cx] [__dx] \
    __value             [__eax] \
    __modify __exact    [__eax]

#pragma aux _TinySetVect = \
        _SAVE_DSCX      \
        _MOV_AH DOS_SET_INT \
        __INT_21        \
        _REST_DS        \
    __parm __caller [__al] [__cx __edx] \
    __value         \
    __modify __exact [__ah _MODIF_DS]

#pragma aux _TinyGetVect = \
        _SAVE_ES        \
        _MOV_AH DOS_GET_INT \
        __INT_21        \
        _MOV_CX_ES      \
        _REST_ES        \
    __parm __caller [__al] \
    __value         [__cx __ebx] \
    __modify __exact [__eax __ebx __ecx _MODIF_ES]

#pragma aux _TinyLock = \
        "mov  edx,ecx"   \
        "shr  ecx,16"    \
        "mov  edi,esi"   \
        "shr  esi,16"    \
        _MOV_AX_W 0 DOS_RECORD_LOCK \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__bx] [__ecx] [__esi] \
    __value             [__eax] \
    __modify __exact    [__eax __ecx __edx __edi __esi]

#pragma aux _TinyUnlock = \
        "mov  edx,ecx"   \
        "shr  ecx,16"    \
        "mov  edi,esi"   \
        "shr  esi,16"    \
        _MOV_AX_W 1 DOS_RECORD_LOCK \
        __INT_21        \
        "rcl  eax,1"    \
        "ror  eax,1"    \
    __parm __caller     [__bx] [__ecx] [__esi] \
    __value             [__eax] \
    __modify __exact    [__eax __ecx __edx __edi __esi]

#elif defined( _M_I86 )

/**************************
 * 8086 versions of pragmas
 **************************/

#pragma aux _nTinyBufferedInput = \
        _MOV_AH DOS_BUFF_INPUT \
        __INT_21        \
    __parm __caller     [__dx] \
    __value             \
    __modify __exact    [__ah]

#pragma aux _fTinyBufferedInput = \
        _SET_DS_SREG_SAFE \
        _MOV_AH DOS_BUFF_INPUT \
        __INT_21        \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] \
    __value             \
    __modify __exact    [__ax]

#pragma aux _nTinyOpen = \
        _SET_DS_DGROUP_SAFE \
        _MOV_AH DOS_OPEN \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] [__al] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyOpen = \
        _SET_DS_SREG_SAFE \
        _MOV_AH DOS_OPEN \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] [__al] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _nTinyCreate = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_CREAT \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyCreate = \
        _SET_DS_SREG    \
        _MOV_AH DOS_CREAT    \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _nTinyCreateEx = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_EXT_CREATE \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__si] [__bx] [__cx] [__dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __cx __dx]

#pragma aux _fTinyCreateEx = \
        _SET_DS_SREG    \
        _MOV_AH DOS_EXT_CREATE \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __si] [__bx] [__cx] [__dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __cx __dx]

#pragma aux _nTinyCreateNew = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_CREATE_NEW \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyCreateNew = \
        _SET_DS_SREG    \
        _MOV_AH DOS_CREATE_NEW \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _nTinyCreateTemp = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_CREATE_TMP \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyCreateTemp = \
        _SET_DS_SREG    \
        _MOV_AH DOS_CREATE_TMP \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _TinyClose = \
        _MOV_AH DOS_CLOSE    \
        __INT_21        \
        _SBB_DX_DX      \
    __parm __caller     [__bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _TinyCommitFile = \
        _MOV_AH DOS_COMMIT_FILE    \
        _CLC            \
        __INT_21        \
        _SBB_DX_DX      \
    __parm __caller     [__bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _nTinyWrite = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_WRITE    \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__bx] [__dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _nTinyRead = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_READ    \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__bx] [__dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyWrite = \
        _SET_DS_SREG    \
        _MOV_AH DOS_WRITE    \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [__bx] [_SREG __dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyRead = \
        _SET_DS_SREG    \
        _MOV_AH DOS_READ    \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [__bx] [_SREG __dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _TinyLSeek = \
        _MOV_AH DOS_LSEEK \
        __INT_21        \
        "mov  ss:[di],ax" \
        "mov  ss:2[di],dx" \
        _RCL_DX_1       \
        _ROR_DX_1       \
    __parm __caller     [__bx] [__dx __cx] [__al] [__di] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _TinySeek = \
        _MOV_AH DOS_LSEEK \
        __INT_21        \
        _RCL_DX_1       \
        _ROR_DX_1       \
    __parm __caller     [__bx] [__dx __cx] [__al] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _nTinyGetFileAttr = \
        _SET_DS_DGROUP  \
        _MOV_AX_W _GET_ DOS_CHMOD \
        __INT_21        \
        _MOV_AX_CX      \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __cx __dx]

#pragma aux _nTinySetFileAttr = \
        _SET_DS_DGROUP  \
        _MOV_AX_W _SET_ DOS_CHMOD \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyGetFileAttr = \
        _SET_DS_SREG    \
        _MOV_AX_W _GET_ DOS_CHMOD \
        __INT_21        \
        _MOV_AX_CX      \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __cx __dx]

#pragma aux _fTinySetFileAttr = \
        _SET_DS_SREG    \
        _MOV_AX_W _SET_ DOS_CHMOD \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _nTinyDelete = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_UNLINK \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _nTinyRename = \
        _SET_DS_DGROUP  \
        _MOV_AX_SS      \
        _MOV_ES_AX      \
        _MOV_AH DOS_RENAME \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] [__di] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx __es]

#pragma aux _fTinyDelete = \
        _SET_DS_SREG    \
        _MOV_AH DOS_UNLINK \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyRename = \
        _SET_DS_SREG    \
        _MOV_ES_CX      \
        _MOV_AH DOS_RENAME \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] [__cx __di] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx __es]

#pragma aux _nTinyMakeDir = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_MKDIR \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _nTinyRemoveDir = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_RMDIR \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _nTinyChangeDir = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_CHDIR \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _nTinyGetCWDir = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_GETCWD \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__si] [__dl] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyMakeDir = \
        _SET_DS_SREG    \
        _MOV_AH DOS_MKDIR \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyRemoveDir = \
        _SET_DS_SREG    \
        _MOV_AH DOS_RMDIR \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyChangeDir = \
        _SET_DS_SREG    \
        _MOV_AH DOS_CHDIR \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyGetCWDir = \
        _SET_DS_SREG    \
        _MOV_AH DOS_GETCWD \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __si] [__dl] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx __si]

#pragma aux _TinyDup = \
        _MOV_AH DOS_DUP \
        __INT_21        \
        _SBB_BX_BX      \
    __parm __caller     [__bx] \
    __value             [__bx __ax] \
    __modify __exact    [__ax __bx]

#pragma aux _TinyDup2 = \
        _MOV_AH DOS_DUP2    \
        __INT_21        \
        _SBB_BX_BX      \
    __parm __caller     [__bx] [__cx] \
    __value             [__bx __ax] \
    __modify __exact    [__ax __bx]

#pragma aux _TinyAllocBlock = \
        _MOV_AH DOS_ALLOC_SEG    \
        __INT_21        \
        _SBB_BX_BX      \
    __parm __caller     [__bx] \
    __value             [__bx __ax] \
    __modify __exact    [__ax __bx]

#pragma aux _TinyTestAllocBlock = \
        _MOV_AH DOS_ALLOC_SEG    \
        __INT_21        \
        _SBB_DX_DX      \
        "jns short finish" \
        _MOV_AX_BX      \
    "finish:"           \
    __parm __caller     [__bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __bx __dx]

#pragma aux _TinyMaxAlloc = \
        _XOR_BX_BX      \
        _DEC_BX         \
        _MOV_AH DOS_ALLOC_SEG \
        __INT_21        \
    __parm __caller     [] \
    __value             [__bx] \
    __modify __exact    [__ax __bx]

#pragma aux _TinyFreeBlock = \
        _MOV_AH DOS_FREE_SEG \
        __INT_21        \
        _SBB_DX_DX      \
    __parm __caller     [__es] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _TinySetBlock = \
        _MOV_AH DOS_MODIFY_SEG    \
        __INT_21        \
        _SBB_BX_BX      \
    __parm __caller     [__bx] [__es] \
    __value             [__bx __ax] \
    __modify __exact    [__ax __bx]

#pragma aux _TinyMaxSet = \
        _XOR_BX_BX      \
        _DEC_BX         \
        _MOV_AH DOS_MODIFY_SEG    \
        __INT_21        \
    __parm __caller     [__es] \
    __value             [__bx] \
    __modify __exact    [__ax __bx]

#pragma aux _TinyGetDeviceInfo = \
        _MOV_AX_W _GET_ DOS_IOCTL \
        __INT_21        \
        _SBB_CX_CX      \
    __parm __caller     [__bx] \
    __value             [__cx __dx] \
    __modify __exact    [__ax __cx __dx]

#pragma aux _TinySetDeviceInfo = \
        _XOR_DH_DH      \
        _MOV_AX_W _SET_ DOS_IOCTL \
        __INT_21        \
        _SBB_CX_CX      \
    __parm __caller     [__bx] [__dl] \
    __value             [__cx __dx] \
    __modify __exact    [__ax __cx __dx]

#pragma aux _TinyGetCtrlBreak = \
        _MOV_AX_W _GET_ DOS_CTRL_BREAK \
        __INT_21        \
    __parm __caller     [] \
    __value             [__dl] \
    __modify __exact    [__ax __dl]

#pragma aux _TinySetCtrlBreak = \
        _MOV_AX_W _SET_ DOS_CTRL_BREAK \
        __INT_21        \
    __parm __caller     [__dl] \
    __value             \
    __modify __exact    [__ax __dl]

#pragma aux _TinyTerminateProcess = \
        _MOV_AH DOS_EXIT \
        __INT_21        \
    __parm __caller     [__al] \
    __aborts

#pragma aux _TinyGetDate = \
        _MOV_AH DOS_GET_DATE    \
        __INT_21        \
        _SUB_CX_N 0x6c 0x07 /* 1900 */ \
        _MOV_CH_AL      \
    __parm __caller     [] \
    __value             [__cx __dx] \
    __modify __exact    [__ax __cx __dx]

#pragma aux _TinyGetTime = \
        _MOV_AH DOS_GET_TIME \
        __INT_21        \
    __parm __caller     [] \
    __value             [__cx __dx] \
    __modify __exact    [__ax __cx __dx]

#pragma aux _TinyGetCurrDrive = \
        _MOV_AH DOS_CUR_DISK \
        __INT_21        \
    __parm __caller     [] \
    __value             [__al] \
    __modify __exact    [__ax]

#pragma aux _TinySetCurrDrive = \
        _MOV_AH DOS_SET_DRIVE \
        __INT_21        \
    __parm __caller     [__dl] \
    __value             \
    __modify __exact    [__ax __dl]

#pragma aux _nTinySetDTA = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_SET_DTA \
        __INT_21        \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] \
    __value             \
    __modify __exact    [__ax]

#pragma aux _nTinyFindFirst = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_FIND_FIRST \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _TinyGetDTA = \
        "push es"       \
        _MOV_AH DOS_GET_DTA \
        __INT_21        \
        "mov  cx,es"    \
        "pop  es"       \
    __parm __caller     [] \
    __value             [__cx __bx] \
    __modify __exact    [__ah __bx __cx]

#pragma aux _TinyChangeDTA = \
        "push es"       \
        _MOV_AH DOS_GET_DTA \
        __INT_21        \
        "push ds"       \
        "mov  ds,cx"    \
        _MOV_AH DOS_SET_DTA \
        __INT_21        \
        "pop  ds"       \
        "mov  cx,es"    \
        "pop  es"       \
    __parm __caller     [__cx __dx] \
    __value             [__cx __bx] \
    __modify __exact    [__ah __bx __cx]

#pragma aux _fTinySetDTA = \
        _SET_DS_SREG    \
        _MOV_AH DOS_SET_DTA \
        __INT_21        \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] \
    __value             \
    __modify __exact    [__ax]

#pragma aux _fTinyFindFirst = \
        _SET_DS_SREG    \
        _MOV_AH DOS_FIND_FIRST \
        __INT_21        \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] [__cx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _TinyFindNext = \
        _MOV_AH DOS_FIND_NEXT \
        __INT_21        \
        _SBB_DX_DX      \
    __parm __caller     [] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _TinyGetFileStamp = \
        _MOV_AX_W _GET_ DOS_FILE_DATE \
        __INT_21        \
        _SBB_BX_BX      \
        _OR_DX_BX       \
        _MOV_AX_CX      \
    __parm __caller     [__bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __bx __cx __dx]

#pragma aux _TinySetFileStamp = \
        _MOV_AX_W _SET_ DOS_FILE_DATE \
        __INT_21        \
        _SBB_DX_DX      \
    __parm __caller     [__bx] [__cx] [__dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _TinySetIntr = \
        _SAVE_DSCS      \
        _MOV_AH DOS_SET_INT \
        __INT_21        \
        _REST_DS         \
    __parm __caller     [__al] [__dx] \
    __value           \
    __modify __exact    [__ah _MODIF_DS]

#pragma aux _TinySetVect = \
        _SAVE_DSCX      \
        _MOV_AH DOS_SET_INT \
        __INT_21        \
        _REST_DS        \
    __parm __caller [__al] [__cx __dx] \
    __value         \
    __modify __exact [__ah _MODIF_DS]

#pragma aux _TinyGetVect = \
        _MOV_AH DOS_GET_INT \
        __INT_21        \
    __parm __caller     [__al] \
    __value             [__es __bx] \
    __modify __exact    [__ah __bx __es]

#pragma aux _TinyDOSVersion = \
        _MOV_AH DOS_GET_VERSION    \
        __INT_21        \
    __parm __caller     [] \
    __value             [__ax] \
    __modify __exact    [__ax __bx __cx]

#pragma aux _TinyGetCH = \
        _MOV_AH DOS_GET_CHAR_NO_ECHO_CHECK \
        __INT_21        \
    __parm __caller     [] \
    __value             [__al] \
    __modify __exact    [__ax]

#pragma aux _TinyGetCHE = \
        _MOV_AH DOS_GET_CHAR_ECHO_CHECK \
        __INT_21        \
    __parm __caller     [] \
    __value             [__al] \
    __modify __exact    [__ax]

#pragma aux _TinyGetSwitchChar = \
        _MOV_AX_W _GET_ DOS_SWITCH_CHAR   \
        __INT_21            \
    __parm __caller     [] \
    __value             [__dl] \
    __modify __exact    [__ax __dl]

#pragma aux _TinySetSwitchChar = \
        _MOV_AX_W _SET_ DOS_SWITCH_CHAR   \
        __INT_21            \
    __parm __caller     [__dl] \
    __value             \
    __modify __exact    [__ax __dl]

#pragma aux _TinyFreeSpace = \
        _MOV_AH DOS_DRIVE_FREE_SPACE \
        __INT_21        \
        _MUL_CX         \
        _MUL_BX         \
    __parm __caller     [__dl] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __bx __cx __dx]

#pragma aux _nTinyGetCountry = \
        _SET_DS_DGROUP  \
        _MOV_AX_W 0x00 DOS_COUNTRY_INFO \
        __INT_21        \
        _SBB_CX_CX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] \
    __value             [__cx __bx] \
    __modify __exact    [__ax __bx __cx]     /* note dx not modified */

#pragma aux _fTinyGetCountry = \
        _SET_DS_SREG    \
        _MOV_AX_W 0x00 DOS_COUNTRY_INFO \
        __INT_21        \
        _SBB_CX_CX      \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] \
    __value             [__cx __bx] \
    __modify __exact    [__ax __bx __cx]     /* note _SREG dx not modified */

#pragma aux _TinySetCountry = \
        _XOR_DX_DX      \
        _DEC_DX         \
        _MOV_AX_W 0xff DOS_COUNTRY_INFO \
        _TEST_BH_BH     \
        "jnz short finish" \
        _MOV_AL_BL      \
    "finish:"           \
        __INT_21        \
        _SBB_DX_DX      \
    __parm __caller     [__bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __bx __dx]

#pragma aux _nTinyFCBPrsFname = \
        _SET_DS_DGROUP_SAFE \
        _MOV_AH DOS_PARSE_FCB \
        __INT_21        \
        _CBW            \
        _CWD            \
        _RST_DS_DGROUP  \
    __parm __caller     [__si] [__es __di] [__al] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx __si]     /* note es di not modified */

#if defined( _M_I86MM ) || defined( _M_I86SM ) || defined(__SW_ZDP)
#pragma aux _fTinyFCBPrsFname = \
        _PUSH_DS        \
        _MOV_DS_DX      \
        _MOV_AH DOS_PARSE_FCB \
        __INT_21        \
        _CBW            \
        _CWD            \
        _POP_DS         \
    __parm __caller     [__dx __si] [__es __di] [__al] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx __si]     /* note es di not modified */
#else
#pragma aux _fTinyFCBPrsFname = \
        _MOV_AH DOS_PARSE_FCB \
        __INT_21        \
        _CBW            \
        _CWD            \
    __parm __caller     [__ds __si] [__es __di] [__al] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx __si]     /* note es di, ds not modified */
#endif

#pragma aux _nTinyFCBDeleteFile = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_DELETE_FCB \
        __INT_21        \
        _CBW            \
        _CWD            \
        _RST_DS_DGROUP  \
    __parm __caller     [__dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _fTinyFCBDeleteFile = \
        _SET_DS_SREG    \
        _MOV_AH DOS_DELETE_FCB \
        __INT_21        \
        _CBW            \
        _CWD            \
        _RST_DS_SREG    \
    __parm __caller     [_SREG __dx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#pragma aux _TinyLock = \
        _XCHG_SI_DI     \
        _MOV_AX_W 0 DOS_RECORD_LOCK \
        __INT_21        \
        _SBB_DX_DX      \
    __parm __caller     [__bx] [__cx __dx] [__di __si] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx __di __si]

#pragma aux _TinyUnlock = \
        _XCHG_SI_DI     \
        _MOV_AX_W 1 DOS_RECORD_LOCK \
        __INT_21        \
        _SBB_DX_DX      \
    __parm __caller     [__bx] [__cx __dx] [__si __di] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx __di __si]

#pragma aux _TinyCreatePSP = \
        _PUSHF          \
        _MOV_AH DOS_CREATE_PSP \
        __INT_21        \
        _POPF           \
    __parm __caller     [__dx] \
    __value             \
    __modify __exact    [__ax]

#pragma aux _TinySetPSP = \
        _PUSHF          \
        _MOV_AH DOS_SET_PSP \
        __INT_21        \
        _POPF           \
    __parm __caller     [__bx] \
    __value             \
    __modify __exact    [__ah]

#pragma aux _TinyGetPSP = \
        _PUSHF          \
        _MOV_AH DOS_GET_PSP \
        __INT_21        \
        _POPF           \
    __parm __caller     [] \
    __value             [__bx] \
    __modify __exact    [__ah __bx]

#pragma aux _TinySetMaxHandleCount = \
        _MOV_AH DOS_SET_HCOUNT \
        __INT_21        \
        "sbb  dx,dx"  \
    __parm __caller     [__bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __dx]

#endif


/*********************************************************
 * BIOS absolute read/write related pragmas (INT 25h/26h)
 ********************************************************/

#pragma aux _nTinyAbsRead = \
        _SET_DS_DGROUP_SAFE  \
        _INT_25         \
        _POP_DX         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__al] [__dx] [__cx] [__bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __bx __cx __dx __si __di]

#pragma aux _fTinyAbsRead = \
        _SET_DS_SREG_SAFE \
        _INT_25         \
        _POP_DX         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [__al] [__dx] [__cx] [_SREG __bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __bx __cx __dx __si __di]

#pragma aux _nTinyAbsWrite = \
        _SET_DS_DGROUP_SAFE \
        _INT_26         \
        _POP_DX         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
    __parm __caller     [__al] [__dx] [__cx] [__bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __bx __cx __dx __si __di]

#pragma aux _fTinyAbsWrite = \
        _SET_DS_SREG_SAFE \
        _INT_26         \
        _POP_DX         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
    __parm __caller     [__al] [__dx] [__cx] [_SREG __bx] \
    __value             [__dx __ax] \
    __modify __exact    [__ax __bx __cx __dx __si __di]


#endif
