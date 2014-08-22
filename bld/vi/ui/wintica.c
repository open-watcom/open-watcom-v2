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


#include "vi.h"
#include "win.h"

/*
 * WindowTile - tile windows given maximum in each direction
 */
vi_rc WindowTile( int maxx, int maxy )
{
    int         cnt = 0, max = maxx * maxy, xdiv, ydiv, tc = 0, i;
    int         xstart = editw_info.x1;
    int         xend = editw_info.x2;
    int         ystart = editw_info.y1;
    int         yend = editw_info.y2;
    int         ystep, xstep, x, y, xextra, yextra, xx, yy, sxextra;
    info        *cinfo, *cwinfo;
    vi_rc       rc;

    /*
     * "untile" cmd
     */
    if( CurrentInfo == NULL ) {
        return( ERR_NO_ERR );
    }
    SaveCurrentInfo();
    cwinfo = CurrentInfo;
    if( maxx == 1 && maxy == 1 ) {
        for( cinfo = InfoHead; cinfo != NULL; cinfo = cinfo->next ) {
            BringUpFile( cinfo, false );
            WindowAuxUpdate( CurrentWindow, WIND_INFO_TEXT_COLOR,
                             editw_info.text.foreground );
            WindowAuxUpdate( CurrentWindow, WIND_INFO_BACKGROUND_COLOR,
                             editw_info.text.background );
            WindowAuxUpdate( CurrentWindow, WIND_INFO_TEXT_FONT,
                             editw_info.text.font );
            WindowAuxUpdate( CurrentWindow, WIND_INFO_BORDER_COLOR2,
                             editw_info.border_color2 );
            CurrentWindowResize( editw_info.x1, editw_info.y1, editw_info.x2,
                                 editw_info.y2 );
        }
        BringUpFile( cwinfo, false );
        return( ERR_NO_ERR );
    }

    /*
     * count the number of files
     */
    cnt = GimmeFileCount();
    if( cnt > max ) {
        cnt = max;
    }
    if( cnt > maxx ) {
        xdiv = maxx;
    } else {
        xdiv = cnt;
    }
    ydiv = (cnt - 1) / maxx + 1;

    /*
     * figure out positions
     */
    ystep = (yend - ystart + 1) / ydiv;
    yextra = (yend - ystart + 1) % ydiv;
    xstep = (xend - xstart + 1) / xdiv;
    sxextra = xextra = (xend - xstart + 1) % xdiv;

    /*
     * save current file
     */
    SaveCurrentInfo();
    cwinfo = cinfo = CurrentInfo;

    /*
     * do retiling
     */
    for( y = 0; y < ydiv; y++ ) {
        /*
         * y-direction round off allowance
         */
        if( yextra ) {
            yextra--;
            yy = 1;
        } else {
            yy = 0;
        }

        for( x = 0; x < xdiv; x++ ) {

            /*
             * x-direction round off allowance
             */
            if( xextra ) {
                xextra--;
                xx = 1;
            } else {
                xx = 0;
            }

            /*
             * resize the window and display it
             */
            BringUpFile( cinfo, false );
            if( EditVars.TileColors != NULL ) {
                for( i = 0; i < EditVars.MaxTileColors; i++, tc++ ) {
                    if( tc > EditVars.MaxTileColors )
                        tc = 0;
                    if( EditVars.TileColors[tc].foreground != -1 && EditVars.TileColors[tc].background != -1 ) {
                        WindowAuxUpdate( CurrentWindow, WIND_INFO_TEXT_COLOR, EditVars.TileColors[tc].foreground );
                        WindowAuxUpdate( CurrentWindow, WIND_INFO_BACKGROUND_COLOR, EditVars.TileColors[tc].background );
                        /* tile fonts? Nah... sounds real stupid... */
                        WindowAuxUpdate( CurrentWindow, WIND_INFO_BORDER_COLOR2, EditVars.TileColors[tc].background );
                        tc++;
                        break;
                    }
                }
            }

            rc = CurrentWindowResize( xstart, ystart, xstart + xx + xstep - 1,
                                     ystart + yy + ystep - 1 );
            if( rc != ERR_NO_ERR ) {
                return( rc );
            }
            SaveInfo( cinfo );

            /*
             * go to next file
             */
            cnt--;
            if( cnt == 0 ) {
                break;
            }
            xstart += xstep + xx;
            cinfo = cinfo->next;
            if( cinfo == NULL ) {
                cinfo = InfoHead;
            }

        }
        xstart = editw_info.x1;
        xextra = sxextra;
        ystart += ystep + yy;

    }
    BringUpFile( cwinfo, false );

    return( ERR_NO_ERR );

} /* WindowTile */

/*
 * WindowCascade - cascade windows
 */
vi_rc WindowCascade( void )
{
    int         cnt, i, j;
    int         xstart = editw_info.x1;
    int         xend = editw_info.x2;
    int         ystart = editw_info.y1;
    int         yend = editw_info.y2;
    info        *cinfo;
//    info        *cwinfo;
    vi_rc       rc;

    /*
     * get number of files to cascade
     */
    cnt = GimmeFileCount();
    j  = xend - xstart + 2;
    if( j < yend - ystart + 2 ) {
        j = yend - ystart + 2;
    }
    if( cnt > j ) {
        cnt = j;
    }
    if( cnt <= 0 ) {
        return( ERR_NO_ERR );
    }

    /*
     * init for cascade
     */
    xend -= cnt - 1;
    yend -= cnt - 1;
    SaveCurrentInfo();
//    cwinfo = cinfo = CurrentInfo;
    cinfo = CurrentInfo;

    /*
     * resize all the files
     */
    for( i = 0; i < cnt; i++ ) {

        BringUpFile( cinfo, false );
        rc = CurrentWindowResize( xstart, ystart, xend, yend );
        if( rc != ERR_NO_ERR ) {
            return( rc );
        }
        SaveInfo( cinfo );
        xstart++;
        xend++;
        yend++;
        ystart++;
        cinfo = cinfo->next;
        if( cinfo == NULL ) {
            cinfo = InfoHead;
        }

    }
    return( ERR_NO_ERR );

} /* WindowCascade */
