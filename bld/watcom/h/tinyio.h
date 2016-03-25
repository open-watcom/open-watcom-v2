/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2003-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS and DPMI interrupt interfacing.
*
****************************************************************************/


#ifndef _TINYIO_H_INCLUDED
#define _TINYIO_H_INCLUDED

#if defined(__SW_ZDP) && !defined(ZDP)
#define ZDP
#endif

#include <dosfunc.h>
#include "watcom.h"

#define DOS_GET_DTA     0x2F

#pragma pack( 1 )

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
    TIO_SEEK_START              = 0,
    TIO_SEEK_SET                = 0,
    TIO_SEEK_CURR               = 1,
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
    char                reserved[ 21 ]; /* dos uses this area */
    uint_8              attr;           /* attribute of file */
    tiny_ftime_t        time;
    tiny_fdate_t        date;
    uint_32             size;
    char                name[ TIO_NAME_MAX ];
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
 * call_struct definition for TinyDPMISimulateRealInt
 */
typedef struct {
    uint_32     edi;
    uint_32     esi;
    uint_32     ebp;
    uint_32     reserved;
    union {
        uint_32 ebx;
        uint_16 bx;
    };
    union {
        uint_32 edx;
        uint_16 dx;
    };
    union {
        uint_32 ecx;
        uint_16 cx;
    };
    union {
        uint_32 eax;
        uint_16 ax;
    };
    uint_16     flags;
    uint_16     es;
    uint_16     ds;
    uint_16     fs;
    uint_16     gs;
    uint_16     ip;
    uint_16     cs;
    uint_16     sp;
    uint_16     ss;
} call_struct;

/* Definitions for manipulating protected mode descriptors ... used
 * with TinyDPMIGetDescriptor, TinyDPMISetDescriptor, TinyDPMISetRights, etc.
 */
typedef enum {
    TIO_ACCESSED        = 0x01,
    TIO_RDWR            = 0x02,
    TIO_EXPAND_DOWN     = 0x04,
    TIO_EXECUTE         = 0x08,
    TIO_MUST_BE_1       = 0x10,
    TIO_PRESENT         = 0x80,
    TIO_USER_AVAIL      = 0x1000,
    TIO_MUST_BE_0       = 0x2000,
    TIO_USE32           = 0x4000,
    TIO_PAGE_GRANULAR   = 0x8000
} tiny_dscp_flags;

typedef struct {
    uint_8      accessed : 1;
    uint_8      rdwr     : 1;
    uint_8      exp_down : 1;
    uint_8      execute  : 1;
    uint_8      mustbe_1 : 1;
    uint_8      dpl      : 2;
    uint_8      present  : 1;
} tiny_dscp_type;

typedef struct {
    uint_8               : 4;
    uint_8      useravail: 1;
    uint_8      mustbe_0 : 1;
    uint_8      use32    : 1;
    uint_8      page_gran: 1;
} tiny_dscp_xtype;

typedef struct {
    uint_16             lim_0_15;
    uint_16             base_0_15;
    uint_8              base_16_23;
    tiny_dscp_type      type;
    union {
        struct {
            uint_8      lim_16_19: 4;
            uint_8               : 4;
        };
        tiny_dscp_xtype xtype;
    };
    uint_8              base_24_31;
} tiny_dscp;

/*
 *  DOS FCB structure definitions for TinyFCB... functions
 */
#define TIO_EXTFCB_FLAG    0xff
typedef union {
    uint_8  extended_fcb_flag;  /* == TIO_EXTFCB_FLAG when extended */

    /* from MSDOS Encyclopedia pg 1473 */
    struct {
        uint_8          drive_identifier;   /* != TIO_EXTFCB_FLAG */
        char            filename[ 8 ];
        char            file_extension[ 3 ];
        uint_16         current_block_num;
        uint_16         record_size;
        uint_32         file_size;
        tiny_fdate_t    date_stamp;
        tiny_ftime_t    time_stamp;
        char            reserved[ 8 ];
        uint_8          current_record_num;
        uint_8          random_record_number[ 4 ];
    } normal;

    /* from MSDOS Encyclopedia pg 1476 */
    struct {
        uint_8          extended_fcb_flag;  /* == TIO_EXTFCB_FLAG */
        char            reserved1[ 6 ];
        create_attr     file_attribute;
        uint_8          drive_identifier;
        char            filename[ 8 ];
        char            file_extension[ 3 ];
        uint_16         current_block_num;
        uint_16         record_size;
        uint_32         file_size;
        tiny_fdate_t    date_stamp;
        tiny_ftime_t    time_stamp;
        char            reserved2[ 9 ];
        uint_8          current_record_num;
        uint_8          random_record_number[ 4 ];
    } extended;
} tiny_fcb_t;

enum {
    TIO_PRSFN_IGN_SEPARATORS    = 0x01, /* if (separators) present ignore them*/
    TIO_PRSFN_DONT_OVW_DRIVE    = 0x02, /* leave drive in FCB unaltered if not*/
    TIO_PRSFN_DONT_OVW_FNAME    = 0x04, /* present in parsed string.  Same for*/
    TIO_PRSFN_DONT_OVW_EXT      = 0x08  /* fname and ext                      */
};

/*
 * type definitions
 */
typedef int             tiny_handle_t;
typedef int_32          tiny_ret_t;

/*
 * pointer to data on the stack, used with TinyLSeek
 */
typedef uint_32 __based( __segname( "_STACK" ) )    *u32_stk_ptr;

/*
 * macro defintions
 */
#define TINY_ERROR( h )         ((int_32)(h)<0)
#define TINY_OK( h )            ((int_32)(h)>=0)
#define TINY_INFO( h )          ((uint_16)(h))
#define TINY_LINFO( h )          ((uint_32)(h))
/* 5-nov-90 AFS TinySeek returns a 31-bit offset that must be sign extended */
#define TINY_INFO_SEEK( h )     (((int_32)(h)^0xc0000000L)-0xc0000000L)

/*
 * match up functions with proper pragma for memory model
 */
#define TinyFarAccess           _fTinyAccess
#define TinyFarOpen             _fTinyOpen
#define TinyFarCreate           _fTinyCreate
#define TinyFarCreateEx         _fTinyCreateEx
#define TinyFarCreateNew        _fTinyCreateNew
#define TinyFarCreateTemp       _fTinyCreateTemp
#define TinyClose               _TinyClose
#define TinyCommitFile          _TinyCommitFile
#define TinyFarWrite            _fTinyWrite
#define TinyFarRead             _fTinyRead
#define TinyFarAbsWrite         _fTinyAbsWrite
#define TinyFarAbsRead          _fTinyAbsRead
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
#define TinyDPMISetRealVect     _TinyDPMISetRealVect
#define TinyDPMIGetRealVect     _TinyDPMIGetRealVect
#define TinyDPMISetProtectVect  _TinyDPMISetProtectVect
#define TinyDPMIGetProtectVect  _TinyDPMIGetProtectVect
#define TinyDPMISetProtectExcpt _TinyDPMISetProtectExcpt
#define TinyDPMIGetProtectExcpt _TinyDPMIGetProtectExcpt
#define TinyDPMIRawRMtoPMAddr   _TinyDPMIRawRMtoPMAddr
#define TinyDPMIRawPMtoRMAddr   _TinyDPMIRawPMtoRMAddr
#define TinyDPMISaveRMStateAddr _TinyDPMISaveRMStateAddr
#define TinyDPMISavePMStateAddr _TinyDPMISavePMStateAddr
#define TinyDPMISaveStateSize   _TinyDPMISaveStateSize
#define TinyDPMIBase            _TinyDPMIBase
#define TinyDPMISetBase         _TinyDPMISetBase
#define TinyDPMISegToSel        _TinyDPMISegToSel
#define TinyDPMICreateSel       _TinyDPMICreateSel
#define TinyDPMICreateCSAlias   _TinyDPMICreateCSAlias
#define TinyDPMIFreeSel         _TinyDPMIFreeSel
#define TinyDPMISetLimit        _TinyDPMISetLimit
#define TinyDPMISetRights       _TinyDPMISetRights
#define TinyDPMIGetDescriptor   _TinyDPMIGetDescriptor
#define TinyDPMISetDescriptor   _TinyDPMISetDescriptor

/* handle small/large data models */
#if defined( _M_I86SM ) || defined( _M_I86MM ) || defined( __386__ )

#define TinyAccess              _nTinyAccess
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
#define TinyAbsWrite            _nTinyAbsWrite
#define TinyAbsRead             _nTinyAbsRead
#define TinyMemAlloc            _TinyMemAlloc
#define TinyDPMIAlloc(x)        _TinyDPMIAlloc((x) >> 16, (x))
#define TinyDPMIRealloc(addr,x) _TinyDPMIRealloc(addr, (x) >> 16, (x))
#define TinyDPMIFree(x)         _TinyDPMIFree((x) >> 16, (x))
#define TinyCBAlloc             _TinyCBAlloc
#define TinyDPMIDOSAlloc        _TinyDPMIDOSAlloc
#define TinyDPMIDOSFree         _TinyDPMIDOSFree
#define TinyDPMISimulateRealInt _TinyDPMISimulateRealInt
#define TinyDPMICallRealIntFrame _TinyDPMICallRealIntFrame
#define TinyDPMICallRealFarFrame _TinyDPMICallRealFarFrame

#else

#define TinyAccess              _fTinyAccess
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
#define TinyAbsWrite            _fTinyAbsWrite
#define TinyAbsRead             _fTinyAbsRead

#endif

#define tiny_call

/*
 *  Function prototypes (_f functions not supported under 386)
 *
 *  WARNING! Don't change a _n or _ prototype without verifying that it
 *      won't break the WINDOWS 386 library code!! DJG
 */
tiny_ret_t              _fTinyAccess( const char __far *__n, access_mode __amode );
tiny_ret_t  tiny_call   _nTinyAccess( const char __near *__n, access_mode __amode );
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
tiny_ret_t              _fTinyAbsWrite( uint_8 __drive, uint __sector,
                                uint __sectorcount, const void __far *__buff );
tiny_ret_t              _nTinyAbsWrite( uint_8 __drive, uint __sector,
                                uint __sectorcount, const void __near *__buff );
tiny_ret_t              _fTinyAbsRead( uint_8 __drive, uint __sector,
                                uint __sectorcount, const void __far *__buff );
tiny_ret_t              _nTinyAbsRead( uint_8 __drive, uint __sector,
                                uint __sectorcount, const void __near *__buff );
tiny_ret_t  tiny_call   _nTinyWrite( tiny_handle_t, const void __near *, uint );
tiny_ret_t              _fTinyRead( tiny_handle_t, void __far *, uint );
tiny_ret_t  tiny_call   _nTinyRead( tiny_handle_t, void __near *, uint );
tiny_ret_t  tiny_call   _TinySeek( tiny_handle_t, uint_32, uint_16 __ax );
tiny_ret_t  tiny_call   _TinyLSeek( tiny_handle_t, uint_32, uint_16 __ax, u32_stk_ptr );
tiny_ret_t              _fTinyDelete( const char __far * );
tiny_ret_t  tiny_call   _nTinyDelete( const char __near * );
tiny_ret_t              _fTinyRename( const char __far *__o,
                                const char __far *__n );
tiny_ret_t  tiny_call   _nTinyRename( const char __near *__o,
                                const char __near *__n);
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
void        tiny_call   _TinyTerminateProcess( uint_16 __ax );
tiny_date_t tiny_call   _TinyGetDate( void );
tiny_time_t tiny_call   _TinyGetTime( void );
uint_8      tiny_call   _TinyGetCurrDrive( void );
void        tiny_call   _TinySetCurrDrive( uint_8 );
void          __far *   _TinyGetDTA( void );
void          __far *   _TinyChangeDTA( void __far * );
void                    _fTinySetDTA( void __far * );
void        tiny_call   _nTinySetDTA( void __near * );
tiny_ret_t              _fTinyFindFirst( const char __far *__pattern,
                                create_attr __attr);
tiny_ret_t  tiny_call   _nTinyFindFirst( const char __near *, create_attr );
tiny_ret_t  tiny_call   _nTinyFindFirstDTA( const char __near *, create_attr,
                                void * );
tiny_ret_t  tiny_call   _TinyFindNext( void );
tiny_ret_t  tiny_call   _TinyFindNextDTA( void * );
tiny_ret_t  tiny_call   _TinyFindCloseDTA( void * );
tiny_ret_t  tiny_call   _TinyGetFileStamp( tiny_handle_t );
tiny_ret_t  tiny_call   _TinySetFileStamp( tiny_handle_t, tiny_ftime_t __hms,
                                tiny_fdate_t __ymd );
tiny_ret_t  tiny_call   _nTinyGetFileAttr( const char __near *__file );
tiny_ret_t              _fTinyGetFileAttr( const char __far *__file );
tiny_ret_t  tiny_call   _nTinySetFileAttr( const char __near *__file,
                                create_attr );
tiny_ret_t              _fTinySetFileAttr( const char __far *__file,
                                create_attr );
void        tiny_call   _TinySetIntr( uint_8, uint );
#ifdef __WINDOWS_386__
void        tiny_call   _TinySetVect( uint_8, void __near * );
void *      tiny_call   _TinyGetVect( uint_8 );
#else
void        tiny_call   _TinySetVect(uint_8, void (__far __interrupt *__f)());
void  (__far __interrupt * tiny_call _TinyGetVect( uint_8 ))();
#endif
tiny_dos_version  tiny_call _TinyDOSVersion( void );
char        tiny_call   _TinyGetCH( void );
char        tiny_call   _TinyGetCHE( void );
char        tiny_call   _TinyGetSwitchChar( void );
void        tiny_call   _TinySetSwitchChar( char );
uint_32     tiny_call   _TinyFreeSpace( uint_8 );
tiny_ret_t  tiny_call   _nTinyGetCountry( const tiny_country_info __near * );
tiny_ret_t              _fTinyGetCountry( const tiny_country_info __far * );
tiny_ret_t  tiny_call   _TinySetCountry( uint_16 );
tiny_ret_t  tiny_call   _nTinyFCBPrsFname( const char __near *__str,
                                tiny_fcb_t __far *__fcb, uint_8 __ctrl );
tiny_ret_t  tiny_call   _fTinyFCBPrsFname( const char __far *__str,
                                tiny_fcb_t __far *__fcb, uint_8 __ctrl );
tiny_ret_t  tiny_call   _nTinyFCBDeleteFile( tiny_fcb_t __near *__fcb );
tiny_ret_t              _fTinyFCBDeleteFile( tiny_fcb_t __far *__fcb );
tiny_ret_t  tiny_call   _TinyLock(tiny_handle_t,uint_32 __start,uint_32 __l);
tiny_ret_t  tiny_call   _TinyUnlock(tiny_handle_t,uint_32 __start,uint_32 __l);
uint        tiny_call   _TinyGetPSP( void );
void        tiny_call   _TinySetPSP( uint_16 __seg );
void        tiny_call   _TinyCreatePSP( uint_16 __seg );
tiny_ret_t  tiny_call   _TinySetMaxHandleCount( uint_16 );
void *      tiny_call   _TinyDPMIAlloc( uint_16 __hiw, uint_16 __low );
void *      tiny_call   _TinyDPMIRealloc( void *__addr, uint_16 __hiw, uint_16 __low );
void        tiny_call   _TinyDPMIFree( uint_16 __hiw, uint_16 __low );
void *      tiny_call   _TinyCBAlloc( uint_32 );
uint_32                 _TinyMemAlloc( uint_32 __size );
uint_32     tiny_call   _TinyDPMIDOSAlloc( uint_16 __paras );
void        tiny_call   _TinyDPMIDOSFree( uint_16 __sel );
uint_32     tiny_call   _TinyDPMIBase( uint_16 __sel );
void __far *tiny_call   _TinyDPMIGetProtectVect( uint_8 __intr );
tiny_ret_t  tiny_call   _TinyDPMISetProtectVect( uint_8 __intr,
                                    void ( __far __interrupt *__f )() );
void __far *tiny_call   _TinyDPMIGetProtectExcpt( uint_8 __intr );
tiny_ret_t  tiny_call   _TinyDPMISetProtectExcpt( uint_8 __intr,
                                    void ( __far __interrupt *__f )() );
uint_32     tiny_call   _TinyDPMIGetRealVect( uint_8 __intr );
tiny_ret_t  tiny_call   _TinyDPMISetRealVect( uint_8 __intr,
                                    uint_16 __seg, uint_16 __offs );
tiny_ret_t  tiny_call   _TinyDPMISimulateRealInt( uint_8 __intr,
                                    uint_8 __flags, uint_16 __copy,
                                    call_struct __far *__struct );
tiny_ret_t  tiny_call   _TinyDPMICallRealIntFrame( uint_8 __flags,
                                    uint_16 __copy,
                                    call_struct __far *__struct );
tiny_ret_t  tiny_call   _TinyDPMICallRealFarFrame( uint_8 __flags,
                                    uint_16 __copy,
                                    call_struct __far *__struct );
void __far  *tiny_call  _TinyDPMIRawPMtoRMAddr( void );
uint_32     tiny_call   _TinyDPMIRawRMtoPMAddr( void );
void __far  *tiny_call  _TinyDPMISaveRMStateAddr( void );
uint_32     tiny_call   _TinyDPMISavePMStateAddr( void );
uint_16     tiny_call   _TinyDPMISaveStateSize( void );
uint_16     tiny_call   _TinyDPMICreateCSAlias( uint_16 __sel );
tiny_ret_t  tiny_call   _TinyDPMIFreeSel( uint_16 __sel );
uint_16     tiny_call   _TinyDPMISegToSel( uint_16 __sel );
uint_16     tiny_call   _TinyDPMICreateSel( uint_16 __nbsels );
tiny_ret_t  tiny_call   _TinyDPMISetBase( uint_16 __sel, uint_32 );
tiny_ret_t  tiny_call   _TinyDPMISetLimit( uint_16 __sel, uint_32 );
tiny_ret_t  tiny_call   _TinyDPMISetRights( uint_16 __sel, uint_16 );
tiny_ret_t  tiny_call   _TinyDPMIGetDescriptor( uint_16 __sel, void __far * );
tiny_ret_t  tiny_call   _TinyDPMISetDescriptor( uint_16 __sel, void __far * );

/*
 * machine code instructions
 */
#define _PUSH_CS        0x0e
#define _PUSH_DS        0x1e
#define _POP_DS         0x1f
#define _PUSH_ES        0x06
#define _POP_ES         0x07
#define _PUSH_SS        0x16
#define _POP_SS         0x17
#define _PUSHF          0x9c
#define _POPF           0x9d

#define _STC            0xf9
#define _CLC            0xf8

#define _CBW            0x98
#define _CWD            0x99

#define _MOV_AL         0xb0
#define _MOV_AH         0xb4
#define _MOV_AX         0xb8
#define _MOV_ES_AX      0x8e 0xc0
#define _MOV_ES_CX      0x8e 0xc1
#define _MOV_ES_DX      0x8e 0xc2
#define _MOV_CX_AX      0x89 0xc1
#define _MOV_CX_DX      0x8b 0xca
#define _MOV_CX_SI      0x89 0xf1
#define _MOV_BX_CX      0x89 0xcb
#define _MOV_DX_AX      0x8b 0xd0
#define _MOV_DX_CX      0x89 0xca
#define _MOV_AX_BX      0x89 0xd8
#define _MOV_AX_CX      0x89 0xc8
#define _MOV_AX_DX      0x8b 0xc2
#define _MOV_AX_ES      0x8c 0xc0
#define _MOV_DS_AX      0x8e 0xd8
#define _MOV_DS_DX      0x8e 0xda
#define _MOV_AX_SS      0x8c 0xd0
#define _MOV_CH_AL      0x88 0xc5
#define _MOV_AL_BL      0x88 0xd8
#define _MOV_DI_SI      0x89 0xf7

#define _NOT_AX         0xf7 0xd0
#define _NOT_BX         0xf7 0xd3
#define _DEC_AX         0x48
#define _DEC_BX         0x4b
#define _DEC_DX         0x4a
#define _SUB_CX_N       0x81 0xe9
#define _SBB_DX_DX      0x19 0xd2
#define _SBB_CX_CX      0x19 0xc9
#define _SBB_BX_BX      0x19 0xdb
#define _SBB_AX_AX      0x19 0xc0
#define _AND_AX_BX      0x21 0xd8
#define _OR_DX_BX       0x09 0xda
#define _XOR_AX_AX      0x31 0xc0
#define _XOR_BX_BX      0x31 0xdb
#define _XOR_CX_CX      0x31 0xc9
#define _XOR_DH_DH      0x30 0xf6
#define _XOR_DX_DX      0x31 0xd2
#define _XOR_DI_DI      0x31 0xff

#define _ADD_SP         0x83 0xc4

#define _XCHG_SI_DI     0x87 0xf7
#define _XCHG_BX_CX     0x87 0xd9

#define _MUL_BX         0xf7 0xe3
#define _MUL_CX         0xf7 0xe1

#define _RCL_DX_1       0xd1 0xd2
#define _ROR_DX_1       0xd1 0xca
#define _RCL_AX_1       0xd1 0xd0
#define _ROR_AX_1       0xd1 0xc8

#define _TEST_CL        0xf6 0xc1
#define _TEST_BL        0xf6 0xc3
#define _TEST_BH_BH     0x84 0xff

#define _JC             0x72
#define _JNC            0x73
#define _JZ             0x74
#define _JNZ            0x75
#define _JS             0x78
#define _JNS            0x79
#define _JMP_SHORT      0xeb

#define _INT            0xcd

#if ( defined( __WINDOWS_386__ ) || defined( __OSI__ ) || defined( __CALL21__ ) ) && !defined( __NOCALL21__ )
 extern  void   __Int21( void );
 #pragma aux __Int21 "*"
 #define _INT_21        "call __Int21"
#else
 #define _INT_21        _INT 0x21
#endif

#if defined( __OSI__ ) && defined( __CALL31__ )
 extern  void   __Int31( void );
 #pragma aux __Int31 "*"
 #define _INT_31        "call __Int31"
#else
 #define _INT_31        _INT 0x31
#endif

#define _GET_           0x00
#define _SET_           0x01

#define _USE16          0x66            /* 16-bit override prefix (for 386) */
#define _SHR_ECX_N      0xc1 0xe9
#define _SHR_ESI_N      0xc1 0xee
#define _SHL_EAX_N      0xc1 0xe0
#define _SHL_EDX_N      0xc1 0xe2
#define _SHL_ECX_N      0xc1 0xe1
#define _SHL_EBX_N      0xc1 0xe3

#if defined(__386__)

 #define _MOV_AX_W      _USE16 _MOV_AX

 #define _SET_DS_DGROUP
 #define _SET_DS_DGROUP_SAFE
 #define _RST_DS_DGROUP
 #define _SET_DS_SREG
 #define _SET_DS_SREG_SAFE
 #define _RST_DS_SREG
 #define _SREG

#elif defined( _M_I86 )

 #define _MOV_AX_W      _MOV_AX

 #if defined( _M_I86SM ) || defined( _M_I86MM ) || defined( ZDP ) || defined( __WINDOWS__ )
  #define _SET_DS_DGROUP
  #define _SET_DS_DGROUP_SAFE
  #define _RST_DS_DGROUP
  #define _SET_DS_SREG          _PUSH_DS        \
                                _MOV_AX_ES      \
                                _MOV_DS_AX
  #define _SET_DS_SREG_SAFE     _PUSH_DS        \
                                _PUSH_ES        \
                                _POP_DS
  #define _RST_DS_SREG          _POP_DS
  #define _SREG                 es

 #else
  #define _SET_DS_DGROUP        _PUSH_DS        \
                                _MOV_AX_SS      \
                                _MOV_DS_AX
  #define _SET_DS_DGROUP_SAFE   _PUSH_DS        \
                                _PUSH_SS        \
                                _POP_DS

  #define _RST_DS_DGROUP        _POP_DS
  #define _SET_DS_SREG
  #define _SET_DS_SREG_SAFE
  #define _RST_DS_SREG
  #define _SREG                 ds

 #endif
#endif


#if defined( __386__ )

/***************************
 * 80386 versions of pragmas
 ***************************/

#pragma aux             _TinyCreatePSP = \
        "pushfd"        \
        "mov ah,26h"    \
        _INT_21         \
        "popfd"         \
        parm caller     [dx] \
        modify exact    [ax];

#pragma aux             _TinySetPSP = \
        "pushfd"        \
        "mov ah,50h"    \
        _INT_21         \
        "popfd"         \
        parm caller     [bx] \
        modify exact    [ah];

#pragma aux             _TinyGetPSP = \
        _PUSHF          \
        "xor eax,eax"   \
        "mov ah,51h"    \
        _INT_21         \
        "mov ax,bx"     \
        _POPF           \
        value           [eax] \
        modify exact    [eax ebx];

#pragma aux _TinySetMaxHandleCount = \
        "mov    ah,67h" \
        _INT_21         \
        "rcl    eax,1"  \
        "ror    eax,1"  \
        parm caller     [bx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyCBAlloc = \
        "mov eax,80004800h" \
        _INT_21         \
        "sbb ebx,ebx"   \
        "not ebx"       \
        "and eax,ebx"   \
        parm caller     [ebx] \
        value           [eax] \
        modify exact    [eax ebx];

#pragma aux             _TinyMemAlloc = \
        _MOV_AH DOS_ALLOC_SEG \
        _INT_21         \
        "sbb ebx,ebx"   \
        "not ebx"       \
        "and eax,ebx"   \
        parm caller     [ebx] \
        value           [eax] \
        modify exact    [eax ebx];

#pragma aux             _TinyDPMISegToSel = \
        "mov ax,2"      \
        _INT_31         \
        "jnc short finish" \
        "xor ax,ax"     \
"finish: "              \
        parm            [bx] \
        value           [ax] \
        modify exact    [ax bx];

#pragma aux             _TinyDPMICreateSel = \
        "xor eax,eax"   \
        _INT_31         \
        "jnc short L1"  \
        "xor ax,ax"     \
    "L1: "              \
        parm            [cx] \
        value           [ax] \
        modify exact    [ax];

#pragma aux             _TinyDPMISetBase = \
        "mov ecx,edx"   \
        "shr ecx,16"    \
        "mov ax,7"      \
        _INT_31         \
        "sbb eax,eax"   \
        parm            [bx] [edx] \
        value           [eax] \
        modify exact    [eax ecx];

#pragma aux             _TinyDPMISetLimit = \
        "mov ecx,edx"   \
        "shr ecx,16"    \
        "mov ax,8"      \
        _INT_31         \
        "sbb eax,eax"   \
        parm            [bx] [edx] \
        value           [eax] \
        modify exact    [eax ecx];

#pragma aux             _TinyDPMISetRights = \
        "mov ax,9"      \
        _INT_31         \
        "sbb eax,eax"   \
        parm            [bx] [cx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyDPMIGetDescriptor = \
        "push es"       \
        "mov es,ecx"    \
        "mov ax,0bh"    \
        _INT_31         \
        "sbb eax,eax"   \
        "pop es"        \
        parm            [bx] [cx edi] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyDPMISetDescriptor = \
        "push es"       \
        "mov es,ecx"    \
        "mov ax,0ch"    \
        _INT_31         \
        "sbb eax,eax"   \
        "pop es"        \
        parm            [bx] [cx edi] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyDPMICreateCSAlias = \
        "mov ax,0ah"    \
        _INT_31         \
        "jnc short finish" \
        "xor ax,ax"     \
"finish: "              \
        parm caller     [bx] \
        value           [ax] \
        modify exact    [ax];

#pragma aux             _TinyDPMIFreeSel = \
        "mov ax,1"      \
        _INT_31         \
        "sbb eax,eax"   \
        parm caller     [bx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyDPMIRawPMtoRMAddr = \
        "mov ax,306h"   \
        "xor edi,edi"   \
        "stc"           \
        _INT_31         \
        "mov cx,si"     \
        "jnc short finish" \
        "xor cx,cx"     \
        "xor edi,edi"   \
"finish: "              \
        value           [cx edi] \
        modify exact    [eax cx si edi];

#pragma aux             _TinyDPMIRawRMtoPMAddr = \
        "mov ax,306h"   \
        "stc"           \
        _INT_31         \
        "jnc short L1"  \
        "xor ebx,ebx"   \
        "jmp short finish" \
"L1:     "              \
        "shl ebx,16"    \
        "mov bx,cx"     \
"finish: "              \
        value           [ebx] \
        modify exact    [eax ebx cx si edi];

#pragma aux             _TinyDPMISaveRMStateAddr = \
        "mov ax,305h"   \
        "stc"           \
        _INT_31         \
        "mov cx,si"     \
        "jnc short finish" \
        "xor cx,cx"     \
        "xor edi,edi"   \
"finish: "              \
        value           [cx edi] \
        modify exact    [ax bx cx si edi];

#pragma aux             _TinyDPMISavePMStateAddr = \
        "mov ax,305h"   \
        _INT_31         \
        "jnc short L1"  \
        "xor cx,cx"     \
        "xor ebx,ebx"   \
        "jmp short finish" \
"L1:     "              \
        "shl ebx,16"    \
        "mov bx,cx"     \
"finish: "              \
        value           [ebx] \
        modify exact    [ax ebx cx si edi];

#pragma aux             _TinyDPMISaveStateSize = \
        "mov ax,305h"   \
        _INT_31         \
        "jnc short finish" \
        "xor ax,ax"     \
"finish: "              \
        value           [ax] \
        modify exact    [ax bx cx si edi];

#pragma aux             _TinyDPMISimulateRealInt = \
        "push es"       \
        "mov es,edx"    \
        "mov ax,300h"   \
        _INT_31         \
        "sbb eax,eax"   \
        "pop es"        \
        parm caller     [bl] [bh] [cx] [dx edi] \
        value           [eax] \
        modify exact    [eax bx cx edi];

#pragma aux             _TinyDPMICallRealFarFrame = \
        "push es"       \
        "mov es,edx"    \
        "mov ax,301h"   \
        _INT_31         \
        "sbb eax,eax"   \
        "pop es"        \
        parm caller     [bh] [cx] [dx edi] \
        value           [eax] \
        modify exact    [eax bx cx edi];

#pragma aux             _TinyDPMICallRealIntFrame = \
        "push es"       \
        "mov es,edx"    \
        "mov ax,302h"   \
        _INT_31         \
        "sbb eax,eax"   \
        "pop es"        \
        parm caller     [bh] [cx] [dx edi] \
        value           [eax] \
        modify exact    [eax bx cx edi];

#pragma aux             _TinyDPMIGetProtectVect = \
        "mov ax,204h"   \
        _INT_31         \
        "jnc short finish" \
        "xor cx,cx"     \
        "xor edx,edx"   \
"finish: "              \
        parm caller     [bl] \
        value           [cx edx] \
        modify exact    [ax bx cx edx];

#pragma aux             _TinyDPMISetProtectVect = \
        "mov ax,205h"   \
        _INT_31         \
        "sbb eax,eax"   \
        parm caller     [bl] [cx edx] \
        value           [eax] \
        modify exact    [eax bx cx edx];

#pragma aux             _TinyDPMIGetProtectExcpt = \
        "mov ax,202h"   \
        _INT_31         \
        "jnc short finish" \
        "xor cx,cx"     \
        "xor edx,edx"   \
"finish: "              \
        parm caller     [bl] \
        value           [cx edx] \
        modify exact    [ax bx cx edx];

#pragma aux             _TinyDPMISetProtectExcpt = \
        "mov ax,203h"   \
        _INT_31         \
        "sbb eax,eax"   \
        parm caller     [bl] [cx edx] \
        value           [eax] \
        modify exact    [eax bx cx edx];

#pragma aux             _TinyDPMIAlloc = \
        "mov    ax,501h" \
        _INT_31         \
        "sbb    eax,eax" /* eax=-1 if alloc failed */ \
        "inc    eax"     /* eax=0  if alloc failed */ \
        "je short finish" \
        "mov    ax,bx"   /* linear address returned in BX:CX */ \
        "shl    eax,16" \
        "mov    ax,cx" \
        "mov    [eax],di"  /* store handle in block */ \
        "mov    2[eax],si" /* ... */ \
"finish: "              \
        parm caller     [bx] [cx] \
        value           [eax] \
        modify exact    [eax ebx ecx esi edi];

#pragma aux             _TinyDPMIRealloc = \
        "mov    di,[eax]"  /* get memory block handle */\
        "mov    si,2[eax]" /* ... */\
        "mov    ax,503h" \
        _INT_31         \
        "sbb    eax,eax"   /* eax=-1 if alloc failed */ \
        "inc    eax"       /* eax=0  if alloc failed */ \
        "je short finish" \
        "mov    ax,bx"     /* linear address returned in BX:CX */ \
        "shl    eax,16" \
        "mov    ax,cx"  \
        "mov    [eax],di"  /* store new handle in block */ \
        "mov    2[eax],si" /* ... */ \
"finish: "              \
        parm caller     [eax] [bx] [cx] \
        value           [eax] \
        modify exact    [eax ebx ecx esi edi];

#pragma aux             _TinyDPMIFree = \
        "mov ax,502h"   \
        _INT_31         \
        parm caller     [si] [di] \
        modify exact    [eax esi edi];

#pragma aux             _TinyDPMIBase = \
        "mov ax,6"      \
        _INT_31         \
        "mov eax,ecx"   \
        "shl eax,16"    \
        "mov ax,dx"     \
        parm caller     [bx] \
        value           [eax] \
        modify exact    [eax ebx ecx edx];

#pragma aux             _TinyDPMIDOSAlloc = \
        "mov ax,100h"   \
        _INT_31         \
        "sbb ebx,ebx"   \
        "not ebx"       \
        "shl eax,16"    \
        "mov ax,dx"     \
        "and eax,ebx"   \
        parm caller     [bx] \
        value           [eax] \
        modify exact    [eax ebx edx];

#pragma aux             _TinyDPMIDOSFree = \
        "mov ax,101h"   \
        _INT_31         \
        parm caller     [dx] \
        modify exact    [eax edx];

#pragma aux             _TinyDPMIGetRealVect = \
        "mov ax,200h"   \
        _INT_31         \
        "shl ecx,16"    \
        "mov cx,dx"     \
        parm caller     [bl] \
        value           [ecx] \
        modify exact    [eax ebx ecx edx];

#pragma aux             _TinyDPMISetRealVect = \
        "mov ax,201h"   \
        _INT_31         \
        "sbb eax,eax"   \
        parm caller     [bl] [cx] [dx] \
        value           [eax] \
        modify exact    [eax ebx ecx edx];

#pragma aux             _nTinyAccess = \
        _MOV_AX_W _GET_ DOS_CHMOD \
        _INT_21         \
        "jc short finish" \
        _TEST_BL 0x02  \
        "jz short finish" \
        _TEST_CL 0x01  \
        "jz short finish" \
        _MOV_AX_W 0x00 EACCES \
        _STC            \
"finish: "              \
        "sbb ecx,ecx"   \
        "mov cx,ax"     \
        parm caller     [edx] [bl] \
        value           [ecx] \
        modify exact    [eax ecx];

#pragma aux             _nTinyBufferedInput = \
        _MOV_AH 0x0A    \
        _INT_21         \
        parm caller     [edx] \
        modify exact    [ah];

#pragma aux             _nTinyOpen = \
        "mov ah,3Dh"    \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] [al] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _nTinyCreate = \
        "mov ah,3Ch"    \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] [ecx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _nTinyCreateEx = \
        "mov ax,6C00h"  \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [esi] [ebx] [ecx] [edx]\
        value           [eax] \
        modify exact    [eax ecx];

#pragma aux             _nTinyCreateNew = \
        "mov ah,5Bh"    \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] [ecx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _nTinyCreateTemp = \
        "mov ah,5Ah"    \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] [ecx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyClose = \
        "mov ah,3Eh"    \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [bx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyCommitFile = \
        "mov ah,68h"    \
        "clc"           \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [bx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _nTinyWrite = \
        "mov ah,40h"    \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [bx] [edx] [ecx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _fTinyWrite = \
        "push ds"       \
        "xchg edx,eax"  \
        "mov ds,eax"    \
        "mov ah,40h"    \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        "pop ds"        \
        parm caller     [bx] [dx eax] [ecx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _nTinyRead = \
        "mov ah,3Fh"    \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [bx] [edx] [ecx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _fTinyRead = \
        "push ds"       \
        "xchg edx,eax"  \
        "mov ds,eax"    \
        "mov ah,3Fh"    \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        "pop ds"        \
        parm caller     [bx] [dx eax] [ecx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyLSeek = \
        "mov ah,42h"    \
        "mov ecx,edx"   \
        "shr ecx,16"    \
        _INT_21         \
        "mov ss:[edi],ax"   \
        "mov ss:2[edi],dx"  \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [bx] [edx] [al] [edi] \
        value           [eax] \
        modify exact    [eax ecx edx];

#pragma aux             _TinySeek = \
        "mov ah,42h"    \
        "mov ecx,edx"   \
        "shr ecx,16"    \
        _INT_21         \
        "rcl dx,1"      \
        "ror dx,1"      \
        "shl edx,16"    \
        "mov dx,ax"     \
        parm caller     [bx] [edx] [al] \
        value           [edx] \
        modify exact    [eax ecx edx];

#pragma aux             _nTinyDelete = \
        _MOV_AH DOS_UNLINK \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] \
        value           [eax] \
        modify exact    [eax];

// 06/22/95 T. Schiller
//
// The only reason we save/restore ebx is that in win386 the high word of
// ebx sometimes (one known cause is that the File Manager is running) gets
// trashed.
#pragma aux             _nTinyRename = \
        "push ebx"      \
        "push es"       \
        "mov es,ecx"    \
        _MOV_AH DOS_RENAME \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        "pop es"        \
        "pop ebx"       \
        parm caller     [edx] [cx edi]\
        value           [eax] \
        modify exact    [eax];

#pragma aux             _nTinyMakeDir = \
        _MOV_AH DOS_MKDIR \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _nTinyRemoveDir = \
        _MOV_AH DOS_RMDIR \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _nTinyChangeDir = \
        _MOV_AH DOS_CHDIR \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _nTinyGetCWDir = \
        _MOV_AH DOS_GETCWD \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [esi] [dl] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _fTinyGetCWDir = \
        "push ds"       \
        "mov  ds,ecx"   \
        _MOV_AH DOS_GETCWD \
        _INT_21         \
        "rcl  eax,1"    \
        "ror  eax,1"    \
        "pop  ds"       \
        parm caller     [cx esi] [dl] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyDup = \
        _MOV_AH DOS_DUP \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [bx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyDup2 = \
        _MOV_AH DOS_DUP2 \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [bx] [cx]\
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyAllocBlock = \
        _MOV_AH DOS_ALLOC_SEG \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [ebx]\
        value           [eax] \
        modify exact    [eax ebx];

#pragma aux             _TinyTestAllocBlock = \
        _MOV_AH DOS_ALLOC_SEG \
        _INT_21         \
        "jnc short finish" \
        "mov eax,ebx"   \
        "rcl eax,1"     \
        "ror eax,1"     \
"finish: "              \
        parm caller     [ebx]\
        value           [eax] \
        modify exact    [eax ebx];

#pragma aux             _TinyMaxAlloc = \
        "xor ebx,ebx"   \
        "dec ebx"       \
        _MOV_AH DOS_ALLOC_SEG \
        _INT_21         \
        parm caller     [] \
        value           [ebx] \
        modify exact    [eax ebx];

#pragma aux             _TinyFreeBlock = \
        "push es"       \
        "mov es,eax"    \
        _MOV_AH DOS_FREE_SEG \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        "pop es"        \
        parm caller     [eax] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinySetBlock = \
        _MOV_AH DOS_MODIFY_SEG \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [ebx] [es] \
        value           [eax] \
        modify exact    [eax ebx];

#pragma aux             _TinyMaxSet = \
        "xor ebx,ebx"   \
        "dec ebx"       \
        _MOV_AH DOS_MODIFY_SEG \
        _INT_21         \
        parm caller     [es] \
        value           [ebx] \
        modify exact    [eax ebx];

#pragma aux             _TinyGetDeviceInfo = \
        _MOV_AX_W _GET_ DOS_IOCTL \
        _INT_21         \
        "rcl edx,1"     \
        "ror edx,1"     \
        parm caller     [bx] \
        value           [edx] \
        modify exact    [eax edx];

#pragma aux             _TinySetDeviceInfo = \
        "xor dh,dh"      \
        _MOV_AX_W _SET_ DOS_IOCTL \
        _INT_21         \
        "rcl edx,1"     \
        "ror edx,1"     \
        parm caller     [bx] [dl] \
        value           [edx] \
        modify exact    [eax edx];

#pragma aux             _TinyGetCtrlBreak = \
        _MOV_AX_W _GET_ DOS_CTRL_BREAK \
        _INT_21         \
        parm caller     [] \
        value           [dl] \
        modify exact    [eax dl];

#pragma aux             _TinySetCtrlBreak = \
        _MOV_AX_W _SET_ DOS_CTRL_BREAK \
        _INT_21         \
        parm caller     [dl] \
        modify exact    [eax dl];

#pragma aux             _TinyTerminateProcess = \
        _MOV_AH DOS_EXIT \
        _INT_21         \
        parm caller     [al] \
        aborts;

#pragma aux             _TinyGetDate = \
        _MOV_AH DOS_GET_DATE \
        _INT_21         \
        "sub cx,1900"   \
        "mov ch,al"     \
        "shl ecx,16"    \
        "mov cx,dx"     \
        parm caller     [] \
        value           [ecx] \
        modify exact    [eax ecx edx];

#pragma aux             _TinyGetTime = \
        _MOV_AH DOS_GET_TIME \
        _INT_21         \
        "shl ecx,16"    \
        "mov cx,dx"     \
        parm caller     [] \
        value           [ecx] \
        modify exact    [eax ecx edx];

#pragma aux             _TinyGetCurrDrive = \
        _MOV_AH DOS_CUR_DISK \
        _INT_21         \
        parm caller     [] \
        value           [al] \
        modify exact    [ax];

#pragma aux             _TinySetCurrDrive = \
        "mov ah,0Eh"    \
        _INT_21         \
        parm caller     [dl] \
        modify exact    [ax];

#pragma aux             _TinyGetDTA = \
        "push es"       \
        _MOV_AH DOS_GET_DTA \
        _INT_21         \
        "mov  ecx,es"   \
        "pop  es"       \
        value           [cx ebx] \
        modify exact    [ah ebx ecx];

#pragma aux             _TinyChangeDTA = \
        "push es"       \
        _MOV_AH DOS_GET_DTA \
        _INT_21         \
        "push ds"       \
        "mov  ds,ecx"   \
        _MOV_AH DOS_SET_DTA \
        _INT_21         \
        "pop  ds"       \
        "mov  ecx,es"   \
        "pop  es"       \
        parm caller     [cx edx] \
        value           [cx ebx] \
        modify exact    [ah ebx ecx];

#pragma aux             _nTinySetDTA = \
        _MOV_AH DOS_SET_DTA \
        _INT_21         \
        parm caller     [edx] \
        modify exact    [ah];

#pragma aux             _fTinySetDTA = \
        "push ds"       \
        "mov  ds,ecx"   \
        _MOV_AH DOS_SET_DTA \
        _INT_21         \
        "pop  ds"       \
        parm caller     [cx edx] \
        modify exact    [ah];

#pragma aux             _nTinyFindFirst = \
        _MOV_AH DOS_FIND_FIRST \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] [cx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _nTinyFindFirstDTA = \
        _MOV_AH DOS_FIND_FIRST \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] [cx] [ebx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyFindNext = \
        _MOV_AH DOS_FIND_NEXT \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyFindNextDTA = \
        _MOV_AX_W 0 DOS_FIND_NEXT \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyFindCloseDTA = \
        _MOV_AX_W 1 DOS_FIND_NEXT \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [edx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyGetFileStamp = \
        _MOV_AX_W _GET_ DOS_FILE_DATE \
        _INT_21         \
        "rcl dx,1"      \
        "ror dx,1"      \
        "shl edx,16"    \
        "mov dx,cx"     \
        parm caller     [bx] \
        value           [edx] \
        modify exact    [eax ecx edx];

#pragma aux             _TinySetFileStamp = \
        _MOV_AX_W _SET_ DOS_FILE_DATE \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [bx] [cx] [dx] \
        value           [eax] \
        modify exact    [eax];

#pragma aux             _TinyLock = \
        "mov edx,ecx"   \
        "shr ecx,16"    \
        "mov edi,esi"   \
        "shr esi,16"    \
        "mov ax,5C00h"  \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [bx] [ecx] [esi] \
        value           [eax] \
        modify exact    [eax ecx edx edi esi];

#pragma aux             _TinyUnlock = \
        "mov edx,ecx"   \
        "shr ecx,16"    \
        "mov edi,esi"   \
        "shr esi,16"    \
        "mov ax,5C01h"  \
        _INT_21         \
        "rcl eax,1"     \
        "ror eax,1"     \
        parm caller     [bx] [ecx] [esi] \
        value           [eax] \
        modify exact    [eax ecx edx edi esi];

#elif defined( _M_I86 )

/**************************
 * 8086 versions of pragmas
 **************************/

#pragma aux             _nTinyAccess = \
        _SET_DS_DGROUP  \
        _MOV_AX _GET_ DOS_CHMOD \
        _INT_21         \
        "jc short finish" \
        _TEST_BL 0x02   \
        "jz short finish" \
        _TEST_CL 0x01   \
        "jz short finish" \
        _MOV_AX 0x00 EACCES \
        _STC            \
"finish: "              \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] [bl] \
        value           [dx ax] \
        modify exact    [ax cx dx];

#pragma aux             _fTinyAccess = \
        _SET_DS_SREG    \
        _MOV_AX _GET_ DOS_CHMOD \
        _INT_21         \
        "jc short finish" \
        _TEST_BL 0x02   \
        "jz short finish" \
        _TEST_CL 0x01   \
        "jz short finish" \
        _MOV_AX 0x00 EACCES\
        _STC            \
"finish: "              \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] [bl] \
        value           [dx ax] \
        modify exact    [ax cx dx];

#pragma aux             _nTinyBufferedInput = \
        _MOV_AH 0x0A    \
        _INT_21         \
        parm caller     [dx] \
        modify exact    [ah];

#pragma aux             _fTinyBufferedInput = \
        _SET_DS_SREG_SAFE \
        _MOV_AH 0x0A    \
        _INT_21         \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] \
        modify exact    [ax];

#pragma aux             _nTinyOpen = \
        _SET_DS_DGROUP_SAFE \
        _MOV_AH DOS_OPEN \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] [al] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyOpen = \
        _SET_DS_SREG_SAFE \
        _MOV_AH DOS_OPEN \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] [al] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyCreate = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_CREAT \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyCreate = \
        _SET_DS_SREG    \
        _MOV_AH DOS_CREAT    \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyCreateEx = \
        _SET_DS_DGROUP  \
        _MOV_AH 0x6C    \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [si] [bx] [cx] [dx]\
        value           [dx ax] \
        modify exact    [ax cx dx];

#pragma aux             _fTinyCreateEx = \
        _SET_DS_SREG    \
        _MOV_AH 0x6C    \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG si] [bx] [cx] [dx]\
        value           [dx ax] \
        modify exact    [ax cx dx];

#pragma aux             _nTinyCreateNew = \
        _SET_DS_DGROUP  \
        _MOV_AH 0x5B    \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyCreateNew = \
        _SET_DS_SREG    \
        _MOV_AH 0x5B    \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyCreateTemp = \
        _SET_DS_DGROUP  \
        _MOV_AH 0x5A    \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyCreateTemp = \
        _SET_DS_SREG    \
        _MOV_AH 0x5A    \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinyClose = \
        _MOV_AH DOS_CLOSE    \
        _INT_21         \
        _SBB_DX_DX      \
        parm caller     [bx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinyCommitFile = \
        _MOV_AH DOS_COMMIT_FILE    \
        _CLC            \
        _INT_21         \
        _SBB_DX_DX      \
        parm caller     [bx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyWrite = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_WRITE    \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [bx] [dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyRead = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_READ    \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [bx] [dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyWrite = \
        _SET_DS_SREG    \
        _MOV_AH DOS_WRITE    \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [bx] [_SREG dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyRead = \
        _SET_DS_SREG    \
        _MOV_AH DOS_READ    \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [bx] [_SREG dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyAbsRead = \
        _SET_DS_DGROUP_SAFE  \
        _INT 0x25       \
        "jc short finish" \
        _ADD_SP 0x02    \
"finish: "              \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [al] [dx] [cx] [bx] \
        value           [dx ax] \
        modify exact    [ax bx cx dx si di];

#pragma aux             _fTinyAbsRead = \
        _SET_DS_SREG_SAFE \
        _INT 0x25       \
        "jc short finish" \
        _ADD_SP 0x02    \
"finish: "              \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [al] [dx] [cx] [_SREG bx] \
        value           [dx ax] \
        modify exact    [ax bx cx dx si di];

#pragma aux             _nTinyAbsWrite = \
        _SET_DS_DGROUP_SAFE \
        _INT 0x26       \
        "jc short finish" \
        _ADD_SP 0x02    \
"finish: "              \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [al] [dx] [cx] [bx] \
        value           [dx ax] \
        modify exact    [ax bx cx dx si di];

#pragma aux             _fTinyAbsWrite = \
        _SET_DS_SREG_SAFE \
        _INT 0x26       \
        "jc short finish" \
        _ADD_SP 0x02    \
"finish: "              \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [al] [dx] [cx] [_SREG bx] \
        value           [dx ax] \
        modify exact    [ax bx cx dx si di];

#pragma aux             _TinyLSeek = \
        _MOV_AH DOS_LSEEK \
        _INT_21         \
        "mov ss:[di],ax" \
        "mov ss:2[di],dx" \
        _RCL_DX_1       \
        _ROR_DX_1       \
        parm caller     [bx] [dx cx] [al] [di] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinySeek = \
        _MOV_AH DOS_LSEEK \
        _INT_21         \
        _RCL_DX_1       \
        _ROR_DX_1       \
        parm caller     [bx] [dx cx] [al] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyGetFileAttr = \
        _SET_DS_DGROUP  \
        _MOV_AX _GET_ DOS_CHMOD \
        _INT_21         \
        _MOV_AX_CX      \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] \
        value           [dx ax] \
        modify exact    [ax cx dx];

#pragma aux             _nTinySetFileAttr = \
        _SET_DS_DGROUP  \
        _MOV_AX _SET_ DOS_CHMOD \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyGetFileAttr = \
        _SET_DS_SREG    \
        _MOV_AX _GET_ DOS_CHMOD \
        _INT_21         \
        _MOV_AX_CX      \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] \
        value           [dx ax] \
        modify exact    [ax cx dx];

#pragma aux             _fTinySetFileAttr = \
        _SET_DS_SREG    \
        _MOV_AX _SET_ DOS_CHMOD \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] [cx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyDelete = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_UNLINK \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyRename = \
        _SET_DS_DGROUP  \
        _MOV_AX_SS      \
        _MOV_ES_AX      \
        _MOV_AH DOS_RENAME \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] [di]\
        value           [dx ax] \
        modify exact    [ax dx es];

#pragma aux             _fTinyDelete = \
        _SET_DS_SREG    \
        _MOV_AH DOS_UNLINK \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyRename = \
        _SET_DS_SREG    \
        _MOV_ES_CX      \
        _MOV_AH DOS_RENAME \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] [cx di]\
        value           [dx ax] \
        modify exact    [ax dx es];

#pragma aux             _nTinyMakeDir = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_MKDIR \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyRemoveDir = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_RMDIR \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyChangeDir = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_CHDIR \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _nTinyGetCWDir = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_GETCWD \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [si] [dl] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyMakeDir = \
        _SET_DS_SREG    \
        _MOV_AH DOS_MKDIR \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyRemoveDir = \
        _SET_DS_SREG    \
        _MOV_AH DOS_RMDIR \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyChangeDir = \
        _SET_DS_SREG    \
        _MOV_AH DOS_CHDIR \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _fTinyGetCWDir = \
        _SET_DS_SREG    \
        _MOV_AH DOS_GETCWD \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG si] [dl] \
        value           [dx ax] \
        modify exact    [ax dx si];

#pragma aux             _TinyDup = \
        _MOV_AH DOS_DUP \
        _INT_21         \
        _SBB_BX_BX      \
        parm caller     [bx] \
        value           [bx ax] \
        modify exact    [ax bx];

#pragma aux             _TinyDup2 = \
        _MOV_AH DOS_DUP2    \
        _INT_21         \
        _SBB_BX_BX      \
        parm caller     [bx] [cx] \
        value           [bx ax] \
        modify exact    [ax bx];

#pragma aux             _TinyAllocBlock = \
        _MOV_AH DOS_ALLOC_SEG    \
        _INT_21         \
        _SBB_BX_BX      \
        parm caller     [bx] \
        value           [bx ax] \
        modify exact    [ax bx];

#pragma aux             _TinyTestAllocBlock = \
        _MOV_AH DOS_ALLOC_SEG    \
        _INT_21         \
        _SBB_DX_DX      \
        "jns short finish" \
        _MOV_AX_BX      \
"finish: "              \
        parm caller     [bx] \
        value           [dx ax] \
        modify exact    [ax bx dx];

#pragma aux             _TinyMaxAlloc = \
        _XOR_BX_BX      \
        _DEC_BX         \
        _MOV_AH DOS_ALLOC_SEG \
        _INT_21         \
        parm caller     [] \
        value           [bx] \
        modify exact    [ax bx];

#pragma aux             _TinyFreeBlock = \
        _MOV_AH DOS_FREE_SEG \
        _INT_21         \
        _SBB_DX_DX      \
        parm caller     [es] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinySetBlock = \
        _MOV_AH DOS_MODIFY_SEG    \
        _INT_21         \
        _SBB_BX_BX      \
        parm caller     [bx] [es] \
        value           [bx ax] \
        modify exact    [ax bx];

#pragma aux             _TinyMaxSet = \
        _XOR_BX_BX      \
        _DEC_BX         \
        _MOV_AH DOS_MODIFY_SEG    \
        _INT_21         \
        parm caller     [es] \
        value           [bx] \
        modify exact    [ax bx];

#pragma aux             _TinyGetDeviceInfo = \
        _MOV_AX _GET_ DOS_IOCTL \
        _INT_21         \
        _SBB_CX_CX      \
        parm caller     [bx] \
        value           [cx dx] \
        modify exact    [ax cx dx];

#pragma aux             _TinySetDeviceInfo = \
        _XOR_DH_DH      \
        _MOV_AX _SET_ DOS_IOCTL \
        _INT_21         \
        _SBB_CX_CX      \
        parm caller     [bx] [dl] \
        value           [cx dx] \
        modify exact    [ax cx dx];

#pragma aux             _TinyGetCtrlBreak = \
        _MOV_AX _GET_ DOS_CTRL_BREAK \
        _INT_21         \
        parm caller     [] \
        value           [dl] \
        modify exact    [ax dl];

#pragma aux             _TinySetCtrlBreak = \
        _MOV_AX _SET_ DOS_CTRL_BREAK \
        _INT_21         \
        parm caller     [dl] \
        modify exact    [ax dl];

#pragma aux             _TinyTerminateProcess = \
        _MOV_AH DOS_EXIT \
        _INT_21         \
        parm caller     [al] \
        aborts;

#pragma aux             _TinyGetDate = \
        _MOV_AH DOS_GET_DATE    \
        _INT_21         \
        _SUB_CX_N 0x6c 0x07 /* 1900 */ \
        _MOV_CH_AL      \
        parm caller     [] \
        value           [cx dx] \
        modify exact    [ax cx dx];

#pragma aux             _TinyGetTime = \
        _MOV_AH DOS_GET_TIME    \
        _INT_21         \
        parm caller     [] \
        value           [cx dx] \
        modify exact    [ax cx dx];

#pragma aux             _TinyGetCurrDrive = \
        _MOV_AH DOS_CUR_DISK \
        _INT_21         \
        parm caller     [] \
        value           [al] \
        modify exact    [ax];

#pragma aux             _TinySetCurrDrive = \
        _MOV_AH 0x0e    \
        _INT_21         \
        parm caller     [dl] \
        modify exact    [ax dl];

#pragma aux             _nTinySetDTA = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_SET_DTA \
        _INT_21         \
        _RST_DS_DGROUP  \
        parm caller     [dx] \
        modify exact    [ax];

#pragma aux             _nTinyFindFirst = \
        _SET_DS_DGROUP  \
        _MOV_AH DOS_FIND_FIRST \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] [cl] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinyGetDTA = \
        "push es"       \
        _MOV_AH DOS_GET_DTA \
        _INT_21         \
        "mov  cx,es"    \
        "pop  es"       \
        value           [cx bx] \
        modify exact    [ah bx cx];

#pragma aux             _TinyChangeDTA = \
        "push es"       \
        _MOV_AH DOS_GET_DTA \
        _INT_21         \
        "push ds"       \
        "mov  ds,cx"    \
        _MOV_AH DOS_SET_DTA \
        _INT_21         \
        "pop  ds"       \
        "mov  cx,es"    \
        "pop  es"       \
        parm caller     [cx dx] \
        value           [cx bx] \
        modify exact    [ah bx cx];

#pragma aux             _fTinySetDTA = \
        _SET_DS_SREG    \
        _MOV_AH DOS_SET_DTA \
        _INT_21         \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] \
        modify exact    [ax];

#pragma aux             _fTinyFindFirst = \
        _SET_DS_SREG    \
        _MOV_AH DOS_FIND_FIRST \
        _INT_21         \
        _SBB_DX_DX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] [cl] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinyFindNext = \
        _MOV_AH DOS_FIND_NEXT \
        _INT_21         \
        _SBB_DX_DX      \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinyGetFileStamp = \
        _MOV_AX _GET_ DOS_FILE_DATE \
        _INT_21         \
        _SBB_BX_BX      \
        _OR_DX_BX       \
        _MOV_AX_CX      \
        parm caller     [bx] \
        value           [dx ax] \
        modify exact    [ax bx cx dx];

#pragma aux             _TinySetFileStamp = \
        _MOV_AX _SET_ DOS_FILE_DATE \
        _INT_21         \
        _SBB_DX_DX      \
        parm caller     [bx] [cx] [dx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinySetIntr = \
        _MOV_AH DOS_SET_INT \
        _PUSH_DS        \
        _PUSH_CS        \
        _POP_DS         \
        _INT_21         \
        _POP_DS         \
        parm caller     [al] [dx] \
        value           \
        modify exact    [ah];

#pragma aux             _TinySetVect = \
        _SET_DS_SREG_SAFE \
        _MOV_AH DOS_SET_INT \
        _INT_21         \
        _RST_DS_SREG    \
        parm caller     [al] [_SREG dx] \
        value           \
        modify exact    [ah];

#pragma aux             _TinyGetVect = \
        _MOV_AH DOS_GET_INT \
        _INT_21         \
        parm caller     [al] \
        value           [es bx] \
        modify exact    [ah bx es];

#pragma aux             _TinyDOSVersion = \
        _MOV_AH DOS_GET_VERSION    \
        _INT_21         \
        value           [ax] \
        modify exact    [ax bx cx];

#pragma aux             _TinyGetCH = \
        _MOV_AH 0x08    \
        _INT_21         \
        parm caller     [] \
        value           [al] \
        modify exact    [ax];

#pragma aux             _TinyGetCHE = \
        _MOV_AH 0x01    \
        _INT_21         \
        parm caller     [] \
        value           [al] \
        modify exact    [ax];

#pragma aux             _TinyGetSwitchChar = \
        _MOV_AX _GET_ DOS_SWITCH_CHAR   \
        _INT_21             \
        parm caller     [] \
        value           [dl] \
        modify exact    [ax dl];

#pragma aux             _TinySetSwitchChar = \
        _MOV_AX _SET_ DOS_SWITCH_CHAR   \
        _INT_21             \
        parm caller     [dl] \
        value           \
        modify exact    [ax dl];

#pragma aux             _TinyFreeSpace = \
        _MOV_AH 0x36    \
        _INT_21         \
        _MUL_CX         \
        _MUL_BX         \
        parm caller     [dl] \
        value           [dx ax] \
        modify exact    [ax bx cx dx];

#pragma aux             _nTinyGetCountry = \
        _SET_DS_DGROUP  \
        _MOV_AX 0x00 0x38 \
        _INT_21         \
        _SBB_CX_CX      \
        _RST_DS_DGROUP  \
        parm caller     [dx] \
        value           [cx bx] \
        modify exact    [ax bx cx];     /* note dx not modified */

#pragma aux             _fTinyGetCountry = \
        _SET_DS_SREG    \
        _MOV_AX 0x00 0x38 \
        _INT_21         \
        _SBB_CX_CX      \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] \
        value           [cx bx] \
        modify exact    [ax bx cx];     /* note _SREG dx not modified */

#pragma aux             _TinySetCountry = \
        _XOR_DX_DX      \
        _DEC_DX         \
        _MOV_AX 0xff 0x38 \
        _TEST_BH_BH     \
        "jnz short finish" \
        _MOV_AL_BL      \
"finish: "              \
        _INT_21         \
        _SBB_DX_DX      \
        parm caller     [bx] \
        value           [dx ax] \
        modify exact    [ax bx dx];

#pragma aux _nTinyFCBPrsFname = \
        _SET_DS_DGROUP_SAFE \
        _MOV_AH 0x29    \
        _INT_21         \
        _CBW            \
        _CWD            \
        _RST_DS_DGROUP  \
        parm caller     [si] [es di] [al] \
        value           [dx ax] \
        modify exact    [ax dx si];     /* note es di not modified */

#if defined( _M_I86MM ) || defined( _M_I86SM ) || defined( ZDP )
#pragma aux _fTinyFCBPrsFname = \
        _PUSH_DS        \
        _MOV_DS_DX      \
        _MOV_AH 0x29    \
        _INT_21         \
        _CBW            \
        _CWD            \
        _POP_DS         \
        parm caller     [dx si] [es di] [al] \
        value           [dx ax] \
        modify exact    [ax dx si];     /* note es di not modified */
#else
#pragma aux _fTinyFCBPrsFname = \
        _MOV_AH 0x29    \
        _INT_21         \
        _CBW            \
        _CWD            \
        parm caller     [ds si] [es di] [al] \
        value           [dx ax] \
        modify exact    [ax dx si];     /* note es di, ds not modified */
#endif

#pragma aux _nTinyFCBDeleteFile = \
        _SET_DS_DGROUP  \
        _MOV_AH 0x13    \
        _INT_21         \
        _CBW            \
        _CWD            \
        _RST_DS_DGROUP  \
        parm caller     [dx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux _fTinyFCBDeleteFile = \
        _SET_DS_SREG    \
        _MOV_AH 0x13    \
        _INT_21         \
        _CBW            \
        _CWD            \
        _RST_DS_SREG    \
        parm caller     [_SREG dx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinyLock = \
        _XCHG_SI_DI     \
        _MOV_AX 0x00 0x5c \
        _INT_21         \
        _SBB_DX_DX      \
        parm caller     [bx] [cx dx] [di si] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinyUnlock = \
        _XCHG_SI_DI     \
        _MOV_AX 0x01 0x5c \
        _INT_21         \
        _SBB_DX_DX      \
        parm caller     [bx] [cx dx] [si di] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinyCreatePSP = \
        _PUSHF          \
        _MOV_AH 0x26    \
        _INT_21         \
        _POPF           \
        parm caller     [dx] \
        modify exact    [ax];

#pragma aux             _TinySetPSP = \
        _PUSHF          \
        _MOV_AH 0x50    \
        _INT_21         \
        _POPF           \
        parm caller     [bx] \
        modify exact    [ah];

#pragma aux             _TinyGetPSP = \
        _PUSHF          \
        _MOV_AH 0x51    \
        _INT_21         \
        _POPF           \
        value           [bx] \
        modify exact    [ah bx];

#pragma aux _TinySetMaxHandleCount = \
        "mov    ah,67h" \
        _INT_21         \
        "sbb    dx,dx"  \
        parm caller     [bx] \
        value           [dx ax] \
        modify exact    [ax dx];

#pragma aux             _TinyDPMIGetRealVect = \
        "mov ax,200h"   \
        _INT_31         \
        parm caller     [bl] \
        value           [cx dx] \
        modify exact    [ax bx cx dx];

#pragma aux             _TinyDPMISetRealVect = \
        "mov ax,201h"   \
        _INT_31         \
        "jnc short L1"  \
        _MOV_AX 0xff 0xff \
        _MOV_DX_AX      \
        "jmp short finish" \
"L1:     "              \
        _XOR_AX_AX      \
        _XOR_DX_DX      \
"finish: "              \
        parm caller     [bl] [cx] [dx] \
        value           [ax dx] \
        modify exact    [ax bx cx dx];

#pragma aux             _TinyDPMIGetProtectVect = \
        "mov ax,204h"   \
        _INT_31         \
        "jnc short finish" \
        _XOR_CX_CX      \
        _XOR_DX_DX      \
"finish: "              \
        parm caller     [bl] \
        value           [cx dx] \
        modify exact    [ax bx cx dx];

#pragma aux             _TinyDPMISetProtectVect = \
        "mov ax,205h"   \
        _INT_31         \
        "jnc short L1"  \
        _MOV_AX 0xff 0xff \
        _MOV_DX_AX      \
        "jmp short finish" \
"L1:     "              \
        _XOR_AX_AX      \
        _XOR_DX_DX      \
"finish: "              \
        parm caller     [bl] [cx dx] \
        value           [ax dx] \
        modify exact    [ax bx cx dx];

#pragma aux             _TinyDPMIRawPMtoRMAddr = \
        "mov ax,306h"   \
        _INT_31         \
        "jnc short L1"  \
        _XOR_CX_CX      \
        _XOR_DI_DI      \
        "jmp short finish" \
"L1:     "              \
        _MOV_CX_SI      \
"finish: "              \
        value           [cx di] \
        modify exact    [ax bx cx si di];

#pragma aux             _TinyDPMIRawRMtoPMAddr = \
        "mov ax,306h"   \
        _INT_31         \
        "jnc short finish" \
        _XOR_BX_BX      \
        _XOR_CX_CX      \
        _XCHG_BX_CX     \
"finish: "              \
        value           [bx cx] \
        modify exact    [ax bx cx si di];

#pragma aux             _TinyDPMISaveRMStateAddr = \
        "mov ax,305h"   \
        _INT_31         \
        "jnc short L1"  \
        _XOR_CX_CX      \
        _XOR_DI_DI      \
        "jmp short finish" \
"L1:     "              \
        _MOV_CX_SI      \
"finish: "              \
        value           [cx di] \
        modify exact    [ax bx cx si di];

#pragma aux             _TinyDPMISavePMStateAddr = \
        "mov ax,305h"   \
        _INT_31         \
        "jnc short finish" \
        _XOR_CX_CX      \
        _XOR_BX_BX      \
"finish: "              \
        _XCHG_BX_CX     \
        value           [bx cx] \
        modify exact    [ax bx cx si di];

#pragma aux             _TinyDPMISaveStateSize = \
        "mov ax,305h"   \
        _INT_31         \
        "jnc short finish" \
        _XOR_AX_AX      \
"finish: "              \
        value           [ax] \
        modify exact    [ax bx cx si di];

#pragma aux             _TinyDPMICreateCSAlias = \
        "mov ax,0ah"    \
        _INT_31         \
        "jnc short finish" \
        _XOR_AX_AX      \
"finish: "              \
        parm caller     [bx] \
        value           [ax] \
        modify exact    [ax];

#pragma aux             _TinyDPMIFreeSel = \
        "mov ax,1"      \
        _INT_31         \
        "jc short L1"   \
        _XOR_AX_AX      \
        _XOR_DX_DX      \
        "jmp short finish" \
"L1:     "              \
        _XOR_AX_AX      \
        _DEC_AX         \
        _MOV_DX_AX      \
"finish: "              \
        parm caller     [bx] \
        value           [ax dx] \
        modify exact    [ax];

#pragma aux             _TinyDPMIBase = \
        "mov ax,6"      \
        _INT_31         \
        "jnc short finish" \
        _XOR_DX_DX      \
        _DEC_DX         \
        _MOV_CX_DX      \
"finish: "              \
        parm caller     [bx] \
        value           [cx dx] \
        modify exact    [ax bx cx dx];

#pragma aux             _TinyDPMISegToSel = \
        "mov ax,2"      \
        _INT_31         \
        "jnc short finish" \
        _XOR_AX_AX      \
"finish: "              \
        parm            [bx] \
        value           [ax] \
        modify exact    [ax bx];

#pragma aux             _TinyDPMICreateSel = \
        "xor ax,ax"     \
        _INT_31         \
        "jnc short finish" \
        _XOR_AX_AX      \
"finish: "              \
        parm            [cx] \
        value           [ax] \
        modify exact    [ax];

#pragma aux             _TinyDPMISetBase = \
        "mov ax,7"      \
        _INT_31         \
        "jc short L1"   \
        _XOR_AX_AX      \
        _XOR_DX_DX      \
        "jmp short finish" \
"L1:     "              \
        _XOR_AX_AX      \
        _DEC_AX         \
        _MOV_DX_AX      \
"finish: "              \
        parm            [bx] [cx dx] \
        value           [ax dx] \
        modify exact    [ax dx];

#pragma aux             _TinyDPMISetLimit = \
        "mov ax,8"      \
        _INT_31         \
        "jc short L1"   \
        _XOR_AX_AX      \
        _XOR_DX_DX      \
        "jmp short finish" \
"L1:     "              \
        _XOR_AX_AX      \
        _DEC_AX         \
        _MOV_DX_AX      \
"finish: "              \
        parm            [bx] [cx dx] \
        value           [ax dx] \
        modify exact    [ax dx];

#pragma aux             _TinyDPMISetRights = \
        "mov ax,9"      \
        _INT_31         \
        "jc short L1"   \
        _XOR_AX_AX      \
        _XOR_DX_DX      \
        "jmp short finish" \
"L1:     "              \
        _XOR_AX_AX      \
        _DEC_AX         \
        _MOV_DX_AX      \
"finish: "              \
        parm            [bx] [cx] \
        value           [ax dx] \
        modify exact    [ax dx];

#pragma aux             _TinyDPMIGetDescriptor = \
        _PUSH_ES        \
        _MOV_ES_CX      \
        "mov ax,0bh"    \
        _INT_31         \
        "jc short L1"   \
        _XOR_AX_AX      \
        _XOR_DX_DX      \
        "jmp short finish" \
"L1:     "              \
        _XOR_AX_AX      \
        _DEC_AX         \
        _MOV_DX_AX      \
"finish: "              \
        _POP_ES         \
        parm            [bx] [cx di] \
        value           [ax dx] \
        modify exact    [ax dx];

#pragma aux             _TinyDPMISetDescriptor = \
        _PUSH_ES        \
        _MOV_ES_CX      \
        "mov ax,0bh"    \
        _INT_31         \
        "jc short L1"   \
        _XOR_AX_AX      \
        _XOR_DX_DX      \
        "jmp short finish" \
"L1:     "              \
        _XOR_AX_AX      \
        _DEC_AX         \
        _MOV_DX_AX      \
"finish: "              \
        _POP_ES         \
        parm            [bx] [cx di] \
        value           [ax dx] \
        modify exact    [ax dx];

#endif

#pragma pack()

#endif
