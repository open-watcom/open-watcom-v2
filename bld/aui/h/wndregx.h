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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


// This stuff is just for the RX processor itself

#define ALLOC WndAlloc
#define STANDALONE_RX
#define CASEIGNORE SrchIgnoreCase
#define MAGICFLAG FALSE
#define MAGICSTR SrchIgnoreMagic
#define META SrchMetaChars
extern char SrchIgnoreMagic[];
extern char near SrchMetaChars[];
extern void *WndAlloc(unsigned int );
#define STANDALONE_RX
#ifdef bool
    #define BOOL_DEFINED
#endif
#include "regexp.h"

// This stuff is for public consumption
extern char SrchIgnoreCase;
extern char SrchMagicChars[];
extern char SrchRX;

extern void             *WndCompileRX( char *expr );
extern void             WndFreeRX( void *rx );
extern bool             WndRXFind( void *rx, char **pos, char **endpos );
extern void             WndSetMagicStr( char *str );
