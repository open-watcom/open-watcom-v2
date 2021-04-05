/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Formatting routines prototypes.
*
****************************************************************************/


#ifndef FORMAT_H
#define FORMAT_H

#include "common.h"
#include "strings.h"
#include "param.h"

#define SCREENWIDTH     78
#define RULERSTRING     "-"

void indentMore( int level );
void indentLess( int level );
void printIndent( bool newline, Parameters *param );
void printRuler( bool indent, bool newline, Parameters *param );
void printParameters( Parameters *param );
void printDefaultParameters( void );
void printBanner( void );
void printHelp( void );
void printDosHeader( ExeFile *exeFile, Parameters *param );
void printPeHeader( ExeFile *exeFile, Parameters *param );
void printResObject( ExeFile *exeFile, Parameters *param );
void printTableContents( ResTableEntry *table, ExeFile *exeFile,
                         Parameters *param, long addr, int depth );
void printDirContents( ResDirEntry *dir, ExeFile *exeFile,
                       Parameters  *param, long addr, int depth );
void printDataContents( ResDataEntry *data, ExeFile *exeFile,
                        Parameters   *param, long addr );
void printHexHeader( long addr, size_t length, ExeFile *exeFile );

#endif
