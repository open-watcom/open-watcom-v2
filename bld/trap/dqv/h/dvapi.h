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



typedef unsigned long int       dword;
typedef unsigned long int       ulong;
typedef unsigned int            word;
typedef unsigned int            uint;
typedef unsigned char           byte;

#ifdef __WATCOMC__
 #include <stddef.h>
 #define _CDECL cdecl
#else
 #define _CDECL
#endif

/*--------General Functions-------*/
void _CDECL api_beginc(void);
void api_cancel(void);
void _CDECL api_endc(void);
void _CDECL api_exit(void);
void _CDECL api_freebit(uint);
uint _CDECL api_getbit(int (*__handler)());
char * _CDECL api_getmem(uint);
uint _CDECL api_init(void);
int  _CDECL api_isobj(ulong);
void _CDECL api_justify(int);
void _CDECL api_kmouse(int);
void _CDECL api_level(uint);
void _CDECL api_pause(void);
void _CDECL api_poke(char);
void _CDECL api_pushkey(uint);
void _CDECL api_putmem(char *);
void _CDECL api_setbit(uint);
char * _CDECL api_shadow(void);
void _CDECL api_sound(int,int);
void _CDECL api_update(char *,int);
/*--------Application Management Functions-------*/
void app_foreonly(dword ,int );
void _CDECL app_free(ulong);
void app_goback(dword );
void app_gofore(dword );
void app_hide(dword );
ulong _CDECL app_new(int (*)(),char *,int,char *,int,int,int);
int  _CDECL app_number(void);
void app_show(dword );
ulong _CDECL app_start(char *,int);
void app_suspend(dword );
/*--------Field Management Functions-------*/
void fld_altmode(dword ,int );
void fld_attr(dword ,int ,int );
void fld_char(dword ,int ,int );
void fld_clear(dword ,int );
void fld_cursor(dword ,int );
void fld_entry(dword ,word ,char *);
void fld_header(dword ,char *);
void fld_marker(dword ,int );
void fld_point(dword ,int ,int ,int );
void fld_reset(dword );
void fld_scroll(dword ,int ,int ,int );
void fld_swrite(dword ,word ,byte *);
void fld_type(dword ,int ,int );
void fld_write(dword ,word ,byte *,word );
/*--------Control Functions-------*/
void _CDECL dvfreeb(void);
void _CDECL dvlockb(void);
/*--------Keyboard Management Functions-------*/
void _CDECL key_addto(ulong,uint);
void _CDECL key_close(ulong);
void _CDECL key_erase(ulong);
void _CDECL key_free(ulong);
uint _CDECL key_getc(ulong);
ulong _CDECL key_me(void);
ulong _CDECL key_new(void);
ulong _CDECL key_of(ulong);
void _CDECL key_open(ulong,ulong);
void _CDECL key_read(ulong,char **,int *);
void _CDECL key_setesc(ulong,int (far *)());
int  _CDECL key_sizeof(ulong);
int  _CDECL key_status(ulong);
void _CDECL key_subfrom(ulong,uint);
void _CDECL key_write(ulong,char *,int,int);
/*--------Mailbox Management Functions-------*/
ulong _CDECL mal_addr(ulong);
void _CDECL mal_addto(ulong,char *,int,int);
void _CDECL mal_close(ulong);
void _CDECL mal_erase(ulong);
ulong _CDECL mal_find(char far *,int);
void _CDECL mal_free(ulong);
void _CDECL mal_lock(ulong);
ulong _CDECL mal_me(void);
void _CDECL mal_name(ulong,char far *,int);
ulong _CDECL mal_new(void);
ulong _CDECL mal_of(ulong);
void _CDECL mal_open(ulong);
int  _CDECL mal_read(ulong,char far * far*,int far *);
int  _CDECL mal_sizeof(ulong);
int  _CDECL mal_status(ulong);
void _CDECL mal_subfrom(ulong,char far *,int,int);
void _CDECL mal_unlock(ulong);
void _CDECL mal_write(ulong,char far *,int);
/*--------Objectq Management Functions-------*/
void _CDECL obq_close(void);
void _CDECL obq_erase(void);
void _CDECL obq_open(void);
ulong _CDECL obq_read(void);
int  _CDECL obq_sizeof(void);
int  _CDECL obq_status(void);
void _CDECL obq_subfrom(ulong);
/*--------Panel Management Functions-------*/
int  _CDECL pan_apply(ulong,ulong,char *,int,ulong *,ulong *);
void _CDECL pan_close(ulong);
void _CDECL pan_dir(ulong,char **,int *);
void _CDECL pan_free(ulong);
ulong _CDECL pan_new(void);
int  _CDECL pan_open(ulong,char *,int);
int  _CDECL pan_sizeof(ulong);
int  _CDECL pan_status(ulong);
/*--------Pointer Management Functions-------*/
void _CDECL ptr_addto(ulong,uint);
void _CDECL ptr_close(ulong);
void _CDECL ptr_erase(ulong);
void _CDECL ptr_free(ulong);
void _CDECL ptr_getscale(ulong,int *,int *);
ulong _CDECL ptr_new(void);
void _CDECL ptr_open(ulong,ulong);
void _CDECL ptr_read(ulong,void **,int *);
void _CDECL ptr_setscale(ulong,int,int);
int  _CDECL ptr_sizeof(ulong);
int  _CDECL ptr_status(ulong);
void _CDECL ptr_subfrom(ulong,uint);
void _CDECL ptr_write(ulong,int,int);
/*--------Query Functions-------*/
int  qry_atread(dword );
int  qry_attr(dword );
int  qry_color(dword ,int ,int );
int  qry_ctrl(dword );
void qry_cursor(dword ,int *,int *);
int  qry_entry(dword ,word ,char *);
void qry_field(dword ,int ,char *,int);
int  qry_frame(dword );
int  qry_frattr(dword ,int );
void qry_header(dword ,char *);
int  qry_hidden(dword );
int  _CDECL qry_kmouse(void);
int  qry_leave(dword );
int  qry_logattr(dword );
void qry_lsize(dword ,int *,int *);
void qry_origin(dword ,int *,int *);
void qry_position(dword ,int *,int *);
void qry_size(dword ,int *,int *);
void qry_title(dword ,char *,int );
int  qry_type(dword ,int );
/*--------Timer Management Functions-------*/
void _CDECL tim_addto(ulong,ulong);
void _CDECL tim_close(ulong);
void _CDECL tim_erase(ulong);
void _CDECL tim_free(ulong);
ulong _CDECL tim_len(ulong);
ulong _CDECL tim_new(void);
void _CDECL tim_open(ulong);
ulong _CDECL tim_read(ulong);
ulong _CDECL tim_sizeof(ulong);
int  _CDECL tim_status(ulong);
void _CDECL tim_write(ulong,ulong);
/*--------Task Management Functions-------*/
void _CDECL tsk_free(ulong);
ulong _CDECL tsk_me(void);
ulong _CDECL tsk_new(int (*)(),char *,int,char *,int,int,int);
void _CDECL tsk_pgmint(ulong,int (*)(),int, char *);
void _CDECL tsk_post(ulong);
void _CDECL tsk_start(ulong);
void _CDECL tsk_stop(ulong);
/*--------Window Management Functions-------*/
void _CDECL win_addto(ulong,char *,int,char *,int);
void win_adopt(dword );
void win_allow(dword ,int );
void _CDECL win_async(ulong,int (*)());
void win_atread(dword ,int );
void win_attach(dword ,int );
void win_attr(dword ,int );
void win_blanks(dword ,int );
void win_bottom(dword );
int _CDECL win_buffer(ulong,char **,int *);
void win_cancel(dword ,int );
void win_color(dword ,int ,int ,int );
void win_ctrl(dword ,int );
void _CDECL win_cursor(ulong,int,int);
void win_dflt(dword );
void win_disallow(dword ,int );
int  _CDECL win_disperor(ulong,char *,int,int,int,int,int);
int  _CDECL win_eof(ulong);
void _CDECL win_erase(ulong);
void win_frame(dword ,int );
void win_frattr(dword ,int ,int );
void _CDECL win_free(ulong);
void _CDECL win_hcur(ulong);
void win_hide(dword );
void win_leave(dword ,int );
int  _CDECL win_len(ulong);
ulong _CDECL win_locate(ulong,int,int);
void win_logattr(dword ,int );
void win_lsize(dword ,word ,word );
void win_maxsize(dword ,word ,word );
ulong _CDECL win_me(void);
void win_minsize(dword ,word ,word );
void win_move(dword ,word ,word );
ulong _CDECL win_new(char *,int,int,int);
void win_notify(dword ,int );
void _CDECL win_nread(ulong,int,char **,int *);
void _CDECL win_open(ulong,int);
void win_origin(dword ,word ,word );
void win_orphan(dword );
void win_paste(dword );
void win_point(dword );
void _CDECL win_poswin(ulong,ulong,int,int,int,int,int);
void _CDECL win_printf(ulong,char *,...);
void _CDECL win_putc(ulong,int,int);
void _CDECL win_read(ulong,char **,int *);
void _CDECL win_redraw(ulong);
void _CDECL win_reorder(ulong,...);
void win_repattr(dword ,int ,int );
void win_repchar(dword ,int ,int );
void win_resize(dword ,word ,word );
void win_scroll(dword ,int ,int ,int ,int ,int ,int );
int  _CDECL win_sizeof(ulong);
ulong _CDECL win_stream(ulong,void *);
void _CDECL win_subfrom(ulong,char *,int);
void win_swrite(dword ,byte *);
void win_title(dword ,byte *,int );
void win_top(dword );
void win_topsys(dword );
void win_unhide(dword );
ulong _CDECL win_write(ulong,char *,int);

/********
        The following constants are for use as parameters to the
        win_allow and win_disallow functions.
********/

#define ALW_HMOVE       0x00    /* horizontal movement of window */
#define ALW_VMOVE       0x01    /* vertical movement of window */
#define ALW_HSIZE       0x02    /* horizontal resize of window */
#define ALW_VSIZE       0x03    /* vertical resize of window */
#define ALW_HSCROLL     0x04    /* horizontal scroll of window */
#define ALW_VSCROLL     0x05    /* vertical scroll of window */
#define ALW_CLOSE       0x06    /* closing of application */
#define ALW_HIDE        0x07    /* hiding of application */
#define ALW_FREEZE      0x08    /* freezing of application */
#define ALW_COPY        0x0E    /* copying information from window */
#define ALW_DVMENU      0x10    /* access to DESQview menu */
#define ALW_SWITCH      0x11    /* access to Switch Windows menu */
#define ALW_OPEN        0x12    /* access to Open Window menu */
#define ALW_QUIT        0x13    /* quitting of DESQview */

/********
        The following constants are for use as parameters to the
        win_notify and win_cancel functions.
********/

#define NTF_HMOVE       0x40    /* horizontal movement of window */
#define NTF_VMOVE       0x41    /* vertical movement of window */
#define NTF_HSIZE       0x42    /* horizontal resize of window */
#define NTF_VSIZE       0x43    /* vertical resize of window */
#define NTF_HSCROLL     0x44    /* horizontal scroll of window */
#define NTF_VSCROLL     0x45    /* vertical scroll of window */
#define NTF_CLOSE       0x46    /* closing of application */
#define NTF_HIDE        0x47    /* hiding of application */
#define NTF_HELP        0x48    /* help is requested */
#define NTF_POINTER     0x49    /* pointer message is sent to application */
#define NTF_FORE        0x4A    /* application is made foreground */
#define NTF_BACK        0x4B    /* application is made background */
#define NTF_VIDEO       0x4C    /* video mode changes */
#define NTF_CUT         0x4D    /* Cut is requested */
#define NTF_COPY        0x4E    /* Copy is requested */
#define NTF_PASTE       0x4F    /* Paste is requested */
#define NTF_DVKEY       0x50    /* DESQview menu is requested */
#define NTF_DVDONE      0x51    /* DESQview request is ended */
#define NTF_DEFAULT     0xAE    /* this is default notify window */

/********
        Modes passed to the win_poswin function.
********/

#define PSW_SCREEN      0x00    /* offset from current position */
#define PSW_CENTER      0x01    /* center window then offset */
#define PSW_LEFT        0x02    /* left justify window then offset */
#define PSW_RIGHT       0x03    /* right justify window then offset */

/********
        Keyboard flag masks passed to key_addto and key_subfrom
********/

#define KBF_FIELD       0x01    /* field mode (vs. keystroke mode) */
#define KBF_CURSOR      0x02    /* hardware cursor enabled */
#define KBF_INSERT      0x04    /* insert mode */
#define KBF_ASYNC       0x08    /* async. menu mode */
#define KBF_ALLESC      0x10    /* all keys go to SETESC routine */

/********
        Pointer flag masks passed to ptr_addto and ptr_subfrom
********/

#define PTF_CLICKS      0x01    /* only report clicks */
#define PTF_UPDOWN      0x04    /* give press and release messages */
#define PTF_RELSCR      0x08    /* coords are screen relative */
#define PTF_MULTI       0x10    /* give multi-click messages */
#define PTF_BACK        0x20    /* gives messages even in background */
#define PTF_NOTTOP      0x40    /* gives messages even if not topmost */
#define PTF_HIDE        0x80    /* hide pointer when in window area */

/********
        Scroll directions of win_scroll and fld_scroll
********/

#define SCR_UP          0       /* scroll up */
#define SCR_DOWN        1       /* scroll down */
#define SCR_LEFT        2       /* scroll left */
#define SCR_RIGHT       3       /* scroll right */

/********
        Field type values for fld_type and qry_type.
********/

#define FLT_INACTIVE    0x00    /* field is inactive */
#define FLT_OUTPUT      0x60    /* output field */
#define FLT_INPUT       0x80    /* user input field */
#define FLT_DESELECT    0xC0    /* deselected field */
#define FLT_SELECT      0xC2    /* selected field */


/********
        Structure definition for Field Table Headers
********/

typedef struct field_header {
        byte    fh_size;        /* # of fields */
        byte    fh_format;      /* format */
        byte    fh_curin;       /* current input field */
        byte    fh_cursel;      /* current select field */
        byte    fh_pntattr;     /* pointed-at attribute */
        byte    fh_selattr;     /* selected attribute */
        } FHEAD, *FHEADPTR;

/********
        Bit assignments for fh_format field of Field Table Headers
********/

#define FTH_KEYSELECT   0x40    /* keystroke selection */
#define FTH_1BUTTON     0x20    /* button 1 mode */
#define FTH_2BUTTON     0x10    /* button 2 mode */
#define FTH_SELFORMAT   0x08    /* select field format */
#define FTH_AUTORESET   0x04    /* auto reset */
#define FTH_MASKDATA    0x03    /* returned data format mask */
#define   FTH_MODIFIED  0x03    /* return modified data with headers */
#define   FTH_HEADERS   0x02    /* return data with headers */
#define   FTH_DATAONLY  0x01    /* return just data */
#define   FTH_NODATA    0x00    /* return no data */

/********
        Structure definition for Field Table Entries
********/

typedef struct field_entry {
        byte    fe_srow;        /* starting logical row */
        byte    fe_scol;        /* starting logical column */
        byte    fe_erow;        /* ending logical row */
        byte    fe_ecol;        /* ending logical column */
        byte    fe_type;        /* field type/status */
        byte    fe_key1;        /* select field - first select key */
/* also called  fe_inmode;         input field - control flags */
        byte    fe_norattr;     /* select field - normal attribute */
        byte    fe_key2;        /* select field - second select key */
        } FENTRY, *FENTRYPTR;

#define fe_inmode fe_key1       /* alias for fe_key1 */

/********
        Bit assignments for fe_type field of Field Table Entries
********/

#define FTE_TYPEMASK    0xC0    /* field type mask */
#define   FTE_SELECT    0xC0    /* select field */
#define   FTE_INPUT     0x80    /* input field */
#define   FTE_OUTPUT    0x40    /* output field */
#define   FTE_INACTIVE  0x00    /* inactive field */
#define FTE_PROGOUT     0x20    /* program output field */
#define FTE_CHOSEN      0x02    /* selected flag */
#define FTE_MODIFIED    0x01    /* modified flag */

/********
        Bit assignments for fe_inmode field of Field Table Entries
********/

#define FTE_ENTER       0x80    /* auto enter */
#define FTE_SKIP        0x40    /* auto skip */
#define FTE_RIGHTJUST   0x20    /* right justified */
#define FTE_UPPERCASE   0x10    /* uppercase */
#define FTE_CLEARDFLT   0x08    /* clear default */
#define FTE_VALIDATE    0x04    /* validate */

/********
        Macro to aid in construction of Field Tables
********/

#define ftab(size,format,curin,cursel,pntattr,selattr) 0x1B,0,\
                (size*8+9)%256,(size*8+9)/256,0xE5,0x18,0xFF,\
                size,format,curin,cursel,pntattr,selattr

