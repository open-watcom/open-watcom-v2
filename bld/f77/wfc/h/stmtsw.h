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


#ifndef _STMTSW_H_INCLUDED
#define _STMTSW_H_INCLUDED

typedef enum {
    SS_HOLLERITH         = 0x0001,
    SS_DATA_INIT         = 0x0002,
    SS_COMMA_FOUND       = 0x0004,
    SS_EQUALS_FOUND      = 0x0008,
    SS_COMMA_THEN_EQ     = 0x0010,
    SS_EQ_THEN_COMMA     = 0x0020,
    SS_SCANNING          = 0x0040,
    SS_ISN_DONE          = 0x0080,
    SS_SF_REFERENCED     = 0x0100,
    SS_CONT_ERROR_ISSUED = 0x0200
} STMTSW;

#endif
