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


/* This program is used to link against an assembly file containing
   code bursts for inline pragmas.  The information from the assembly
   is output to either ..\h\codei86.gh (16-bit) or ..\h\code386.gh (32-bit).
   This allows us to use an assembler to translate the code bursts into
   the appropriate byte sequences and this program generates statically
   initialized arrays that can be compiled with either the 16-bit or
   32-bit compilers. (It is very hard to link a 16-bit object file into
   a 32-bit hosted compiler OR a 32-bit object file into a 16-bit hosted
   compiler.)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __386__
 #define OUTFILE        "..\\h\\code386.gh"
#else
 #define OUTFILE        "..\\h\\codei86.gh"
#endif

struct bursts {
        char    *defs;
        char    *name;
        char    *burst;
};
extern struct bursts Functions[];


main()
{
    FILE        *fp;
    int         i;
    int         len;
    char        *p;
    struct bursts *cb;

    fp = fopen( OUTFILE, "w" );
    cb = Functions;
    for(;;) {
        p = cb->defs;
        if( p == NULL ) break;
        for(;;) {
            fprintf( fp, "%s\n", p );
            while( *p != '\0' )  ++p;
            ++p;
            if( *p == '\0' ) break;
        }
        fprintf( fp, "static byte_seq %s = {\n    ", cb->name );
        p = cb->burst;
        len = (*p) + 1;
        i = 0;
        for(;;) {
            fprintf( fp, "0x%2.2X", *p++ );
            --len;
            if( len == 0 ) break;
            fprintf( fp, "," );
            i++;
            if( i == 10 ) {
                fprintf( fp, "\n    " );
                i = 0;
            }
        }
        fprintf( fp, "\n};\n\n" );
        cb++;
    }
    fclose( fp );
    return 0;
}
