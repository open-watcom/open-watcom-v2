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
* Description:  Enhanced Unix cp global variables.
*
****************************************************************************/


#include "cp.h"

bool    rflag = false;
bool    iflag = false;
bool    npflag = true;
bool    aflag = false;
bool    fflag = false;
bool    tflag2 = false;
bool    Tflag1 = false;
bool    dflag2 = false;
bool    Dflag1 = false;
bool    sflag = false;
bool    todflag = false;
bool    pattrflag = false;
bool    rxflag = false;

char OutOfMemory[] = "Out of memory!\n";
char InvalidTDStr[] = "Invalid time/date string\n";

ctrl_block      *CBHead = NULL;
ctrl_block      *CBTail = NULL;

long            TotalBytes = 0;
unsigned        DumpCnt = 0;
unsigned        FileCnt = 0;
unsigned        TotalFiles = 0;
unsigned        DirCnt = 0;
unsigned        TotalDirs = 0;
unsigned long   TotalTime;
clock_t         StartTime;
timedate        after_t_d = { 0,0,0,0,0,0 };
timedate        before_t_d = { 1000,1000,1000,1000,1000,1000 };
