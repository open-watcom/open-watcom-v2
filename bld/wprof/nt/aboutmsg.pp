



























 


 


 







typedef unsigned short wchar_t;








 typedef unsigned size_t;



 
  
 


extern void *memchr( const void *__s, int __c, size_t __n );
extern int   memcmp( const void *__s1, const void *__s2, size_t __n );
extern void *memcpy( void *__s1, const void *__s2, size_t __n );
extern void *memmove( void *__s1, const void *__s2, size_t __n );
extern void *memset( void *__s, int __c, size_t __n );
extern char *strcat( char *__s1, const char *__s2 );
extern char *strchr( const char *__s, int __c );
extern int strcmp( const char *__s1, const char *__s2 );
extern int strcoll( const char *__s1, const char *__s2 );
extern size_t strxfrm( char *__s1, const char *__s2, size_t __n );
extern char *strcpy( char *__s1, const char *__s2 );
extern size_t strcspn( const char *__s1, const char *__s2 );
extern char *strerror( int __errnum );
extern size_t strlen( const char *__s );
extern char *strncat( char *__s1, const char *__s2, size_t __n );
extern int strncmp( const char *__s1, const char *__s2, size_t __n );
extern char *strncpy( char *__s1, const char *__s2, size_t __n );
extern char *strpbrk( const char *__s1, const char *__s2 );
extern char *strrchr( const char *__s, int __c );
extern size_t strspn( const char *__s1, const char *__s2 );
extern char *strstr( const char *__s1, const char *__s2 );
extern char *strtok( char *__s1, const char *__s2 );






extern void __far *_fmemccpy(void __far *__s1,const void __far *__s2,int __c,size_t __n);


extern void __far *_fmemchr( const void __far *__s, int __c, size_t __n );


extern void __far *_fmemcpy( void __far *__s1, const void __far *__s2, size_t __n );


extern void __far *_fmemmove( void __far *__s1, const void __far *__s2, size_t __n );


extern void __far *_fmemset( void __far *__s, int __c, size_t __n );


extern int  _fmemcmp( const void __far *__s1, const void __far *__s2, size_t __n );


extern int  _fmemicmp( const void __far *__s1, const void __far *__s2, size_t __n );


extern char __far *_fstrcat( char __far *__s1, const char __far *__s2 );


extern char __far *_fstrchr( const char __far *__s, int __c );


extern int  _fstrcmp( const char __far *__s1, const char __far *__s2 );


extern char __far *_fstrcpy( char __far *__s1, const char __far *__s2 );


extern size_t _fstrcspn( const char __far *__s1, const char __far *__s2 );


extern char __far *_fstrdup( const char __far *__string );


extern int  _fstricmp( const char __far *__s1, const char __far *__s2 );


extern char __far *_fstrncat( char __far *__s1, const char __far *__s2, size_t __n );


extern size_t _fstrlen( const char __far *__s );


extern char __far *_fstrlwr( char __far *__string );


extern int  _fstrncmp( const char __far *__s1, const char __far *__s2, size_t __n );


extern char __far *_fstrncpy( char __far *__s1, const char __far *__s2, size_t __n );


extern int  _fstrnicmp( const char __far *__s1, const char __far *__s2, size_t __n );


extern char __far *_fstrnset( char __far *__string, int __c, size_t __len );


extern char __far *_fstrpbrk( const char __far *__s1, const char __far *__s2 );


extern char __far *_fstrrchr( const char __far *__s, int __c );


extern char __far *_fstrrev( char __far *__string );


extern char __far *_fstrset( char __far *__string, int __c );


extern size_t _fstrspn( const char __far *__s1, const char __far *__s2 );


extern char __far *_fstrstr( const char __far *__s1, const char __far *__s2 );


extern char __far *_fstrtok( char __far *__s1, const char __far *__s2 );


extern char __far *_fstrupr( char __far *__string );

extern void movedata( unsigned __srcseg, unsigned __srcoff,
	       unsigned __tgtseg, unsigned __tgtoff, unsigned __len );
extern void *memccpy( void *__s1, const void *__s2, int __c, size_t __n );
extern int  memicmp( const void *__s1, const void *__s2, size_t __n );
extern int  _memicmp( const void *__s1, const void *__s2, size_t __n );
extern int   strcmpi( const char *__s1, const char *__s2 );
extern char *strdup( const char *__string );
extern char *_strdup( const char *__string );
extern int   stricmp( const char *__s1, const char *__s2 );
extern int   _stricmp( const char *__s1, const char *__s2 );
extern char *strlwr( char *__string );
extern char *_strlwr( char *__string );
extern int   strnicmp( const char *__s1, const char *__s2, size_t __n );
extern int   _strnicmp( const char *__s1, const char *__s2, size_t __n );
extern char *strnset( char *__string, int __c, size_t __len );
extern char *strrev( char *__string );
extern char *_strrev( char *__string );
extern char *strset( char *__string, int __c );
extern char *strupr( char *__string );
extern char *_strupr( char *__string );






 #pragma intrinsic(movedata,_fmemchr,_fmemcmp,_fmemcpy,_fmemset)
 #pragma intrinsic(_fstrcat,_fstrcmp,_fstrcpy,_fstrlen)

 #pragma intrinsic(memchr,memcmp,memcpy,strcat,strcpy,strlen,strchr)
 
















































 
  typedef int	ptrdiff_t;
 







 extern int *__threadid(void);	 
 













 
#pragma pack(__push,1);




















    


typedef struct	{
	int	quot;
	int	rem;
} div_t;

typedef struct	{
	long	quot;
	long	rem;
} ldiv_t;

extern void	 abort( void );
extern int	 abs( int __j );
extern int	 atexit( register void ( *__func )( void ) );
extern double	 atof( const char *__nptr );
extern int	 atoi( const char *__nptr );
extern long int  atol( const char *__nptr );
extern void	*bsearch( const void *__key, const void *__base,
		    size_t __nmemb, size_t __size,
		    int ( *__compar )( const void *__pkey, const void *__pbase ) );
extern void	 break_on( void );
extern void	 break_off( void );
extern void	*calloc( size_t __n, size_t __size );
extern div_t	 div( int __numer, int __denom );
extern void	 exit( int __status );
extern void	 free( void *__ptr );
extern char	*getenv( const char *__name );
extern long int  labs( long int __j );
extern ldiv_t	 ldiv( long int __numer, long int __denom );
extern void	*malloc( size_t __size );
extern int	 mblen( const char *__s, size_t __n );
extern size_t	 mbstowcs( wchar_t *__pwcs, const char *__s, size_t __n );
extern int	 mbtowc( wchar_t *__pwc, const char *__s, size_t __n );
extern size_t	 wcstombs( char *__s, const wchar_t *__pwcs, size_t __n );
extern int	 wctomb( char *__s, wchar_t __wchar );
extern void	 qsort( void *__base, size_t __nmemb, size_t __size,
		    int (*__compar)( const void *, const void * ) );
extern int	 rand( void );
extern void	*realloc( void *__ptr, size_t __size );
extern void	 srand( unsigned int __seed );
extern double	 strtod( const char *__nptr, char **__endptr );
extern long int  strtol( const char *__nptr, char **__endptr, int __base );
extern unsigned long	 strtoul( const char *__nptr, char **__endptr, int __base );
extern int	 system( const char *__string );


 #pragma intrinsic(abs,div,labs)
 
  #pragma intrinsic(ldiv)
 









extern void	 _exit( int __status );
extern char	*ecvt( double __val, int __ndig, int *__dec, int *__sign );
extern char	*_ecvt( double __val, int __ndig, int *__dec, int *__sign );
extern char	*fcvt( double __val, int __ndig, int *__dec, int *__sign );
extern char	*_fcvt( double __val, int __ndig, int *__dec, int *__sign );
extern char	*_fullpath( char *__buf, const char *__path, size_t __size );
extern char	*gcvt( double __val, int __ndig, char *__buf );
extern char	*_gcvt( double __val, int __ndig, char *__buf );
extern char	*itoa( int __value, char *__buf, int __radix );
extern char	*_itoa( int __value, char *__buf, int __radix );
extern unsigned long _lrotl( unsigned long __value, unsigned int __shift );
extern unsigned long _lrotr( unsigned long __value, unsigned int __shift );
extern char	*ltoa( long int __value, char *__buf, int __radix );
extern char	*_ltoa( long int __value, char *__buf, int __radix );
extern void	 _makepath( char *__path, const char *__drive,
		    const char *__dir, const char *__fname,
		    const char *__ext );
extern unsigned int _rotl( unsigned int __value, unsigned int __shift );
extern unsigned int _rotr( unsigned int __value, unsigned int __shift );
extern int	 putenv( const char *__string );
extern void	 _searchenv( const char *__name, const char *__env_var,
		    char *__buf );
extern void	 _splitpath2( const char *__inp, char *__outp,
		    char **__drive, char **__dir, char **__fn, char **__ext );
extern void	 _splitpath( const char *__path, char *__drive,
		    char *__dir, char *__fname, char *__ext );
extern void	 swab( char *__src, char *__dest, int __num );
extern char	*ultoa( unsigned long int __value, char *__buf, int __radix );
extern char	*utoa( unsigned int __value, char *__buf, int __radix );

extern char	*_utoa( unsigned int __value, char *__buf, int __radix );


 #pragma intrinsic(_rotl,_rotr)
 
  #pragma intrinsic(_lrotl,_lrotr)
 


































extern char **__near environ;	 


    

extern int  (*__get_errno_ptr()) ;

extern	int		 (*__get_doserrno_ptr()) ;	 
extern	unsigned __near _psp;		 


extern	unsigned char __near _osmode;	 
extern	int	__near	_fmode; 	 
extern char *		sys_errlist[];	 
extern int	__near	sys_nerr;	 
extern unsigned __near	__minreal;	 
extern unsigned long __near __win_alloc_flags;  
extern unsigned long __near __win_realloc_flags; 
extern unsigned char __near _osmajor;	 
extern unsigned char __near _osminor;	 
extern unsigned __near	    _amblksiz;	 

extern void __near *__brk(unsigned __new_brk_value);
extern void __near *sbrk(int __increment);
 typedef void (*onexit_t)();
extern onexit_t onexit(onexit_t __func);


 
#pragma pack(__pop);


























    













extern int  (*__get_errno_ptr()) ;





















































    
        
    



    typedef unsigned        uint;


typedef unsigned char	uint_8;
typedef unsigned short	uint_16;
typedef unsigned long	uint_32;
typedef unsigned char	unsigned_8;
typedef unsigned short	unsigned_16;
typedef unsigned long	unsigned_32;

typedef signed char	int_8;
typedef signed short	int_16;
typedef signed long	int_32;
typedef signed char	signed_8;
typedef signed short	signed_16;
typedef signed long	signed_32;

typedef struct {
    union {
	unsigned_32	_32[2];
	unsigned_16	_16[4];
	unsigned_8	 _8[8];
	struct {
	    unsigned		: 16;
	    unsigned		: 16;
	    unsigned		: 16;
	    unsigned		: 15;
	    unsigned	v	: 1;
	}	sign;
    } u;
} unsigned_64;
typedef unsigned_64	signed_64;




    
    

































 
    
    typedef unsigned char bool;


    


    






typedef int             file_handle;



typedef void *          pointer;


typedef enum { B_FALSE = (0==1), B_TRUE = (0==0) } bint;

  













 

 
 































 
 



 
    
























































































 





typedef int gui_ord;





typedef enum {
    GUI_NO_EVENT,       
    GUI_INIT_WINDOW,
    GUI_INIT_DIALOG,
    GUI_NOT_ACTIVE,
    GUI_NOW_ACTIVE,
    GUI_MOVE,
    GUI_RESIZE,
    GUI_ICONIFIED,
    GUI_FONT_CHANGED,
    GUI_PAINT,            
    GUI_KEYDOWN,          
    GUI_KEYUP,
    GUI_KEY_CONTROL,
    GUI_SCROLL_UP,        
    GUI_SCROLL_PAGE_UP,
    GUI_SCROLL_TOP,
    GUI_SCROLL_DOWN,
    GUI_SCROLL_PAGE_DOWN,
    GUI_SCROLL_BOTTOM,
    GUI_SCROLL_VERTICAL,
    GUI_SCROLL_LEFT,
    GUI_SCROLL_PAGE_LEFT,
    GUI_SCROLL_FULL_LEFT,
    GUI_SCROLL_RIGHT,
    GUI_SCROLL_PAGE_RIGHT,
    GUI_SCROLL_FULL_RIGHT,
    GUI_SCROLL_HORIZONTAL,
    GUI_CLICKED,                 
    GUI_CONTROL_DCLICKED,        
    GUI_LBUTTONDOWN,       
    GUI_LBUTTONUP,
    GUI_LBUTTONDBLCLK,
    GUI_RBUTTONDOWN,
    GUI_RBUTTONUP,
    GUI_RBUTTONDBLCLK,
    GUI_MOUSEMOVE,
    GUI_TOOLBAR_DESTROYED,       
    GUI_TOOLBAR_FIXED,
    GUI_TOOLBAR_FLOATING,
    GUI_CLOSE,
    GUI_DESTROY,
    GUI_HSCROLL_NOTIFY,
    GUI_VSCROLL_NOTIFY,
    GUI_CONTROL_NOT_ACTIVE,
    GUI_CONTROL_CLICKED,         
    GUI_INITMENUPOPUP,           
    GUI_CONTROL_RCLICKED,        
    GUI_DIALOG_ESCAPE,           
    GUI_STATUS_CLEARED,          
    GUI_QUERYENDSESSION,         
                                


    GUI_ENDSESSION,              
    GUI_KEYTOITEM,
    GUI_ACTIVATEAPP,
    GUI_CONTEXTHELP		 
} gui_event;

typedef struct gui_coord {
    gui_ord x;
    gui_ord y;
} gui_coord;

typedef struct gui_point {
    int x;
    int y;
} gui_point;

typedef struct gui_window gui_window;

typedef enum {
    GUI_PUSH_BUTTON,
    GUI_DEFPUSH_BUTTON,
    GUI_RADIO_BUTTON,      
    GUI_CHECK_BOX,
    GUI_COMBOBOX,
    GUI_EDIT,
    GUI_LISTBOX,
    GUI_SCROLLBAR,
    GUI_STATIC,
    GUI_GROUPBOX,
    GUI_EDIT_COMBOBOX,
    GUI_EDIT_MLE,
    GUI_NUM_CONTROL_CLASSES,
    GUI_FIRST_CONTROL_CLASS = GUI_PUSH_BUTTON,
    GUI_LAST_CONTROL_CLASS = GUI_EDIT_MLE
} gui_control_class;

typedef enum {
    GUI_MENU_PLAIN,
    GUI_MENU_STANDOUT,
    GUI_MENU_GRAYED,
    GUI_MENU_ACTIVE,
    GUI_MENU_ACTIVE_STANDOUT,
    GUI_BACKGROUND,
    GUI_MENU_FRAME,
    GUI_TITLE_INACTIVE,
    GUI_FRAME_ACTIVE,
    GUI_FRAME_INACTIVE,
    GUI_ICON,
    GUI_MENU_GRAYED_ACTIVE,
    GUI_FRAME_RESIZE,
    GUI_FIRST_ATTR = GUI_MENU_PLAIN,
    GUI_LAST_ATTR  = GUI_FRAME_RESIZE,
    GUI_FIRST_UNUSED = GUI_LAST_ATTR + 1,
    GUI_NUM_ATTRS = GUI_LAST_ATTR + 1
} gui_attr;

typedef enum {
    GUI_DLG_NORMAL,
    GUI_DLG_FRAME,
    GUI_DLG_SHADOW,
    GUI_DLG_SCROLL_ICON,
    GUI_DLG_SCROLL_BAR,
    GUI_DLG_BUTTON_PLAIN,
    GUI_DLG_BUTTON_STANDOUT,
    GUI_DLG_BUTTON_ACTIVE,
    GUI_DLG_BUTTON_ACTIVE_STANDOUT,
    GUI_DLG_NUM_ATTRS,
} gui_dlg_attr;

typedef enum {
    GUI_ENABLED         = 0x00,
    GUI_MENU_CHECKED    = 0x01,
    GUI_GRAYED          = 0x02,
    GUI_SEPARATOR       = 0x04,
    GUI_IGNORE          = 0x08,  
    GUI_MDIWINDOW       = 0x10,
    GUI_UTIL_1          = 0x40,  
    GUI_UTIL_2          = 0x80,
} gui_menu_styles ;

typedef unsigned long gui_help_instance;

typedef enum {
    GUI_HELP_CONTENTS
,   GUI_HELP_ON_HELP
,   GUI_HELP_SEARCH
,   GUI_HELP_CONTEXT
,   GUI_HELP_KEY
} gui_help_actions;

typedef long gui_bitmap;

typedef struct gui_toolbar_struct {
    char                *label;
    gui_bitmap          bitmap;
    int                 id;
    char                *hinttext;
} gui_toolbar_struct;

typedef struct gui_menu_struct {
    char                        *label;
    int                         id;
    gui_menu_styles             style;
    char                        *hinttext;
    int                         num_child_menus;
    struct gui_menu_struct      *child;
} gui_menu_struct;

typedef enum {
        GUI_BLACK,
        GUI_BLUE,
        GUI_GREEN,
        GUI_CYAN,
        GUI_RED,
        GUI_MAGENTA,
        GUI_BROWN,
        GUI_WHITE,
        GUI_GREY,
        GUI_BRIGHT_BLUE,
        GUI_BRIGHT_GREEN,
        GUI_BRIGHT_CYAN,
        GUI_BRIGHT_RED,
        GUI_BRIGHT_MAGENTA,
        GUI_BRIGHT_YELLOW,
        GUI_BRIGHT_WHITE,
        GUI_FIRST_COLOUR = GUI_BLACK,
        GUI_LAST_COLOUR = GUI_BRIGHT_WHITE,
        GUI_NUM_COLOURS = GUI_LAST_COLOUR + 1
} gui_colour;





typedef struct gui_colour_set {
    gui_colour fore;
    gui_colour back;
} gui_colour_set;

typedef unsigned long gui_rgb;







typedef struct gui_rect {
    gui_ord     x;
    gui_ord     y;
    gui_ord     width;
    gui_ord     height;
} gui_rect;

typedef enum gui_scroll_styles {
    GUI_NOSCROLL        = 0x00,
    GUI_HSCROLL         = 0x01,
    GUI_VSCROLL         = 0x02,
    GUI_HDRAG           = 0x04,  
    GUI_VDRAG           = 0x08,  
    GUI_HTRACK          = 0x10,  
    GUI_VTRACK          = 0x20,  
    GUI_HCOLS           = 0x40,  
    GUI_VROWS           = 0x80,  
    GUI_HSCROLL_ALL     = GUI_HSCROLL | GUI_HDRAG | GUI_HTRACK | GUI_HCOLS,
    GUI_VSCROLL_ALL     = GUI_VSCROLL | GUI_VDRAG | GUI_VTRACK | GUI_VROWS,
    GUI_SCROLL_BOTH     = GUI_HSCROLL | GUI_VSCROLL | GUI_HCOLS | GUI_VROWS,
    GUI_SCROLL_BOTH_ALL = GUI_HSCROLL_ALL | GUI_VSCROLL_ALL
} gui_scroll_styles;

typedef enum gui_create_styles {
    GUI_NONE            = 0x0000,
    GUI_HSCROLL_EVENTS  = 0x0001,
    GUI_VSCROLL_EVENTS  = 0x0002,
    GUI_CURSOR          = 0x0004,
    GUI_RESIZEABLE      = 0x0008,
    GUI_MAXIMIZE        = 0x0010,
    GUI_MINIMIZE        = 0x0020,
    GUI_CLOSEABLE       = 0x0040,
    GUI_SYSTEM_MENU     = 0x0080,
    GUI_VISIBLE         = 0x0100,
    GUI_DIALOG_LOOK     = 0x0200,
    GUI_INIT_INVISIBLE  = 0x0400,
    GUI_CHANGEABLE_FONT = 0x0800,
    GUI_POPUP           = 0x1000,
    GUI_INIT_MAXIMIZED  = 0x2000,
    GUI_INIT_MINIMIZED  = 0x4000,
    GUI_NOFRAME         = 0x8000,
    GUI_SCROLL_EVENTS   = GUI_HSCROLL_EVENTS | GUI_VSCROLL_EVENTS,
    GUI_GADGETS         = GUI_CURSOR | GUI_RESIZEABLE | GUI_MINIMIZE |
                          GUI_MAXIMIZE | GUI_CLOSEABLE | GUI_SYSTEM_MENU,
    GUI_ALL             = GUI_GADGETS | GUI_VISIBLE | GUI_CHANGEABLE_FONT
} gui_create_styles;

typedef enum gui_control_styles {
    GUI_NOSTYLE                         = 0x00,
    GUI_CHECKED                         = 0x01,
    GUI_TAB_GROUP                       = 0x02,
    GUI_AUTOMATIC                       = 0x04,
    GUI_GROUP                           = 0x08,
    GUI_FOCUS                           = 0x10,
    GUI_CONTROL_INIT_INVISIBLE          = 0x20,
    GUI_CONTROL_LEFTNOWORDWRAP          = 0x40,
    GUI_CONTROL_NOPREFIX                = 0x80,
    GUI_CONTROL_CENTRE                  = 0x100,
    GUI_CONTROL_NOINTEGRALHEIGHT        = 0x200,
    GUI_CONTROL_SORTED                  = 0x400,
    GUI_CONTROL_MULTILINE               = 0x800,
    GUI_CONTROL_WANTRETURN              = 0x1000,
    GUI_EDIT_INVISIBLE                  = 0x2000,
    GUI_CONTROL_3STATE                  = 0x4000,
    GUI_CONTROL_WANTKEYINPUT            = 0x8000,
} gui_control_styles;

typedef enum gui_line_styles {
    GUI_PEN_SOLID       = 0x00,
    GUI_PEN_DASH        = 0x01,
    GUI_PEN_DOT         = 0x02,
    GUI_PEN_DASHDOT     = 0x04,
    GUI_PEN_DASHDOTDOT  = 0x08,
} gui_line_styles;

typedef enum gui_bar_styles {
    GUI_BAR_SIMPLE      = 0x01,
    GUI_BAR_SHADOW      = 0x02,
    GUI_BAR_3DRECT1     = 0x04,
    GUI_BAR_3DRECT2     = 0x08,
    GUI_FULL_BAR        = 0x10,
} gui_bar_styles;

typedef struct gui_resource {
    gui_bitmap  res;
    char        *chars;  
} gui_resource;

typedef struct gui_control_info {
   gui_control_class    control_class;
    char                *text;
    gui_rect            rect;
    gui_window          *parent;
    gui_scroll_styles   scroll;
    gui_control_styles  style;
    unsigned            id;
} gui_control_info;

typedef bool (GUICALLBACK)( gui_window *, gui_event, void * );
typedef void (ENUMCALLBACK)( gui_window *, void *param );
typedef void (CONTRENUMCALLBACK)( gui_window *parent, unsigned id, void *param );
typedef void (PICKCALLBACK)( gui_window *, int id );
typedef void (PICKDLGOPEN)( char *name, int rows, int cols,
                             struct gui_control_info *ctl, int num_controls,
                             GUICALLBACK *rtn, void *extra );

typedef struct gui_create_info {
    char                *text;
    gui_rect            rect;
    gui_scroll_styles   scroll;
    gui_create_styles   style;
    gui_window          *parent;
    int                 num_menus;
    gui_menu_struct     *menu;
    int                 num_attrs;
    gui_colour_set      *colours;       
    GUICALLBACK         *call_back;
    void                *extra;
    gui_resource        *icon;
    int			resource_menu;
} gui_create_info;

typedef enum {
    GUI_ABORT_RETRY_IGNORE      = 0x001,
    GUI_EXCLAMATION             = 0x002,
    GUI_INFORMATION             = 0x004,
    GUI_QUESTION                = 0x008,
    GUI_STOP                    = 0x010,
    GUI_OK                      = 0x020,
    GUI_OK_CANCEL               = 0x040,
    GUI_RETRY_CANCEL            = 0x080,
    GUI_YES_NO                  = 0x100,
    GUI_YES_NO_CANCEL           = 0x200,
    GUI_SYSTEMMODAL             = 0x400
} gui_message_type;

typedef enum {
    GUI_RET_ABORT,
    GUI_RET_CANCEL,
    GUI_RET_IGNORE,
    GUI_RET_NO,
    GUI_RET_OK,
    GUI_RET_RETRY,
    GUI_RET_YES
} gui_message_return;

typedef struct gui_text_metrics {
    gui_coord avg;
    gui_coord  max ;
} gui_text_metrics;

typedef struct gui_system_metrics {
    gui_coord   resize_border;
    bool        colour;
    bool        mouse;
    gui_coord   top_left;
    gui_coord   bottom_right;
    gui_coord   scrollbar_size;
    gui_coord   dialog_top_left_size;
    gui_coord   dialog_bottom_right_size;
    gui_ord     caption_size;
} gui_system_metrics;

typedef enum {
    GUI_TRACK_NONE      = 0x00,
    GUI_TRACK_LEFT      = 0x01,
    GUI_TRACK_RIGHT     = 0x02,
    GUI_TRACK_BOTH      = GUI_TRACK_LEFT | GUI_TRACK_RIGHT
} gui_mouse_track;

typedef enum {
    GUI_KS_NONE       =    0x00,
    GUI_KS_ALT        =    0x01,
    GUI_KS_SHIFT      =    0x02,
    GUI_KS_CTRL       =    0x04
} gui_keystate;

typedef enum {
    GUI_ARROW_CURSOR,
    GUI_HOURGLASS_CURSOR,
    GUI_CROSS_CURSOR
} gui_mouse_cursor;

typedef enum {
    GUI_NO_CURSOR,
    GUI_NORMAL_CURSOR,
    GUI_INSERT_CURSOR,
} gui_char_cursor;

typedef enum {
    GUI_FRAME_TOP,
    GUI_FRAME_UL_CORNER,
    GUI_FRAME_LEFT,
    GUI_FRAME_LL_CORNER,
    GUI_FRAME_BOTTOM,
    GUI_FRAME_LR_CORNER,
    GUI_FRAME_RIGHT,
    GUI_FRAME_UR_CORNER,
    GUI_LR_VERT_BAR,
    GUI_LR_HORZ_BAR,
    GUI_LEFT_TITLE_MARK,
    GUI_RIGHT_TITLE_MARK,
    GUI_LEFT_GADGET_MARK,
    GUI_RIGHT_GADGET_MARK,
    GUI_TITLE_SPACE,
    GUI_CLOSER,
    GUI_MAXIMIZE_GADGET,
    GUI_MINIMIZE_GADGET,
    GUI_RESIZE_GADGET,
    GUI_HOR_SCROLL,
    GUI_VERT_SCROLL,
    GUI_LEFT_SCROLL_ARROW,
    GUI_RIGHT_SCROLL_ARROW,
    GUI_UP_SCROLL_ARROW,
    GUI_DOWN_SCROLL_ARROW,
    GUI_SCROLL_SLIDER,
    GUI_INACT_FRAME_TOP,
    GUI_INACT_FRAME_UL_CORNER,
    GUI_INACT_FRAME_LEFT,
    GUI_INACT_FRAME_LL_CORNER,
    GUI_INACT_FRAME_BOTTOM,
    GUI_INACT_FRAME_LR_CORNER,
    GUI_INACT_FRAME_RIGHT,
    GUI_INACT_FRAME_UR_CORNER,
    GUI_INACT_LR_VERT_BAR,
    GUI_INACT_LR_HORZ_BAR,
    GUI_INACT_LEFT_TITLE_MARK,
    GUI_INACT_RIGHT_TITLE_MARK,
    GUI_INACT_LEFT_GADGET_MARK,
    GUI_INACT_RIGHT_GADGET_MARK,
    GUI_INACT_TITLE_SPACE,
    GUI_INACT_CLOSER,
    GUI_INACT_MAXIMIZE_GADGET,
    GUI_INACT_MINIMIZE_GADGET,
    GUI_INACT_RESIZE_GADGET,
    GUI_INACT_HOR_SCROLL,
    GUI_INACT_VERT_SCROLL,
    GUI_INACT_LEFT_SCROLL_ARROW,
    GUI_INACT_RIGHT_SCROLL_ARROW,
    GUI_INACT_UP_SCROLL_ARROW,
    GUI_INACT_DOWN_SCROLL_ARROW,
    GUI_INACT_SCROLL_SLIDER,
    GUI_DIAL_VERT_SCROLL,
    GUI_DIAL_UP_SCROLL_ARROW,
    GUI_DIAL_DOWN_SCROLL_ARROW,
    GUI_DIAL_SCROLL_SLIDER,
    GUI_NUM_DRAW_CHARS,
    GUI_INACTIVE_OFFSET = GUI_INACT_FRAME_TOP,
} gui_draw_char;

typedef enum {
    GUI_PLAIN           = 0x00,
    GUI_CHARMAP_MOUSE   = 0x01,
    GUI_INACT_GADGETS   = 0x02,
    GUI_INACT_SAME      = 0x04,
    GUI_CHARMAP_DLG     = 0x08,
    GUI_GMOUSE          = (GUI_CHARMAP_DLG|GUI_CHARMAP_MOUSE),
} gui_window_styles;









 
































typedef struct gui_row_num {
    gui_ord     start;
    int         num;
} gui_row_num;













extern void *GUIAlloc( unsigned );
extern void GUIFree( void * );
extern void *GUIRealloc( void *ptr, unsigned size );
extern void GUIMemOpen();
extern void GUIMemClose( void );
extern void GUIMemPrtUsage( void );



extern bool GUIWndInit( unsigned rate, gui_window_styles style );
extern void GUIGMouseOn( void );
extern void GUIGMouseOff( void );
extern void GUIMDIInit( void );
extern void GUIMDIInitMenuOnly( void );
extern bool GUI3DDialogInit( void );
extern void GUISetBetweenTitles( int between_titles );
extern void GUIGetRoundScale( gui_coord *scale );
extern void GUISetScale( gui_rect *screen );
extern void GUIGetScale( gui_rect *screen );
extern void GUIGetScreen( gui_rect *rect );
extern void GUISetDClickRate( unsigned rate );
extern void GUISetCharacter( gui_draw_char draw_char, char new_char );
extern char GUIGetCharacter( gui_draw_char draw_char );
extern bool GUIIsInit( void );
extern void GUISetF10Menus( bool setting );
extern void GUICleanup( void );



extern void GUISetBackgroundColour( gui_colour_set *colour );
extern void GUIGetDialogColours( gui_colour_set *colours );
extern void GUISetDialogColours( gui_colour_set *colours );
extern void GUISetWindowColours( gui_window *wnd, int num_colours, gui_colour_set *colours );
extern bool GUISetRGB( gui_colour colour, gui_rgb rgb );
extern bool GUIGetRGB( gui_colour colour, gui_rgb *rgb );
extern bool GUIGetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set );
extern bool GUISetWndColour( gui_window *wnd, gui_attr attr, gui_colour_set *colour_set );
extern bool GUIGetRGBFromUser( gui_rgb init_rgb, gui_rgb *new_rgb );
extern bool GUIGetColourFromUser( char *title, gui_colour *init, gui_colour *new_colour );



extern bool GUIInitHotSpots( int num_hot_spots, gui_resource *hot );
extern int  GUIGetNumHotSpots( void );
extern bool GUIGetHotSpotSize( int hot_spot, gui_coord *size );
extern void GUIDrawHotSpot( gui_window *wnd, int hot_spot, gui_ord row,
                            gui_ord indent, gui_attr attr );



extern gui_window *GUICreateWindow( gui_create_info *info );
extern int GUIGetNumWindowColours( gui_window *wnd );
extern gui_colour_set *GUIGetWindowColours( gui_window *wnd );
extern void GUIDestroyWnd( gui_window *wnd );
extern void GUIWndDirty( gui_window *wnd );
extern void GUIControlDirty( gui_window *wnd, unsigned id );
extern void GUIWndDirtyRow( gui_window *wnd, gui_ord row );
extern void GUIWndDirtyRect( gui_window *wnd, gui_rect *rect );
extern void GUIRefresh( void );
extern void GUIWndUpdate( gui_window *wnd );
extern bool GUISetBackgroundChar( gui_window *wnd, char background );

extern void GUIActivateNC( gui_window *wnd, bool activate );
extern void GUIBringToFront( gui_window *wnd );
extern gui_window *GUIGetFront( void );
extern gui_window *GUIGetParentWindow( gui_window *wnd );
extern bool GUIIsRootWindow( gui_window *wnd );
extern gui_window *GUIGetRootWindow( void );
extern gui_window *GUIGetNextWindow( gui_window *wnd );
extern gui_window *GUIGetFirstSibling( gui_window *wnd );
extern bool GUIIsValidWindow( gui_window *wnd );
extern bool GUISetFocus( gui_window *wnd, unsigned id );
extern bool GUIGetFocus( gui_window *wnd, unsigned *id );

extern bool GUIResizeWindow( gui_window *wnd, gui_rect *rect );
extern bool GUIIsMinimized( gui_window *wnd );
extern bool GUIIsMaximized( gui_window *wnd );
extern void GUIMaximizeWindow( gui_window *wnd );
extern void GUIMinimizeWindow( gui_window *wnd );
extern void GUIRestoreWindow( gui_window *wnd );
extern void GUIHideWindow( gui_window *wnd );
extern void GUIShowWindow( gui_window *wnd );
extern void GUIShowWindowNA( gui_window *wnd );
extern bool GUIIsWindowVisible( gui_window *wnd );
extern void GUISetRestoredSize( gui_window *wnd, gui_rect *rect );
extern bool GUIGetRestoredSize( gui_window *wnd, gui_rect *rect );
extern bool GUISetIcon( gui_window * wnd, gui_resource *res );
extern bool GUISetRedraw( gui_window *wnd, bool redraw );

extern bool GUICascadeWindows( void );
extern void GUIWantPartialRows( gui_window *wnd, bool want );
extern void GUISetCheckResizeAreaForChildren( gui_window *wnd, bool check );



extern bool GUIGetCursorPos( gui_window *wnd, gui_point *point );
extern bool GUISetCursorPos( gui_window *wnd, gui_point *point );
extern bool GUIGetCursorType( gui_window *wnd, gui_char_cursor *cursor );
extern bool GUISetCursorType( gui_window *wnd, gui_char_cursor cursor );

extern void *GUISetMouseCursor( gui_mouse_cursor type );
extern void GUIResetMouseCursor( void* old_cursor );



extern bool GUIFontsSupported( void );
extern bool GUIChangeFont( gui_window *wnd );
extern char *GUIGetFontInfo( gui_window *wnd );
extern bool GUISetFontInfo( gui_window *wnd, char *fontinfo );
extern bool GUISetSystemFont( gui_window *wnd, bool fixed );
extern char *GUIGetFontFromUser( char *fontinfo );



extern bool GUIFillRect( gui_window *wnd, gui_rect *rect, gui_attr attr );
extern bool GUIFillBar( gui_window *wnd, gui_rect *rect, gui_attr attr );
extern bool GUIDrawRect( gui_window *wnd, gui_rect *rect, gui_attr attr );
extern bool GUIDrawLine( gui_window *wnd, gui_point *start, gui_point *end,
                         gui_line_styles style, gui_ord thickness, gui_attr attr );
extern bool GUIFillRectRGB( gui_window *wnd, gui_rect *rect, gui_rgb rgb );
extern bool GUIDrawRectRGB( gui_window *wnd, gui_rect *rect, gui_rgb rgb );
extern bool GUIDrawLineRGB( gui_window *wnd, gui_point *start, gui_point *end,
                         gui_line_styles style, gui_ord thickness, gui_rgb rgb );
extern void GUIDrawText( gui_window *wnd, char *text, int length,
                         gui_ord row, gui_ord indent, gui_attr attr );
extern void GUIDrawTextPos( gui_window *wnd, char *text, int length,
                            gui_coord *pos, gui_attr attr );
extern void GUIDrawTextExtent( gui_window *wnd, char *text, int length,
                               gui_ord row, gui_ord indent, gui_attr attr,
                               gui_ord extentx );
extern void GUIDrawTextExtentPos( gui_window *wnd, char *text, int length,
                               gui_coord *pos, gui_attr attr, gui_ord extentx );
extern void GUIDrawTextRGB( gui_window *wnd, char *text, int length,
                            gui_ord row, gui_ord indent,
                            gui_rgb fore, gui_rgb back );
extern void GUIDrawTextPosRGB( gui_window *wnd, char *text, int length,
                               gui_coord *pos, gui_rgb fore, gui_rgb back );
extern void GUIDrawTextExtentRGB( gui_window *wnd, char *text, int length,
                                  gui_ord row, gui_ord indent, 
                                  gui_rgb fore, gui_rgb back,
                                  gui_ord extentx );
extern void GUIDrawTextExtentPosRGB( gui_window *wnd, char *text, int length,
                                     gui_coord *pos, 
                                     gui_rgb fore, gui_rgb back,
                                     gui_ord extentx );
extern bool GUIDrawBar( gui_window *wnd, gui_ord row, gui_ord start,
                        gui_ord width, gui_bar_styles bstyle, gui_attr attr,
                        bool selected );
extern bool GUIDrawBarGroup( gui_window *wnd, gui_ord row, gui_ord start,
                        gui_ord width1, gui_ord width2, gui_bar_styles bstyle,
                        gui_attr attr1, gui_attr attr2, bool selected );



extern bool GUISetWindowText( gui_window * wnd, char * data );
extern int GUIGetWindowTextLength( gui_window *wnd );
extern int GUIGetWindowText( gui_window *wnd, char *data, int max_length );
extern gui_ord GUIGetRow( gui_window * wnd, gui_point *pos );
extern gui_ord GUIGetCol( gui_window * wnd, char *text, gui_point *pos );
extern gui_ord GUIGetStringPos( gui_window *wnd, gui_ord indent, 
                                char * string, int mouse_x );
extern gui_ord GUIGetExtentX( gui_window *wnd, char * text, int length );
extern gui_ord GUIGetExtentY( gui_window *wnd, char * text );
extern gui_ord GUIGetControlExtentX( gui_window * wnd, unsigned id, char * text, int length );
extern gui_ord GUIGetControlExtentY( gui_window * wnd, unsigned id, char * text );
extern void GUIGetTextMetrics( gui_window *wnd, gui_text_metrics *metrics );
extern void GUIGetDlgTextMetrics( gui_text_metrics *metrics );
extern void GUIGetPoint( gui_window* wnd, gui_ord extent, gui_ord row,
                         gui_point *point );



extern bool GUICreateFloatingPopup( gui_window *wnd, gui_point *location,
                                    int num_menu_items, gui_menu_struct *menu,
                                    gui_mouse_track track, int *curr_item );
extern bool GUITrackFloatingPopup( gui_window *wnd, gui_point *location,
                               gui_mouse_track track, int *curr_item );
extern bool GUIEnableMenuItem( gui_window *wnd, int id, bool enabled, bool floating );
extern bool GUICheckMenuItem( gui_window *wnd, int id, bool check, bool floating );
extern bool GUISetMenuText( gui_window *wnd, int id, char *text, bool floating );
extern bool GUISetHintText( gui_window *wnd, int id, char *hinttext );

extern bool GUIEnableMDIMenus( bool enable );
extern bool GUIEnableMenus( gui_window *wnd, bool enable );  
extern bool GUIDeleteMenuItem( gui_window *wnd, int id, bool floating );

extern bool GUIResetMenus( gui_window *wnd, int num_menus, gui_menu_struct *menu );

extern int  GUIGetMenuPopupCount( gui_window *wnd, int id );

extern bool GUIInsertMenu( gui_window *wnd, int offset, gui_menu_struct *menu, bool floating );
extern bool GUIInsertMenuByID( gui_window *wnd, unsigned id, gui_menu_struct *menu );
extern bool GUIAppendMenu( gui_window *wnd, gui_menu_struct *menu, bool floating );
extern bool GUIAppendMenuByOffset( gui_window *wnd, int offset, gui_menu_struct *menu );
extern bool GUIAppendMenuToPopup( gui_window *wnd, unsigned id, gui_menu_struct *menu, bool floating );
extern bool GUIInsertMenuToPopup( gui_window *wnd, unsigned id, int offset, gui_menu_struct *menu, bool floating );



extern bool GUICreateFloatToolBar( gui_window *wnd, bool fixed,
				   gui_ord height, int num_items,
				   gui_toolbar_struct *toolbar, bool excl,
				   gui_colour_set *plain,
				   gui_colour_set *standout, gui_rect *rect );
extern bool GUICreateToolBar( gui_window *wnd, bool fixed, gui_ord height,
                              int num_items, gui_toolbar_struct *toolbar,
                              bool excl, gui_colour_set *plain,
                              gui_colour_set *standout );
extern bool GUICloseToolBar( gui_window *wnd );
extern bool GUIHasToolBar( gui_window *wnd );
extern bool GUIChangeToolBar( gui_window *wnd );
extern bool GUIToolBarFixed( gui_window *wnd );



extern bool GUICreateStatusWindow( gui_window *wnd, gui_ord x, gui_ord height,
                                   gui_colour_set *colour );
extern bool GUICloseStatusWindow( gui_window *wnd );
extern bool GUIHasStatus( gui_window *wnd );
extern bool GUIDrawStatusText( gui_window *wnd, char *text );
extern bool GUIClearStatusText( gui_window *wnd );
extern bool GUIResizeStatusWindow( gui_window *wnd, gui_ord x, gui_ord height );



 
 
 
 
extern gui_help_instance GUIHelpInit( gui_window *wnd, char *file, char *title );
extern void GUIHelpFini( gui_help_instance inst, gui_window *wnd, char *file );
extern bool GUIShowHelp( gui_help_instance inst, gui_window *wnd, gui_help_actions act, char *file, char *topic );

 
 
extern bool GUIDisplayHelp( gui_window *wnd, char *file, char *topic );
extern bool GUIDisplayHelpId( gui_window *wnd, char *file, int id );










extern void GUIInitHScrollCol( gui_window *wnd, int hscroll_pos );
extern void GUIInitVScrollRow( gui_window *wnd, int vscroll_pos );
extern void GUISetHScrollCol( gui_window *wnd, int hscroll_pos );
extern void GUISetVScrollRow( gui_window *wnd, int vscroll_pos );
extern int GUIGetHScrollCol( gui_window *wnd );
extern int GUIGetVScrollRow( gui_window *wnd );
extern void GUISetHScrollRangeCols( gui_window *wnd, gui_ord range );
extern void GUISetVScrollRangeRows( gui_window *wnd, gui_ord range );
extern gui_ord GUIGetHScrollRangeCols( gui_window *wnd );
extern gui_ord GUIGetVScrollRangeRows( gui_window *wnd );
extern void GUIDoHScroll( gui_window *wnd, int cols );
extern void GUIDoVScroll( gui_window *wnd, int rows );
extern void GUIDoHScrollClip( gui_window *wnd, int cols, int start, int end );
extern void GUIDoVScrollClip( gui_window *wnd, int rows, int start, int end );



extern void GUISetHScrollThumb( gui_window * wnd, int percent );
extern void GUISetVScrollThumb( gui_window * wnd, int percent );



extern void GUIInitHScroll( gui_window *wnd, gui_ord hscroll_pos );
extern void GUIInitVScroll( gui_window *wnd, gui_ord vscroll_pos );
extern void GUISetHScroll( gui_window *wnd, gui_ord hscroll_pos );
extern void GUISetVScroll( gui_window *wnd, gui_ord vscroll_pos );
extern gui_ord GUIGetHScroll( gui_window *wnd );
extern gui_ord GUIGetVScroll( gui_window *wnd );
extern void GUISetHScrollRange( gui_window *wnd, gui_ord range );
extern void GUISetVScrollRange( gui_window *wnd, gui_ord range );
extern gui_ord GUIGetHScrollRange( gui_window *wnd );
extern gui_ord GUIGetVScrollRange( gui_window *wnd );
extern gui_ord GUIGetNumRows( gui_window *wnd );



extern gui_message_return GUIDisplayMessage( gui_window *wnd,
                                             char *message, char *caption,
                                             gui_message_type type );
extern gui_message_return GUIGetNewVal( char *title, char *old, char **new_val );
extern int GUIDlgPick( char *text, PICKCALLBACK *InitPick );
extern int GUIDlgPickWithRtn( char *text, PICKCALLBACK *InitPick, PICKDLGOPEN* );



extern bool GUICreateDialog( gui_create_info *dialog, int num_controls,
                             gui_control_info *controls );
extern bool GUICreateSysModalDialog( gui_create_info *dialog,
                                     int num_controls,
                                     gui_control_info *controls );
extern bool GUICreateResDialog( gui_create_info *dialog, long dlg_id );
extern bool GUICreateDialogFromRes( int id, gui_window *parent,
				    GUICALLBACK cb, void *extra );
extern void GUICloseDialog( gui_window * wnd );



extern bool GUIAddControl( gui_control_info *info, gui_colour_set *plain,
                           gui_colour_set *standout );
extern bool GUIDeleteControl( gui_window *wnd, unsigned id );
extern bool GUIResizeControl( gui_window *wnd, unsigned id, gui_rect *rect );
extern bool GUIEnableControl( gui_window *wnd, unsigned id, bool enable );
extern bool GUIIsControlEnabled( gui_window *wnd, unsigned id );
extern bool GUIGetControlRect( gui_window *wnd, unsigned id, gui_rect *rect );
extern bool GUIGetControlClass( gui_window *wnd, unsigned id, gui_control_class *control_class );
extern void GUIHideControl( gui_window *wnd, unsigned id );
extern void GUIShowControl( gui_window *wnd, unsigned id );
extern bool GUIIsControlVisible( gui_window *wnd, unsigned id );


extern bool GUIControlSetRedraw( gui_window *wnd, unsigned control, bool redraw );
extern bool GUIAddText( gui_window *wnd, unsigned id, char *text );
extern bool GUISetListItemData( gui_window *wnd, unsigned id, unsigned choice, void *data );
extern void *GUIGetListItemData( gui_window *wnd, unsigned id, unsigned choice );
extern bool GUIAddTextList( gui_window *wnd, unsigned id, unsigned items,
                            void *handle, char *(*getstring)(void*,unsigned) );
extern bool GUIInsertText( gui_window *wnd, unsigned id, int choice, char *text );
extern bool GUISetTopIndex( gui_window *wnd, unsigned id, int choice );
extern int GUIGetTopIndex( gui_window *wnd, unsigned id );
extern bool GUISetHorizontalExtent( gui_window *wnd, unsigned id, int extent );
extern bool GUIClearList( gui_window *wnd, unsigned id );
extern bool GUIDeleteItem( gui_window *wnd, unsigned id, int choice );
extern int GUIGetListSize( gui_window *wnd, unsigned id );
extern int GUIGetCurrSelect( gui_window *wnd, unsigned id );
extern bool GUISetCurrSelect( gui_window *wnd, unsigned id, int choice );
extern char *GUIGetListItem( gui_window *wnd, unsigned id, int choice );

extern bool GUISetText( gui_window *wnd, unsigned id, char *text );
extern bool GUIClearText( gui_window *wnd, unsigned id );
extern char * GUIGetText( gui_window *wnd, unsigned id );
extern bool GUISelectAll( gui_window *wnd, unsigned id, bool select );
extern bool GUISetEditSelect( gui_window *wnd, unsigned id, int start, int end );
extern bool GUIGetEditSelect( gui_window *wnd, unsigned id, int *start, int *end );
extern bool GUILimitEditText( gui_window *wnd, unsigned id, int len );
extern bool GUIDropDown( gui_window *wnd, unsigned id, bool drop );

extern unsigned GUIIsChecked( gui_window *wnd, unsigned id );
extern bool GUISetChecked( gui_window *wnd, unsigned id, unsigned check );



extern void GUIGetKeyState( gui_keystate *state );
extern void GUIFlushKeys( void );
extern void GUISetExtra( gui_window *wnd, void *extra );
extern void *GUIGetExtra( gui_window *wnd );
extern void GUIGetClientRect( gui_window *wnd, gui_rect *client );
extern bool GUIGetPaintRect( gui_window *wnd, gui_rect *paint );
extern void GUIGetAbsRect( gui_window *wnd, gui_rect *rect );
extern void GUIGetRect( gui_window *wnd, gui_rect *rect );
extern gui_scroll_styles GUIGetScrollStyle( gui_window *wnd );
extern gui_create_styles GUIGetCreateStyle( gui_window *wnd );
extern void GUITruncToPixel( gui_coord *coord );
extern bool GUIGetMousePosn( gui_window *wnd, gui_point *point );
extern void GUIGetSystemMetrics( gui_system_metrics *metrics );
extern bool GUIGetMinSize( gui_coord *size );
extern void GUIEnumChildWindows( gui_window *wnd, ENUMCALLBACK *func, void *param );
extern void GUIEnumControls( gui_window *wnd, CONTRENUMCALLBACK *func, void *param );
extern bool GUIIsGUI( void );
extern bool GUIGetArgs( char ***argv, int *argc );


extern void GUISpawnStart( void );
extern void GUISpawnEnd( void );


extern bool GUILoadStrInit( char * fname );
extern bool GUILoadStrFini( void );
extern bool GUILoadString( int string_id, char *buffer, int buffer_length );
extern bool GUIIsLoadStrInitialized( void );


void GUIHookF1( void );
void GUIUnHookF1( void );


extern int GUICharLen( int );

bool GUIIsFirstInstance( void );

void GUIHookFileDlg( bool hook );










typedef enum gui_key {
     GUI_KEY_CTRL_KP_ENTER = 0x0a ,
     GUI_KEY_KP_ENTER = 0x0d ,
     GUI_KEY_KP_ASTERISK = 0x2a ,
     GUI_KEY_KP_PLUS = 0x2b ,
     GUI_KEY_KP_MINUS = 0x2d ,
     GUI_KEY_KP_PERIOD = 0x2e ,
     GUI_KEY_KP_SLASH = 0x2f ,
     GUI_KEY_CTRL_A = 0x01 ,
     GUI_KEY_CTRL_B = 0x02 ,
     GUI_KEY_CTRL_C = 0x03 ,
     GUI_KEY_CTRL_D = 0x04 ,
     GUI_KEY_CTRL_E = 0x05 ,
     GUI_KEY_CTRL_F = 0x06 ,
     GUI_KEY_CTRL_G = 0x07 ,
     GUI_KEY_CTRL_H = 512 + 0x08 ,
     GUI_KEY_CTRL_I = 512 + 0x09 ,
     GUI_KEY_TAB = 0x09 ,
     GUI_KEY_CTRL_J = 0x0a ,
     GUI_KEY_CTRL_K = 0x0b ,
     GUI_KEY_CTRL_L = 0x0c ,
     GUI_KEY_CTRL_M = 512 + 0x0d ,
     GUI_KEY_CTRL_N = 0x0e ,
     GUI_KEY_CTRL_O = 0x0f ,
     GUI_KEY_CTRL_P = 0x10 ,
     GUI_KEY_CTRL_Q = 0x11 ,
     GUI_KEY_CTRL_R = 0x12 ,
     GUI_KEY_CTRL_S = 0x13 ,
     GUI_KEY_CTRL_T = 0x14 ,
     GUI_KEY_CTRL_U = 0x15 ,
     GUI_KEY_CTRL_V = 0x16 ,
     GUI_KEY_CTRL_W = 0x17 ,
     GUI_KEY_CTRL_X = 0x18 ,
     GUI_KEY_CTRL_Y = 0x19 ,
     GUI_KEY_CTRL_Z = 0x1a ,
     GUI_KEY_CTRL_BACKSLASH = 0x1c ,
     GUI_KEY_CTRL_RIGHT_BRACKET = 0x1d ,
     GUI_KEY_CTRL_LEFT_BRACKET = 512 + 0x1b ,
     GUI_KEY_CTRL_6 = 0x1e ,
     GUI_KEY_CTRL_MINUS = 0x1f ,
     GUI_KEY_CTRL_BACKSPACE = 0x7f ,
     GUI_KEY_CTRL_ENTER = 0x0a ,
     GUI_KEY_BACKQUOTE = '`' ,
     GUI_KEY_1 = '1' ,
     GUI_KEY_2 = '2' ,
     GUI_KEY_3 = '3' ,
     GUI_KEY_4 = '4' ,
     GUI_KEY_5 = '5' ,
     GUI_KEY_6 = '6' ,
     GUI_KEY_7 = '7' ,
     GUI_KEY_8 = '8' ,
     GUI_KEY_9 = '9' ,
     GUI_KEY_0 = '0' ,
     GUI_KEY_MINUS = '-' ,
     GUI_KEY_EQUAL = '=' ,
     GUI_KEY_q = 'q' ,
     GUI_KEY_w = 'w' ,
     GUI_KEY_e = 'e' ,
     GUI_KEY_r = 'r' ,
     GUI_KEY_t = 't' ,
     GUI_KEY_y = 'y' ,
     GUI_KEY_u = 'u' ,
     GUI_KEY_i = 'i' ,
     GUI_KEY_o = 'o' ,
     GUI_KEY_p = 'p' ,
     GUI_KEY_LEFT_BRACKET = '[' ,
     GUI_KEY_RIGHT_BRACKET = ']' ,
     GUI_KEY_BACKSLASH = '\\' ,
     GUI_KEY_a = 'a' ,
     GUI_KEY_s = 's' ,
     GUI_KEY_d = 'd' ,
     GUI_KEY_f = 'f' ,
     GUI_KEY_g = 'g' ,
     GUI_KEY_h = 'h' ,
     GUI_KEY_j = 'j' ,
     GUI_KEY_k = 'k' ,
     GUI_KEY_l = 'l' ,
     GUI_KEY_SEMICOLON = ';' ,
     GUI_KEY_QUOTE = '\'' ,
     GUI_KEY_z = 'z' ,
     GUI_KEY_x = 'x' ,
     GUI_KEY_c = 'c' ,
     GUI_KEY_v = 'v' ,
     GUI_KEY_b = 'b' ,
     GUI_KEY_n = 'n' ,
     GUI_KEY_m = 'm' ,
     GUI_KEY_COMMA = ',' ,
     GUI_KEY_PERIOD = '.' ,
     GUI_KEY_SLASH = '/' ,
     GUI_KEY_TILDE = '~' ,
     GUI_KEY_EXCLAMATION = '!' ,
     GUI_KEY_AT = '@' ,
     GUI_KEY_OCTATHORPE = '#' ,
     GUI_KEY_DOLLAR = '$' ,
     GUI_KEY_PERCENT = '%' ,
     GUI_KEY_CARET = '^' ,
     GUI_KEY_AMPERSAND = '&' ,
     GUI_KEY_ASTERISK = '*' ,
     GUI_KEY_LEFT_PAREN = '(' ,
     GUI_KEY_RIGHT_PAREN = ')' ,
     GUI_KEY_UNDERSCORE = '_' ,
     GUI_KEY_PLUS = '+' ,
     GUI_KEY_Q = 'Q' ,
     GUI_KEY_W = 'W' ,
     GUI_KEY_E = 'E' ,
     GUI_KEY_R = 'R' ,
     GUI_KEY_T = 'T' ,
     GUI_KEY_Y = 'Y' ,
     GUI_KEY_U = 'U' ,
     GUI_KEY_I = 'I' ,
     GUI_KEY_O = 'O' ,
     GUI_KEY_P = 'P' ,
     GUI_KEY_LEFT_BRACE = '{' ,
     GUI_KEY_RIGHT_BRACE = '}' ,
     GUI_KEY_BAR = '|' ,
     GUI_KEY_A = 'A' ,
     GUI_KEY_S = 'S' ,
     GUI_KEY_D = 'D' ,
     GUI_KEY_F = 'F' ,
     GUI_KEY_G = 'G' ,
     GUI_KEY_H = 'H' ,
     GUI_KEY_J = 'J' ,
     GUI_KEY_K = 'K' ,
     GUI_KEY_L = 'L' ,
     GUI_KEY_COLON = ':' ,
     GUI_KEY_DOUBLEQUOTE = '"' ,
     GUI_KEY_Z = 'Z' ,
     GUI_KEY_X = 'X' ,
     GUI_KEY_C = 'C' ,
     GUI_KEY_V = 'V' ,
     GUI_KEY_B = 'B' ,
     GUI_KEY_N = 'N' ,
     GUI_KEY_M = 'M' ,
     GUI_KEY_LESS = '<' ,
     GUI_KEY_GREATER = '>' ,
     GUI_KEY_QUESTIONMARK = '?' ,
     GUI_KEY_BACKSPACE = 0x08 ,
     GUI_KEY_SPACE = ' ' ,
     GUI_KEY_ENTER = 0x0d ,
     GUI_KEY_ESCAPE = 0x1b ,
     GUI_KEY_KP_0 = '0' ,
     GUI_KEY_KP_1 = '1' ,
     GUI_KEY_KP_2 = '2' ,
     GUI_KEY_KP_3 = '3' ,
     GUI_KEY_KP_4 = '4' ,
     GUI_KEY_KP_5 = '5' ,
     GUI_KEY_KP_6 = '6' ,
     GUI_KEY_KP_7 = '7' ,
     GUI_KEY_KP_8 = '8' ,
     GUI_KEY_KP_9 = '9' ,
     GUI_KEY_HOME = 256 + 0x47 ,
     GUI_KEY_END = 256 + 0x4f ,
     GUI_KEY_UP = 256 + 0x48 ,
     GUI_KEY_DOWN = 256 + 0x50 ,
     GUI_KEY_PAGEUP = 256 + 0x49 ,
     GUI_KEY_PAGEDOWN = 256 + 0x51 ,
     GUI_KEY_LEFT = 256 + 0x4b ,
     GUI_KEY_RIGHT = 256 + 0x4d ,
     GUI_KEY_BLANK = 256 + 0x4c ,
     GUI_KEY_INSERT = 256 + 0x52 ,
     GUI_KEY_DELETE = 256 + 0x53 ,
     GUI_KEY_CTRL_HOME = 256 + 0x77 ,
     GUI_KEY_CTRL_END = 256 + 0x75 ,
     GUI_KEY_CTRL_UP = 256 + 0x8d ,
     GUI_KEY_CTRL_DOWN = 256 + 0x91 ,
     GUI_KEY_CTRL_PAGEUP = 256 + 0x84 ,
     GUI_KEY_CTRL_PAGEDOWN = 256 + 0x76 ,
     GUI_KEY_CTRL_LEFT = 256 + 0x73 ,
     GUI_KEY_CTRL_RIGHT = 256 + 0x74 ,
     GUI_KEY_CTRL_BLANK = 256 + 0x8f ,
     GUI_KEY_CTRL_INSERT = 256 + 0x92 ,
     GUI_KEY_CTRL_DELETE = 256 + 0x93 ,
     GUI_KEY_CTRL_KP_PLUS = 256 + 0x90 ,
     GUI_KEY_CTRL_KP_MINUS = 256 + 0x8e ,
     GUI_KEY_CTRL_KP_ASTERISK = 256 + 0x96 ,
     GUI_KEY_CTRL_KP_SLASH = 256 + 0x95 ,
     GUI_KEY_ALT_KP_PLUS = 256 + 0x4e ,
     GUI_KEY_ALT_KP_MINUS = 256 + 0x4a ,
     GUI_KEY_ALT_KP_ASTERISK = 256 + 0x37 ,
     GUI_KEY_ALT_KP_SLASH = 256 + 0xa4 ,
     GUI_KEY_ALT_KP_ENTER = 256 + 0xa6 ,
     GUI_KEY_ALT_EXT_INSERT = 256 + 0xa2 ,
     GUI_KEY_ALT_EXT_DELETE = 256 + 0xa3 ,
     GUI_KEY_ALT_EXT_HOME = 256 + 0x97 ,
     GUI_KEY_ALT_EXT_END = 256 + 0x9f ,
     GUI_KEY_ALT_EXT_PAGEUP = 256 + 0x99 ,
     GUI_KEY_ALT_EXT_PAGEDOWN = 256 + 0xa1 ,
     GUI_KEY_ALT_EXT_LEFT = 256 + 0x9b ,
     GUI_KEY_ALT_EXT_RIGHT = 256 + 0x9d ,
     GUI_KEY_ALT_EXT_UP = 256 + 0x98 ,
     GUI_KEY_ALT_EXT_DOWN = 256 + 0xa0 ,
     GUI_KEY_F1 = 256 + 0x3b ,
     GUI_KEY_F2 = 256 + 0x3c ,
     GUI_KEY_F3 = 256 + 0x3d ,
     GUI_KEY_F4 = 256 + 0x3e ,
     GUI_KEY_F5 = 256 + 0x3f ,
     GUI_KEY_F6 = 256 + 0x40 ,
     GUI_KEY_F7 = 256 + 0x41 ,
     GUI_KEY_F8 = 256 + 0x42 ,
     GUI_KEY_F9 = 256 + 0x43 ,
     GUI_KEY_F10 = 256 + 0x44 ,
     GUI_KEY_F11 = 256 + 0x85 ,
     GUI_KEY_F12 = 256 + 0x86 ,
     GUI_KEY_CTRL_F1 = 256 + 0x5e ,
     GUI_KEY_CTRL_F2 = 256 + 0x5f ,
     GUI_KEY_CTRL_F3 = 256 + 0x60 ,
     GUI_KEY_CTRL_F4 = 256 + 0x61 ,
     GUI_KEY_CTRL_F5 = 256 + 0x62 ,
     GUI_KEY_CTRL_F6 = 256 + 0x63 ,
     GUI_KEY_CTRL_F7 = 256 + 0x64 ,
     GUI_KEY_CTRL_F8 = 256 + 0x65 ,
     GUI_KEY_CTRL_F9 = 256 + 0x66 ,
     GUI_KEY_CTRL_F10 = 256 + 0x67 ,
     GUI_KEY_CTRL_F11 = 256 + 0x89 ,
     GUI_KEY_CTRL_F12 = 256 + 0x8a ,
     GUI_KEY_ALT_F1 = 256 + 0x68 ,
     GUI_KEY_ALT_F2 = 256 + 0x69 ,
     GUI_KEY_ALT_F3 = 256 + 0x6a ,
     GUI_KEY_ALT_F4 = 256 + 0x6b ,
     GUI_KEY_ALT_F5 = 256 + 0x6c ,
     GUI_KEY_ALT_F6 = 256 + 0x6d ,
     GUI_KEY_ALT_F7 = 256 + 0x6e ,
     GUI_KEY_ALT_F8 = 256 + 0x6f ,
     GUI_KEY_ALT_F9 = 256 + 0x70 ,
     GUI_KEY_ALT_F10 = 256 + 0x71 ,
     GUI_KEY_ALT_F11 = 256 + 0x8b ,
     GUI_KEY_ALT_F12 = 256 + 0x8c ,
     GUI_KEY_SHIFT_F1 = 256 + 0x54 ,
     GUI_KEY_SHIFT_F2 = 256 + 0x55 ,
     GUI_KEY_SHIFT_F3 = 256 + 0x56 ,
     GUI_KEY_SHIFT_F4 = 256 + 0x57 ,
     GUI_KEY_SHIFT_F5 = 256 + 0x58 ,
     GUI_KEY_SHIFT_F6 = 256 + 0x59 ,
     GUI_KEY_SHIFT_F7 = 256 + 0x5a ,
     GUI_KEY_SHIFT_F8 = 256 + 0x5b ,
     GUI_KEY_SHIFT_F9 = 256 + 0x5c ,
     GUI_KEY_SHIFT_F10 = 256 + 0x5d ,
     GUI_KEY_SHIFT_F11 = 256 + 0x87 ,
     GUI_KEY_SHIFT_F12 = 256 + 0x88 ,
     GUI_KEY_SHIFT_TAB = 256 + 0x0f ,
     GUI_KEY_ALT_BACKQUOTE = 256 + 0x29 ,
     GUI_KEY_ALT_1 = 256 + 0x78 ,
     GUI_KEY_ALT_2 = 256 + 0x79 ,
     GUI_KEY_ALT_3 = 256 + 0x7a ,
     GUI_KEY_ALT_4 = 256 + 0x7b ,
     GUI_KEY_ALT_5 = 256 + 0x7c ,
     GUI_KEY_ALT_6 = 256 + 0x7d ,
     GUI_KEY_ALT_7 = 256 + 0x7e ,
     GUI_KEY_ALT_8 = 256 + 0x7f ,
     GUI_KEY_ALT_9 = 256 + 0x80 ,
     GUI_KEY_ALT_0 = 256 + 0x81 ,
     GUI_KEY_ALT_MINUS = 256 + 0x82 ,
     GUI_KEY_ALT_EQUAL = 256 + 0x83 ,
     GUI_KEY_ALT_Q = 256 + 0x10 ,
     GUI_KEY_ALT_W = 256 + 0x11 ,
     GUI_KEY_ALT_E = 256 + 0x12 ,
     GUI_KEY_ALT_R = 256 + 0x13 ,
     GUI_KEY_ALT_T = 256 + 0x14 ,
     GUI_KEY_ALT_Y = 256 + 0x15 ,
     GUI_KEY_ALT_U = 256 + 0x16 ,
     GUI_KEY_ALT_I = 256 + 0x17 ,
     GUI_KEY_ALT_O = 256 + 0x18 ,
     GUI_KEY_ALT_P = 256 + 0x19 ,
     GUI_KEY_ALT_LEFT_BRACKET = 256 + 0x1a ,
     GUI_KEY_ALT_RIGHT_BRACKET = 256 + 0x1b ,
     GUI_KEY_ALT_BACKSLASH = 256 + 0x2b ,
     GUI_KEY_ALT_A = 256 + 0x1e ,
     GUI_KEY_ALT_S = 256 + 0x1f ,
     GUI_KEY_ALT_D = 256 + 0x20 ,
     GUI_KEY_ALT_F = 256 + 0x21 ,
     GUI_KEY_ALT_G = 256 + 0x22 ,
     GUI_KEY_ALT_H = 256 + 0x23 ,
     GUI_KEY_ALT_J = 256 + 0x24 ,
     GUI_KEY_ALT_K = 256 + 0x25 ,
     GUI_KEY_ALT_L = 256 + 0x26 ,
     GUI_KEY_ALT_SEMICOLON = 256 + 0x27 ,
     GUI_KEY_ALT_QUOTE = 256 + 0x28 ,
     GUI_KEY_ALT_Z = 256 + 0x2c ,
     GUI_KEY_ALT_X = 256 + 0x2d ,
     GUI_KEY_ALT_C = 256 + 0x2e ,
     GUI_KEY_ALT_V = 256 + 0x2f ,
     GUI_KEY_ALT_B = 256 + 0x30 ,
     GUI_KEY_ALT_N = 256 + 0x31 ,
     GUI_KEY_ALT_M = 256 + 0x32 ,
     GUI_KEY_ALT_COMMA = 256 + 0x33 ,
     GUI_KEY_ALT_PERIOD = 256 + 0x34 ,
     GUI_KEY_ALT_SLASH = 256 + 0x35 ,
     GUI_KEY_ALT_BACKSPACE = 256 + 0x0e ,
     GUI_KEY_ALT_ENTER = 256 + 0x1c ,
     GUI_KEY_ALT_TAB = 256 + 0xa5 ,
     GUI_KEY_ALT_ESCAPE = 256 + 0x01 ,
     GUI_KEY_CTRL_2 = 256 + 0x03 ,
     GUI_KEY_CTRL_8 = 256 + 0x09 ,
     GUI_KEY_CTRL_TAB = 256 + 0x94 
} gui_key;



typedef struct {
    gui_key		key;
    gui_keystate	state;
} gui_key_state;

typedef struct {
    unsigned		id;
    gui_key_state	key_state;
} gui_key_control;

































































































enum {
    OFN_RC_FAILED_TO_INITIALIZE,
    OFN_RC_NO_FILE_SELECTED,
    OFN_RC_FILE_SELECTED,
    OFN_RC_RUNTIME_ERROR
};

typedef struct
{
    char		*filter_list;
    int			filter_index;
    char		*file_name;
    int			max_file_name;
    char		*base_file_name;
    int			max_base_file_name;
    char		*title;
    unsigned long	flags;
    char		*initial_dir;
    char		*last_path;
} open_file_name;

int GUIGetFileName( gui_window *wnd, open_file_name *ofn );



    struct a_window;
    typedef struct a_window a_window;


 







typedef struct {
    unsigned char	area[ 512 ];
    unsigned 		first_cmd;
    unsigned 		first_free;
    unsigned 		curr_cmd;
    bool		last_was_next;
} save_area;









 












typedef enum {
    WND_FIRST_UNUSED = GUI_FIRST_UNUSED,
} wnd_attr;






























typedef unsigned long wnd_switches;


typedef int             wnd_row;
typedef char            wnd_class;
typedef unsigned long   wnd_update_list;

typedef struct wnd_line_piece {
    char        *text;                   
    wnd_attr    attr;                    
    gui_ord     indent;                  
    gui_ord     extent;                  
    unsigned    tabstop         : 1;     
    unsigned    static_text     : 1;     
    unsigned    hot             : 1;     
    unsigned    underline       : 1;     
    unsigned    use_key         : 1;     
    unsigned    use_piece0_attr : 1;     
    unsigned    use_prev_attr   : 1;     
    unsigned    bitmap          : 1;     
    unsigned    master_tabstop  : 1;     
    unsigned    draw_bar        : 1;     
    unsigned    vertical_line   : 1;     
    unsigned    draw_hook       : 1;     
    unsigned    draw_line_hook  : 1;     
    unsigned    length;                  
    char        *hint;                   
} wnd_line_piece;




typedef struct wnd_create_struct {
    struct wnd_info     *info;
    char                *text;
    wnd_class           class;
    void                *extra;
    gui_create_styles   style;
    gui_scroll_styles   scroll;
    gui_colour_set      *colour;
    gui_rect            rect;
    int                 title_size;
} wnd_create_struct;

typedef struct wnd_posn {
    float               x,y;
    float               width,height;
} wnd_posn;


typedef int wnd_menu_id;



typedef struct {
        wnd_row                 row;
        int                     piece;
        int                     col;
} wnd_coord;

typedef struct {
        wnd_row                 row;
        int                     piece;
        int                     col;
        int                     end_col;
} wnd_dirt;

typedef struct {
        wnd_row                 row;
        int                     piece;
        int                     col;
        int                     end;
} wnd_subpiece;

typedef struct {
    wnd_row             min_rows;
    int                 min_cols;
    wnd_row             max_rows;
    int                 max_cols;
} wnd_metrics;

typedef struct {
    gui_bar_styles      bar_style;
    gui_colour          bar_colour;
    gui_colour          bar_colour2;
    gui_ord             bar_size2;
    unsigned            bar_group    : 1;
    unsigned            bar_selected : 1;
} wnd_bar_info;




























typedef struct aui_private_window_structure {
        gui_window              *gui;
        union {  
            struct aui_private_window_structure *__free_field;
            struct {
                char            row;
                char            piece;
            } button_down;
        } u;
        void                    *extra;
        struct wnd_info         *info;
        char                    *popitem;
        char                    *searchitem;
        wnd_coord               sel_start;
        wnd_coord               sel_end;
        wnd_coord               current;
        wnd_row                 rows;
        gui_coord               max_char;
        gui_ord                 width;
        gui_ord                 max_indent;
        int                     top;
        char                    button_down_screen_row;
        char                    keyindex;
        char                    keypiece;
        wnd_class               class;
        wnd_switches            switches;
        int                     vscroll_pending;
        int                     hscroll_pending;
        wnd_row                 max_row;
        char                    *select_chars;
        int                     title_size;
        gui_ord                 avg_char_x;
        gui_ord                 mid_char_x;
        int                     last_popup;
        int                     current_col;
        char                    num_popups;
        gui_menu_struct         *popupmenu;
        char                    dirtyrects;
        wnd_dirt                dirty[1];
} aui_private_window_structure;



typedef bool (WNDCALLBACK)(a_window*,gui_event,void*);
typedef void (WNDREFRESH)(a_window*);
typedef void (WNDMENU)(a_window*,unsigned,int,int);
typedef void (WNDMODIFY)(a_window *,int,int);
typedef int (WNDSCROLL)(a_window*,int);
typedef int (WNDNUMROWS)(a_window*);
typedef int (WNDNEXTROW)(a_window*,int,int);
typedef bool (WNDGETLINE)(a_window *wnd, wnd_row row,int piece,wnd_line_piece*);
typedef void (WNDNOTIFY)(a_window *wnd, wnd_row row,int piece);
typedef void (WNDBEGPAINT)(a_window *wnd,wnd_row row,int num);
typedef void (WNDENDPAINT)(a_window *wnd,wnd_row row,int num);
typedef a_window * (WNDOPEN)();
typedef a_window *(WNDCREATE)(char*, struct wnd_info*, wnd_class,void*);
typedef void (WNDCLOSE)(a_window*);


typedef struct wnd_info {
        WNDCALLBACK             *event;
        WNDREFRESH              *refresh;
        WNDGETLINE              *getline;
        WNDMENU                 *menuitem;
        WNDSCROLL               *scroll;
        WNDBEGPAINT             *begpaint;
        WNDENDPAINT             *endpaint;
        WNDMODIFY               *modify;
        WNDNUMROWS              *numrows;
        WNDNEXTROW              *nextrow;
        WNDNOTIFY               *notify;
        wnd_update_list         flags;
        char                    num_popups;
        gui_menu_struct         *popupmenu;
} wnd_info;


typedef char*           DLGPICKTEXT(void*,int);
typedef int             GUIPICKER( char *, PICKCALLBACK * );
extern int              DlgPickWithRtn( char *text, void *from, int def, DLGPICKTEXT *rtn, unsigned );
extern int              DlgPickWithRtn2( char *text, void *from, int def, DLGPICKTEXT *rtn, unsigned, GUIPICKER * );
extern int              DlgPick( char *text, char **from, int def, unsigned num );
extern bool             DlgNew( char *text, char *buff, int buff_len );
extern void             DlgOpen(char*,int,int,gui_control_info*,int,GUICALLBACK*,void*);
extern void             ResDlgOpen(GUICALLBACK*,void*,int);
extern int              DlgGetFileName( open_file_name *ofn );
extern bool             DlgFileBrowse( char *title, char *filter, char *path,
                                       unsigned len, unsigned long flags );

extern a_window         **WndFindOwner(a_window *);
extern a_window         *WndFirst();

extern gui_ord          WndExtentX(a_window *,char *);

extern void             WndFreshAll(void);
extern bool             WndStopRefresh(bool);

extern void             WndNoMemory(void);
extern void             WndMemInit();
extern void             WndMemFini();
extern void             *WndAlloc(unsigned int );
extern void             *WndRealloc(void*, unsigned int );
extern void             WndFree(void *);
extern void             *WndMustAlloc(unsigned int );
extern void             *WndMustRealloc(void*,unsigned int );
extern void             WndCreateFloatingPopup(a_window*,gui_point*,char,
                                               gui_menu_struct*, int*);

extern void             WndFixedThumb(a_window *);
extern void             WndSetThumbPos(a_window *,int );
extern void             WndSetThumbPercent(a_window *,int );
extern void             WndSetThumb(a_window * );
extern WNDSCROLL        WndScroll;
extern WNDSCROLL        WndScrollAbs;


extern wnd_row          WndCurrRow(a_window *);
extern bool             WndHasCurrent(a_window *);
extern void             WndNewCurrent(a_window *,wnd_row, int);
extern void             WndMoveCurrent( a_window *wnd, wnd_row row, int piece );
extern void             WndGetCurrent( a_window *, wnd_row *, int *);
extern void             WndNoCurrent(a_window *);
extern bool             WndFirstCurrent(a_window *);
extern bool             WndLastCurrent(a_window *);

extern void             WndNoSelect(a_window *);

extern void             WndCleanUp(void);
extern a_window         *WndFindActive(void);

extern void             WndDestroy(a_window *);
extern void             WndClose(a_window *);
extern void             WndRestoreToFront(a_window *);
extern void             WndToFront(a_window *);  

extern void             WndCurrToGUIPoint( a_window *wnd, gui_point *point );

extern WNDCREATE        WndCreate;
extern void             WndInitCreateStruct( wnd_create_struct * );
extern a_window         *WndCreateWithStruct( wnd_create_struct * );
extern a_window         *WndCreateWithStructAndMenuRes( wnd_create_struct *, int );
extern void             WndShrinkToMouse( a_window *wnd,wnd_metrics*);
extern bool             WndInit(char*);
extern bool             WndInitWithMenuRes(char*, int );
extern bool             WndFini(void);
extern bool             WndMainMenuProc( a_window *, unsigned );
extern void             WndSetSrchItem( a_window *wnd, char *expr );
extern bool             WndSearch(a_window *,bool,int);
extern void             WndInitNumRows( a_window* );
extern void             WndRXError( int );

extern void             WndFreshAll(void);
extern a_window         *WndNext( a_window * );

extern WNDCALLBACK      NoEventProc;
extern WNDREFRESH       NoRefresh;
extern WNDGETLINE       NoGetLine;
extern WNDMENU          NoMenuItem;
extern WNDMODIFY        NoModify;
extern WNDMODIFY        WndFirstMenuItem;
extern WNDSCROLL        NoScroll;
extern WNDBEGPAINT      NoBegPaint;
extern WNDENDPAINT      NoEndPaint;
extern WNDNOTIFY        NoNotify;
extern WNDNUMROWS       NoNumRows;
extern WNDNEXTROW       NoNextRow;

extern WNDCALLBACK      WndEvent;
extern WNDREFRESH       WndRefresh;
extern WNDGETLINE       WndGetLine;
extern WNDGETLINE       WndGetLineAbs;
extern WNDMENU          WndMenuItem;
extern WNDMODIFY        WndModify;
extern WNDSCROLL        WndScroll;
extern WNDBEGPAINT      WndBegPaint;
extern WNDENDPAINT      WndEndPaint;
extern WNDNOTIFY        WndNotify;
extern WNDNUMROWS       WndNumRows;
extern WNDNEXTROW       WndNextRow;
extern bool             WndHasNumRows( a_window * );

extern wnd_switches     WndSwitches;
extern a_window         *Windows;
extern gui_coord        WndMax;
extern gui_coord        WndScreen;
extern wnd_update_list  WndFlags;
extern bool             WndOkToShow;
extern bool             WndIgnoreAllEvents;

extern bool             WndProcMacro( a_window *wnd, unsigned key );

extern  void            WndSetTitle( a_window *, char * );
extern  int             WndGetTitle( a_window *wnd, char *data, int max_length );

extern void             Ring(void);

extern void             WndSysInit(void);
extern void             WndDoInput(void);
extern void             WndStartFreshAll();
extern void             WndEndFreshAll();
extern void             WndZapped(a_window*);

extern a_window         *WndFindClass(a_window *,wnd_class );
extern void             WndForAllClass( wnd_class class, void (*rtn)( a_window * ) );

extern int              SpawnP( void (*func)(void*), void *parm );
extern int              Spawn( void (*func)(void) );
extern void             Suicide();



extern  char            *WndSetIDChars( a_window *, char * );

extern  bool            WndIDChar( a_window *, char ch );
extern  bool            WndKeyChar( char ch );

extern bool             WndCursorRight( a_window *wnd );
extern bool             WndCursorLeft( a_window *wnd );
extern bool             WndTabLeft( a_window *wnd, bool wrap );
extern bool             WndTabRight( a_window *wnd, bool wrap );

extern void             WndCursorStart( a_window *wnd );
extern void             WndCursorEnd( a_window *wnd );

extern gui_ord          WndVScrollWidth( a_window *wnd );
extern void             WndResetScroll( a_window *wnd );
extern void             WndPageDown( a_window *wnd );
extern void             WndPageUp( a_window *wnd );
extern void             WndCursorDown( a_window *wnd );
extern void             WndScrollDown( a_window *wnd );
extern void             WndScrollTop( a_window *wnd );
extern void             WndScrollBottom( a_window *wnd );
extern void             WndCursorUp( a_window *wnd );
extern void             WndScrollUp( a_window *wnd );
extern void             WndSetVScrollRange( a_window *wnd, wnd_row rows );
extern void             WndChooseNew(void);
extern void             WndKeyPopUp(a_window *,gui_menu_struct *);
extern void             WndPopUp(a_window *,gui_menu_struct *);
extern void             WndMenuIgnoreAll( a_window *wnd );
extern void             WndMenuRespectAll( a_window *wnd );
extern void             WndMenuEnableAll( a_window *wnd );
extern void             WndMenuGrayAll( a_window *wnd );
extern void             WndMenuIgnore( a_window *wnd, int id, bool ignore );
extern void             WndMenuEnable( a_window *wnd, int id, bool enable );
extern void             WndMenuCheck( a_window *wnd, int id, bool check );
extern void             WndCheckMainMenu( int id, bool check );
extern void             WndEnableMainMenu( int id, bool enable );
 

extern  void            WndPieceDirty( a_window *wnd, wnd_row row, int piece );
extern void             WndRowDirty( a_window *wnd, wnd_row row );
extern void             WndRowDirtyImmed( a_window *wnd, wnd_row row );
extern void             WndDirty(a_window*);
extern void             WndDirtyCurr(a_window*);
extern void             WndSetColours( a_window *, int, gui_colour_set *);
extern void             WndBackGround( gui_colour colour );
extern void             WndDirtyRect( a_window *wnd, gui_ord x, wnd_row y,
                                      gui_ord width, wnd_row height );

extern void             WndReDrawAll();
extern void             WndSetIcon( a_window *, gui_resource *);
extern void             WndSetMainMenuText( gui_menu_struct * );
extern void             WndShowAll();
extern void             WndShowWndMain();
extern void             WndInitWndMain( wnd_create_struct *);
extern void             WndShowWindow( a_window *wnd );
extern void             WndResizeWindow( a_window *wnd, gui_rect * );
extern void             WndMinimizeWindow( a_window *wnd );
extern void             WndMaximizeWindow( a_window *wnd );
extern bool             WndIsMinimized( a_window *wnd );
extern bool             WndIsMaximized( a_window *wnd );
extern void             WndRestoreWindow( a_window *wnd );
extern void             WndGetRect( a_window *wnd, gui_rect *rect );

extern void             WndStartChoose(a_window *wnd);

extern void             WndCreateToolBar(gui_ord,bool,int,gui_toolbar_struct*);
extern bool             WndHaveToolBar();
extern void             WndCloseToolBar();
extern gui_ord          WndToolHeight();
extern bool             WndToolFixed();

extern void             *WndHourGlass( void * );
extern void             *WndHourCursor();
extern void             *WndArrowCursor();

extern void             WndCreateStatusWindow( gui_colour_set * );
extern bool             WndStatusText( char * );
extern bool             WndHaveStatusWindow();
extern void             WndCloseStatusWindow();

extern void             WndMainResized();
extern bool             WndShutDownHook(void);
extern bool             WndQueryEndSessionHook(void);
extern void             WndEndSessionHook(void);
extern void             WndResizeHook(a_window*);
extern void             WndFontHook(a_window*);

extern void             WndSetTitleSize( a_window *, int );
extern void             WndForcePaint( a_window *wnd );

extern bool             WndDoingSearch;

extern bool             WndDisplayHelp( char *file, char *topic );

extern bool             WndSetFontInfo( a_window *,char* );
extern bool             WndSetSysFont( a_window *, bool fixed );
extern char             *WndGetFontInfo( a_window * );

extern void             *WndInitHistory(void);
extern void             WndFiniHistory(void*);
extern bool             WndNextFromHistory( save_area *save, char *cmd );
extern bool             WndPrevFromHistory( save_area *save, char *cmd );
extern void             WndSaveToHistory( save_area *save, char *cmd );

extern void             WndSetDClick( int );
extern int              WndGetDClick( void );

extern char		*WndLoadString( int );
extern void		NullPopupMenu( gui_menu_struct *menu );

extern void             WndChangeMenuAll( gui_menu_struct *menu, int num_popups, bool on, int bit );
extern gui_message_return WndDisplayMessage( char *msg, char *cap,
                                             gui_message_type type );

extern void     WndRectToPos( gui_rect *rect, wnd_posn *posn, gui_coord *scale );
extern void     WndPosToRect( wnd_posn *posn, gui_rect *rect, gui_coord *scale );
typedef bool (WNDCLICKHOOK)( a_window *wnd, unsigned id );
extern void WndInstallClickHook( WNDCLICKHOOK *rtn );

extern void     WndForceRefresh(void);
extern void     WndSetWndMainSize( wnd_create_struct *info );

typedef int wnd_gadget_type;

extern void             WndGadgetInit();
extern void             WndSetGadgetLine( a_window *wnd, wnd_line_piece *line, wnd_gadget_type type, unsigned length );
extern void             WndGetGadgetSize( wnd_gadget_type type, gui_coord * );
extern wnd_gadget_type  WndGadgetSecondary;
extern char             *WndGadgetHint[];
extern gui_resource     WndGadgetArray[];
extern int              WndGadgetArraySize;
extern wnd_attr         WndGadgetAttr;

extern wnd_info         NoInfo;

extern wnd_attr         WndPlainAttr;
extern wnd_attr         WndTabStopAttr;
extern wnd_attr         WndSelectedAttr;
extern wnd_attr         WndCursorAttr;
extern wnd_attr         WndMapTabAttr( wnd_attr );
extern int              WndMaxDirtyRects;
extern gui_window_styles        WndStyle;
extern char             WndBackgroundChar;








extern  void    WndSetMainMenu( gui_menu_struct *menu, int num_menus );

 


























enum {
    DLG_ROW_0 = 30000,
    DLG_ROW_n = DLG_ROW_0 + 100,
    DLG_COL_0,
    DLG_COL_n = DLG_COL_0 + 100,
};







































extern void GUIDlgOpen( char *name, int rows, int cols,
                       gui_control_info *ctl, int num_controls,
		       GUICALLBACK *rtn, void *extra );

extern void GUIModalDlgOpen( gui_window *parent, char *name, int rows,
			     int cols, gui_control_info *ctl,
			     int num_controls, GUICALLBACK *rtn, void *extra );

extern void GUISysModalDlgOpen( char *name, int rows, int cols,
                           gui_control_info *ctl, int num_controls,
		           GUICALLBACK *rtn, void *extra );

extern unsigned GUIDlgBuffGetText( gui_window *gui, unsigned id,
                             char *buff, unsigned max_len );













#pragma aux fatal aborts;





    








 extern char  _Literal_Assertion_Failed[sizeof( "Internal error: %s" )]; 
 extern char  _Literal_Cannot_Open_Smp_File[sizeof( "Cannot open sample file: %s" )]; 
 extern char  _Literal_Cannot_Process_Ovly[sizeof( "Cannot process overlay information for '%s'" )]; 
 extern char  _Literal_Dip_Find_Failed[sizeof( "Cannot find a debug interface to load" )]; 
 extern char  _Literal_Dip_Info_Failed[sizeof( "Cannot load the symbolic information for '%s': %s" )]; 
 extern char  _Literal_Dip_Init_Failed[sizeof( "Cannot initialize the debug interface, status = %d" )]; 
 extern char  _Literal_Dip_Load_Failed[sizeof( "Cannot load the debug interface '%s': %s" )]; 
 extern char  _Literal_Exe_Has_Changed[sizeof( "'%s' has changed since the sample was taken" )]; 
 extern char  _Literal_Exe_Not_Found[sizeof( "Cannot find the image '%s'" )]; 
 extern char  _Literal_Incompat_Smp_File[sizeof( "'%s' is an incompatible sample file" )]; 
 extern char  _Literal_Incompat_Ver_Ovly[sizeof( "Incompatible version of overlay information in executable '%s'" )]; 
 extern char  _Literal_Invalid_Smp_File[sizeof( "'%s' is not a valid sample file" )]; 
 extern char  _Literal_Memfull[sizeof( "Out of memory" )]; 
 extern char  _Literal_Memfull_Realloc[sizeof( "Unable to reallocate memory" )]; 
 extern char  _Literal_Smp_File_IO_Err[sizeof( "An error occured while accessing '%s'" )]; 
 extern char  _Literal_Usage[sizeof( "Usage: wprof [-option] [sample_file]" )]; 
 extern char  _Literal_Usage1[sizeof( "Usage: wprof [-option] [sample_file]" )]; 
 extern char  _Literal_Usage2[sizeof( "  where option is one or more of" )]; 
 extern char  _Literal_Usage3[sizeof( "    -DIP=dip_name" )]; 
 extern char  _Literal_Usage4[sizeof( "    -?" )]; 
 extern char  _Literal_Usage5[sizeof( "    -Help" )]; 
 extern char  _Literal_Usage6[sizeof( "  for dos systems only" )]; 
 extern char  _Literal_Usage7[sizeof( "    -NOGraphicsmouse" )]; 
 extern char  _Literal_Usage8[sizeof( "    -NOCHarremap" )]; 
 extern char  _Literal_Empty_Str[sizeof( "" )]; 
 extern char  _Literal_File_Does_Not_Exist[sizeof( "File '%s' does not exist" )]; 
 extern char  _Literal_Bad_System_Load[sizeof( "Could not start the command processor" )]; 
 extern char  _Literal_About_WPROF[sizeof( "About WPROF" )]; 
 extern char  _Literal_WPROF_TITLE[sizeof( "WATCOM Profiler" )]; 
 extern char  _Literal_Enter_Sample[sizeof( "Enter Sample File Name" )]; 
 extern char  _Literal_OK_Str[sizeof( "OK" )]; 
 extern char  _Literal_Cancel_Str[sizeof( "Cancel" )]; 
 extern char  _Literal_Unknown_Image[sizeof( "***Unknown_Image" )]; 
 extern char  _Literal_Unknown_Module[sizeof( "***Unknown_Module" )]; 
 extern char  _Literal_Unknown_File[sizeof( "***Unknown_File" )]; 
 extern char  _Literal_Unknown_Routine[sizeof( "***Unknown_Routine" )]; 
 extern char  _Literal_Gathered_Images[sizeof( "***Gathered_Images" )]; 
 extern char  _Literal_Gathered_Modules[sizeof( "***Gathered_Modules" )]; 
 extern char  _Literal_Gathered_Files[sizeof( "***Gathered_Files" )]; 
 extern char  _Literal_Gathered_Routines[sizeof( "***Gathered_Routines" )]; 
 extern char  _Literal_No_Symbol_Info[sizeof( "'%s' does not have symbol information" )]; 
 extern char  _Literal_No_Routine_Names[sizeof( "'%s' does not have any routines" )]; 
 extern char  _Literal_Src_File_Not_Found[sizeof( "Source file '%s' not found" )]; 
 extern char  _Literal_Src_File_Not_Known[sizeof( "Source file is not known" )]; 
 extern char  _Literal_Rel_Header[sizeof( "Rel:" )]; 
 extern char  _Literal_Abs_Header[sizeof( "Abs:" )]; 
 extern char  _Literal_Sample_Data[sizeof( "Sample: %s" )]; 
 extern char  _Literal_Smp_File_Raw_Err[sizeof( "Sample file '%s' has an incorrect number of samples" )]; 
 extern char  _Literal_Sample_Header[sizeof( "Sample:  " )]; 
 extern char  _Literal_Image_Header[sizeof( "Image:  " )]; 
 extern char  _Literal_Module_Header[sizeof( "Module:  " )]; 
 extern char  _Literal_File_Header[sizeof( "File:  " )]; 
 extern char  _Literal_Routine_Header[sizeof( "Routine:  " )]; 
 extern char  _Literal_Source_Header[sizeof( "Source Lines" )]; 
 extern char  _Literal_Image_Names[sizeof( "Image Names" )]; 
 extern char  _Literal_Module_Names[sizeof( "Module Names" )]; 
 extern char  _Literal_File_Names[sizeof( "File Names" )]; 
 extern char  _Literal_Routine_Names[sizeof( "Routine Names" )]; 
 extern char  _Literal_Source_Line[sizeof( "Source Line" )]; 
 extern char  _Literal_Assembler_Instructions[sizeof( "Assembler Instructions" )]; 
 extern char  _Literal_Options[sizeof( "Options" )]; 
 extern char  _Literal_Cmd_Option_Not_Valid[sizeof( "Command line option '%s' is not valid" )]; 
 extern char  _Literal_Unable_To_Open_Src[sizeof( "Unable to open source file" )]; 
 extern char  _Literal_Unable_To_Open_Help[sizeof( "Unable to open help file '%s'" )]; 
 extern char  _Literal_Convert_Data[sizeof( "Convert Data to a File" )]; 
 extern char  _Literal_Convert_File_Name[sizeof( "Enter Convert File Name" )]; 
 extern char  _Literal_Mad_Init_Failed[sizeof( "Cannot initialize the machine specific interface" )]; 
 extern char  _Literal_LDS_FSEEK_FAILED[sizeof( "File seek failed" )]; 
 extern char  _Literal_LDS_FREAD_FAILED[sizeof( "File read failed" )]; 
 extern char  _Literal_LDS_FOPEN_FAILED[sizeof( "File open failed" )]; 
 extern char  _Literal_LMS_INVALID_MAD_VERSION[sizeof( "Invalid machine architecture version" )]; 
 extern char  _Literal_LMS_INVALID_MAD[sizeof( "Invalid machine architecture file" )]; 
 extern char  _Literal_LMS_UNREGISTERED_MAD[sizeof( "Unknown machine architecture" )]; 
 extern char  _Literal_LMS_RECURSIVE_MAD_FAILURE[sizeof( "Can not recover from machine architecture file failures" )]; 


 
 
extern void *ProfAlloc(size_t size);
extern void ProfFree(void *ptr);

extern char 	*AboutMessage[];
extern int	AboutSize;

 static  bool aboutEventProc( a_window *, gui_event, void * );
 static  int AboutNumRows( a_window * );
 static  bint aboutGetLine( a_window *, int, int, wnd_line_piece * );

static a_window *   aboutWindow =  0 ;
static bint         aboutOn =  1 ;



wnd_info AboutInfo = {
    aboutEventProc,
    NoRefresh,
    aboutGetLine,
    NoMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    AboutNumRows,
    NoNextRow,
    NoNotify,
    0,
     0, 0 ,
};



extern void AboutOpen()

{
    if( aboutWindow ==  0  ) {
        aboutWindow = WndCreate(
				 (_Literal_WPROF_TITLE) ,
				&AboutInfo,
                                  ((wnd_class)-1) ,
				  0  );
    }
    if( aboutWindow !=  0  ) {
        WndShowWindow( aboutWindow );
    }
}



extern void AboutClose()

{
    a_window *  wnd;

    if( aboutWindow !=  0  ) {
        wnd = aboutWindow;
        WndClose( wnd );
    }
}



 static  int AboutNumRows( a_window * wnd )

{
    int     ret_size;

    wnd=wnd;
    if( aboutOn ) {
        ret_size = AboutSize;
 
 
 
        return( ret_size );
    }
    return( 0 );
}



 static  bint aboutGetLine( a_window * wnd, int row, int piece,
                                      wnd_line_piece * line )

{
 

    wnd=wnd;
    if( piece != 0 || !aboutOn ) return( B_FALSE );
    if( row >= AboutSize ) {


 
 
 
 
 
        return( B_FALSE );
    }
    line->text = AboutMessage[ row ];
    line->indent = (  (((struct aui_private_window_structure *)(wnd))->width )  - WndExtentX( wnd, line->text ) ) / 2;
    return( B_TRUE );
}



 static  bool aboutEventProc( a_window * wnd, gui_event gui_ev, void * parm )

{
    wnd=wnd;
    parm=parm;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        return(  1  );
    case GUI_DESTROY :
        aboutWindow =  0 ;
        return(  1  );
    }
    return(  0  );
}



extern void AboutSetOff()

{
    aboutOn = B_FALSE;
    if( aboutWindow !=  0  ) {
        WndZapped( aboutWindow );
    }
}



extern void DlgAbout()

{
    char *      about_data;
    char *      about_rover;
    int         about_len;
    int         index;

    about_len = 0;
    for( index = 0; index < AboutSize; ++index ) {
        about_len += strlen( AboutMessage[index] ) + 1;
    }
    about_data = ProfAlloc( about_len+1 );
    about_rover = about_data;
    for( index = 0; index < AboutSize; ++index ) {
        about_len = strlen( AboutMessage[index] );
        memcpy( about_rover, AboutMessage[index], about_len );
        about_rover += about_len;
        *about_rover++ = '\r';
    }
    *about_rover =  0 ;
    WndDisplayMessage( about_data,  (_Literal_About_WPROF) , GUI_INFORMATION );
    ProfFree( about_data );
}
