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


//
// rank_pick  - used for enum name and value, also for string table in fnovprt
// rank_pick_ - used for synonym enum name and ignored in fnovprt string table
//
#ifdef __DEFINE_FNOVRANK
#define rank_pick( name, value )        name = value,
#define rank_pick_( name, value )       name = value,
#undef __DEFINE_FNOVRANK
#else
#define rank_pick( name, value )        __STR( name ),
#define rank_pick_( name, value )
#endif
rank_pick(  OV_RANK_NULL,               0x00 )
rank_pick(  OV_RANK_EXACT,              0x01 )
rank_pick(  OV_RANK_SAME,               0x02 )
rank_pick(  OV_RANK_TRIVIAL,            0x03 )
rank_pick(  OV_RANK_PROMOTION,          0x04 )
rank_pick(  OV_RANK_STD_CONV,           0x05 )
rank_pick(  OV_RANK_STD_CONV_DERIV,     0x06 )
rank_pick(  OV_RANK_STD_CONV_VOID,      0x07 )
rank_pick(  OV_RANK_STD_BOOL,           0x08 )
rank_pick(  OV_RANK_UD_CONV,            0x09 )
rank_pick(  OV_RANK_UD_CONV_AMBIG,      0x0a )
rank_pick(  OV_RANK_ELLIPSIS,           0x0b )
rank_pick(  OV_RANK_NO_MATCH,           0x0c )
#undef rank_pick
#undef rank_pick_

/*
                        Description             Category        Rank
                        -----------             --------        ----
OV_RANK_EXACT           No conversion req'd     Identity        Exact Match
OV_RANK_SAME            ????                    Lvalue          Exact Match
 note: not_exact=1                              Transformations?
OV_RANK_TRIVIAL:        Qualification           Qualification   Exact Match
 note: trivial=1          Conversion                  Ajustment
--------------------------------------------------------------------------
OV_RANK_PROMOTION:      Integral Promotions     Promotion       Promotion
  note: promotion=1
--------------------------------------------------------------------------
note: standard++ executed
OV_RANK_STD_CONV        Intergral Conversions   Conversion      Conversion
                        Floating point Conv.    Conversion      Conversion
                        Floating-Integral Conv. Conversion      Conversion
OV_RANK_STD_CONV_DERIV: Base Class Converions   Conversion      Conversion
  note: numderiv set
OV_RANK_STD_CONV_VOID   Pointer Conversions     Conversion      Conversion
OV_RANK_STD_BOOL        Boolean Conversion      Conversion      Conversion


From [over.ics.rank]

OV_RANK_STD_BOOL is worse that other Conversions
OV_RANK_STD_VOID is worse that OV_RANK_STD_CONV_DERIV
*/
