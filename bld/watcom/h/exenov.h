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
* Description:  Novell NetWare NLM executable image format.
*
****************************************************************************/


#ifndef _EXENOV_H

#include "pushpck1.h"

#define MAX_DESCRIPTION_LENGTH			127
#define MAX_SCREEN_NAME_LENGTH			71
#define MAX_THREAD_NAME_LENGTH			17
#define OLD_THREAD_NAME_LENGTH			5
#define NLM_SIGNATURE					"NetWare Loadable Module\x01a"
#define NLM_VERSION						4
#define VERSION_SIGNATURE				"VeRsIoN#"
#define VERSION_SIGNATURE_LENGTH		8
#define COPYRIGHT_SIGNATURE				"CoPyRiGhT="
#define COPYRIGHT_SIGNATURE_LENGTH		10
#define MAX_COPYRIGHT_LENGTH			255
#define EXTENDED_NLM_SIGNATURE			"MeSsAgEs"
#define EXTENDED_NLM_SIGNATURE_LENGTH	(sizeof(EXTENDED_NLM_SIGNATURE)-1)
#define MSG_FILE_SIGNATURE				"NetWare Message File: "
#define MSG_FILE_SIGNATURE_LENGTH		22
#define LANGUAGE_ID_OFFSET				106
#define NUM_MESSAGES_OFFSET				110
/*
//	Internal module name must 8.3 maximum
*/
#define NOV_MAX_NAME_LEN				8
#define NOV_MAX_EXT_LEN					3
#define NOV_MAX_MODNAME_LEN				(NOV_MAX_NAME_LEN + NOV_MAX_EXT_LEN + 1)

/* the fields marked vbl are variable length. */

/* the first "fixed chunk" of the header */

typedef struct nlm_header {
    char            signature[ sizeof( NLM_SIGNATURE ) - 1 ];
    unsigned_32     version;
    char            moduleName[14];     /* 1st character is a length byte.*/
    unsigned_32     codeImageOffset;
    unsigned_32     codeImageSize;
    unsigned_32     dataImageOffset;
    unsigned_32     dataImageSize;
    unsigned_32     uninitializedDataSize;
    unsigned_32     customDataOffset;
    unsigned_32     customDataSize;
    unsigned_32     moduleDependencyOffset;
    unsigned_32     numberOfModuleDependencies;
    unsigned_32     relocationFixupOffset;
    unsigned_32     numberOfRelocationFixups;
    unsigned_32     externalReferencesOffset;
    unsigned_32     numberOfExternalReferences;
    unsigned_32     publicsOffset;
    unsigned_32     numberOfPublics;
    unsigned_32     debugInfoOffset;
    unsigned_32     numberOfDebugRecords;
    unsigned_32     codeStartOffset;
    unsigned_32     exitProcedureOffset;
    unsigned_32     checkUnloadProcedureOffset;
    unsigned_32     moduleType;
    unsigned_32     flags;
    unsigned_8      descriptionLength;
} nlm_header;

/* variable length part of the header  */

typedef struct nlm_header_2 {
    char            descriptionText[ MAX_DESCRIPTION_LENGTH + 1 ]; /*vbl*/
    unsigned_32     stackSize;
    unsigned_32     reserved;
    char            oldThreadName[ OLD_THREAD_NAME_LENGTH];
    unsigned_8      screenNameLength;
    char            screenName[ MAX_SCREEN_NAME_LENGTH + 1 ];   /*vbl*/
    unsigned_8      threadNameLength;
    char            threadName[ MAX_THREAD_NAME_LENGTH + 1 ];       /*vbl*/
} nlm_header_2;

/* the second "fixed chunk" of the header */

typedef struct nlm_header_3 {
    char            versionSignature[ VERSION_SIGNATURE_LENGTH ];
    unsigned_32     majorVersion;
    unsigned_32     minorVersion;
    unsigned_32     revision;
    unsigned_32     year;
    unsigned_32     month;      /* starts at 1 */
    unsigned_32     day;
} nlm_header_3;

/* remaining header fields */

typedef struct nlm_header_4 {
    char            copyrightSignature[ COPYRIGHT_SIGNATURE_LENGTH ];
    unsigned_8      copyrightLength;
    char            copyrightString[ MAX_COPYRIGHT_LENGTH + 1 ];    /*vbl*/
} nlm_header_4;

typedef struct {
    unsigned_8  stamp[ EXTENDED_NLM_SIGNATURE_LENGTH ];
    unsigned_32 languageID;
    unsigned_32 messageFileOffset;
    unsigned_32 messageFileLength;
    unsigned_32 messageCount;
    unsigned_32 helpFileOffset;
    unsigned_32 helpFileLength;
    unsigned_32 RPCDataOffset;
    unsigned_32 RPCDataLength;
    unsigned_32 sharedCodeOffset;
    unsigned_32 sharedCodeLength;
    unsigned_32 sharedDataOffset;
    unsigned_32 sharedDataLength;
    unsigned_32 sharedRelocationFixupOffset;
    unsigned_32 sharedRelocationFixupCount;
    unsigned_32 sharedExternalReferenceOffset;
    unsigned_32 sharedExternalReferenceCount;
    unsigned_32 sharedPublicsOffset;
    unsigned_32 sharedPublicsCount;
    unsigned_32 sharedDebugRecordOffset;
    unsigned_32 sharedDebugRecordCount;
    unsigned_32 sharedInitializationOffset;
    unsigned_32 sharedExitProcedureOffset;
    unsigned_32 productID;
    unsigned_32 reserved0;
    unsigned_32 reserved1;
    unsigned_32 reserved2;
    unsigned_32 reserved3;
    unsigned_32 reserved4;
    unsigned_32 reserved5;
} extended_nlm_header;

/* values for the flags field above. */
#define NOV_REENTRANT           0x0001
#define NOV_MULTIPLE            0x0002
#define NOV_SYNCHRONIZE         0x0004
#define NOV_PSEUDOPREEMPTION    0x0008
#define NOV_OS_DOMAIN           0x0010		/*	16	*/
#define NOV_AUTOUNLOAD          0x0040		/*	64	*/

#define DBG_DATA 0
#define DBG_CODE 1

typedef struct nov_dbg_info {
    unsigned_8          type;
    unsigned_32         offset;
    unsigned_8          namelen;
} nov_dbg_info;

#define DEFAULT_PRELUDE_FN_CLIB	"_Prelude"
#define DEFAULT_EXIT_FN_CLIB	"_Stop"

#define DEFAULT_PRELUDE_FN_LIBC	"_LibCPrelude"
#define DEFAULT_EXIT_FN_LIBC	"_LibCPostlude"

/* values for upper bits of the relocation offsets. */
#define NOV_IMP_NONRELATIVE		0x80000000
#define NOV_IMP_ISCODE			0x40000000

#define NOV_EXP_ISCODE			0x80000000

#include "poppck.h"

#define _EXENOV_H
#endif
