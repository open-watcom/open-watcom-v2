/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Extension to cmdlnprs.gc code
*
****************************************************************************/


/*
 * Add another string to an OPT_STRING linked list.
 */
static void add_string( OPT_STRING **p, const char *str, char quote )
/*******************************************************************/
{
    OPT_STRING      *buf;
    OPT_STRING      *curElem;
    size_t          len;
    bool            add_quote;

    /*** Check for necessary quotes ***/
    len = strlen( str );
    add_quote = false;
    if( quote != '\0' ) {
        if( str[0] != '"' || str[len - 1] != '"'  ) {
            if( str[0] != '\'' || str[len - 1] != '\'' ) {
                len += 2;
                add_quote = true;
            }
        }
    }
    /*** Make a new list item ***/
    buf = AllocMem( sizeof( OPT_STRING ) + len );
    if( add_quote ) {
        buf->data[0] = quote;
        strcpy( &(buf->data[1]), str );
        buf->data[len - 1] = quote;
        buf->data[len] = '\0';
    } else {
        strcpy( buf->data, str );
    }
    buf->next = NULL;
    /*** Put it at the end of the list ***/
    if( *p == NULL ) {
        *p = buf;
    } else {
        curElem = *p;
        while( curElem->next != NULL )
            curElem = curElem->next;
        curElem->next = buf;
    }
}


/*
 * Destroy an OPT_STRING linked list.
 */
#ifndef NO_CLEAN_STRING
void OPT_CLEAN_STRING( OPT_STRING **p )
/*************************************/
{
    OPT_STRING  *s;

    while( (s = *p) != NULL ) {
        *p = s->next;
        FreeMem( s );
    }
}
#endif
