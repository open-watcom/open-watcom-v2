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


#include <wlib.h>

static unsigned long GetARNumeric( char *str, int max, int base )
/***************************************************************/
// get a numeric value from an ar_header
{
    char                save;
    unsigned long       value;

    save = *(str + max);
    *(str + max) = '\0';
    value = strtoul( str, NULL, base );
    *(str + max) = save;
    return value;
}

static unsigned long ARstrlen( char * str )
/*****************************************/
// find the length of a NULL or /\n terminated string.
{
    char *c;

    for( c=str; *c!='\0'; c++ ) {
        if( (c[0]=='/') && (c[1]=='\n') ) {
            break;
        }
    }
    return( c-str );
}

extern char *GetARName( ar_header *header, arch_header *arch )
/************************************************************/
{
    char        buffer[AR_NAME_LEN + 1];
    char *      buf;
    char *      name;
    file_offset len;

    if( header->name[0] == '/' ) {
        len = GetARNumeric( &header->name[1], AR_NAME_LEN - 1, AR_ELEMENT_BASE );
        buf = arch->fnametab + len;
    } else {
        GetARValue( header->name, AR_NAME_LEN, AR_NAME_END_CHAR, buffer );
        buf = buffer;
    }
    len = ARstrlen(buf);
    name = (char *) MemAlloc( len + 1 );
    memcpy( name, buf, len );
    name[ len ] = '\0';
    return( name );
}

extern char *GetFFName( arch_header *arch )
{
    char        *name=NULL;
    file_offset len;

    if( arch->ffnametab && arch->nextffname ) {
        len = strlen( arch->nextffname );
        name = (char *) MemAlloc( len + 1 );
        memcpy( name, arch->nextffname, len + 1 );
        arch->nextffname += len + 1;
        if( arch->nextffname >= arch->lastffname || (arch->nextffname[0]=='\n'
                && arch->nextffname+1 >= arch->lastffname) ) {
            arch->nextffname = NULL;
        }
    }
    return( name );
}

static void GetARValue( char *element, ar_len len, char delimiter, char *buffer )
/***************************************************************************/
// function to copy a value from an ar_header into a buffer so
// that it is null-terminated rather than blank-padded
{
    ar_len      loop = 0;

    while( loop < len && element[loop] != delimiter ) {
        loop++;
    }
    if( loop > 0 ) {
        strncpy( buffer, element, loop );
    }
    buffer[loop] = '\0';
}

void GetARHeaderValues( ar_header *header, arch_header * arch )
{
    arch->date = GetARNumeric( header->date, AR_DATE_LEN, AR_ELEMENT_BASE );
    arch->uid = GetARNumeric( header->uid, AR_UID_LEN, AR_ELEMENT_BASE );
    arch->gid = GetARNumeric( header->gid, AR_GID_LEN, AR_ELEMENT_BASE );
    arch->mode = GetARNumeric( header->mode, AR_MODE_LEN, AR_MODE_BASE );
    arch->size = GetARNumeric( header->size, AR_SIZE_LEN, AR_ELEMENT_BASE );
}

static void PutARPadding( char * element, ar_len current_len, ar_len desired_len )
{
    ar_len      loop;

    for( loop = current_len; loop < desired_len; loop++ ) {
        element[loop] = AR_VALUE_END_CHAR;
    }
}

static void PutARName( char *ar_name, char * arch_name )
{
    ar_len      name_len;


    name_len = strlen( arch_name );
    strncpy( ar_name, arch_name, name_len );
    if( name_len < AR_NAME_LEN ) {
        PutARPadding( ar_name, name_len, AR_NAME_LEN );
    }
}

static void PutARValue( char *element, uint_32 value, uint_8 base, ar_len desired_len )
{
    ar_len      value_len;

    ultoa( value, element, base );
    value_len = strlen( element );
    if( value_len < desired_len ) {
        PutARPadding( element, value_len, desired_len );
    }
}

void CreateARHeader( ar_header *ar, arch_header * arch )
{
    PutARName( ar->name, arch->name );
    PutARValue( ar->date, arch->date, AR_ELEMENT_BASE, AR_DATE_LEN );
    PutARValue( ar->uid, arch->uid, AR_ELEMENT_BASE, AR_UID_LEN );
    PutARValue( ar->gid, arch->gid, AR_ELEMENT_BASE, AR_GID_LEN );
    PutARValue( ar->mode, arch->mode, AR_MODE_BASE, AR_MODE_LEN );
    PutARValue( ar->size, arch->size, AR_ELEMENT_BASE, AR_SIZE_LEN );
    strncpy( ar->header_ident, AR_HEADER_IDENT, AR_HEADER_IDENT_LEN );
}
