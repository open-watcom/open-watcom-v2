#include "dump.h"
/*
   Date		By		Reason
   ====		==		======
   18-dec-91	C.G.Eisler	defined
*/

typedef int BOOL;

void SetClipboardMsgs( BOOL val ) GOOD;
void SetDDEMsgs( BOOL val ) GOOD;
void SetInitMsgs( BOOL val ) GOOD;
void SetInputMsgs( BOOL val ) GOOD;
void SetMDIMsgs( BOOL val ) GOOD;
void SetMouseMsgs( BOOL val ) GOOD;
void SetNCMiscMsgs( BOOL val ) GOOD;
void SetNCMouseMsgs( BOOL val ) GOOD;
void SetOtherMsgs( BOOL val ) GOOD;
void SetSystemMsgs( BOOL val ) GOOD;
void SetUserMsgs( BOOL val ) GOOD;
void SetWindowMsgs( BOOL val ) GOOD;

typedef struct {
char flag;
void (*fn)( BOOL );
} filter;

typedef struct {
filter clipboard;
filter dde;
filter init;
filter input;
filter mdi;
filter mouse;
filter ncmisc;
filter ncmouse;
filter other;
filter system;
filter user;
filter window;
} _filters;

typedef union {
_filters filts;
filter array[ 12 ];
} filters;

void foobar( void )
{
    static filters Filters =
		{ {
		    { '1', SetClipboardMsgs },
		    { '1', SetDDEMsgs },
		    { '1', SetInitMsgs },
		    { '1', SetInputMsgs },
		    { '1', SetMDIMsgs },
		    { '1', SetMouseMsgs },
		    { '1', SetNCMiscMsgs },
		    { '1', SetNCMouseMsgs },
		    { '1', SetOtherMsgs },
		    { '1', SetSystemMsgs },
		    { '1', SetUserMsgs },
		    { '1', SetWindowMsgs }
		} };
    (*Filters.filts.clipboard.fn)( 1 );
    (*Filters.filts.dde.fn)( 1 );
    (*Filters.filts.init.fn)( 1 );
    (*Filters.filts.input.fn)( 1 );
    (*Filters.filts.mdi.fn)( 1 );
    (*Filters.filts.mouse.fn)( 1 );
    (*Filters.filts.ncmisc.fn)( 1 );
    (*Filters.filts.ncmouse.fn)( 1 );
    (*Filters.filts.other.fn)( 1 );
    (*Filters.filts.system.fn)( 1 );
    (*Filters.filts.user.fn)( 1 );
    (*Filters.filts.window.fn)( 1 );
    int i;
    for( i = 0 ; i < 12 ; i++ ) {
	(*Filters.array[i].fn)( 1 );
    }
}

void foobar2( void )
{
    static filters Filters =
		{
		     '1', SetClipboardMsgs,
		     '1', SetDDEMsgs,
		     '1', SetInitMsgs,
		     '1', SetInputMsgs,
		     '1', SetMDIMsgs,
		     '1', SetMouseMsgs,
		     '1', SetNCMiscMsgs,
		     '1', SetNCMouseMsgs,
		     '1', SetOtherMsgs,
		     '1', SetSystemMsgs,
		     '1', SetUserMsgs,
		     '1', SetWindowMsgs 
		};
    (*Filters.filts.clipboard.fn)( 1 );
    (*Filters.filts.dde.fn)( 1 );
    (*Filters.filts.init.fn)( 1 );
    (*Filters.filts.input.fn)( 1 );
    (*Filters.filts.mdi.fn)( 1 );
    (*Filters.filts.mouse.fn)( 1 );
    (*Filters.filts.ncmisc.fn)( 1 );
    (*Filters.filts.ncmouse.fn)( 1 );
    (*Filters.filts.other.fn)( 1 );
    (*Filters.filts.system.fn)( 1 );
    (*Filters.filts.user.fn)( 1 );
    (*Filters.filts.window.fn)( 1 );
    int i;
    for( i = 0 ; i < 12 ; i++ ) {
	(*Filters.array[i].fn)( 1 );
    }
}
int main( void )
{
    foobar();
    foobar2();
    CHECK_GOOD( 744 );
    return 0;
}
