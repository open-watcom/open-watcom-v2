#define FILE_SAVE               0
#define FILE_OPEN               1
#define FILE_SAVE_VERIFY        2

typedef struct fileopen {
BYTE    type;           /* kind of open (save/load) */
HWND    hwnd;           /* owner of dialog box */
LPSTR   name;           /* where to store name */
WORD    namelen;        /* length of name */
LPSTR   ext;            /* default extension */
LPSTR   title;          /* title of dialog box */
} FILEOPEN;

#if defined(__WINDOWS_386__) || defined(__NT__)
typedef struct fileopen *LPFILEOPEN;
#else
typedef struct fileopen __far *LPFILEOPEN;
#endif
