typedef struct an_MDI_icon {
    char        *file_name;
    unsigned    short current_icon;
    HBITMAP     XOR_bitmap;
    HBITMAP     AND_bitmap;
} an_MDI_icon;

extern HWND MdiReadIcon(FILE *,char *,char *,long );
extern HWND MdiOpenIcon(void);
extern long _EXPORT FAR PASCAL MdiIconProc(HWND,UINT,WPARAM,LPARAM);
