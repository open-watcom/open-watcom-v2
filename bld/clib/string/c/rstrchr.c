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
* Description:  Implementation of strchr() for RISC architectures.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <string.h>
#include "riscstr.h"


CHAR_TYPE *__F_NAME(strchr,wcschr)( const CHAR_TYPE *s, INTCHAR_TYPE c )
/**********************************************************************/
{
#if USE_INT64
    RISC_DATA_LOCALREF;
#endif
    UINT                *dw = ROUND(s); /* round down to dword */
    UINT                dword, cdword, tmpdword;
    INT                 len = 0;
    int                 offset = OFFSET(s);
#ifdef __WIDECHAR__
    UINT                cShl16;
#else
    INT                 cShl8, cShl16, cShl24;
#if USE_INT64
    INT                 cShl32, cShl40, cShl48, cShl56;
#endif
#endif

#ifdef __WIDECHAR__
    if( offset % 2 )
        return( __simple_wcschr( s, c ) );
#endif

    /*** Initialize locals ***/
    c &= CHR1MASK;
#ifdef __WIDECHAR__
    cShl16 = c << 16;
    cdword = cShl16 | c;
#else
    cShl8 = c << 8;
    cShl16 = c << 16;
    cShl24 = c << 24;
  #if USE_INT64
    cShl32 = (UINT)c << 32;
    cShl40 = (UINT)c << 40;
    cShl48 = (UINT)c << 48;
    cShl56 = (UINT)c << 56;
    cdword = cShl56 | cShl48 | cShl40 | cShl32 | cShl24 | cShl16 | cShl8 | c;
  #else
    cdword = cShl24 | cShl16 | cShl8 | c;
  #endif
#endif

    /*** Scan any bytes up to a 4-byte alignment ***/
    if( offset != 0 ) {
        dword = *dw++;
#ifdef __WIDECHAR__
        tmpdword = CHR2(dword);
        if( tmpdword == cShl16 ) {
            return( (CHAR_TYPE*)s + len );
        } else if( tmpdword == 0 ) {
            return( NULL );
        }
        len++;
#else
        switch( offset ) {
          case 1:
            tmpdword = CHR2(dword);
            if( tmpdword == cShl8 ) {
                return( (CHAR_TYPE*)s + len );
            } else if( tmpdword == 0 ) {
                return( NULL );
            }
            len++;
            /* fall through */
          case 2:
            tmpdword = CHR3(dword);
            if( tmpdword == cShl16 ) {
                return( (CHAR_TYPE*)s + len );
            } else if( tmpdword == 0 ) {
                return( NULL );
            }
            len++;
            /* fall through */
        #if USE_INT64
          case 3:
            tmpdword = CHR4(dword);
            if( tmpdword == cShl24 ) {
                return( (CHAR_TYPE*)s + len );
            } else if( tmpdword == 0 ) {
                return( NULL );
            }
            len++;
            /* fall through */
          case 4:
            tmpdword = CHR5(dword);
            if( tmpdword == cShl32 ) {
                return( (CHAR_TYPE*)s + len );
            } else if( tmpdword == 0 ) {
                return( NULL );
            }
            len++;
            /* fall through */
          case 5:
            tmpdword = CHR6(dword);
            if( tmpdword == cShl40 ) {
                return( (CHAR_TYPE*)s + len );
            } else if( tmpdword == 0 ) {
                return( NULL );
            }
            len++;
            /* fall through */
          case 6:
            tmpdword = CHR7(dword);
            if( tmpdword == cShl48 ) {
                return( (CHAR_TYPE*)s + len );
            } else if( tmpdword == 0 ) {
                return( NULL );
            }
            len++;
            /* fall through */
          default:
            tmpdword = CHR8(dword);
            if( tmpdword == cShl56 ) {
                return( (CHAR_TYPE*)s + len );
            } else if( tmpdword == 0 ) {
                return( NULL );
            }
            len++;
        #else
          default:
            tmpdword = CHR4(dword);
            if( tmpdword == cShl24 ) {
                return( (CHAR_TYPE*)s + len );
            } else if( tmpdword == 0 ) {
                return( NULL );
            }
            len++;
        #endif
        }
#endif
    }

    /*** Scan in aligned 4-byte groups ***/
    for( ;; ) {
        dword = *dw++;
        if( GOT_NIL(dword) )
            break;
        tmpdword = dword ^ cdword;
        if( GOT_NIL(tmpdword) ) {
#ifdef __WIDECHAR__
            if( !CHR1(tmpdword) )  return( (CHAR_TYPE*)s + len );
            /* otherwise */        return( (CHAR_TYPE*)s + len + 1 );
#else
            if( !CHR1(tmpdword) )  return( (char*)s + len );
            if( !CHR2(tmpdword) )  return( (char*)s + len + 1 );
            if( !CHR3(tmpdword) )  return( (char*)s + len + 2 );
            if( !CHR4(tmpdword) )  return( (char*)s + len + 3 );
    #if USE_INT64
            if( !CHR5(tmpdword) )  return( (char*)s + len + 4 );
            if( !CHR6(tmpdword) )  return( (char*)s + len + 5 );
            if( !CHR7(tmpdword) )  return( (char*)s + len + 6 );
            if( !CHR8(tmpdword) )  return( (char*)s + len + 7 );
    #endif
#endif
        }
        len += CHARS_PER_WORD;
    }

    /*** Scan the last byte(s) in the string ***/
    tmpdword = CHR1(dword);
    if( tmpdword == c ) {
        return( (CHAR_TYPE*)s + len );
    } else if( tmpdword == 0 ) {
        return( NULL );
    }

#ifdef __WIDECHAR__
    tmpdword = CHR2(dword);
    if( tmpdword == cShl16 ) {
        return( (CHAR_TYPE*)s + len + 1 );
    } else if( tmpdword == 0 ) {
        return( NULL );
    }
#else
    tmpdword = CHR2(dword);
    if( tmpdword == cShl8 ) {
        return( (CHAR_TYPE*)s + len + 1 );
    } else if( tmpdword == 0 ) {
        return( NULL );
    }

    tmpdword = CHR3(dword);
    if( tmpdword == cShl16 ) {
        return( (CHAR_TYPE*)s + len + 2 );
    } else if( tmpdword == 0 ) {
        return( NULL );
    }

    tmpdword = CHR4(dword);
    if( tmpdword == cShl24 ) {
        return( (CHAR_TYPE*)s + len + 3 );
    } else if( tmpdword == 0 ) {
        return( NULL );
    }

  #if USE_INT64
    tmpdword = CHR5(dword);
    if( tmpdword == cShl32 ) {
        return( (CHAR_TYPE*)s + len + 4 );
    } else if( tmpdword == 0 ) {
        return( NULL );
    }

    tmpdword = CHR6(dword);
    if( tmpdword == cShl40 ) {
        return( (CHAR_TYPE*)s + len + 5 );
    } else if( tmpdword == 0 ) {
        return( NULL );
    }

    tmpdword = CHR7(dword);
    if( tmpdword == cShl48 ) {
        return( (CHAR_TYPE*)s + len + 6 );
    } else if( tmpdword == 0 ) {
        return( NULL );
    }

    tmpdword = CHR8(dword);
    if( tmpdword == cShl48 ) {
        return( (CHAR_TYPE*)s + len + 7 );
    } else if( tmpdword == 0 ) {
        return( NULL );
    }
  #endif
#endif

    return( NULL );
}
