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
#include "rxsupp.h"
#include "clibext.h"

extern void Lead( char c, int num, char *buff );

#ifndef CHARBITS
#define UCHARAT(p)      ((int)*(unsigned char *)(p))
#else
#define UCHARAT(p)      ((int)*(p)&CHARBITS)
#endif

/*
 * RegSub - perform substitutions after a regexp match
 */
bool RegSub( regexp *prog, char *source, char *dest, linenum lineno )
{
    char        *src, *dst, c;
    int         no, len;
    bool        splitit = false;
    bool        upper_flag = false;
    bool        lower_flag = false;
    bool        perm_flag = false;
    char        *tmp;
    char        buff[MAX_STR];
    int         i, j;
    linenum     ll;


    tmp = StaticAlloc();

    src = source;
    dst = dest;
    while( (c = *src++) != '\0' ) {
        if( c == '&' ) {
            no = 0;
        } else if( c == '\\' && '0' <= *src && *src <= '9' ) {
            no = *src++ - '0';
        } else {
            no = -1;
        }

        /*
         * do we have a normal character?
         */
        if( no < 0 ) {
            /*
             * is this an escaped character?
             */
            if( c == '\\' && EditFlags.RegSubMagic ) {
                switch( *src ) {
                case '|':
                    src++;
                    if( !isdigit( *src ) ) {
                        break;
                    }
                    i = 0;
                    while( isdigit( *src ) ) {
                        buff[i++] = *src++;
                    }
                    buff[i] = 0;
                    j = atoi( buff ) - 1;
                    *dst = 0;
                    i = VirtualLineLen( dest );
                    while( i < j ) {
                        *dst++ = ' ';
                        i++;
                    }
                    break;
                case '#':
                    src++;
                    CFindLastLine( &ll );
                    sprintf( buff, "%ld", ll );
                    i = strlen( buff );
                    sprintf( buff, "%ld", lineno );
                    Lead( '0', i, buff );
                    for( j = 0; j < i; j++ ) {
                        *dst++ = buff[j];
                    }
                    break;
                case 't':
                    src++;
                    if( EditFlags.RealTabs ) {
                        *dst++ = '\t';
                    } else {
                        *dst++ = 't';
                    }
                    break;
                case 'n':
                    src++;
                    if( EditFlags.AllowRegSubNewline ) {
                        *dst++ = SPLIT_CHAR;
                        splitit = true;
                    } else {
                        *dst++ = 'n';
                    }
                    break;
                case '\\':
                case '&':
                    c = *src++;
                    *dst++ = c;
                    break;
                case 'u':
                    src++;
                    upper_flag = true;
                    perm_flag = false;
                    break;
                case 'l':
                    src++;
                    lower_flag = true;
                    perm_flag = false;
                    break;
                case 'L':
                    src++;
                    lower_flag = true;
                    perm_flag = true;
                    break;
                case 'U':
                    src++;
                    upper_flag = true;
                    perm_flag = true;
                    break;
                case 'e':
                case 'E':
                    src++;
                    upper_flag = lower_flag = perm_flag = false;
                    break;
                default:
                    *dst++ = '\\';
                    break;
                }
            /*
             * not escaped, so just copy it in
             */
            } else {
                if( upper_flag ) {
                    c = toupper( c );
                    if( !perm_flag ) {
                        upper_flag = false;
                    }
                } else if( lower_flag ) {
                    c = tolower( c );
                    if( !perm_flag ) {
                        lower_flag = false;
                    }
                }
                *dst++ = c;
            }
        /*
         * copy in a sub expression
         */
        } else if( prog->startp[no] != NULL && prog->endp[no] != NULL ) {
            len = prog->endp[no] - prog->startp[no];
            if( upper_flag ) {
                strcpy( tmp, prog->startp[no] );
                strncpy( dst, strupr( tmp ), len );
                if( !perm_flag ) {
                    upper_flag = false;
                }
            } else if( lower_flag ) {
                strcpy( tmp, prog->startp[no] );
                strncpy( dst, strlwr( tmp ), len );
                if( !perm_flag ) {
                    lower_flag = false;
                }
            } else {
                strncpy( dst, prog->startp[no], len );
            }
            dst += len;
        }

    }
    *dst++ = 0;
    StaticFree( tmp );
    return( splitit );

} /* RegSub */
