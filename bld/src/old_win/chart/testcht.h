#define MENU_ABOUT              1

#ifdef __WINDOWS_386__
#define _EXPORT
typedef float *LPFLOAT;
extern HINDIR ChartAddHandle;
extern HINDIR ChartDrawHandle;
extern HINDIR ChartBarClickHandle;
extern HINDIR ChartSetBarColorHandle;
extern HINDIR ChartGetBarColorHandle;
extern HINDIR ChartDeleteHandle;
extern HINDIR ChartCloseUseHandle;
extern HINDIR ChartInitUseHandle;
#else
#define _EXPORT __export
typedef float far *LPFLOAT;
#endif

