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


#ifndef CGR_ICGRDEF_H
#define CGR_ICGRDEF_H
/*
 Description:
 ============
     This file defines the externs for the internal chart graphics
     routines.

*/

extern cgr_err set_up_chart(void);
extern void finish_up_chart(void);
extern cgr_err APIENTRY cgr_display_chart(chart_hld , WPI_PRECT ,WPI_PRES ,bool ,bool ,bool );
extern void init_scale(void);
extern void APIENTRY cgr_set_manual_values(chart_hld );
extern void set_scale(void);
extern float scale_normalize(float ,scale_xy );
extern float cgr_data(int ,int ,bool *);
extern float cgr_in_data(int ,int ,bool *);
extern char *cgr_row_string(int );
extern char *cgr_col_string(int );
extern int map_index(int );
extern pie_attr *map_pie_attr(int );
extern bla_attr *map_bla_attr(int );
extern cgr_err get_more_attrs(int );
extern bool pie_other_slice(void);
extern cgr_err WINEXP cgr_set_data(chart_hld ,unsigned int ,unsigned int );
extern char *cgr_in_row_string(int );
extern char *cgr_in_col_string(int );
extern int col_display(void);
extern float p_col_name_size(text_def *);
extern float marker_size(float *);
extern float max_row_name_size(text_def *,int *);
extern bool set_data(void);
extern void end_data(void);
extern col_type column_type(gda_index );
extern char *xy_col_string(int );
extern char *bubble_col_string(int );
extern float get_xy_data(int ,int ,bool *);
extern float get_bubble_data(int ,int ,bool *);
extern cgr_err display_xy(void);
extern cgr_err display_radar(void);
extern cgr_err display_stock(void);
extern cgr_err display_box_whisker(void);
extern cgr_err display_draw_items(void);
extern void APIENTRY cgr_save(chart_hld ,void (APIENTRY *)(char __far *,void __far *),void __far *);
extern cgr_err APIENTRY cgr_load(chart_hld ,char __far *(APIENTRY *)(void __far *),void __far *,bool );
extern chart_hld APIENTRY cgr_chart_add(chart_type ,cgr_add_chart __far *);
extern bool APIENTRY cgr_chart_big_change(chart_hld ,cgr_add_chart __far *);
extern void set_default_draw_attr(cgr_chart *,int ,int );
extern void set_default_bla_attr(cgr_chart *,int ,int );
extern void set_default_pie_attr(cgr_chart *,int ,int );
extern void APIENTRY cgr_chart_default(chart_hld );
extern chart_hld APIENTRY cgr_chart_copy(chart_hld );
extern bool APIENTRY cgr_chart_done(chart_hld );
extern bool APIENTRY cgr_chart_move(chart_hld ,chart_hld );
extern bool chart_change(cgr_chart *,chart_type );
extern void APIENTRY cgr_get_default_light_3d( light_3d far * light );
extern cgr_err APIENTRY cgr_chart_change(chart_hld ,chart_type );
extern bool APIENTRY cgr_chart_info(chart_hld ,cgr_add_chart __far *);
extern cgr_err APIENTRY cgr_chart_check(chart_hld );
extern bool APIENTRY cgr_pie_slice_in_other(chart_hld ,int );
extern char *cgr_read_line(void);
extern void display_trend( bla_attr * attr, curve_map * map );
extern void display_line(void);
extern void display_curve( bla_attr * attr, curve_map * map );
extern cgr_err display_lines(void);
extern char __far *APIENTRY cgr_error(cgr_err );
extern WPI_FONT text_init(text_def *,text_dir );
extern WPI_FONT text_init_size( text_def * text, text_dir dir, unsigned size );
extern void text_output(float ,float ,char *,int ,int ,WPI_FONT );
extern void text_done(WPI_FONT );
extern void text_now( const text_out * );
extern void text_delay_add( const text_out * );
extern void text_delay_dump( void );
extern void value_label(float ,float ,int ,int ,scale_xy, bool ,bool );
extern void value_label_init(void);
extern void value_label_store(float ,float ,int ,text_align ,scale_xy, bool, bool );
extern void value_label_dump(void);
extern void value_label_end(void);
extern compact_op compactation( scale_information * scale );
extern void make_label(char *,struct format_desc *,int ,compact_op ,float );
extern void display_before(void);
extern void display_after(void);
extern void display_chart_titles(void);
extern void display_legend(void);
extern cgr_err cgr_group_by_check(chart_hld );
extern float title_height( title_type );
extern float scale_width(scale_xy ,int *);
extern void set_view(void);
extern cgr_err display_area(void);
extern void bar_overlap_varying(float ,gda_index );
extern void bar_gantt(float ,gda_index );
extern void APIENTRY cgr_done(void);
extern int APIENTRY WEP(int );
extern cgr_err display_bar(void);
extern cgr_err display_bubble(void);
extern cgr_err display_pie(void);
extern void global_renew(void __far **,HANDLE *,unsigned int );
extern long APIENTRY cgr_get_app_info(chart_hld );
extern bool APIENTRY cgr_set_app_info(chart_hld ,long );
extern cgr_chart __far *APIENTRY cgr_lock_chart(chart_hld );
extern bool APIENTRY cgr_unlock_chart(chart_hld );
extern void __far *cgr_get_region_list(chart_hld );
extern legend_type cgr_get_float_info(chart_hld );
extern void cgr_set_region_list(chart_hld ,HANDLE );
extern bool cgr_del_hld(chart_hld );
extern bool cgr_move_hld(chart_hld ,chart_hld );
extern chart_hld cgr_add_hld(void );
extern int signum(float );
extern void rotate_check(int ,wcoord *);
extern void norm_and_rotate(int ,wcoord *,scale_xy );
extern void cgr_set_color(int );
extern void set_frame_color(void);
extern void set_obj_color(cgr_color );
extern void set_3d_obj_colour(cgr_color );
extern void set_line_style( line_style style, line_width width );
extern void draw_lines(line_style ,int ,wcoord *);
extern void draw_pie(fill_style ,wcoord *,bool );
extern void draw_pie3( wcoord *pie_defn, text_pt *text, bool above_view, bool all_pie );
extern void draw_polygon(fill_style ,line_style ,int ,wcoord *);
extern void draw_polygon_rgn(fill_style ,line_style ,int ,wcoord * ,BOOL );
extern void draw_polygon3( int num_pts, w3coord * pts, bool * );
extern void draw_marker(marker_style ,float ,float ,float ,float );
extern void draw_rectangle(fill_style ,float ,float ,wcoord ,BOOL );
extern bool clip_polygon(wcoord *,int ,wcoord **,int *,int *,wcoord *,scale_xy );
extern bool check_y_scales(void);
extern void set_row_scale(void);
extern bool APIENTRY cgr_init(void);
extern int APIENTRY LibMain(WPI_INST ,WORD ,WORD ,LPSTR );
extern WPI_COLOUR __far *APIENTRY cgr_colour_list(void);
extern WPI_COLOUR APIENTRY cgr_index_to_colour(int );
extern void _winitwindow(WPI_RECT *);
extern void APIENTRY cgr_aspect_ratio(float __far *);
extern void _wsetwindow(float ,float ,float ,float );
extern bool _wsetstylewrap( bool save_style );
extern float max_psize(float ,int );
extern WPI_FONT _wtextinit(text_def *);
extern void _wtextdone(WPI_FONT );
extern float _ptextwidth(char *,text_def *);
extern float _ptextmaxwidth(int ,text_def *);
extern float _ptextheight(text_def *);
extern float _wtextheight(text_def *);
extern float _wtextheight_font( char * text, WPI_FONT font );
extern float _ptextheightsize(int ,text_def *);
extern float _wtextwidth(char *,text_def *);
extern float _wtextwidth_font( char *text_line, WPI_FONT font );
extern float _wtextmaxwidth(int ,text_def *);
extern void _wsetpath(text_dir );
extern void _wtextout(char *,float ,float ,int ,int ,void * );
extern void _wsetlinestyle(line_style );
extern void _wsetlinewidth( line_width width );
extern void _wsetendmarker( bool end_marker );
extern void _wsetfillstyle(fill_style );
extern HBRUSH APIENTRY cgr_make_brush(WPI_COLOUR ,fill_style );
extern void APIENTRY cgr_delete_brush(HBRUSH );
extern void WINEXP cgr_get_default_logfont( WPI_PRES dc, LOGFONT * font );
extern void _wsetcolor(int );
extern void _wsetrgbcolor(WPI_COLOUR );
extern void _wpie(int ,float ,float ,float ,float ,float ,float ,float ,float );
extern void _wmoveto(float ,float );
extern void _wlineto(float ,float ,BOOL );
extern void _wline(int ,int ,BOOL ,BOOL ,BOOL );
extern void _wpolygon(int ,int ,wcoord *);
extern void _wpolygon_rgn(int ,int ,wcoord * ,BOOL );
extern void _wellipse(int ,float ,float ,float ,float );
extern void _world_ellipse(int ,int ,int ,int ,int );
extern void _wdot(float ,float );
extern void _wdot_world(int ,int );
extern void wrgn_rectangle(float ,float ,float ,float );
extern void wrgn_polygon(int ,wcoord *);
extern void wrgn_set_add(wcoord *);
extern void _warc(int ,float ,float ,float ,float ,float ,float ,float ,float );
extern void _wrectangle(int ,float ,float ,float ,float );
extern void rgn_off(void);
extern void rgn_on(void);
extern int rgn_disable(void);
extern void rgn_enable(int );
extern void rgn_begin_group(void);
extern void rgn_end_group(void);
extern void rgn_begin_set(void);
extern void rgn_set_add(WPI_POINT *);
extern void rgn_end_set(void);
extern int rgn_man_group_new( void );
extern void rgn_man_group_begin( int group_num );
extern void rgn_man_group_end( void );
extern void rgn_begin(void);
extern HANDLE rgn_end(void);
extern BOOL rgn_is_on(void);
extern void rgn_set_ids(int ,int ,int ,int );
extern void rgn_rectangle(int ,int ,int ,int );
extern void rgn_ellipse(int ,int ,int ,int );
extern void rgn_ellipse_border(int ,int ,int ,int );
extern void rgn_line(int ,int ,int ,int );
extern void rgn_line_boxes(int ,int ,int ,int );
extern void rgn_polygon(WPI_POINT __far *,int );
extern void rgn_pie(int ,int ,int ,int ,int ,int ,int ,int );
extern void __far *rgn_find(void __far *,int ,int );
extern void rgn_mark_free(HANDLE );
extern void rgn_unmark(void __far *,WPI_PRES ,HANDLE );
extern HANDLE rgn_mark(void __far *,void __far *,WPI_PRES ,BOOL, ibool );
extern void get_rgn_rect(void __far *, WPI_RECT * );
extern void get_rgn_line(void __far *, WPI_POINT *, WPI_POINT * );
extern int get_line_point(void __far *, int, int);
extern int rgn_ids(void __far *,int __far *,int __far *,int __far *);
extern int APIENTRY cgr_rgn_ids(chart_hld ,unsigned int ,int __far *,int __far *,int __far *);
extern void APIENTRY cgr_rgn_leg(chart_hld ,int ,int, int, int );
extern void APIENTRY cgr_rgn_mark_free(HANDLE );
extern void APIENTRY cgr_rgn_unmark(chart_hld ,unsigned int ,WPI_PRES ,HANDLE );
extern HANDLE APIENTRY cgr_rgn_mark(chart_hld ,unsigned int ,WPI_PRES ,BOOL, BOOL );
extern unsigned int APIENTRY cgr_rgn_find(chart_hld ,int ,int );
extern void APIENTRY cgr_group_icon(WPI_PRES ,void __far *,int ,chart_hld , WPI_PRECT );
extern void *pipe_alloc(size_t );
extern void pipe_free_all(void );
extern bool rend_list_init(rend_list *);
extern void rend_list_add(rend_list *,rend_obj *);
extern void rend_list_free(rend_list *);
extern void rend_list_sort(rend_list *,int (*)(const rend_obj **,const rend_obj **));
extern void rend_list_bin_insert( rend_list *, rend_list_ref, rend_list_ref,
        rend_obj **, int (*compare) (const rend_obj **, const rend_obj **) );
extern void rend_list_insert(rend_list *,rend_list_ref *,rend_obj *,bool );
extern void rend_list_delete(rend_list *,rend_list_ref *);
extern vector cross_prod(vector ,vector );
extern float norm(vector );
extern vector calculate_normal_vector( int , void *, point (*get_pt)( void *, int ) );
extern vector point_diff(point ,point );
extern point mult_matrix_pt(float (*)[4],point );
extern bool proj_line_intersection(point *,point *);
extern void rgb_to_hls(WPI_COLOUR ,hls_colour *);
extern void hls_to_rgb(WPI_COLOUR *,hls_colour );
extern void dbg_print_list(rend_list *);
extern void dbg_print_list_long(rend_list *);
extern void pipe3d_display(void );
extern rend_obj *rend_obj_new_poly(void );
extern polygon * rend_obj_get_poly( rend_obj * obj );
extern rend_obj *rend_obj_dup(rend_obj *);
extern void rend_obj_add_to_list(rend_obj *,rend_list *);
extern void rend_obj_lfree(rend_obj *);
extern void rend_obj_compute_vis_info(rend_obj *);
extern vector rend_obj_get_norm(rend_obj *);
extern int rend_obj_get_num_pts(rend_obj *);
extern point rend_obj_get_pt(rend_obj *,int );
extern void rend_obj_calculate_normal(rend_obj *);
extern rend_obj *rend_obj_clip(rend_obj *,vector );
extern rend_obj * rend_obj_create_sweep( rend_obj_add *add );
extern void rend_obj_add_sweep( rend_obj_add *add );
extern void rend_obj_transform(rend_obj *,float (*)[4]);
extern void rend_obj_homo_to_3d(rend_obj *);
extern hls_colour rend_obj_get_base_colour(rend_obj *);
extern void dbg_print_pt(FILE *,point ,char *);
extern void dbg_print_vect(FILE *,vector ,char *);
extern void dbg_print_vertex_list(FILE *,vertex_list *);
extern void dbg_print_index( FILE *, int, index_elt *, char *);
extern void dbg_print_solid(FILE *,polyhedron *);
extern void dbg_print_poly( FILE *fp, polygon *poly );
extern void dbg_print_one_obj(FILE *,rend_obj *,bool );
extern void dbg_print_obj(rend_obj *);
extern void dbg_get_filename( char * path );
extern void pipe3d_add( rend_obj * obj, text_info * text, bool disp_now );
extern void pipe3d_init( pipe_view *view, pipe_illum *illum, bool all_poly_convex );
extern void pipe3d_shutdown(void );
extern void set_illumination( pipe_illum *illum );
extern void set_poly_convex_info( bool all_poly_convex );
extern void _w3init( three_d_op * view, float base_zoom, float light_distance, light_vert_pos auto_pos, bool all_poly_convex );
extern void _w3shutdown(void );
extern void _w3display_all(void );
extern void _w3pie( float, float, float, float, float, float, float, float, float height, text_pt *text, bool above_view, int resolution, bool all_pie );
extern void _w3setcolour(WPI_COLOUR );
extern bool polygon_edge_hilight( rend_obj *obj, int edge_num );
extern void APIENTRY cgr_change_palette(chart_hld ,WPI_COLOUR __far *,int );
extern WPI_COLOUR __far *APIENTRY cgr_get_palette(cgr_chart __far *,int *);
extern WPI_COLOUR APIENTRY cgr_get_color(chart_hld ,int );
extern void global_free_and_new(void __far **,HANDLE *,unsigned int );
extern WPI_COLOUR get_palette_color(int );
extern void cgr_del_all_hlds( void );
extern bool rect_ray_inter( wcoord rect[2], wcoord ray_start, wcoord ray_dir, wcoord *inter );
extern void project_pt( point pt, float *x, float *y );
extern void macro_init(void);
extern void macro_done(void);
extern char *macro_substitute(char *);
extern char *title_substitute( title_type );
extern void _w3bar( w3coord pt1, w3coord pt2, float depth, bool black1, bool black2 );
extern void draw_bar3( w3coord pt1, w3coord pt2, float depth, bool black1, bool black2 );
extern void draw_illuminated_obj( rend_obj *obj );
extern void draw_lines3( line_style style, int num_pts, w3coord * pts );
extern void set_3d_line_style( line_style style );
extern void _w3setlinestyle( line_style style );
extern void _w3moveto( w3coord pt );
extern void _w3moveto_xyz( float x, float y, float z );
extern void _w3lineto( w3coord point, bool disp_now );
extern void _w3lineto_xyz( float x, float y, float z, bool disp_now );
extern void _w3polygon( int num_pts, w3coord * pts, bool * );
extern line * rend_obj_get_line( rend_obj * obj );
extern void _w3get_view_dir( view_3d * view, w3coord * dir );
extern void set_3d_frame_colour( void );
extern void _w3setblackedges( bool black_edges );
extern rend_obj * rend_obj_create_poly( int num_pts, point * pts, hls_colour base, bool black_edges, region_info * rgn, bool *, int );
extern rend_obj * rend_obj_create_line( point start, point end, hls_colour colour, line_style style, region_info * rgn );
extern void point_homo_to_3d( point * pt );
extern void pipe3d_trans_pt( point * pt );
extern void _w3transformpoint( const w3coord * pt_3d, wcoord * pt_2d );
extern BOOL APIENTRY cgr_display_wire_frame( WPI_PRES, WPI_PRECT, view_3d far *, chart_type );
extern bool _wrectvisible( float x1, float y1, float x2, float y2 );
extern void ellipse_pt( float h_sqr, float v_sqr, float cx, float cy, float alpha, float * x, float * y );
extern void rend_obj_region_begin( rend_obj * obj );
extern void rend_obj_region_end( rend_obj * obj );
extern _w3rgnon( int major_id, int minor_id, int data_row, int data_col, int set_num, bool use_group );
extern _w3rgnoff( void );
extern void _w3rgnsetadd( w3coord * pt_3 );
extern int rgn_man_set_new( void );
extern void rgn_man_set_begin( int set_num );
extern void rgn_man_set_end( void );
extern bool is_3d_chart( cgr_chart * chart );
extern bool has_z_axis( cgr_chart * chart );
extern void init_3d_chart( void );
extern void debug_output( char * fmt, ... );
extern cgr_err chart_check(chart_hld ,bool );
extern bool use_z_axis( void );
extern void _w3area( int num_pts, const w3coord * pts, float height, int );
extern void draw_area3( int num_pts, const w3coord * pts, float depth, int );
extern void area_3d_depth( void );
extern void area_3d_stack( void );
extern void draw_polyline3( int num_pts, w3coord * pts, float width );
extern int lines_in_legend( void );
extern int look_in_legend( float * );
extern float pwidth_legend( void );
extern void set_float_legend( chart_hld );
extern void convert_pt( float, float, int *, int * );
extern int _left_coord( void );
extern int _right_coord( void );
extern int _top_coord( void );
extern int _bottom_coord( void );
extern float wwidth_legend( bool );
extern float wheight_legend( bool );
extern float legend_frame_gap();
extern int count_col( gda_index, scale_xy, bool, curve_map * );
extern void set_align( wcoord, int *, int * );
extern void set_location( wcoord *, float, float, float, position_op, three_d_scale );
extern void draw_tick( wcoord *, position_op, tick_type );
extern void draw_xy_scale_label( wcoord, position_op, labels_desc *, int,
        compact_op, WPI_FONT, float, label_coords * );
extern void convert_to_radar( wcoord * );
extern void total_row( gda_index, float *, float * );
extern void draw_area( int, wcoord *, bla_attr *, bool );
extern void APIENTRY cgr_draw_marker(WPI_PRES ,marker_style ,WPI_PRECT );
extern void draw_xy_scale( scale_information *, labels_desc *, float, float,
        position_op, scale_xy, tick_type, three_d_scale, label_coords * );
extern void display_row_labels_3d( w3coord, label_coords * );
extern void display_col_labels_3d( w3coord, label_coords * );
extern void display_axis_labels_3d( w3coord, label_coords *, label_coords *,
        label_coords * );
extern void set_label_coords( label_coords *, const w3coord *, const w3coord * );
extern void draw_3d_compact_label( labels_desc *, w3coord *, compact_op,
        label_coords * );
extern void set_xz_left_corner( w3coord, float *, float * );
extern void display_axis_labels( void );
extern void display_row_labels( void );
extern float text_pos( text_align );
extern int get_num_groups( void );
extern bool is_legend_displayed( void );
extern void intersect_lines(wcoord *,wcoord *,wcoord *,wcoord *);
extern BOOL chart_bla_data_labels(chart_hld );
extern bool is_horz_legend(void);
extern int valid_group_idx(int );
extern char *stock_col_string(int ,bool *);
extern char *gantt_col_string(void);
extern float pie_other_value( void );

#endif
