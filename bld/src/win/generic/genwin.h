typedef struct {
char far *cmdline;
} extra_data;

#define EXTRA_DATA_OFFSET       0

#define MENU_ABOUT              1
#define MENU_CMDSTR             2

#ifdef __WINDOWS_386__
#define _EXPORT
#else
#define _EXPORT __export
#endif
