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
* Description:  DOS/16M specific constants and routines.
*
****************************************************************************/


// these are the values used in the struct fmt_d16m_data flags field.

enum {
    TRANS_DATA          = 0x0001,   // set if transparent data chosen
    TRANS_STACK         = 0x0002,   // set if transparent stack chosen
    FORCE_NO_RELOCS     = 0x0004,   // set if no relocs option chosen.
    DATASIZE_SPECD      = 0x0008,   // datasize option was chosed.
};

#define TRANS_SPECD (TRANS_DATA | TRANS_STACK)

extern void     Fini16MLoadFile( void );
extern unsigned ToD16MSel( unsigned );
extern segment  Find16MSeg( segment );
extern void     CalcGrpSegs( void );
extern void     MakeDos16PM( void );
