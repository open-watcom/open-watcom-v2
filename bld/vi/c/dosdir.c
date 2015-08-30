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
* Description:  DOSish directory access functions.
*
****************************************************************************/


#include "vi.h"
#include <direct.h>
#include <dos.h>
#include "wio.h"

#include "clibext.h"


/*
 * MyGetFileSize - do just that
 */
vi_rc MyGetFileSize( char *name, long *size )
{
    DIR         *d;

    d = opendir( name );
    if( d == NULL ) {
        return( ERR_FILE_NOT_FOUND );
    }
    *size = d->d_size;
    closedir( d );
    return( ERR_NO_ERR );

} /* MyGetFileSize */

/*
 * IsDirectory - check if a specified path is a directory
 */
bool IsDirectory( char *name )
{
    struct find_t       dta;
    unsigned            rc;

    if( strpbrk( name, "?*" ) != NULL ) {
        return( false );
    }
    rc = access( name, F_OK );
    if( rc != 0 )
        return( false ); /* not valid */

    if( name[1] == ':' && name[2] == '\\' && name[3] == 0 ) {
        /* this is a root dir -- this is OK */
        return( true );
    }

    /* check if it is actually a sub-directory */
    rc = _dos_findfirst( name, _A_NORMAL | _A_RDONLY | _A_HIDDEN |
            _A_SYSTEM | _A_SUBDIR | _A_ARCH, &dta );
    if( rc != 0 ) {
        return( false );
    }
    return( (dta.attrib & _A_SUBDIR) != 0 );

} /* IsDirectory */

/*
 * GetFileInfo - get info from a directory entry
 */
void GetFileInfo( direct_ent *tmp, struct dirent *nd, char *path )
{
    path = path;

    tmp->attr = nd->d_attr;
    tmp->date = *((date_struct *) &nd->d_date);
    tmp->time = *((time_struct *) &nd->d_time);
    tmp->fsize = nd->d_size;

} /* GetFileInfo */

/*
 * FormatFileEntry - print a file entry
 */
void FormatFileEntry( direct_ent *file, char *res )
{
    char        buff[FILENAME_MAX], tmp[FILENAME_MAX];
    long        size;

    if( file->attr & _A_SUBDIR ) {
        MySprintf(tmp, " " FILE_SEP_STR "%S", file->name);
    } else {
        if( !IsTextFile( file->name ) ) {
            MySprintf(tmp, " *%S", file->name);
        } else {
            MySprintf(tmp, "  %S", file->name);
        }
    }

    /*
     * build attributeibutes
     */
    strcpy( buff, "-------" );
    size = file->fsize;
    if( file->attr & _A_SUBDIR ) {
        buff[0] = 'd';
        size = 0;
    }
    if( file->attr & _A_ARCH ) {
        buff[1] = 'a';
    }
    if( file->attr & _A_HIDDEN ) {
        buff[2] = 'h';
    }
    if( file->attr & _A_SYSTEM ) {
        buff[3] = 's';
    }
    buff[4] = 'r';
    if( !(file->attr & _A_RDONLY) ) {
        buff[5] = 'w';
    }
    if( !IsTextFile( file->name ) ) {
        buff[6] = 'x';
    }

    MySprintf( res, "%s  %s %L  %D/%D/%d  %D:%D",
               tmp,
               buff,
               size,
               (int)file->date.month,
               (int)file->date.day,
               (int)file->date.year + 1980,
               (int)file->time.hour,
               (int)file->time.min );

} /* FormatFileEntry */
