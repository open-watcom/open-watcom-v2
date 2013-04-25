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
* Description:  Interface for DLLs pluggable into the Watcom IDE (and wmake).
*
****************************************************************************/


#ifndef IDEDLL_H_INCLUDED
#define IDEDLL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <sys/stat.h>

#include "pushpck4.h"

#if defined( __WATCOMC__ ) && defined( __386__ )
#define IDEAPI                  __stdcall
#else
#define IDEAPI
#endif

#if !defined( IDE_PGM )
#define IDEDLLENTRY             __declspec(dllexport)
#else
#define IDEDLLENTRY
#endif

#define IDE_CUR_DLL_VER         3
#define IDE_CUR_INFO_VER2       2       //VERSION 2 (see below)
#define IDE_CUR_INFO_VER3       3       //VERSION 3 (see below)
#define IDE_CUR_INFO_VER4       4       //VERSION 4 (see below)
#define IDE_CUR_INFO_VER5       5       //VERSION 5 (see below)
#define IDE_CUR_INFO_VER        6

typedef unsigned short  IDEBool;
typedef void            *IDEDllHdl;
typedef void            *IDECBHdl;

struct IDEMsgInfo;
typedef void (*BatchFilter)( void *cookie, const char *msg );
typedef void (*BatchDllFilter)( void *cookie, struct IDEMsgInfo *errinfo );

/*******************************************
 *
 * Error Information
 *
 *******************************************/

#define __IdeMsgSeverity     \
  _IdeMsgSeverity( WARNING ) \
, _IdeMsgSeverity( ERROR   ) \
, _IdeMsgSeverity( NOTE    ) \
, _IdeMsgSeverity( BANNER  ) \
, _IdeMsgSeverity( DEBUG   ) \
, _IdeMsgSeverity( NOTE_MSG)


typedef unsigned IDEMsgSeverity;
enum {
#   define _IdeMsgSeverity(a) IDEMSGSEV_ ## a
    __IdeMsgSeverity
#   undef _IdeMsgSeverity
};

#define IdeSeverityDiagnostic   ( 0 \
        | ( 1 << IDEMSGSEV_WARNING )    \
        | ( 1 << IDEMSGSEV_ERROR )      \
        | ( 1 << IDEMSGSEV_NOTE )       \
        )

#define IdeSeverityNonDebugText ( 0 \
        | ( 1 << IDEMSGSEV_BANNER )     \
        | ( 1 << IDEMSGSEV_NOTE_MSG )   \
        )

#define IdeSeverityIs( s, m )   ((( 1 << (s) ) & (m) ) != 0 )

#define IDE_INFO_READABLE       0x00000001
#define IDE_INFO_HAS_SRC_FILE   0x00000002
#define IDE_INFO_HAS_SRC_LINE   0x00000004
#define IDE_INFO_HAS_SRC_COL    0x00000008
#define IDE_INFO_HAS_HELP       0x00000010
#define IDE_INFO_HAS_MSG_NUM    0x00000020
#define IDE_INFO_HAS_LNK_FILE   0x00000040
#define IDE_INFO_HAS_LNK_SYMBOL 0x00000080
#define IDE_INFO_HAS_MSG_GRP    0x00000100

#define IDE_IS_READABLE( x )    ( ((x)->flags) & IDE_INFO_READABLE )
#define IDE_HAS_SRC_FILE( x )   ( ((x)->flags) & IDE_INFO_HAS_SRC_FILE )
#define IDE_HAS_SRC_LINE( x )   ( ((x)->flags) & IDE_INFO_HAS_SRC_LINE )
#define IDE_HAS_SRC_COL( x )    ( ((x)->flags) & IDE_INFO_HAS_SRC_COL )
#define IDE_HAS_HELP( x )       ( ((x)->flags) & IDE_INFO_HAS_HELP )
#define IDE_HAS_MSG_NUM( x )    ( ((x)->flags) & IDE_INFO_HAS_MSG_NUM )
#define IDE_HAS_LNK_FILE( x )   ( ((x)->flags) & IDE_INFO_HAS_LNK_FILE )
#define IDE_HAS_LNK_SYMBOL( x ) ( ((x)->flags) & IDE_INFO_HAS_LNK_SYMBOL )
#define IDE_HAS_MSG_GRP( x )    ( ((x)->flags) & IDE_INFO_HAS_MSG_GRP )

typedef struct IDEMsgInfo {             // IDE MESSAGE INFORMATION
    IDEMsgSeverity       severity;      // - severity code
                                        // Message Information Present
    unsigned long        flags;
                                        // Help Iinformation
    char const           *helpfile;     // - name of help file
    unsigned long        helpid;        // - help identifier
                                        // Message Information
    const char           *msg;          // - message
    char const           *src_symbol;   // - symbol
    char const           *src_file;     // - source/link-file name
    unsigned long        src_line;      // - source-file line
    unsigned long        src_col;       // - source-file column
    unsigned long        msg_no;        // - message number
    char                 msg_group[8];  // - message group
} IDEMsgInfo;

#define __IdeMsgClass          \
  _IdeMsgClass( LINE_COL     ) \
, _IdeMsgClass( LINE         ) \
, _IdeMsgClass( FILE         ) \
, _IdeMsgClass( GENERIC      ) \
, _IdeMsgClass( SYMBOL       ) \
, _IdeMsgClass( SYMBOL_FILE  )

typedef unsigned IDEMsgClass;
enum {
#   define _IdeMsgClass(a) IDEMSGINFO_ ## a
    __IdeMsgClass
#   undef _IdeMsgClass
};

/*******************************************
 * Output Information structure used by
 * version 2 DLL's
 *******************************************/
typedef struct {
    unsigned    line;           // line where error occurred
    unsigned    column;         // column where error occurred
    const char  *file;          // source file where error occurred
} LineColErr;

typedef struct {
    unsigned    line;           // line where error occurred
    const char  *file;          // source file where error occurred
} LineErr;

typedef struct {
    const char  *file;          // source file where error occurred
} FileErr;

// these structures are for use by the linker
typedef struct {
    const char  *symbol;        // symbol that caused the error
    const char  *file;          // file where the symbol was referenced
} SymbolFileErr;

typedef struct {
    const char  *symbol;        // symbol that caused the error
} SymbolErr;


typedef struct IDEMsgInfo2 {
    IDEMsgSeverity      severity;
    const char          *msg;

    const char          *helpfile;
    unsigned long       helpid;         /* only used if helpfile is non NULL */

    IDEMsgClass         type;
    union {
        LineColErr      line_col;
        LineErr         line;
        FileErr         file;
        SymbolFileErr   sym_file;
        SymbolErr       symbol;
    } u;
} IDEMsgInfo2;


typedef struct {
    unsigned            ver;
    unsigned char       ignore_env;
    unsigned char       cmd_line_has_files;     //VERSION 2
    unsigned char       console_output;         //VERSION 3
    unsigned char       progress_messages;      //VERSION 4
    unsigned char       progress_index;         //VERSION 5
} IDEInitInfo;

typedef void IDEDataInfo;

typedef enum {
    IDE_READ
  , IDE_WRITE
} IDEIOOpts;

typedef unsigned        IDEInfoType;
#define IDE_GET_SOURCE_FILE     1
#define IDE_GET_TARGET_FILE     2
#define IDE_GET_OBJ_FILE        3
#define IDE_GET_LIB_FILE        4
#define IDE_GET_RES_FILE        5
#define IDE_GET_ENV_VAR         6
#define IDE_GET_AI              7

// character values passed in the code parameter of IDEJavaSrcDepFile
// callback function
#define IDE_DEP_FILETYPE_SOURCE         'S'
#define IDE_DEP_FILETYPE_TARGET         'T'
#define IDE_DEP_FILETYPE_DEP_CLS        'C'
#define IDE_DEP_FILETYPE_DEP_ZIP        'Z'

#if defined( _WIN64 )
typedef unsigned __int64 IDEGetInfoWParam;
typedef unsigned __int64 IDEGetInfoLParam;
#else
typedef unsigned long IDEGetInfoWParam;
typedef unsigned long IDEGetInfoLParam;
#endif

typedef IDEBool IDEAPI (*IDERunBatchFn)( IDECBHdl hdl, const char *cmdline,
                                BatchFilter cb, void *cookie );
typedef IDEBool IDEAPI (*IDEPrintMsgFn)( IDECBHdl hdl, const char *message );
typedef IDEBool IDEAPI (*IDEGetInfoFn)( IDECBHdl hdl, IDEInfoType type,
                                IDEGetInfoWParam wparam, IDEGetInfoLParam lparam );
typedef IDEBool IDEAPI (*IDEMsgInfoFn)( IDECBHdl hdl, IDEMsgInfo *info );
typedef IDEBool IDEAPI (*IDERunDllFn)( IDECBHdl hdl, const char *dllname,
                                const char *cmdline, BatchDllFilter cb,
                                void *cookie );
typedef IDEBool IDEAPI (*IDERunBatchCwdFn)( IDECBHdl hdl,
                                const char *cmdline, const char *cwd,
                                BatchFilter cb, void *cookie );
typedef IDEBool IDEAPI (*IDEOpenJavaSource)( IDECBHdl hdl, const char *name, IDEIOOpts opt, IDEDataInfo **info );
typedef IDEBool IDEAPI (*IDEOpenClassFile)( IDECBHdl hdl, const char *name, IDEIOOpts opt, IDEDataInfo **info );
typedef IDEBool IDEAPI (*IDEPackageExists)( IDECBHdl hdl, const char *name );
typedef int     IDEAPI (*IDEGetSize)( IDECBHdl hdl, IDEDataInfo *info );
typedef time_t  IDEAPI (*IDEGetTimeStamp)( IDECBHdl hdl, IDEDataInfo *info );
typedef IDEBool IDEAPI (*IDEIsReadOnly)( IDECBHdl hdl, IDEDataInfo *info );
typedef int     IDEAPI (*IDEReadData)( IDECBHdl hdl, IDEDataInfo *info, char *buffer, int max_len );
typedef IDEBool IDEAPI (*IDEClose)( IDECBHdl hdl, IDEDataInfo *info );
typedef size_t  IDEAPI (*IDEReceiveOutput)( IDECBHdl hdl, IDEDataInfo *info, void const *buffer, size_t len );
typedef void    IDEAPI (*IDEReceiveIndex)( IDECBHdl hdl, unsigned index /* 0-99 */ );

typedef IDEBool IDEAPI (*IDEJavaSrcDepBegin)( IDECBHdl hdl );
//the values passed in the code parameter of the IDEJavaSrcDepFile are
//defined as the macros IDE_DEP_FILETYPE_...
typedef IDEBool IDEAPI (*IDEJavaSrcDepFile)( IDECBHdl hdl, char code, char const *filename );
typedef IDEBool IDEAPI (*IDEJavaSrcDepEnd)( IDECBHdl hdl );

// structure used by version 1 DLL's
typedef struct {
    // building functions
    IDERunBatchFn               RunBatch;
    IDEPrintMsgFn               PrintMessage;
    IDEPrintMsgFn               PrintWithCRLF;

    // Query functions
    IDEGetInfoFn                GetInfo;
} IDECallBacks1;

// structure used by current version DLL's
typedef struct {
    // building functions
    IDERunBatchFn               RunBatch;
    IDEPrintMsgFn               PrintMessage;
    IDEPrintMsgFn               PrintWithCRLF;
    IDEMsgInfoFn                PrintWithInfo;

    // Query functions
    IDEGetInfoFn                GetInfo;

    IDEPrintMsgFn               ProgressMessage;
    IDERunDllFn                 RunDll;         // may be NULL
    IDERunBatchCwdFn            RunBatchCWD;    // may be NULL
    IDEOpenJavaSource           OpenJavaSource; // may be NULL
    IDEOpenClassFile            OpenClassFile;  // may be NULL
    IDEPackageExists            PackageExists;  // may be NULL
    IDEGetSize                  GetSize;        // may be NULL
    IDEGetTimeStamp             GetTimeStamp;   // may be NULL
    IDEIsReadOnly               IsReadOnly;     // may be NULL
    IDEReadData                 ReadData;       // may be NULL
    IDEClose                    Close;          // may be NULL
    IDEReceiveOutput            ReceiveOutput;  // may be NULL

    IDEReceiveIndex             ProgressIndex;

    IDEJavaSrcDepBegin          SrcDepBegin;    // may be NULL (begin java src dependencies)
    IDEJavaSrcDepFile           SrcDepFile;     // (java src dependency file)
    IDEJavaSrcDepEnd            SrcDepEnd;      // (end java src dependendency)
} IDECallBacks;

/*********************************************************
 * Routines implemented by the DLL
 *********************************************************/
IDEDLLENTRY unsigned IDEAPI IDEGetVersion( void );
IDEDLLENTRY IDEBool  IDEAPI IDEInitDLL( IDECBHdl hdl, IDECallBacks *cb, IDEDllHdl *info );
IDEDLLENTRY IDEBool  IDEAPI IDEPassInitInfo( IDEDllHdl hdl, IDEInitInfo *info );
IDEDLLENTRY void     IDEAPI IDEFiniDLL( IDEDllHdl hdl );
IDEDLLENTRY IDEBool  IDEAPI IDEDetermineBuildStatus( IDEDllHdl hdl, const char *opts, IDEBool *status );
IDEDLLENTRY IDEBool  IDEAPI IDERunYourSelf( IDEDllHdl hdl, const char *opts, IDEBool *fatalerr );
IDEDLLENTRY IDEBool  IDEAPI IDERunYourSelfArgv( IDEDllHdl hdl, int argc, char **argv, IDEBool *fatalerr );
IDEDLLENTRY void     IDEAPI IDEStopRunning( void );
IDEDLLENTRY void     IDEAPI IDEFreeHeap( void );

/**********************************************************
 * Routines the DLL should use to fill in the Output Message
 * information structure
 **********************************************************/
void IdeMsgInit                 // INITIALIZE MSG STRUCTURE
    ( IDEMsgInfo *info          // - message information
    , IDEMsgSeverity severity   // - message severity
    , char const *msg )         // - the message
;
void IdeMsgSetHelp              // SET HELP INFORMATION
    ( IDEMsgInfo *info          // - message information
    , char const *file          // - help file
    , unsigned long id )        // - help id
;
void IdeMsgSetLnkFile           // SET LINK FILE
    ( IDEMsgInfo *info          // - message information
    , char const *file )        // - file name
;
void IdeMsgSetLnkSymbol         // SET LINK SYMBOL
    ( IDEMsgInfo *info          // - message information
    , char const *sym )         // - symbol
;
void IdeMsgSetMsgGroup          // SET MESSAGE GROUP
    ( IDEMsgInfo *info          // - message information
    , char const *group )       // - group name
;
void IdeMsgSetMsgNo             // SET MESSAGE NUMBER
    ( IDEMsgInfo *info          // - message information
    , unsigned msg_no )         // - message number
;
void IdeMsgSetReadable          // MARK MSG AS "READABLE"
    ( IDEMsgInfo *info )        // - message information
;
void IdeMsgSetSrcColumn         // SET SOURCE COLUMN
    ( IDEMsgInfo *info          // - message information
    , unsigned col )            // - column number
;
void IdeMsgSetSrcFile           // SET SOURCE FILE
    ( IDEMsgInfo *info          // - message information
    , char const *file )        // - file name
;
void IdeMsgSetSrcLine           // SET SOURCE LINE
    ( IDEMsgInfo *info          // - message information
    , unsigned line )           // - line number
;


/*****************************************************
 * Routines that can be used by drivers to format
 * messages from DLL's
 *****************************************************/
void IdeMsgFormat               // FORMAT A MESSAGE
    ( IDECBHdl handle           // - handle for requestor
    , IDEMsgInfo const *info    // - message information
    , char *buffer              // - buffer
    , size_t bsize              // - buffer size
    , IDEMsgInfoFn displayer )  // - display function
;
void IdeMsgStartDll             // START OF FORMATING FOR A DLL (EACH TIME)
    ( void )
;

#include "poppck.h"

#ifdef __cplusplus
}
#endif

#endif
