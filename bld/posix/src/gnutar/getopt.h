/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983_2002 Sybase, Inc. All Rights Reserved.
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
*    NON_INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Declare prototypes for getopt.c & getoopt.c
*
****************************************************************************/

#ifndef _GETOPT_H_DD6E8779_03B2_4F3E_AE3D_C31192E79DE1
#define _GETOPT_H_DD6E8779_03B2_4F3E_AE3D_C31192E79DE1

// gnutar getopt changed to ow_getopt as it clashes with
// unistd.h getopt which should be used
extern int  ow_getopt(int nargc, char ** nargv, char *ostr);
extern int  getoldopt( int argc, char **argv, char *optstring );
extern int  optind; /* index into parent argv vector */
extern char *optarg;      

#endif /* _GETOPT_H_DD6E8779_03B2_4F3E_AE3D_C31192E79DE1 */

