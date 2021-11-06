/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


/*@H************************ < COMPRESS utility> ****************************
*                                                                           *
*   compress : compusi.uni <Unix/Xenix support functions>                   *
*                                                                           *
*   port by  : Donald J. Gloistein                                          *
*                                                                           *
*   Source, Documentation, Object Code:                                     *
*   released to Public Domain. This code is ported from compress v4.0       *
*   release joe.                                                            *
*                                                                           *
*---------------------------  Module Description  --------------------------*
*   Unix system dependent routines. These are specific to either the        *
*   unix file structure or some unix only functions.                        *
*                                                                           *
*   Separated out for ease of writing the main module.                      *
*                                                                           *
*--------------------------- Implementation Notes --------------------------*
*                                                                           *
*   compiled with : compress.h compress.fns                                 *
*   linked with   : compress.o compapi.o                                    *
*                                                                           *
*   To use, copy or rename this file to compusi.c and recompile.            *
*   Set the defines in compress.h to reflect the status of your compiler's  *
*   runtime library, allocation type for malloc()'s, and memory models if   *
*   applicable, and your library function call for malloc() and free().     *
*                                                                           *
*   problems: header has some hardcoded defines you may need to change      *
*             for your compiler. Please read the header thoroughly.         *
*                                                                           *
*---------------------------      Author(s)        -------------------------*
*     Initials ---- Name ---------------------------------                  *
*      DjG          Donald J. Gloistein                                     *
*                   Plus many others, see rev.hst file for full list        *
*      LvR          Lyle V. Rains, thanks for the improved implementation   *
*                   of the compression and decompression routines.          *
*************************************************************************@H*/

#include <stdio.h>
#include <stdlib.h>
#include "bool.h"
#include "wio.h"

#include "compress.h" /* contains the rest of the include file declarations */

#include "clibext.h"

/* For those who don't have it in libc.a */

#ifdef NO_STRRCHR
char  *strrchr(char *s,int c)
{
    int count;

    while (*s){
        s++;
        count++;
    }
    s--;
    while (count--)
        if (*s == (char)c)
            return(s);
        else
            s--;
    return(NULL);
}
#endif

char *get_program_name(char *ptr)
{
    char *cp;
    if ((cp = strrchr(ptr, '/')) != NULL)
        cp++;
    else
        cp = ptr;

    if(strcmp(cp,"UNCOMPRE.EXP") == 0) {
        do_decomp = true;
    }
    else
    if(strcmp(cp, "zcat") == 0) {
        keep = true;
        zcat_flg = true;
        do_decomp = true;
    }
    return (cp);
}


char *name_index(char *ptr)
{
    char *p;

    p = strrchr(ptr,'/');
    return ((p)? ++p: ptr);
}

bool is_z_name(char *ptr)   /* checks if it is already a z name */
{
//dsmk  return (!(strcmp(ptr + strlen(ptr) -2,".Z")));
    return( strchr(ptr,'.') != NULL );
}

bool make_z_name(char *ptr)
{
#ifndef BSD4_2
    if (strlen(name_index(ptr)) > 12 ) {
        fprintf(stderr,"%s: filename too long to add .Z\n",name_index(ptr));
        return( false );
    }
#endif
    strcat(ptr,".Z");
    return( true );
}
void unmake_z_name(char *ptr)
{
    size_t len = strlen( ptr ) - 2;

    ptr[len] = '\0';
}

#ifndef NOSIGNAL
SIGTYPE onintr( int signum )
{
    /* unused parameters */ (void)signum;

    if (!zcat_flg && !keep_error){
        fclose(stdout);
        remove( ofname );
    }
    exit ( ERROR );
}

SIGTYPE oops( int signum )    /* wild pointer -- assume bad input */
{
    /* unused parameters */ (void)signum;

    if ( do_decomp )
        fprintf ( stderr, "%s: corrupt input: %s\n",prog_name,ifname);
    if (!zcat_flg && !keep_error){
        fclose(stdout);
        remove( ofname );
    }
    exit ( ERROR );
}
#endif

void copystat( void )
{
    struct stat statbuf;
    mode_t mode;
    struct utimbuf timep;

    fclose(stdout);
    if (stat(ifname, &statbuf)) {       /* Get stat on input file */
        perror(ifname);
        return;
    }
    if ((statbuf.st_mode & S_IFMT/*0170000*/) != S_IFREG/*0100000*/) {
        if(quiet)
            fprintf(stderr, "%s: ", ifname);
        fprintf(stderr, " -- not a regular file: unchanged");
        exit_stat = 1;
    } else if (statbuf.st_nlink > 1) {
        if(quiet)
            fprintf(stderr, "%s: ", ifname);
        fprintf(stderr, " -- has %d other links: unchanged",
            statbuf.st_nlink - 1);
        exit_stat = ERROR;
    } else if (exit_stat == NOSAVING && (!force)) { /* No compression: remove file.Z */
        if(!quiet)
            fprintf(stderr, " -- no savings -- file unchanged");
    } else if (exit_stat == NOMEM){
        if (!quiet)
            fprintf(stderr, " -- file unchanged");
        if (!do_decomp)
            exit(ERROR);
        else
            return;     /* otherwise will remove outfile */
    } else if (exit_stat == OK) {  /* ***** Successful Compression ***** */
        mode = statbuf.st_mode & 07777;
        if (chmod(ofname, mode))        /* Copy modes */
                perror(ofname);
#if 0 /* AFS */
        chown(ofname,statbuf.st_uid,statbuf.st_gid); /* Copy Ownership */
#endif
        timep.actime = statbuf.st_atime;
        timep.modtime = statbuf.st_mtime;
        utime(ofname,&timep);   /* Update last accessed and modified times */
        if (!keep){
            fclose(stdin);
            if (remove(ifname)) /* Remove input file */
                perror(ifname);
            if(!quiet)
                fprintf(stderr, " -- replaced with %s", ofname);
        }
        else{
            if(!quiet)
                fprintf(stderr, " -- compressed to %s", ofname);
        }
        return;     /* Successful return */
    }

    /* Unsuccessful return -- one of the tests failed */
    fclose(stdout);
    if (remove(ofname))
        perror(ofname);
}
void version( void )
{
#ifdef XENIX
#ifndef NDEBUG
    fprintf(stderr, "%s\nOptions: Xenix %s MAXBITS = %d\n", rcs_ident,
        "DEBUG",MAXBITS);
#else
    fprintf(stderr, "%s\nOptions: Xenix MAXBITS = %d\n", rcs_ident,MAXBITS);
#endif
#else
#ifndef NDEBUG
    fprintf(stderr, "%s\nOptions: Unix %s MAXBITS = %d\n", rcs_ident,
        "DEBUG",MAXBITS);
#else
    fprintf(stderr, "%s\nOptions: Unix MAXBITS = %d\n", rcs_ident,MAXBITS);
#endif
#endif
}

ALLOCTYPE FAR *emalloc(unsigned int x,int y)
{
    ALLOCTYPE FAR *p;
    p = (ALLOCTYPE FAR *)ALLOCATE(x,y);
    return(p);
}

void efree(ALLOCTYPE FAR *ptr)
{
    FREEIT(ptr);
}

