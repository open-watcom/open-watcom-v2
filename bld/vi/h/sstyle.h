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
* Description:  Syntax highlighting interface.
*
****************************************************************************/


#ifndef _SSTYLE_INCLUDED
#define _SSTYLE_INCLUDED

/*----- INCLUDES -----*/
#include "limits.h"

/*----- CONSTANTS -----*/
#define BEYOND_TEXT     SHRT_MAX
#define MAX_SS_BLOCKS   200

/*----- STRUCTURES -----*/

/*----- EXPORTS -----*/
extern type_style   SEType[SE_NUMTYPES];


/*----- PROTOTYPES -----*/
void            SSInitLanguageFlags( linenum );
void            SSInitLanguageFlagsGivenValues( ss_flags * );
void            SSGetLanguageFlags( ss_flags * );
bool            SSKillsFlags( char );
void            SSDifBlock( ss_block *, char *, int, line *, linenum, int * );
ss_block        *SSNewBlock( void );
void            SSKillBlock( ss_block * );
syntax_element  SSGetStyle( int, int );
void            SSInitBeforeConfig( void );
void            SSInitAfterConfig( void );
void            SSFini( void );

#endif
