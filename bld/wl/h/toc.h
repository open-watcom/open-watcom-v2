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


extern void             InitToc( void );
extern void             CleanToc( void );
extern void             AddSymToToc( symbol * );
extern void             AddSdataOffToToc( segdata *, offset );
extern void             PrepareToc( void );
extern signed_32        FindSdataOffPosInToc( segdata *, offset );
extern offset           FindIATSymAbsOff( symbol * );
extern signed_32        FindSymPosInToc( symbol * );
extern signed_32        FindSymPosInTocv( symbol * );
extern unsigned long    GetTocSize( void );
extern void             WriteToc( virt_mem );
extern void             SetTocAddr( offset , group_entry * );
extern void             CheckIfTocSym( symbol * );
extern bool             IsTocSym( symbol * );

extern offset TocSize;
extern offset TocShift;

