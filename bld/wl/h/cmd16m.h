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
* Description:  DOS/16M command line parsing function prototypes.
*
****************************************************************************/


extern bool     ProcMemory16M( void );
extern bool     ProcTryExtended( void );
extern bool     ProcTryLow( void );
extern bool     ProcForceExtended( void );
extern bool     ProcForceLow( void );
extern bool     ProcTransparent( void );
extern bool     ProcTStack( void );
extern bool     ProcTData( void );
extern bool     ProcKeyboard( void );
extern bool     ProcOverload( void );
extern bool     ProcInt10( void );
extern bool     ProcInit00( void );
extern bool     ProcInitFF( void );
extern bool     ProcRotate( void );
extern bool     ProcSelectors( void );
extern bool     ProcAuto( void );
extern bool     ProcBuffer( void );
extern bool     ProcGDTSize( void );
extern bool     ProcRelocs( void );
extern bool     Proc16MNoRelocs( void );
extern bool     ProcSelStart( void );
extern bool     ProcExtended( void );
extern bool     ProcDataSize( void );
extern void     SetD16MFmt( void );
extern void     FreeD16MFmt( void );
extern bool     Proc16M( void );
