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


#ifndef READ_H
#define READ_H

#include "common.h"
#include "strings.h"

#define RESOURCE_OBJECT_NAME    ".rsrc"

TableOrData tableOrData( ResDirEntry *dir );
pe_va getDirNameRva( ResDirEntry *dir );
long int getDirNameAbs( ResDirEntry *dir, ExeFile *exeFile );
long int getDirChildAbs( ResDirEntry *dir, ExeFile *exeFile );
long int getResDataRva( ResDataEntry *data, ExeFile *exeFile );
long int getResDataAbs( ResDataEntry *data, ExeFile *exeFile );
bool openExeFile( ExeFile *exeFile, Parameters *param );
bool readExeHeaders( ExeFile *exeFile );
bool findResourceObject( ExeFile *exeFile );
bool loadTableEntry( ResTableEntry *table, ExeFile *exeFile, long int addr );
bool loadDirEntry( ResDirEntry *dir, ExeFile *exeFile );
bool loadDataEntry( ResDataEntry *data, ExeFile *exeFile, long int addr );
bool loadAllResources( ExeFile *exeFile );
void freeAllResources( ExeFile *exeFile );

#endif
