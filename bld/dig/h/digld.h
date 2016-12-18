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
* Description:  DIG loader file I/O interface functions (used for DIP/MAD/TRAP)
*
****************************************************************************/


#if defined( _WIN64 )
#define DIGLD_FID2PH(dfh)   (((int)(unsigned __int64)(dfh)) - 1)
#define DIGLD_PH2FID(ph)    ((dig_ldhandle)(unsigned __int64)(ph + 1))
#else
#define DIGLD_FID2PH(dfh)   (((int)(unsigned long)(dfh)) - 1)
#define DIGLD_PH2FID(ph)    ((dig_ldhandle)(unsigned long)(ph + 1))
#endif
#define DIGLD_FID2FH(dfh)   ((FILE *)(dfh))
#define DIGLD_FH2FID(fh)    ((dig_ldhandle)(fh))

#define DIGLoader(n)        DIGLoader ## n

typedef void                *dig_ldhandle;
#define DIGLD_NIL_HANDLE    ((dig_ldhandle)NULL)

extern dig_ldhandle     DIGLoader( Open )( const char *name, size_t name_len, const char *ext, char *buff, size_t buff_size );
extern int              DIGLoader( Close )( dig_ldhandle ldfh );
extern int              DIGLoader( Read )( dig_ldhandle ldfh, void *buff, unsigned len );
extern int              DIGLoader( Seek )( dig_ldhandle ldfh, unsigned long offs, dig_seek where );
