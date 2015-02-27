/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  wgml path and separator char definitions, Environment variables
*               for includes, ...
*
****************************************************************************/

#ifndef GTYPE_H_INCLUDED
#define GTYPE_H_INCLUDED

/* default filename extensions */
#define DEF_EXT         ".def"
#define ERR_EXT         ".err"
#define GML_EXT         ".gml"
#define LAY_EXT         ".lay"
#define OPT_EXT         ".opt"
#define COP_EXT         ".cop"
#define PCD_EXT         ".pcd"
#define FON_EXT         ".fon"

//================= Some global defines ========================
#define TAG_NAME_LENGTH 15              // :tag name length
#define SYM_NAME_LENGTH 10              // symbol name length

#define LINENO_MAX  ( _UI32_MAX - 1 )


/***************************************************************************/
/*  Space units Horiz + Vert              to be redesigned      TBD        */
/***************************************************************************/

typedef enum space_units {
    SU_undefined   = 0,                 // don't care = value zero
    SU_chars_lines = 16,                // undimensioned value
    SU_chars       = 1,                 // chars horizontal
    SU_lines       = 1,                 // lines vertical
    SU_cicero,                          // C   Cicero
    SU_cm,                              // cm  centimeter
    SU_dv,                              // dv  device units
    SU_ems,                             // M   Ems
    SU_inch,                            // i   inch
    SU_mm,                              // mm  millimeter
    SU_pica,                            // p   pica

    SU_lay_left    = 100,    // special values used for layout definition
    SU_lay_centre,           // ... :BANREGION indent, hoffset and width attr
    SU_lay_right,
    SU_lay_extend            // additions before SU_lay_left please
} space_units;

#define MAX_SU_CHAR     12            // length of space units in char format

typedef struct {
    char        su_txt[MAX_SU_CHAR];    // space unit as entered in chars
    int32_t     su_whole;               // integer part
    int32_t     su_dec;                 // decimal part (if any)
    int32_t     su_inch;                // value in inch with 4 decimals
    int32_t     su_mm;                  // value in mm with 4 decimals
    bool        su_relative;            // + - sign found
    space_units su_u;                   // unit
} su;

/***************************************************************************/
/*  Symbolic variables related                                             */
/***************************************************************************/

typedef enum {
    min_subscript = -1000000L,          // smallest valid subscript
    max_subscript =  1000000L,          // largest  valid subscript
    no_subscript  = 0x11223344          // value if not subscripted
                // must be outside of range min_subscript,max_subscript
} sub_index;

typedef enum {
    local_var   = 0x0001,
    subscripted = 0x0002,
    auto_inc    = 0x0004,
    predefined  = 0x0008,               // predefined at startup
    ro          = 0x0010,               // value not changable
    no_free     = 0x0020,               // symbol is defined at compile time
//    type_long   = 0x0040,
//    type_str    = 0x0080,
//    type_char   = 0x0100,
    access_fun  = 0x0200,               // get value via function call
    late_subst  = 0x0400,               // substituted not too early
    deleted     = 0x0800
} symbol_flags;


/***************************************************************************/
/*  entry for a (subscripted) symbolic variable                            */
/***************************************************************************/
typedef struct symsub {
    struct symsub   *   next;           // next subscript entry
    struct symvar   *   base;           // the base symvar
    sub_index           subscript;      // the subscript
    char            *   value;          // the value ptr
} symsub;


/***************************************************************************/
/*  Symbolic variable base entry                                           */
/***************************************************************************/
typedef struct symvar {
    struct symvar   *   next;           // next base entry
    char                name[SYM_NAME_LENGTH + 2];
    long                last_auto_inc;// last autoincremented subscript value
    long                subscript_used; // count of used subscripts
    symsub          *   subscripts;     // subscript entries
    symsub          *   sub_0;          // special subscript 0 entry
    void                (*varfunc)( struct symvar * e );// access function
    symbol_flags        flags;
} symvar;


#endif                                  // GTYPE_H_INCLUDED
