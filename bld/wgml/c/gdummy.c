/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description: implement dummy support for not (yet) implemented
*              control words and tags
*                    gml_dummy()
*                    lay_dummy()
*                    scr_dummy()
*
****************************************************************************/

#include "wgml.h"

/***************************************************************************/
/*  scr_dummy        processing                                            */
/***************************************************************************/

void    scr_dummy( void )
{
    char            cwcurr[4];          // control word string for msg

    cwcurr[0] = SCR_char;
    cwcurr[1] = *(token_buf);
    cwcurr[2] = *(token_buf + 1);
    cwcurr[3] = '\0';

    scan_restart = scan_stop;

    g_warn( wng_unsupp_cw, cwcurr );
    wng_count++;
    file_mac_info();
}


/***************************************************************************/
/*  gml_dummy        processing                                            */
/***************************************************************************/

void    gml_dummy( gml_tag tag )
{

    scan_start = scan_stop;

    g_warn( wng_unsupp_tag, gml_tagname( tag ) );
    wng_count++;
    file_mac_info();
}

/***************************************************************************/
/*  lay_dummy        processing                                            */
/***************************************************************************/

void    lay_dummy( lay_tag tag )
{

    scan_start = scan_stop;

    if( GlobalFlags.firstpass ) {       // layout msg only in pass 1
        g_warn( wng_unsupp_lay, lay_tagname( tag ) );
        wng_count++;
        show_include_stack();
    }
    eat_lay_sub_tag();                  // ignore any attribute / value
}
