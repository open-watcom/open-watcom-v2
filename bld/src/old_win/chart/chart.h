typedef HANDLE chart_hld;

#ifdef __WINDOWS_386__
#define ChartAdd( fetch_data )  \
        InvokeIndirectFunction( ChartAddHandle, fetch_data )
#define ChartDraw( chart, win_hld, win_dc ) \
        InvokeIndirectFunction( ChartDrawHandle, chart, win_hld, win_dc )
#define ChartBarClick( x, y, chart ) \
        InvokeIndirectFunction( ChartBarClickHandle, x, y, chart )
#define ChartSetBarColor( bar, color, chart ) \
        InvokeIndirectFunction( ChartSetBarColorHandle, bar, color, chart )
#define ChartGetBarColor( bar, chart ) \
        InvokeIndirectFunction( ChartGetBarColorHandle, bar, chart )
#define ChartDelete( chart ) \
        InvokeIndirectFunction( ChartDeleteHandle, chart )
#define ChartCloseUse() InvokeIndirectFunction( ChartCloseUseHandle )
#define ChartInitUse() InvokeIndirectFunction( ChartInitUseHandle )
#else
#define WINEXP __export FAR PASCAL

chart_hld WINEXP ChartAdd( FARPROC fetch_data );
BOOL WINEXP ChartDraw( chart_hld chart, HWND win_hld, HDC win_dc );
int WINEXP ChartBarClick( int x, int y, chart_hld chart );
BOOL WINEXP ChartSetBarColor( int bar, COLORREF color, chart_hld chart );
COLORREF WINEXP ChartGetBarColor( int bar, chart_hld chart );
BOOL WINEXP ChartDelete( chart_hld chart );
void WINEXP ChartCloseUse( void );
void WINEXP ChartInitUse( void );
#endif
