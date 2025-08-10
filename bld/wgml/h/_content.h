/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML region content atrributes related data declaration.
*
****************************************************************************/


pick( "none",      no_content        )
pick( "author",    author_content    )
pick( "bothead",   bothead_content   )
pick( "date",      date_content      )
pick( "docnum",    docnum_content    )
pick( "head0",     head0_content     )
pick( "head1",     head1_content     )
pick( "head2",     head2_content     )
pick( "head3",     head3_content     )
pick( "head4",     head4_content     )
pick( "head5",     head5_content     )
pick( "head6",     head6_content     )
pick( "headnum0",  headnum0_content  )
pick( "headnum1",  headnum1_content  )
pick( "headnum2",  headnum2_content  )
pick( "headnum3",  headnum3_content  )
pick( "headnum4",  headnum4_content  )
pick( "headnum5",  headnum5_content  )
pick( "headnum6",  headnum6_content  )
pick( "headtext0", headtext0_content )
pick( "headtext1", headtext1_content )
pick( "headtext2", headtext2_content )
pick( "headtext3", headtext3_content )
pick( "headtext4", headtext4_content )
pick( "headtext5", headtext5_content )
pick( "headtext6", headtext6_content )
/*
 * The "d" forms must be first or they will never be found
 */
pick( "pgnumad",   pgnumad_content   )
pick( "pgnuma",    pgnuma_content    )
pick( "pgnumrd",   pgnumrd_content   )
pick( "pgnumr",    pgnumr_content    )
pick( "pgnumcd",   pgnumcd_content   )
pick( "pgnumc",    pgnumc_content    )
pick( "rule",      rule_content      )
pick( "sec",       sec_content       )
pick( "stitle",    stitle_content    )
pick( "title",     title_content     )
pick( "time",      time_content      )
pick( "tophead",   tophead_content   )
/*
 * Must be last: will match any following entries
 */
pick( "",          string_content    )  /* special */
