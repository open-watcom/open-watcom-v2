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


#ifndef wsyshelp_class
#define wsyshelp_class

#include "wobject.hpp"
#include "wwindow.hpp"

WCLASS WSystemHelp : public WObject {
    public:
        WEXPORT WSystemHelp( WWindow *, const char *title,
                        const char *library, const char *chmfile = NULL,
                        int offset = 0 );
        WEXPORT ~WSystemHelp();

        bool WEXPORT sysHelpContent( void );
        bool WEXPORT sysHelpOnHelp( void );
        bool WEXPORT sysHelpSearch( const char *topic );
        bool WEXPORT sysHelpTopic( const char *topic );
        bool WEXPORT sysHelpId( gui_hlp_id help_id );
        const char * WEXPORT getHelpFile( void );
        const char * WEXPORT getHelpTitle( void ) { return( _title ); };
        int WEXPORT getHelpOffset( void ) { return( _offset ); };

    private:
        const char              *_title;
        const char              *_library;
        const char              *_chmfile;
        int                     _offset;
        WWindow                 *_helpWindow;
        gui_help_instance       _helpInstance;
};

#endif
