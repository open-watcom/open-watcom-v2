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


#ifndef _FTS_INCLUDED
#define _FTS_INCLUDED

typedef struct template_ll {
    struct  template_ll     *next, *prev;
    char                    data[ 1 ];
} template_ll;

typedef struct cmd_ll {
    struct  cmd_ll      *next, *prev;
    char                data[ 1 ];
} cmd_ll;

typedef struct ft_src {
    struct  ft_src      *next, *prev;
    template_ll         *template_head, *template_tail;
    cmd_ll              *cmd_head, *cmd_tail;
} ft_src;

void FTSInit( void );
void FTSFini( void );
int FTSStart( char * );
int FTSAddCmd( char *, int );
int FTSAddBoolean( bool, char * );
int FTSAddChar( char, char * );
int FTSAddInt( int, char * );
int FTSAddStr( char *, char * );
int FTSRunCmds( char * );
int FTSEnd( void );
void FTSBarfData( FILE * );
ft_src *FTSGetFirst( void );
ft_src *FTSGetNext( ft_src * );
template_ll *FTSGetFirstTemplate( ft_src * );
template_ll *FTSGetNextTemplate( template_ll * );
ft_src *FTSMatchTemplate( template_ll * );
ft_src *FTSMatchTemplateData( char * );
void FTSKill( ft_src * );

#endif
