#define NOAUTOPROCS
#include <windows.h>
#include "testcht.h"
#include "chart.h"

BOOL _EXPORT FAR PASCAL FetchData( int, LPFLOAT );

void CreateMe( HWND hwnd )
{
CALLBACKPTR             cbp;

#ifdef __WINDOWS_386__
    DefineUserProc16( GETPROC_USERDEFINED_1, (PROCPTR) FetchData,
            UDP16_WORD, UDP16_DWORD, UDP16_ENDLIST );
#endif
    cbp = GetProc16( (PROCPTR) FetchData, GETPROC_USERDEFINED_1 );
    SetWindowWord( hwnd, 0,
                    ChartAdd( MakeProcInstance( cbp,
                    GetWindowWord( hwnd, GWW_HINSTANCE ) ) ) );
}
