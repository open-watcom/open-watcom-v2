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


#ifndef _UICHARS_H_
#define _UICHARS_H_
#define PC_smiley           1
#define PC_bsmiley          2
#define PC_hart             3
#define PC_diamond          4
#define PC_club             5
#define PC_spade            6
#define PC_bullet           7
#define PC_bbullet          8
#define PC_hole             9
#define PC_bhole            10
#define PC_male             11
#define PC_female           12
#define PC_quaver           13
#define PC_dblequaver       14
#define PC_sun              15
#define PC_triangright      16
#define PC_triangleft       17
#define PC_arrowupdn        18
#define PC_dblexclam        19
#define PC_paragraph        20
#define PC_section          21
#define PC_hbar             22
#define PC_arrowupdngrnd    23
#define PC_arrowup          24
#define PC_arrowdown        25
#define PC_arrowright       26
#define PC_arrowleft        27
#define PC_hook             28
#define PC_arrowboth        29
#define PC_triangup         30
#define PC_triangdown       31

#define PC_house            127

#define PC_Ccedilla         128
#define PC_udieresis        129
#define PC_eacute           130
#define PC_acicumflex       131
#define PC_adieresis        132
#define PC_agrave           133
#define PC_aring            134
#define PC_ccedilla         135
#define PC_ecircumflex      136
#define PC_edieresis        137
#define PC_egrave           138
#define PC_idieresis        139
#define PC_icircumflex      140
#define PC_igrave           141
#define PC_Adieresis        142
#define PC_Aring            143
#define PC_Eacute           144
#define PC_ae               145
#define PC_AE               146
#define PC_ocircumflex      147
#define PC_odieresis        148
#define PC_ograve           149
#define PC_ucircumflex      150
#define PC_ugrave           151
#define PC_ydieresis        152
#define PC_Odieresis        153
#define PC_Udieresis        154
#define PC_cent             155
#define PC_sterling         156
#define PC_yen              157
#define PC_Pt               158
#define PC_florin           159

#define PC_aacute           160
#define PC_iacute           161
#define PC_oacute           162
#define PC_uacute           163
#define PC_ntilde           164
#define PC_Ntilde           165
#define PC_ordfeminine      166
#define PC_ordmasculine     167
#define PC_questiondown     168
#define PC_bracketlefttp    169
#define PC_bracketrighttp   170
#define PC_half             171
#define PC_quarter          172
#define PC_exclamdown       173
#define PC_guillemotleft    174
#define PC_guillemotright   175
#define PC_sparseblock      176
#define PC_mediumblock      177
#define PC_denseblock       178
#define PC_vertical         179
#define PC_vertleft         180
#define PC_vertdblleft      181
#define PC_dblvertleft      182
#define PC_toprightdbldown  183
#define PC_toprightdblleft  184
#define PC_dblvertdblleft   185
#define PC_dblvert          186
#define PC_dbltopright      187
#define PC_dblbotright      188
#define PC_botrightdblup    189
#define PC_botrightdblleft  190
#define PC_topright         191

#define PC_botleft          192
#define PC_botmiddle        193
#define PC_topmiddle        194
#define PC_vertright        195
#define PC_horizontal       196
#define PC_vertcross        197
#define PC_vertdblright     198
#define PC_dblvertright     199
#define PC_dblbotleft       200
#define PC_dbletopleft      201
#define PC_dblbotdblmiddle  202
#define PC_dbltopdblmiddle  203
#define PC_dblvertdblright  204
#define PC_dblhorizontal    205
#define PC_dblvertdblcross  206
#define PC_dblbotmiddle     207
#define PC_botdblmiddle     208
#define PC_dbltopmiddle     209
#define PC_topdblmiddle     210
#define PC_botleftdblup     211
#define PC_botleftdblright  212
#define PC_topleftdblright  213
#define PC_topleftdbldown   214
#define PC_dblvertcross     215
#define PC_vertdblcross     216
#define PC_botright         217
#define PC_topleft          218
#define PC_solid            219
#define PC_solidbot         220
#define PC_solidleft        221
#define PC_solidright       222
#define PC_solidtop         223

#define PC_alpha            224
#define PC_beta             225
#define PC_Gamma            226
#define PC_pi               227
#define PC_Sigma            228
#define PC_sigma            229
#define PC_mu               230
#define PC_tau              231
#define PC_Phi              232
#define PC_theta            233
#define PC_Omega            234
#define PC_delta            235
#define PC_infinity         236
#define PC_emptyset         237
#define PC_element          238
#define PC_intersection     239
#define PC_equivalence      240
#define PC_plusminus        241
#define PC_greaterequal     242
#define PC_lessequal        243
#define PC_integraltp       244
#define PC_integralbt       245
#define PC_divide           246
#define PC_approxequal      247
#define PC_degree           248
#define PC_bigdotmath       249
#define PC_dotmath          250
#define PC_radical          251
#define PC_npower           252
#define PC_square           253
#define PC_smallsolid       254
#define PC_notdef           255

#if defined(_NEC_PC)

#undef PC_arrowup
#undef PC_arrowdown
#undef PC_arrowright
#undef PC_arrowleft
#undef PC_triangup
#undef PC_triangdown
#undef PC_triangright
#undef PC_triangleft
#undef PC_sparseblock
#undef PC_solid

#define PC_arrowup          30
#define PC_arrowdown        31
#define PC_arrowright       28
#define PC_arrowleft        29
#define PC_triangup         30
#define PC_triangdown       31
#define PC_triangright      62
#define PC_triangleft       60
#define PC_sparseblock      32
#define PC_solid            32

#elif defined(_FMR_PC)

#undef PC_arrowup
#undef PC_arrowdown
#undef PC_arrowright
#undef PC_arrowleft
#undef PC_triangup
#undef PC_triangdown
#undef PC_triangright
#undef PC_triangleft
#undef PC_sparseblock
#undef PC_solid

#define PC_arrowup          30
#define PC_arrowdown        31
#define PC_arrowright       28
#define PC_arrowleft        29
#define PC_triangup         30
#define PC_triangdown       31
#define PC_triangright      62
#define PC_triangleft       60
#define PC_sparseblock      254
#define PC_solid            135

#endif
#endif
