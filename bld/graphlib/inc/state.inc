;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


; offsets for members of videoinfo structure

                                        ; struct videoinfo {
                                        ;       struct videoconfig vc {
numxpixels      equ     0               ;         short numxpixels
numypixels      equ     2               ;         short numxpixels
numtextcols     equ     4               ;         short numtextcols
numtextrows     equ     6               ;         short numtextrows
numcolors       equ     8               ;         short numcolors
bitsperpixel    equ     10              ;         short bitsperpixel
numvideopages   equ     12              ;         short numvideopages
mode            equ     14              ;         short mode
adapter         equ     16              ;         short adapter
monitor         equ     18              ;         short monitor
memory          equ     20              ;         short memory }

deviceptr       equ     22              ;       void    **deviceptr
                                        ;       struct clip_region clip {
clipxmin        equ     26              ;         short xmin
clipxmax        equ     28              ;         short xmax
clipymin        equ     30              ;         short ymin
clipymax        equ     32              ;         short ymax }
                                        ;       struct clip_region clip_def {
dclipxmin       equ     34              ;         short xmin
dclipxmax       equ     36              ;         short xmax
dclipymin       equ     38              ;         short ymin
dclipymax       equ     40              ;         short ymax }
screen_seg      equ     42              ;       unsigned short screen_seg
screen_off      equ     44              ;       unsigned int   screen_off
screen_seg_base equ     46              ;       unsigned short screen_seg_base
screen_off_base equ     48              ;       unsigned int   screen_off_base;
page_size       equ     50              ;       unsigned short page_size
misc_info       equ     52              ;       unsigned short misc_info

; additional offsets for members of pxa_info structures

bytesperrow     equ     54
actualseg       equ     56              ; segment of current strip
virtualseg      equ     58              ; segment of current strip
segsize         equ     60              ; size in paragraphs of strip
