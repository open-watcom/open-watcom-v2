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


#include <stddef.h>
#include <stdio.h>

/*--------------------- Resources --------------------------------*/

void initWicResources( char * fname );
int getResStr( int resourceid, char *buffer );
void zapWicResources(void);

/*------------------------ Error reporting ----------------------*/

void initErrorFile(char *name);
void zapErrorFile(void);

/* Report error and take appropriate action (may be terminate) */
void reportError(WicErrors err, ...);

/*-------------------- Memory management ------------------------------------*/

#include "fmem.h"
void initMemory(void);
void zapMemory(void);
void *BasicMalloc(size_t size);
void BasicFree(void *ptr);
void outOfMemory(void);
char *wicStrdup(const char *src);
void checkMemory(void);
#define wicMalloc FAlloc
#define wicFree   FFree

/*---------------------- Exit --------------------------------------*/

void wicExit(int exitCode);  // Defined in main.c
void printUsageAndExit(void);

/*------------------------ Debugging ----------------------------*/

/* Use this in your code to warn yourself, as in:
argc = argc;  WARNING;  // Don't forget to implement this !!!
*/
#define WARNING  1

void initDebug(void);
void zapDebug(void);
void wicAssert( int exprTrue, char *expr, char *file, int line);
void debugOut(char *format, ...);

#ifdef NDEBUG
 #define assert(__ignore) ((void)0)
#else
 #if !defined(NO_EXT_KEYS) /* extensions enabled */
  #define assert(expr)   ((expr)?(void)0:wicAssert(0,#expr,__FILE__,__LINE__))
 #else
  #define assert(expr)  wicAssert(expr,#expr,__FILE__,__LINE__)
 #endif
#endif

/*-------------------------- File IO -----------------------------*/

void wicFclose(FILE *fp);
FILE *wicFopen(const char *filename, const char *mode);
int fileReadable(char *fname);

/*---------------------------- Other ----------------------------*/

#define WITHIN_RANGE(a, min, max)  (((min) <= (a) && (a) <= (max)) ? 1:0)
void dribble(void);
void wicPrintMessage(char *s);
char *setNewFileExt(char *newName, char *oldName, char *newExt);
void setDefaultExt(char *fname, char *ext);
