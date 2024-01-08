/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  command line parameter handling interface routines
*
****************************************************************************/

#ifndef PARAM_INCLUDED
#define PARAM_INCLUDED

#include "cmdlnprs.gh"
#include "cmdlnprs.h"
#include "cmdscan.h"
#ifndef NO_REPLACE
#include "scan.h"
#endif


extern void     ScanParamInit( void );
extern void     ScanParamFini( void );
extern int      ProcOptions( OPT_STORAGE *data, const char *str );
extern int      SetOptions( OPT_STORAGE *data, const char *infile, const char *outfile );
#ifndef NO_REPLACE
extern char     *FindAndReplace( char *stringFromFile, FRStrings *frStrings );
extern void     PrependToString( ScanValue *value, char *stringFromFile );
#endif

#endif
