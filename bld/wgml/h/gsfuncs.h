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
* Description:  WGML script string functions definitions
*
*   some functions are implemented without some optional parameters
*   as these are not used in the OW doc build system
****************************************************************************/

#ifndef pick
    #error macro pick not defined
#endif
//                                 additional
//      function  name  mandatory  optional
//      name     length   parmcount     routine name
  pick( delstr,      6,   2,      1,    scr_delstr     )

  pick( c2d,         3,   1,      1,    scr_c2d        )// parm 2 is ignored
  pick( c2x,         3,   1,      0,    scr_c2x        )
  pick( d2c,         3,   1,      0,    scr_d2c        )// optional parm 2
//pick( d2c,         3,   1,      1,    scr_d2c        )// ..not implemented

/*
 * index and pos are only different in the order of the first 2 parms
 */
  pick( index,       5,   2,      1,    scr_index      )// parms haystack,needle,
  pick( lastpos,     7,   2,      1,    scr_lpos       )// parms needle,haystack,
  pick( pos,         3,   2,      1,    scr_pos        )// parms needle,haystack,

  pick( insert,      6,   2,      1,    scr_insert     )// optional parms 4 + 5
//pick( insert,      6,   2,      3,    scr_insert     )// ..not implemented

  pick( left,        4,   2,      0,    scr_left       )// optional parm 3
//pick( left,        4,   2,      1,    scr_left       )// ..not implemented

  pick( length,      6,   1,      0,    scr_length     )
  pick( lower,       5,   1,      2,    scr_lower      )
  pick( min,         3,   2,      4,    scr_min        )// restricted to 6 parms
  pick( right,       5,   2,      1,    scr_right      )
  pick( strip,       5,   1,      2,    scr_strip      )
  pick( substr,      6,   2,      2,    scr_substr     )
  pick( subword,     7,   2,      1,    scr_subword    )
  pick( translate,   9,   1,      3,    scr_translate  )
  pick( upper,       5,   1,      2,    scr_upper      )

  pick( veclastpos, 10,   2,      0,    scr_veclastpos )// optional parms 3 + 4
  pick( vecpos,      6,   2,      0,    scr_vecpos     )// ..not implemented
//pick( veclastpos, 10,   2,      2,    scr_veclastpos )
//pick( vecpos,      6,   2,      2,    scr_vecpos     )

  pick( width,       5,   1,      1,    scr_width      )
  pick( word,        4,   2,      0,    scr_word       )
  pick( wordpos,     7,   2,      1,    scr_wordpos    )
  pick( words,       5,   1,      0,    scr_words      )

#undef  pick

