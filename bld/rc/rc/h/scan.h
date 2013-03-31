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
* Description:  Data types used by lexical scanner.
*
****************************************************************************/


#ifndef SCAN_H_INCLUDED
#define SCAN_H_INCLUDED

#include "varstr.h"

typedef struct ScanString {
    int         lstring;        /* was string prefixed by L like this L"bob" */
    int         length;
    char        *string;
} ScanString;

typedef enum {
    SCAN_INT_TYPE_DEFAULT,
    SCAN_INT_TYPE_LONG,
    SCAN_INT_TYPE_UNSIGNED
} ScanIntType;

typedef struct {
    ScanIntType         type;   /* non-default int type - long/unsigned */
    unsigned long       val;
    char                *str;
} ScanInt;

typedef union {
    ScanInt     intinfo;
    ScanString  string;
    char        UnknownChar;
} ScanValue;

extern void  ScanInit( void );
extern int   Scan( ScanValue * value );
extern void  ScanInitStatics( void );
extern void  PrependToString( ScanValue *value, char *stringFromFile );
#endif
