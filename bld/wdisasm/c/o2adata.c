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


#include "disasm.h"


FILE                    *ObjFile;
FILE                    *Output;
FILE                    *Source;
int                     ErrCount;
int_16                  LastNum;        /* last line num record dumped */
int                     Repeats;
char                    *SrcName;
char                    *CommentName;
char                    *CodeName;
form_option             Options;
char                    DataBytes;
char                    DataLen;
bool                    ExpDump;
bool                    ImpDump;
char                    Pass;
bool                    RetFarUsed;
char                    DataString[ MAX_INSTR_LEN ];
char                    PrevString[ MAX_INSTR_LEN ];
bool                    IsPharLap;
bool                    Is32Record;
bool                    Is32BitObj;     /* true if any 32-bit records found
                                           or if it is a pharlap obj file */
char                    IntLblStart = 'L';
bool                    UnMangle;
