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


/*
   CPGLOB.C - enhanced unix cp global variables

   Date         By              Reason
   ====         ==              ======
   17-aug-90    Craig Eisler    defined
   19-oct-91    Craig Eisler    more work
   11-nov-91    Craig Eisler    cleaned up
   20-jan-92    Craig Eisler    added rxflag
 */
#include <stdio.h>
#include <dos.h>
#include "cp.h"

int rflag = FALSE, iflag = FALSE, npflag = TRUE,  aflag = FALSE;
int fflag = FALSE, tflag = FALSE, Tflag = FALSE, dflag = FALSE;
int Dflag = FALSE, sflag = FALSE, todflag = FALSE;
int pattrflag = FALSE,rxflag = FALSE;

char OutOfMemory[] = "Out of memory!\n";
char InvalidTDStr[] = "Invalid time/date string\n";

ctrl_block *CBHead=NULL,*CBTail=NULL;

long TotalBytes=0;
unsigned DumpCnt = 0;
unsigned FileCnt = 0,TotalFiles=0;
unsigned DirCnt = 0, TotalDirs = 0;
unsigned long StartTime, TotalTime;
timedate after_t_d = { 0,0,0,0,0,0 };
timedate before_t_d = { 1000,1000,1000,1000,1000,1000 };
