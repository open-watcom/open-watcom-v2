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


#ifndef _XTRARPT_H
#define _XTRARPT_H

#ifdef __cplusplus
extern "C" {
#endif

void ExtraRptDumpReport(        // DUMP REPORT INTO FILE
    const char *name )          // - name to use for generating file name
;
void ExtraRptRegisterCtr(       // REGISTER A COUNTER
    long* a_ctr,                // - addr( counter )
    const char* rpt_line )      // - report line
;
void ExtraRptRegisterMax(       // REGISTER A MAXIMUM
    long* a_ctr,                // - addr( counter )
    const char* rpt_line )      // - report line
;
void ExtraRptRegisterAvg(       // REGISTER AVERAGING
    long* a_total,              // - addr[ total ]
    long* a_count,              // - addr[ count ]
    const char* rpt_line )      // - report line
;
void ExtraRptRegisterTab(       // REGISTER TABLE
    char const* title,          // - title
    char const * const *row_labels,//- row labels
    long* table,                // - table
    unsigned rows,              // - # rows
    unsigned cols )             // - # columns
;
void ExtraRptRegisterTitle(     // REGISTER A TITLE
    const char* title )         // - title line
;

#ifdef __cplusplus
}
#endif

#endif
