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


extern bool     ProcPackExp( void );
extern bool     ProcMinData( void );
extern bool     ProcMaxData( void );
extern bool     ProcPharLap( void );

extern bool     ProcMinReal( void );
extern bool     ProcMaxReal( void );
extern bool     ProcRealBreak( void );
extern bool     ProcCallBufs( void );
extern bool     ProcMiniBuf( void );
extern bool     ProcMaxiBuf( void );
extern bool     ProcNIStack( void );
extern bool     ProcIStkSize( void );
extern bool     ProcUnpriv( void );
extern bool     ProcPriv( void );
extern bool     ProcFlags( void );

extern bool     ProcPharFlat( void );
extern bool     ProcRex( void );
extern bool     ProcPharSegmented( void );

extern void     SetPharFmt( void );
extern void     FreePharFmt( void );
