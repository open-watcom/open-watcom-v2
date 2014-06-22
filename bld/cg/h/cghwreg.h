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


#include "cgstd.h"

#ifdef HW_NEED_160
    #define HW_160( x ) x
    #define HW_128( x ) x
    #define HW_96( x )  x
    #define HW_64( x )  x
#else
    #ifdef HW_NEED_64
        #define HW_64( x ) x
    #else
        #define HW_64( x )
    #endif
#endif
#define HW_COMMA ,

#define __0(x) (hw_reg_part)x##_0
#define __1(x) (hw_reg_part)x##_1
#define __2(x) (hw_reg_part)x##_2
#define __3(x) (hw_reg_part)x##_3
#define __4(x) (hw_reg_part)x##_4
#define __5(x) (hw_reg_part)x##_5
#define __6(x) (hw_reg_part)x##_6
#define __7(x) (hw_reg_part)x##_7


#if INT_MAX == SHRT_MAX

    #error Forget it - no more 16-bit hosts - buy a real machine.

#else

    #define HW_0( x ) x
    #ifdef HW_NEED_160
        #define HW_1( x )       x
        #define HW_2( x )       x
        #define HW_3( x )       x
        #define HW_4( x )       x
        #define HW_5( x )
        #define HW_6( x )
        #define HW_7( x )
    #else
        #ifdef HW_NEED_64   /* don't use HW_64 here! */
            #define HW_1( x ) x
        #else
            #define HW_1( x )
        #endif
        #define HW_2( x )
        #define HW_3( x )
        #define HW_4( x )
        #define HW_5( x )
        #define HW_6( x )
        #define HW_7( x )
    #endif

    #ifdef HW_NEED_160
        #define HW_DEFINE_SIMPLE( r, p_0, p_1, p_2, p_3, p_4 )  \
            enum {                      \
                                 r##_0 = (int)p_0  \
                HW_64(  HW_COMMA r##_1 = (int)p_1 ) \
                HW_96(  HW_COMMA r##_2 = (int)p_2 ) \
                HW_128( HW_COMMA r##_3 = (int)p_3 ) \
                HW_160( HW_COMMA r##_4 = (int)p_4 ) \
            }
    #else
        #define HW_DEFINE_SIMPLE( r, p_0, p_1 )  \
            enum {                      \
                                 r##_0 = (int)p_0  \
                HW_64(  HW_COMMA r##_1 = (int)p_1 )\
            }
    #endif

#endif

typedef unsigned hw_reg_part;
typedef hw_reg_part HWT;

typedef struct hw_reg_set {
    HW_0( HWT _0; )
    HW_1( HWT _1; )
    HW_2( HWT _2; )
    HW_3( HWT _3; )
    HW_4( HWT _4; )
    HW_5( HWT _5; )
    HW_6( HWT _6; )
    HW_7( HWT _7; )
} hw_reg_set;

#define HW_Op1(a        ,x) HW_0(          x (__0(a))                           )\
                            HW_1( HW_COMMA x (__1(a))                           )\
                            HW_2( HW_COMMA x (__2(a))                           )\
                            HW_3( HW_COMMA x (__3(a))                           )\
                            HW_4( HW_COMMA x (__4(a))                           )\
                            HW_5( HW_COMMA x (__5(a))                           )\
                            HW_6( HW_COMMA x (__6(a))                           )\
                            HW_7( HW_COMMA x (__7(a))                           )

#define HW_Op2(a,b      ,x) HW_0(          x (__0(a)+__0(b))                    )\
                            HW_1( HW_COMMA x (__1(a)+__1(b))                    )\
                            HW_2( HW_COMMA x (__2(a)+__2(b))                    )\
                            HW_3( HW_COMMA x (__3(a)+__3(b))                    )\
                            HW_4( HW_COMMA x (__4(a)+__4(b))                    )\
                            HW_5( HW_COMMA x (__5(a)+__5(b))                    )\
                            HW_6( HW_COMMA x (__6(a)+__6(b))                    )\
                            HW_7( HW_COMMA x (__7(a)+__7(b))                    )

#define HW_Op3(a,b,c    ,x) HW_0(          x (__0(a)+__0(b)+__0(c))             )\
                            HW_1( HW_COMMA x (__1(a)+__1(b)+__1(c))             )\
                            HW_2( HW_COMMA x (__2(a)+__2(b)+__2(c))             )\
                            HW_3( HW_COMMA x (__3(a)+__3(b)+__3(c))             )\
                            HW_4( HW_COMMA x (__4(a)+__4(b)+__4(c))             )\
                            HW_5( HW_COMMA x (__5(a)+__5(b)+__5(c))             )\
                            HW_6( HW_COMMA x (__6(a)+__6(b)+__6(c))             )\
                            HW_7( HW_COMMA x (__7(a)+__7(b)+__7(c))             )

#define HW_Op4(a,b,c,d  ,x) HW_0(          x (__0(a)+__0(b)+__0(c)+__0(d))      )\
                            HW_1( HW_COMMA x (__1(a)+__1(b)+__1(c)+__1(d))      )\
                            HW_2( HW_COMMA x (__2(a)+__2(b)+__2(c)+__2(d))      )\
                            HW_3( HW_COMMA x (__3(a)+__3(b)+__3(c)+__3(d))      )\
                            HW_4( HW_COMMA x (__4(a)+__4(b)+__4(c)+__4(d))      )\
                            HW_5( HW_COMMA x (__5(a)+__5(b)+__5(c)+__5(d))      )\
                            HW_6( HW_COMMA x (__6(a)+__6(b)+__6(c)+__6(d))      )\
                            HW_7( HW_COMMA x (__7(a)+__7(b)+__7(c)+__7(d))      )

#define HW_Op5(a,b,c,d,e,x) HW_0(          x (__0(a)+__0(b)+__0(c)+__0(d)+__0(e)))\
                            HW_1( HW_COMMA x (__1(a)+__1(b)+__1(c)+__1(d)+__1(e)))\
                            HW_2( HW_COMMA x (__2(a)+__2(b)+__2(c)+__2(d)+__2(e)))\
                            HW_3( HW_COMMA x (__3(a)+__3(b)+__3(c)+__3(d)+__3(e)))\
                            HW_4( HW_COMMA x (__4(a)+__4(b)+__4(c)+__4(d)+__4(e)))\
                            HW_5( HW_COMMA x (__5(a)+__5(b)+__5(c)+__5(d)+__5(e)))\
                            HW_6( HW_COMMA x (__6(a)+__6(b)+__6(c)+__6(d)+__6(e)))\
                            HW_7( HW_COMMA x (__7(a)+__7(b)+__7(c)+__7(d)+__7(e)))


#define    HW_D_1(a        )    {HW_Op1(a        , )}
#define HW_NotD_1(a        )    {HW_Op1(a        ,~)}
#define    HW_D_2(a,b      )    {HW_Op2(a,b      , )}
#define HW_NotD_2(a,b      )    {HW_Op2(a,b      ,~)}
#define    HW_D_3(a,b,c    )    {HW_Op3(a,b,c    , )}
#define HW_NotD_3(a,b,c    )    {HW_Op3(a,b,c    ,~)}
#define    HW_D_4(a,b,c,d  )    {HW_Op4(a,b,c,d  , )}
#define HW_NotD_4(a,b,c,d  )    {HW_Op4(a,b,c,d  ,~)}
#define    HW_D_5(a,b,c,d,e)    {HW_Op5(a,b,c,d,e, )}
#define HW_NotD_5(a,b,c,d,e)    {HW_Op5(a,b,c,d,e,~)}

#define    HW_D( x )       HW_D_1( x )
#define HW_NotD( x )    HW_NotD_1( x )

#define HW_CEqual( a, c )      (\
                               HW_0(    ( (a)._0 == __0(c) ) )\
                               HW_1( && ( (a)._1 == __1(c) ) )\
                               HW_2( && ( (a)._2 == __2(c) ) )\
                               HW_3( && ( (a)._3 == __3(c) ) )\
                               HW_4( && ( (a)._4 == __4(c) ) )\
                               HW_5( && ( (a)._5 == __5(c) ) )\
                               HW_6( && ( (a)._6 == __6(c) ) )\
                               HW_7( && ( (a)._7 == __7(c) ) )\
                               )

#define HW_COvlap( a, c )      (\
                               HW_0(    ( (a)._0 & __0(c) ) != 0 )\
                               HW_1( || ( (a)._1 & __1(c) ) != 0 )\
                               HW_2( || ( (a)._2 & __2(c) ) != 0 )\
                               HW_3( || ( (a)._3 & __3(c) ) != 0 )\
                               HW_4( || ( (a)._4 & __4(c) ) != 0 )\
                               HW_5( || ( (a)._5 & __5(c) ) != 0 )\
                               HW_6( || ( (a)._6 & __6(c) ) != 0 )\
                               HW_7( || ( (a)._7 & __7(c) ) != 0 )\
                               )

#define HW_CSubset( a, c )     (\
                               HW_0(    ( ( (a)._0&__0(c) ) == __0(c) ) )\
                               HW_1( && ( ( (a)._1&__1(c) ) == __1(c) ) )\
                               HW_2( && ( ( (a)._2&__2(c) ) == __2(c) ) )\
                               HW_3( && ( ( (a)._3&__3(c) ) == __3(c) ) )\
                               HW_4( && ( ( (a)._4&__4(c) ) == __4(c) ) )\
                               HW_5( && ( ( (a)._5&__5(c) ) == __5(c) ) )\
                               HW_6( && ( ( (a)._6&__6(c) ) == __6(c) ) )\
                               HW_7( && ( ( (a)._7&__7(c) ) == __7(c) ) )\
                               )

#define HW_CAsgn( dst, src )   {\
                               HW_0( (dst)._0 = __0(src); )\
                               HW_1( (dst)._1 = __1(src); )\
                               HW_2( (dst)._2 = __2(src); )\
                               HW_3( (dst)._3 = __3(src); )\
                               HW_4( (dst)._4 = __4(src); )\
                               HW_5( (dst)._5 = __5(src); )\
                               HW_6( (dst)._6 = __6(src); )\
                               HW_7( (dst)._7 = __7(src); )\
                               }

#define HW_CTurnOn( a, c )     {\
                               HW_0( (a)._0 |= __0(c); )\
                               HW_1( (a)._1 |= __1(c); )\
                               HW_2( (a)._2 |= __2(c); )\
                               HW_3( (a)._3 |= __3(c); )\
                               HW_4( (a)._4 |= __4(c); )\
                               HW_5( (a)._5 |= __5(c); )\
                               HW_6( (a)._6 |= __6(c); )\
                               HW_7( (a)._7 |= __7(c); )\
                               }

#define HW_CTurnOff( a, c )    {\
                               HW_0( (a)._0 &= ~__0(c); )\
                               HW_1( (a)._1 &= ~__1(c); )\
                               HW_2( (a)._2 &= ~__2(c); )\
                               HW_3( (a)._3 &= ~__3(c); )\
                               HW_4( (a)._4 &= ~__4(c); )\
                               HW_5( (a)._5 &= ~__5(c); )\
                               HW_6( (a)._6 &= ~__6(c); )\
                               HW_7( (a)._7 &= ~__7(c); )\
                               }

#define HW_COnlyOn( a, c )     {\
                               HW_0( (a)._0 &= __0(c); )\
                               HW_1( (a)._1 &= __1(c); )\
                               HW_2( (a)._2 &= __2(c); )\
                               HW_3( (a)._3 &= __3(c); )\
                               HW_4( (a)._4 &= __4(c); )\
                               HW_5( (a)._5 &= __5(c); )\
                               HW_6( (a)._6 &= __6(c); )\
                               HW_7( (a)._7 &= __7(c); )\
                               }

#define HW_Equal( a, b )       (\
                               HW_0(    ( (a)._0 == (b)._0 ) )\
                               HW_1( && ( (a)._1 == (b)._1 ) )\
                               HW_2( && ( (a)._2 == (b)._2 ) )\
                               HW_3( && ( (a)._3 == (b)._3 ) )\
                               HW_4( && ( (a)._4 == (b)._4 ) )\
                               HW_5( && ( (a)._5 == (b)._5 ) )\
                               HW_6( && ( (a)._6 == (b)._6 ) )\
                               HW_7( && ( (a)._7 == (b)._7 ) )\
                               )

#define HW_Ovlap( a, b )       (\
                               HW_0(    ( (a)._0 & (b)._0 ) != 0 )\
                               HW_1( || ( (a)._1 & (b)._1 ) != 0 )\
                               HW_2( || ( (a)._2 & (b)._2 ) != 0 )\
                               HW_3( || ( (a)._3 & (b)._3 ) != 0 )\
                               HW_4( || ( (a)._4 & (b)._4 ) != 0 )\
                               HW_5( || ( (a)._5 & (b)._5 ) != 0 )\
                               HW_6( || ( (a)._6 & (b)._6 ) != 0 )\
                               HW_7( || ( (a)._7 & (b)._7 ) != 0 )\
                               )

#define HW_Subset( a, b )      (\
                               HW_0(    ( ( (a)._0 & (b)._0 ) == (b)._0 ) )\
                               HW_1( && ( ( (a)._1 & (b)._1 ) == (b)._1 ) )\
                               HW_2( && ( ( (a)._2 & (b)._2 ) == (b)._2 ) )\
                               HW_3( && ( ( (a)._3 & (b)._3 ) == (b)._3 ) )\
                               HW_4( && ( ( (a)._4 & (b)._4 ) == (b)._4 ) )\
                               HW_5( && ( ( (a)._5 & (b)._5 ) == (b)._5 ) )\
                               HW_6( && ( ( (a)._6 & (b)._6 ) == (b)._6 ) )\
                               HW_7( && ( ( (a)._7 & (b)._7 ) == (b)._7 ) )\
                               )

#define HW_Asgn( a, b )        {\
                               HW_0( (a)._0 = (b)._0; )\
                               HW_1( (a)._1 = (b)._1; )\
                               HW_2( (a)._2 = (b)._2; )\
                               HW_3( (a)._3 = (b)._3; )\
                               HW_4( (a)._4 = (b)._4; )\
                               HW_5( (a)._5 = (b)._5; )\
                               HW_6( (a)._6 = (b)._6; )\
                               HW_7( (a)._7 = (b)._7; )\
                               }

#define HW_TurnOn( a, b )      {\
                               HW_0( (a)._0 |= (b)._0; )\
                               HW_1( (a)._1 |= (b)._1; )\
                               HW_2( (a)._2 |= (b)._2; )\
                               HW_3( (a)._3 |= (b)._3; )\
                               HW_4( (a)._4 |= (b)._4; )\
                               HW_5( (a)._5 |= (b)._5; )\
                               HW_6( (a)._6 |= (b)._6; )\
                               HW_7( (a)._7 |= (b)._7; )\
                               }

#define HW_TurnOff( a, b )     {\
                               HW_0( (a)._0 &= ~(b)._0; )\
                               HW_1( (a)._1 &= ~(b)._1; )\
                               HW_2( (a)._2 &= ~(b)._2; )\
                               HW_3( (a)._3 &= ~(b)._3; )\
                               HW_4( (a)._4 &= ~(b)._4; )\
                               HW_5( (a)._5 &= ~(b)._5; )\
                               HW_6( (a)._6 &= ~(b)._6; )\
                               HW_7( (a)._7 &= ~(b)._7; )\
                               }

#define HW_OnlyOn( a, b )      {\
                               HW_0( (a)._0 &= (b)._0; )\
                               HW_1( (a)._1 &= (b)._1; )\
                               HW_2( (a)._2 &= (b)._2; )\
                               HW_3( (a)._3 &= (b)._3; )\
                               HW_4( (a)._4 &= (b)._4; )\
                               HW_5( (a)._5 &= (b)._5; )\
                               HW_6( (a)._6 &= (b)._6; )\
                               HW_7( (a)._7 &= (b)._7; )\
                               }

#ifdef HW_DEFINE_VARS
    #define HW_DEFINE_GLOBAL_CONST( x ) hw_reg_set x = \
                                        {\
                                        HW_0(          __0(x) )\
                                        HW_1( HW_COMMA __1(x) )\
                                        HW_2( HW_COMMA __2(x) )\
                                        HW_3( HW_COMMA __3(x) )\
                                        HW_4( HW_COMMA __4(x) )\
                                        HW_5( HW_COMMA __5(x) )\
                                        HW_6( HW_COMMA __6(x) )\
                                        HW_7( HW_COMMA __7(x) )\
                                        }
#else
    #define HW_DEFINE_GLOBAL_CONST( x ) extern hw_reg_set x
#endif

#define HW_ITER( x ) HW_0(   x( 0 ) )\
                     HW_1( ; x( 1 ) )\
                     HW_2( ; x( 2 ) )\
                     HW_3( ; x( 3 ) )\
                     HW_4( ; x( 4 ) )\
                     HW_5( ; x( 5 ) )\
                     HW_6( ; x( 6 ) )\
                     HW_7( ; x( 7 ) )
