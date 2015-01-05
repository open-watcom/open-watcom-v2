/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 2015 Open Watcom contributors. 
*    All Rights Reserved.
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
* Description:  Implementation of POSIX getdelim and getline
*
* Author: J. Armstrong
****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

_WCRTLINK ssize_t getdelim(char **s, size_t *n, int delim, FILE *fp)
{    
    long pos;
    int c;
    size_t linelength;
    int i;

	if (!n || !s) {
		errno = EINVAL;
		return -1;
	}

	if (!*s) *n=0;

    /* First, determine line length */
    pos = ftell(fp);
    linelength = 1; /* For the null character */
    do {
        c = getc(fp);
        linelength++;
    } while(c != EOF && c != delim);
    fseek(fp, pos, SEEK_SET);
    
    /* The EOF character should not be captured */
    if(c == EOF)
        linelength--;
    
    /* If our line length is 1, we have nothing to read,
     * and we should return right now
     */
    if(linelength == 1)
        return -1;
    
    printf("size: %d\n", (int)linelength);

    /* Now with a line length, check if we need a reallocation */
    if(linelength > *n) {
        *s = realloc(*s, linelength*sizeof(char));
        if(!*s) {
            errno = ENOMEM;
            return -1;
        }
        *n = linelength;
    }
    
    /* Copy in the characters */
    i = 0; c = ' ';
    while(i < linelength-1 && c != delim) {
        c = getc(fp);
        (*s)[i++] = (char)c;
    }
    (*s)[i] = '\0';

	return linelength-1;
}

_WCRTLINK ssize_t getline(char **s, size_t *n, FILE *fp)
{
    return getdelim(s, n, '\n', fp);
}
