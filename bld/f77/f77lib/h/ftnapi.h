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
* Description:  increase number of file handles
*
****************************************************************************/


extern intstar4     __fortran GROWHANDLES( intstar4 *handles );
extern intstar4     __fortran SEEKUNIT( intstar4 *unit, intstar4 *offset, intstar4 *origin );
extern intstar2     __fortran SYSHANDLE( intstar4 *unit );
extern intstar4     __fortran SETSYSHANDLE( intstar4 *unit, intstar2 *handle );
extern single       __fortran URAND( intstar4 *xn );
extern intstar4     __fortran IARGC( void );
extern intstar4     __fortran IGETARG( intstar4 *arg, string *dst );
extern intstar4     __fortran DWFSETAPPTITLE( string PGM *title );
extern intstar4     __fortran DWFSETABOUTDLG( string PGM *title, string PGM *text);
extern intstar4     __fortran DWFSETCONTITLE( intstar4 *unit, string PGM *title );
extern intstar4     __fortran DWFDELETEONCLOSE( intstar4 *unit );
extern intstar4     __fortran DWFYIELD( void );
extern intstar4     __fortran DWFSHUTDOWN( void );
extern void         __fortran FEXIT( intstar4 *rc );
extern intstar4     __fortran FGETCMD( string PGM *args );
extern intstar4     __fortran FGETENV( string PGM *env_var, string PGM *value );
extern intstar4     __fortran FILESIZE( intstar4 *unit );
extern void         __fortran FINTR( intstar4 *int_no, intstar4 *regs );
extern intstar4     __fortran FLUSHUNIT( intstar4 *unit );
extern intstar4     __fortran FNEXTRECL( intstar4 *unit );
extern intstar4     __fortran FSPAWN( string PGM *cmd, string PGM *args );
extern intstar4     __fortran FSYSTEM( string PGM *command );
extern int          __fortran BEGINTHREAD( void (*rtn)(void *), unsigned long *stk_size );
extern void         __fortran ENDTHREAD( void );
extern unsigned     __fortran THREADID( void );
extern void         __fortran FTRACEBACK( void );
extern void         __fortran GETDAT( intstar2 *year, intstar2 *month, intstar2 *day );
extern void         __fortran GETTIM( intstar2 *hrs, intstar2 *mins, intstar2 *secs, intstar2 *tics );
