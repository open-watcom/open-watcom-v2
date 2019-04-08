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
* Description:  OS/2 specific utility routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#include "setup.h"
#include "setupwpi.h"
#include "setupinf.h"
#include "guistat.h"


#define ICONFILETAG     "ICONFILE="

static HOBJECT create_group( const VBUF *group, const VBUF *grp_filename )
/************************************************************************/
{
    HOBJECT     hobj;
    VBUF        cmd;
    VBUF        iconfile;

    VbufInit( &cmd );
    VbufInit( &iconfile );

    VbufConcStr( &cmd, "OPEN=ICON;OBJECTID=" );
    VbufConcVbuf( &cmd, grp_filename );
    VbufConcChr( &cmd, ';' );
    // add more parms here if necessary
    SimGetPMApplGroupIconFile( &iconfile );
    if( VbufLen( &iconfile ) > 0 ) {
        ReplaceVars1( &iconfile );
        VbufConcStr( &cmd, ICONFILETAG );
        VbufConcVbuf( &cmd, &iconfile );
        VbufConcChr( &cmd, ';' );
    }
    hobj = WinCreateObject( "WPFolder", VbufString( group ), VbufString( &cmd ), "<WP_DESKTOP>", CO_UPDATEIFEXISTS );

    VbufFree( &iconfile );
    VbufFree( &cmd );
    return( hobj );
}


static void remove_group( const VBUF *group )
/*******************************************/
{
    HOBJECT     hobj;

    hobj = WinQueryObject( VbufString( group ) );
    WinDestroyObject( hobj );
}


bool CreatePMInfo( bool uninstall )
/*********************************/
{
    VBUF                PMProgName;
    VBUF                PMProgDesc;
    VBUF                PMParams;
    VBUF                iconfile;
    VBUF                WorkingDir;
    VBUF                Folder;
    VBUF                Cmd;
    VBUF                tmp;
    VBUF                group;
    VBUF                GroupFileName;
    int                 nDirIndex;
    int                 iconindex;
    int                 i;
    int                 num;
    size_t              len;
    const char          *p;
    bool                ok;

    /*
     * To uninstall, simply nuke all folders
     */
    ok = true;
    if( uninstall ) {
        VbufInit( &GroupFileName );
        num = SimGetPMGroupsNum();
        for( i = 0; i < num; i++ ) {
            SimGetPMGroupFile( i, &GroupFileName );
            if( VbufLen( &GroupFileName ) > 0 ) {
                /*
                 * Delete the PM Group box
                 */
                VbufPrepChr( &GroupFileName, '<' );
                VbufConcChr( &GroupFileName, '>' );
                remove_group( &GroupFileName );
            }
        }
        VbufFree( &GroupFileName );
    } else if( SimIsPMApplGroupDefined() ) {
        VbufInit( &group );
        VbufInit( &GroupFileName );

        SimGetPMApplGroupName( &group );
        SimGetPMApplGroupFile( &GroupFileName );
        if( VbufLen( &GroupFileName ) > 0 ) {
            VbufPrepChr( &GroupFileName, '<' );
            VbufConcChr( &GroupFileName, '>' );
        } else {
            VbufConcStr( &GroupFileName, "<WSETUP_FLDR>" );
        }
        ok = ( create_group( &group, &GroupFileName ) != NULLHANDLE );

        VbufInit( &tmp );
        VbufInit( &PMProgName );
        VbufInit( &PMProgDesc );
        VbufInit( &PMParams );
        VbufInit( &iconfile );
        VbufInit( &WorkingDir );
        VbufInit( &Folder );
        VbufInit( &Cmd );

        /*
         * Add the individual PM files to the Group box.
         */
        StatusLines( STAT_CREATEPROGRAMFOLDER, "" );
        num = SimGetPMsNum();
        StatusAmount( 0, num );
        for( i = 0; ok && ( i < num ); i++ ) {
            StatusAmount( i, num );
            if( !SimCheckPMCondition( i ) ) {
                continue;
            }
            /*
             * Replace '\n' in Description with LineFeed character
             */
            SimGetPMDesc( i, &PMProgDesc );
            for( p = VbufString( &PMProgDesc ); *p != '\0'; ++p ) {
                if( p[0] == '\\' && p[1] == 'n' ) {
                    len = p - VbufString( &PMProgDesc );
                    VbufSetStr( &tmp, p + 2 );
                    VbufSetLen( &PMProgDesc, len );
                    VbufConcChr( &PMProgDesc, '\n' );
                    VbufConcVbuf( &PMProgDesc, &tmp );
                    p = VbufString( &PMProgDesc ) + len + 1;
                }
            }

            if( SimPMIsGroup( i ) ) {
                /*
                 * Process a group (ie. folder)
                 */
                SimGetPMParms( i, &GroupFileName );
                if( VbufLen( &GroupFileName ) == 0 ) {
                    break;
                }
                if( VbufLen( &PMProgDesc ) > 0 ) {
                    VbufPrepChr( &GroupFileName, '<' );
                    VbufConcChr( &GroupFileName, '>' );
                } else {
                    VbufSetStr( &GroupFileName, "<WSETUP_FOL>" );
                }
                ok = ( create_group( &PMProgDesc, &GroupFileName ) != NULLHANDLE );
            } else {
                /*
                 * Process a regular object
                 */
                nDirIndex = SimGetPMProgInfo( i, &PMProgName );
                if( nDirIndex == -1 ) {
                    VbufRewind( &WorkingDir );
                    ReplaceVars1( &PMProgName );
                } else {
                    SimGetDir( nDirIndex, &WorkingDir );
                }
                /*
                 * Get parameters
                 */
                SimGetPMParms( i, &PMParams );
                ReplaceVars1( &PMParams );
                if( VbufString( &PMParams )[0] == '+' ) {
                    p = strchr( VbufString( &PMParams ) + 1, '+' );
                    if( p == NULL ) {
                        /*
                         * Format is: "+folder_name"
                         */
                        VbufSetVbufPos( &Folder, &PMParams, 1 );
                        VbufRewind( &PMParams );
                    } else {
                        /*
                         * Format is: "+folder_name+parameters"
                         */
                        VbufSetBuffer( &Folder, VbufString( &PMParams ) + 1, p - VbufString( &PMParams ) - 1 );
                        VbufSetStr( &PMParams, p + 1);
                    }
                } else {
                    /*
                     * Format is: "" use default folder
                     */
                    VbufSetVbuf( &Folder, &GroupFileName );
                }
                /*
                 * Append the subdir where the icon file is and the icon file's name.
                 */
                nDirIndex = SimGetPMIconInfo( i, &iconfile, &iconindex );
                if( iconindex == -1 ) {
                    iconindex = 0;
                }
                if( nDirIndex != -1 ) {
                    SimGetDir( nDirIndex, &tmp );
                    VbufPrepVbuf( &iconfile, &tmp );
                }
                if( SimPMIsShadow( i ) ) {
                    VbufSetStr( &Cmd, "SHADOWID=" );
                    VbufConcVbuf( &Cmd, &WorkingDir );
                    VbufConcVbuf( &Cmd, &PMProgName );
                    ok = ( WinCreateObject( "WPShadow", VbufString( &PMProgDesc ), VbufString( &Cmd ), VbufString( &Folder ), CO_REPLACEIFEXISTS ) != NULLHANDLE );
                } else {
                    /*
                     * Add the new file to the already created PM Group.
                     */
                    VbufSetStr( &Cmd, "EXENAME=" );
                    VbufConcVbuf( &Cmd, &WorkingDir );
                    VbufConcVbuf( &Cmd, &PMProgName );
                    VbufConcStr( &Cmd, ";PARAMETERS=" );
                    VbufConcVbuf( &Cmd, &PMParams );
                    VbufConcStr( &Cmd, ";STARTUPDIR=" );
                    VbufConcVbuf( &Cmd, &WorkingDir );
                    ok = ( WinCreateObject( "WPProgram", VbufString( &PMProgDesc ), VbufString( &Cmd ), VbufString( &Folder ), CO_REPLACEIFEXISTS ) != NULLHANDLE );
                }
            }
        }
        StatusAmount( num, num );

        VbufFree( &Cmd );
        VbufFree( &Folder );
        VbufFree( &WorkingDir );
        VbufFree( &iconfile );
        VbufFree( &PMParams );
        VbufFree( &PMProgDesc );
        VbufFree( &PMProgName );
        VbufFree( &tmp );
        VbufFree( &GroupFileName );
        VbufFree( &group );
    }
    return( ok );
}


static bool SetEAttr( const char *filename, const char *name, const char *val )
/*****************************************************************************/
{
    FEA2LIST            *fet;
    GEA2LIST            *get;
    EAOP2               eabuf;
    int                 ret;
    int                 len;
    char                *value;
    int                 plen;
    int                 nlen;

    plen = strlen( val );
    nlen = strlen( name );
    len = sizeof( FEA2LIST ) + nlen + plen + 4;
    fet = GUIMemAlloc( len );
    fet->cbList = len;
    fet->list[0].oNextEntryOffset = sizeof( FEA2 ) + nlen + plen + 4;
    fet->list[0].fEA = 0;
    fet->list[0].cbName = nlen;
    fet->list[0].cbValue = plen + 4;
    strcpy( fet->list[0].szName, name );
    value = &fet->list[0].szName[nlen + 1];
    *((USHORT *)value)= EAT_ASCII;
    value += 2;
    *((USHORT *)value)= plen;
    value += 2;
    memcpy( value, val, plen );

    len = sizeof( GEA2LIST ) + nlen ;
    get = GUIMemAlloc( len );
    get->cbList = len;
    get->list[0].oNextEntryOffset = sizeof( GEA2 ) + nlen;
    get->list[0].cbName = nlen;
    strcpy( get->list[0].szName, name );

    eabuf.fpGEA2List = get;
    eabuf.fpFEA2List = fet;
    ret = DosSetPathInfo( filename, FIL_QUERYEASIZE, &eabuf, sizeof( eabuf ), 0 );
    GUIMemFree( fet );
    GUIMemFree( get );
    return( ret == NO_ERROR );
}


void LabelDirs( void )
/********************
 * Process [Label] section - use extended attributes to add
 *                            long label to directories
 */
{
    int         i;
    int         num;
    VBUF        dir_name;
    VBUF        label;

    VbufInit( &dir_name );
    VbufInit( &label );

    num = SimNumLabels();
    for( i = 0; i < num; ++i ) {
        SimGetLabelDir( i, &dir_name );
        ReplaceVars1( &dir_name );
        SimGetLabelLabel( i, &label );
        SetEAttr( VbufString( &dir_name ), ".LONGNAME", VbufString( &label ) );
    }

    VbufFree( &label );
    VbufFree( &dir_name );
}
