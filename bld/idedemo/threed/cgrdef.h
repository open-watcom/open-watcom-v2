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


#ifndef CGR_CGRDEF_H
#define CGR_CGRDEF_H
/*
 Description:
 ============
     This file defines the externs for the external chart graphics
     routines.

*/

#include <wpitypes.h>

/* init.c */
extern bool APIENTRY cgr_init(void);

/* done.c */
extern void APIENTRY cgr_done(void);

/* charthld.c */
extern long    APIENTRY cgr_get_app_info(chart_hld );
extern bool    APIENTRY cgr_set_app_info(chart_hld ,long );
extern bool    APIENTRY cgr_is_valid_hld( chart_hld );
extern cgr_chart __far *APIENTRY cgr_lock_chart(chart_hld );
extern bool    APIENTRY cgr_unlock_chart(chart_hld );
extern void    APIENTRY cgr_width_legend(chart_hld, int *, int *, WPI_PRECT );
extern void    APIENTRY cgr_height_legend(chart_hld, int *, int *, WPI_PRECT );
extern BOOL    APIENTRY cgr_is_floating(chart_hld, int *, int *, WPI_PRECT, int );
extern void    APIENTRY cgr_delete_legend(chart_hld );

/* drawitem.c */
extern cgr_err APIENTRY cgr_add_arrow(chart_hld , WPI_RECT *, int *, int * );
extern cgr_err APIENTRY cgr_add_text(chart_hld , WPI_RECT *, int *, int * );
extern void    APIENTRY cgr_delete_item(chart_hld, int );
extern void    APIENTRY cgr_set_dtext(chart_hld, int *, int *, WPI_PRECT, int );
extern void    APIENTRY cgr_set_arrow(chart_hld, WPI_POINT, WPI_POINT, WPI_PRECT, int );

/* file.c */
extern void    APIENTRY cgr_save(chart_hld ,void (APIENTRY *)(char __far *,void __far *),void __far *);
extern cgr_err APIENTRY cgr_load(chart_hld ,char __far *(APIENTRY *)(void __far *),void __far *,bool );

/* data.c */
extern cgr_err APIENTRY cgr_set_data(chart_hld ,unsigned int ,unsigned int );

/* chart.c */
extern chart_hld APIENTRY cgr_chart_add(chart_type ,cgr_add_chart __far *);
extern chart_hld APIENTRY cgr_chart_add_font(chart_type ,cgr_add_chart __far *,char *,unsigned int, char );
extern bool APIENTRY cgr_chart_big_change(chart_hld ,cgr_add_chart __far *);
extern void APIENTRY cgr_chart_default(chart_hld );
extern chart_hld APIENTRY cgr_chart_copy(chart_hld );
extern bool APIENTRY cgr_chart_done(chart_hld );
extern bool APIENTRY cgr_chart_move(chart_hld ,chart_hld );
extern cgr_err APIENTRY cgr_chart_change(chart_hld ,chart_type );
extern bool APIENTRY cgr_chart_info(chart_hld ,cgr_add_chart __far *);
extern cgr_err APIENTRY cgr_chart_check(chart_hld );
extern bool APIENTRY cgr_pie_slice_in_other(chart_hld ,int );
extern bool APIENTRY cgr_is_3d_chart(chart_hld );
extern void APIENTRY cgr_get_default_light_3d( light_3d far * light );
extern bool APIENTRY cgr_has_z_axis( chart_hld hld );
extern BOOL APIENTRY cgr_def_attrs(chart_hld );

/* grpchk.c */
extern cgr_err cgr_group_by_check(chart_hld );

/* error.c */
extern char __far *APIENTRY cgr_error(cgr_err );

/* display.c */
extern cgr_err APIENTRY cgr_display_chart(chart_hld ,WPI_PRECT ,WPI_PRES ,bool, bool ,bool );

/* window.c */
extern WPI_COLOUR APIENTRY cgr_index_to_colour(int );
extern void APIENTRY cgr_aspect_ratio(float __far *);
extern HBRUSH APIENTRY cgr_make_brush(WPI_COLOUR ,fill_style );
extern void APIENTRY cgr_delete_brush(HBRUSH );
extern WPI_COLOUR __far *APIENTRY cgr_colour_list(void);
extern void WINEXP cgr_get_default_logfont( WPI_PRES dc, LOGFONT * font );

/* scale.c */
extern void APIENTRY cgr_set_manual_values(chart_hld );

/* cgrrgn.c */
extern int APIENTRY cgr_rgn_ids(chart_hld ,unsigned int ,int __far *,int __far *,int __far *);
extern void APIENTRY cgr_rgn_mark_free(HANDLE );
extern void APIENTRY cgr_rgn_unmark(chart_hld ,unsigned int ,WPI_PRES ,HANDLE );
extern HANDLE APIENTRY cgr_rgn_mark(chart_hld ,unsigned int ,WPI_PRES ,BOOL, BOOL );
extern unsigned int APIENTRY cgr_rgn_find(chart_hld ,int ,int );
extern void APIENTRY cgr_get_rect(chart_hld, unsigned int, WPI_RECT * );
extern void APIENTRY cgr_get_line(chart_hld, unsigned int, WPI_POINT *, WPI_POINT * );
extern int  APIENTRY cgr_line_point(chart_hld, unsigned int, int, int);

/* grpicon.c */
extern void APIENTRY cgr_group_icon(WPI_PRES ,void __far *, int , chart_type ,WPI_PRECT );

/* palette.c */
extern void APIENTRY cgr_change_palette(chart_hld ,WPI_COLOUR __far *,int );
extern WPI_COLOUR __far *APIENTRY cgr_get_palette(cgr_chart __far *,int *);
extern WPI_COLOUR APIENTRY cgr_get_color(chart_hld ,int );

/* fake.c */
extern BOOL APIENTRY cgr_fake_data(chart_hld ,int ,int ,float *);
extern LPSTR APIENTRY cgr_fake_row_names(chart_hld ,int );
extern LPSTR APIENTRY cgr_fake_col_names(chart_hld ,int );

/* version verificataion */
extern unsigned cgr_version_check( void );

/* wire.c */
extern BOOL APIENTRY cgr_display_wire_frame( WPI_PRES, WPI_PRECT, view_3d far *, chart_type );

/* util.c */
extern void APIENTRY cgr_draw_marker(WPI_PRES ,marker_style ,WPI_PRECT );

#endif
