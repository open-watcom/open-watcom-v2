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


#ifndef WDE_DEFINE_H_INCLUDED
#define WDE_DEFINE_H_INCLUDED

/****************************************************************************/
/* Controls to change dialog and control data structures (200-360)          */
/****************************************************************************/

#define IDB_TEXT         200
#define IDB_CAPTION      201
#define IDB_ID           202
#define IDB_CLASS        203
#define IDB_MENU         204
#define IDB_FONTNAME     205
#define IDB_POINTSIZE    206
#define IDB_DLGNAME      207
#define IDB_STYLES       208
#define IDB_OBJNAME      209
#define IDB_SYMBOL       210
#define IDB_CAPTEXT      211
#define IDB_HELP         212
#define IDB_HELPSYMBOL   213
#define IDB_HELPID       214

/****************************************************************************/
/* Controls to change pre-defined window styles (220-259)                   */
/****************************************************************************/
#define IDB_WS_OVERLAPPED         220
#define IDB_WS_POPUP              221
#define IDB_WS_CHILD              222
#define IDB_WS_CLIPSIBLINGS       223
#define IDB_WS_CLIPCHILDREN       224
#define IDB_WS_VISIBLE            225
#define IDB_WS_DISABLED           226
#define IDB_WS_MINIMIZE           227
#define IDB_WS_MAXIMIZE           228
#define IDB_WS_CAPTION            229
#define IDB_WS_BORDER             230
#define IDB_WS_DLGFRAME           231
#define IDB_WS_VSCROLL            232
#define IDB_WS_HSCROLL            233
#define IDB_WS_SYSMENU            234
#define IDB_WS_THICKFRAME         235
#define IDB_WS_MINIMIZEBOX        236
#define IDB_WS_MAXIMIZEBOX        237
#define IDB_WS_GROUP              238   // IDB_WS_MINIMIZEBOX
#define IDB_WS_TABSTOP            239   // IDB_WS_MAXIMIZEBOX
#define IDB_WS_SIZEBOX            240   // IDB_WS_THICKFRAME

#define IDB_DS_ABSALIGN           241
#define IDB_DS_SYSMODAL           242
#define IDB_DS_LOCALEDIT          243
#define IDB_DS_SETFONT            244
#define IDB_DS_MODALFRAME         245
#define IDB_DS_NOIDLEMSG          246

#define IDB_DS_SETFOREGROUND      247   /* JPK - new for Win95 */
#define IDB_DS_3DLOOK             248
#define IDB_DS_FIXEDSYS           249
#define IDB_DS_NOFAILCREATE       250
#define IDB_DS_CONTROL            251
#define IDB_DS_CENTER             252
#define IDB_DS_CENTERMOUSE        253
#define IDB_DS_CONTEXTHELP        254

#define IDB_MEM_MOVEABLE          255
#define IDB_MEM_DISCARDABLE       256
#define IDB_MEM_PURE              257
#define IDB_LOAD_PRELOAD          258

#define IDB_SS_LEFT               260
#define IDB_SS_CENTER             261
#define IDB_SS_RIGHT              262
#define IDB_SS_ICON               263
#define IDB_SS_BLACKRECT          264
#define IDB_SS_GRAYRECT           265
#define IDB_SS_WHITERECT          266
#define IDB_SS_BLACKFRAME         267
#define IDB_SS_GRAYFRAME          268
#define IDB_SS_WHITEFRAME         269
#define IDB_SS_SIMPLE             270
#define IDB_SS_LEFTNOWORDWRAP     271
#define IDB_SS_NOPREFIX           272
#define IDB_SS_BITMAP             273   /* JPK - New for Win95 */
#define IDB_SS_CENTERIMAGE        274
#define IDB_SS_ENHMETAFILE        275
#define IDB_SS_ETCHEDFRAME        276
#define IDB_SS_ETCHEDHORZ         277
#define IDB_SS_ETCHEDVERT         278
#define IDB_SS_NOTIFY             279
#define IDB_SS_OWNERDRAW          280
#define IDB_SS_REALSIZEIMAGE      281
#define IDB_SS_RIGHTJUST          282
#define IDB_SS_LEFTJUST           283
#define IDB_SS_SUNKEN             284

#define IDB_BS_PUSHBUTTON         290
#define IDB_BS_DEFPUSHBUTTON      291
#define IDB_BS_CHECKBOX           292
#define IDB_BS_AUTOCHECKBOX       293
#define IDB_BS_RADIOBUTTON        294
#define IDB_BS_3STATE             295
#define IDB_BS_AUTO3STATE         296   /* not used  as IBD_BS_3STATE is a
                                         * radio button
                                         */
#define IDB_BS_GROUPBOX           297
#define IDB_BS_USERBUTTON         298
#define IDB_BS_AUTORADIOBUTTON    299
#define IDB_BS_OWNERDRAW          300
#define IDB_BS_LEFTTEXT           301

#define IDB_BS_BITMAP             302   /* JPK - New for Win95 */
#define IDB_BS_BOTTOM             303
#define IDB_BS_CENTER             304
#define IDB_BS_ICON               305
#define IDB_BS_LEFT               306
#define IDB_BS_MULTILINE          307
#define IDB_BS_NOTIFY             308
#define IDB_BS_PUSHLIKE           309
#define IDB_BS_RIGHT              310
#define IDB_BS_RIGHTBUTTON        311
#define IDB_BS_TEXT               312
#define IDB_BS_TOP                313
#define IDB_BS_VCENTER            314

#define IDB_ES_LEFT               320
#define IDB_ES_CENTER             321
#define IDB_ES_RIGHT              322
#define IDB_ES_MULTILINE          323
#define IDB_ES_UPPERCASE          324
#define IDB_ES_LOWERCASE          325
#define IDB_ES_PASSWORD           326
#define IDB_ES_AUTOVSCROLL        327
#define IDB_ES_AUTOHSCROLL        328
#define IDB_ES_NOHIDESEL          329
#define IDB_ES_OEMCONVERT         330
#define IDB_ES_READONLY           331
#define IDB_ES_WANTRETURN         332
#define IDB_ES_NUMBER             333   /* JPK - New for Win95 */

#define IDB_SBS_HORZ              340
#define IDB_SBS_VERT              341
#define IDB_SBS_TOPALIGN          342
#define IDB_SBS_LEFTALIGN         343
#define IDB_SBS_BOTTOMALIGN       344
#define IDB_SBS_RIGHTALIGN        345
#define IDB_SBS_SIZEBOXTOPLEFTALIGN     346
#define IDB_SBS_SIZEBOXBOTTOMRIGHTALIGN 347
#define IDB_SBS_SIZEGRIP          348   /* JPK - New for Win95 */

#define IDB_LBS_NOTIFY            350
#define IDB_LBS_SORT              351
#define IDB_LBS_NOREDRAW          352
#define IDB_LBS_MULTIPLESEL       353
#define IDB_LBS_OWNERDRAWFIXED    354
#define IDB_LBS_OWNERDRAWVARIABLE 355
#define IDB_LBS_HASSTRINGS        356
#define IDB_LBS_USETABSTOPS       357
#define IDB_LBS_NOINTEGRALHEIGHT  358
#define IDB_LBS_MULTICOLUMN       359
#define IDB_LBS_WANTKEYBOARDINPUT 360
#define IDB_LBS_EXTENDEDSEL       361
#define IDB_LBS_DISABLENOSCROLL   362
#define IDB_LBS_STANDARD          363
#define IDB_LBS_NOSEL             364   /* JPK - New for Win95 */

#define IDB_CBS_SIMPLE            370
#define IDB_CBS_DROPDOWN          371
#define IDB_CBS_DROPDOWNLIST      372
#define IDB_CBS_OWNERDRAWFIXED    373
#define IDB_CBS_OWNERDRAWVARIABLE 374
#define IDB_CBS_AUTOHSCROLL       375
#define IDB_CBS_OEMCONVERT        376
#define IDB_CBS_SORT              377
#define IDB_CBS_HASSTRINGS        378
#define IDB_CBS_NOINTEGRALHEIGHT  379
#define IDB_CBS_DISABLENOSCROLL   380
#define IDB_CBS_LOWERCASE         381   /* JPK - New for Win95 */
#define IDB_CBS_UPPERCASE         383
#define IDB_CBS_BOTHCASE          384

/****************************************************************************/
/* Controls to define extended window styles                                */
/****************************************************************************/

#define IDB_WS_EX_DLGMODALFRAME    400   /* JPK - added these for 10.6 */
#define IDB_WS_EX_NOPARENTNOTIFY   401
#define IDB_WS_EX_TOPMOST          402
#define IDB_WS_EX_ACCEPTFILES      403
#define IDB_WS_EX_TRANSPARENT      404
#define IDB_WS_EX_MDICHILD         405   /* New for Win95 */
#define IDB_WS_EX_TOOLWINDOW       406
#define IDB_WS_EX_WINDOWEDGE       407
#define IDB_WS_EX_CLIENTEDGE       408
#define IDB_WS_EX_CONTEXTHELP      409
#define IDB_WS_EX_RIGHT            410
#define IDB_WS_EX_LEFT             411
#define IDB_WS_EX_RTLREADING       412
#define IDB_WS_EX_LTRREADING       413
#define IDB_WS_EX_LEFTSCROLLBAR    414
#define IDB_WS_EX_RIGHTSCROLLBAR   415
#define IDB_WS_EX_CONTROLPARENT    416
#define IDB_WS_EX_STATICEDGE       417
#define IDB_WS_EX_APPWINDOW        418
#define IDB_WS_EX_OVERLAPPEDWINDOW 419
#define IDB_WS_EX_PALETTEWINDOW    420

/****************************************************************************/
/* Controls to define common control styles                                 */
/****************************************************************************/

#define IDB_HDS_HORZ               500
#define IDB_HDS_BUTTONS            501

#define IDB_SBT_OWNERDRAW          505
#define IDB_SBT_NOBORDERS          506
#define IDB_SBT_POPOUT             507
#define IDB_SBT_RTLREADING         508

#define IDB_TBS_ONESIDE            510
#define IDB_TBS_AUTOTICKS          511
#define IDB_TBS_VERT               512
#define IDB_TBS_HORZ               513
#define IDB_TBS_TOP                514
#define IDB_TBS_BOTTOM             515
#define IDB_TBS_LEFT               516
#define IDB_TBS_RIGHT              517
#define IDB_TBS_BOTH               518
#define IDB_TBS_NOTICKS            519
#define IDB_TBS_ENABLESELRANGE     520
#define IDB_TBS_FIXEDLENGTH        521
#define IDB_TBS_NOTHUMB            522

#define IDB_UDS_WRAP               530
#define IDB_UDS_SETBUDDYINT        531
#define IDB_UDS_ALIGNRIGHT         532
#define IDB_UDS_ALIGNLEFT          533
#define IDB_UDS_AUTOBUDDY          534
#define IDB_UDS_ARROWKEYS          535
#define IDB_UDS_HORZ               536
#define IDB_UDS_NOTHOUSANDS        537

#define IDB_LVS_ICON               540
#define IDB_LVS_REPORT             541
#define IDB_LVS_SMALLICON          542
#define IDB_LVS_LIST               543
#define IDB_LVS_TYPEMASK           544
#define IDB_LVS_SINGLESEL          545
#define IDB_LVS_SHOWSELALWAYS      546
#define IDB_LVS_SORTASCENDING      547
#define IDB_LVS_SORTDESCENDING     548
#define IDB_LVS_SHAREIMAGELISTS    549
#define IDB_LVS_NOLABELWRAP        550
#define IDB_LVS_AUTOARRANGE        551
#define IDB_LVS_EDITLABELS         552
#define IDB_LVS_NOSCROLL           553
#define IDB_LVS_ALIGNTOP           554
#define IDB_LVS_ALIGNLEFT          555
#define IDB_LVS_OWNERDRAWFIXED     556
#define IDB_LVS_NOCOLUMNHEADER     557
#define IDB_LVS_NOSORTHEADER       558
#define IDB_LVS_NOSORTING          559

#define IDB_TVS_HASBUTTONS         560
#define IDB_TVS_HASLINES           561
#define IDB_TVS_LINESATROOT        562
#define IDB_TVS_EDITLABELS         563
#define IDB_TVS_DISABLEDRAGDROP    564
#define IDB_TVS_SHOWSELALWAYS      565

#define IDB_TCS_FORCEICONLEFT      570
#define IDB_TCS_FORCELABELLEFT     571
#define IDB_TCS_TABS               572
#define IDB_TCS_BUTTONS            573
#define IDB_TCS_SINGLELINE         574
#define IDB_TCS_MULTILINE          575
#define IDB_TCS_RIGHTJUSTIFY       576
#define IDB_TCS_FIXEDWIDTH         577
#define IDB_TCS_RAGGEDRIGHT        578
#define IDB_TCS_FOCUSONBUTTONDOWN  579
#define IDB_TCS_OWNERDRAWFIXED     580
#define IDB_TCS_TOOLTIPS           581
#define IDB_TCS_FOCUSNEVER         582

#define IDB_ACS_CENTER             590
#define IDB_ACS_TRANSPARENT        591
#define IDB_ACS_AUTOPLAY           592

#define IDB_CCS_TOP                601
#define IDB_CCS_NOMOVEY            602
#define IDB_CCS_BOTTOM             603
#define IDB_CCS_NORESIZE           604
#define IDB_CCS_NOPARENTALIGN      605
#define IDB_CCS_ADJUSTABLE         606
#define IDB_CCS_NODIVIDER          607

/****************************************************************************/
/* NT specific define statements; needed for differences between NT and 95  */
/****************************************************************************/
#ifdef __NT__
    #define __NT__XX  1
#else
    #define __NT__XX  0
#endif

#endif
