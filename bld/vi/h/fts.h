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
    struct template_ll  *next, *prev;
    char                data[1];
} template_ll;

typedef struct cmd_ll {
    struct cmd_ll       *next, *prev;
    char                data[1];
} cmd_ll;

typedef struct ft_src {
    struct ft_src       *next, *prev;
    template_ll         *template_head, *template_tail;
    cmd_ll              *cmd_head, *cmd_tail;
} ft_src;

extern void         FTSInit( void );
extern void         FTSFini( void );
extern vi_rc        FTSStart( const char * );
extern vi_rc        FTSAddCmd( const char *, int );
extern vi_rc        FTSAddBoolean( bool, const char * );
extern vi_rc        FTSAddChar( char, const char * );
extern vi_rc        FTSAddInt( int, const char * );
extern vi_rc        FTSAddStr( char *, const char * );
extern vi_rc        FTSRunCmds( const char * );
extern int          FTSSearchFTIndex( const char * );
extern vi_rc        FTSEnd( void );
extern void         FTSBarfData( FILE * );
extern ft_src       *FTSGetFirst( void );
extern ft_src       *FTSGetNext( ft_src * );
extern template_ll  *FTSGetFirstTemplate( ft_src * );
extern template_ll  *FTSGetNextTemplate( template_ll * );
extern ft_src       *FTSMatchTemplate( template_ll * );
extern ft_src       *FTSMatchTemplateData( const char * );
extern void         FTSKill( ft_src * );

#endif
