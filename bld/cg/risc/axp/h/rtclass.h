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


/*        NB entries up until OK must match typclass.wif */

typedef enum {
/* misc runtime routines */
        BEG_RTNS,
        RT_DIV_32S = BEG_RTNS,
        RT_DIV_32U,
        RT_DIV_64S,
        RT_DIV_64U,
        RT_MOD_32S,
        RT_MOD_32U,
        RT_MOD_64S,
        RT_MOD_64U,
        RT_MEMCPY,
        RT_STK_CRAWL,
        RT_STK_CRAWL_SIZE,
        RT_STK_STOMP,
        RT_STK_COPY,
        RT_EXCEPT_RTN,
        RT_POW,
        RT_POW_S,
        RT_POW_I,
        RT_ATAN2,
        RT_ATAN2_S,
        RT_FMOD,
        RT_FMOD_S,
        RT_LOG,
        RT_LOG_S,
        RT_COS,
        RT_COS_S,
        RT_SIN,
        RT_SIN_S,
        RT_TAN,
        RT_TAN_S,
        RT_SQRT,
        RT_SQRT_S,
        RT_FABS,
        RT_FABS_S,
        RT_ACOS,
        RT_ACOS_S,
        RT_ASIN,
        RT_ASIN_S,
        RT_ATAN,
        RT_ATAN_S,
        RT_COSH,
        RT_COSH_S,
        RT_EXP,
        RT_EXP_S,
        RT_LOG10,
        RT_LOG10_S,
        RT_SINH,
        RT_SINH_S,
        RT_TANH,
        RT_TANH_S,
        RT_TLS_INDEX,
        RT_TLS_ARRAY,
        RT_NOP,
} rt_class;

/*  beginning of runtime calls */
