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
* Description:  Header file for the simple Portable Executeable DLL loader
*               library. This library can be used to load PE DLL's under
*               any Intel based OS, provided the DLL's do not have any
*               imports in the import table.
*
*               NOTE: This loader module expects the DLL's to be built with
*                     Watcom C++ and may produce unexpected results with
*                     DLL's linked by another compiler.
*
****************************************************************************/

#ifndef __PELOADER_H
#define __PELOADER_H

#include "watcom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

/*---------------------- Macros and type definitions ----------------------*/

#pragma pack(1)

/****************************************************************************
REMARKS:
Structure of a module handle when loaded from disk
{secret}
****************************************************************************/
typedef struct {
    u_char  *pbase;         /* Base of image in memory                  */
    u_char  *ptext;         /* Text section read from disk              */
    u_char  *pdata;         /* Data section read from disk              */
    u_char  *pbss;          /* BSS section read                         */
    u_char  *pimport;       /* Import section read from disk            */
    u_char  *pexport;       /* Export section read from disk            */
    u_long  textBase;       /* Base of text section in image            */
    u_long  dataBase;       /* Base of data section in image            */
    u_long  bssBase;        /* Base of BSS data section in image        */
    u_long  importBase;     /* Offset of import section in image        */
    u_long  exportBase;     /* Offset of export section in image        */
    u_long  exportDir;      /* Offset of export directory               */
    char    *modname;       /* Filename of the image                    */
    } PE_MODULE;

/****************************************************************************
REMARKS:
Defines the error codes returned by the library

HEADER:
drvlib/peloader.h

MEMBERS:
PE_ok                   - No error
PE_fileNotFound         - DLL file not found
PE_outOfMemory          - Out of memory loading DLL
PE_invalidDLLImage      - DLL image is invalid or corrupted
PE_unknownImageFormat   - DLL image is in a format that is not supported
****************************************************************************/
typedef enum {
    PE_ok,
    PE_fileNotFound,
    PE_outOfMemory,
    PE_invalidDLLImage,
    PE_unknownImageFormat
    } PE_errorCodes;

#pragma pack()

/*------------------------- Function Prototypes ---------------------------*/

#ifdef  __cplusplus
extern "C" {            /* Use "C" linkage when in C++ mode */
#endif

u_long      PE_getFileSize(FILE *f,u_long startOffset);
PE_MODULE * PE_loadLibraryExt(FILE *f,u_long offset,u_long *size);
PE_MODULE * PE_loadLibrary(const char *szDLLName);
PE_MODULE * PE_loadLibraryHandle(int fd,const char *szDLLName);
void *      PE_getProcAddress(PE_MODULE *hModule,const char *szProcName);
void        PE_freeLibrary(PE_MODULE *hModule);
int         PE_getError(void);

#ifdef  __cplusplus
}                       /* End of "C" linkage for C++   */
#endif  /* __cplusplus */

#endif  /* __PELOADER_H */

