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


#ifndef WDIS_GLOBALS_INCLUDED
#define WDIS_GLOBALS_INCLUDED

#include "dtypes.h"
#include "msg.h"

#define MAX_OBJ_NAME            1024
#define MAX_INS_NAME            80

#define PREFIX_SIZE_TABS        4

extern dis_format_flags     DFormat;
extern char                 *ObjFileName;
extern char                 *ListFileName;
extern char                 *SourceFileName;
extern const char           *SourceFileInObject;
extern char                 *SourceFileInDwarf;
extern FILE                 *SourceFile;

extern char                 *CommentString;

extern bool                 source_mix;

extern hash_table           HandleToSectionTable;
extern hash_table           HandleToLabelListTable;
extern hash_table           HandleToRefListTable;
extern hash_table           SymbolToLabelTable;
extern hash_table           NameRecognitionTable;
extern hash_table           SkipRefTable;

extern publics_struct       Publics;
extern section_list_struct  Sections;
extern char                 LabelChar;
extern char                 QuoteChar;
extern wd_options           Options;

extern orl_handle           ORLHnd;
extern orl_file_handle      ObjFileHnd;
extern orl_sec_handle       DebugHnd;
extern dis_handle           DHnd;
extern FILE                 *OutputDest;

#endif
