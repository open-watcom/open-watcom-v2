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


/* got this off net.sources */
#include <stdio.h>

/*
 * get option letter from argument vector
 */
int             opterr = 1,             /* useless, never set or used */
                optind = 1,             /* index into parent argv vector */
                optopt;                 /* character checked for validity */
char           *optarg;                 /* argument associated with option */

#define BADCH   (int)'?'
#define EMSG    ""
#define tell(s) fputs(*nargv,stderr);fputs(s,stderr); \
                fputc(optopt,stderr);fputc('\n',stderr);return(BADCH);

getopt(nargc, nargv, ostr)
int             nargc;
char          **nargv, *ostr;
{
        static char    *place = EMSG;           /* option letter processing */
        register char  *oli;            /* option letter list index */
        char           *index();

        if (!*place)
        {                                                       /* update scanning pointer */
                if (optind >= nargc || *(place = nargv[optind]) != '-' || !*++place)
                        return (EOF);
                if (*place == '-')
                {                                               /* found "--" */
                        ++optind;
                        return (EOF);
                }
        }                                                       /* option letter okay? */
        if ((optopt = (int) *place++) == (int) ':' || !(oli = index(ostr, optopt)))
        {
                if (!*place)
                        ++optind;
                tell(": illegal option -- ");
        }
        if (*++oli != ':')
        {                                                       /* don't need argument */
                optarg = NULL;
                if (!*place)
                        ++optind;
        }
        else
        {                                                       /* need an argument */
                if (*place)
                        optarg = place;         /* no white space */
                else
                if (nargc <= ++optind)
                {                                               /* no arg */
                        place = EMSG;
                        tell(": option requires an argument -- ");
                }
                else
                        optarg = nargv[optind];         /* white space */
                place = EMSG;
                ++optind;
        }
        return (optopt);                        /* dump back option letter */
}
