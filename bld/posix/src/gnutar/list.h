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
* Description:  Declare prototypes for list.c
*
****************************************************************************/

#ifndef _LIST_H_D10FF5DF_6352_437B_A86F_F52CDBDC34CA
#define _LIST_H_D10FF5DF_6352_437B_A86F_F52CDBDC34CA

extern void pr_mkdir( char *pathname, int length, int mode );
extern void print_header( char *xname );
extern void read_and( void (*do_something)( char *dummy ) );

extern void list_archive( char *xname );
extern void decode_header( union record *header, struct stat *st, 
                     int *stdp, int wantug );
extern void skip_file( long   size );
extern long from_oct(	int digs, char *where );

extern struct stat hstat[1];    /* Stat struct corresponding */
extern union record   *head;    /* Points to current archive header */
extern struct stat    *phstat;  /* to overcome construct ACK C can't handle */
                     
#endif /* _LIST_H_D10FF5DF_6352_437B_A86F_F52CDBDC34CA */
