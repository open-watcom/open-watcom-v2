#include "wtimer.hpp"

WObjectMap WEXPORT WTimer::_timerMap;
static int _timerId = 0;

extern "C" WORD _export _far _pascal timerProc( HWND /*hwin*/, UINT /*msg*/, int id, DWORD sysTime )
{
	WTimer* timer = (WTimer*)WTimer::_timerMap.findThis( id );
	ifptr( timer ) {
		timer->tick( sysTime );
	}
	return 0;
}

WEXPORT WTimer::WTimer( WObject* owner, cbt notify )
	: _owner( owner )
	, _notify( notify )
	, _id( 0 )
	, _count( 0 )
{
}

WEXPORT WTimer::~WTimer()
{
}

bool WEXPORT WTimer::start( WORD interval, int count )
{
	_count = count;
	_id = SetTimer( NIL, _timerId++, interval, (TIMERPROC) timerProc );
	_timerMap.setThis( this, _id );
	return _id != 0;
}

bool WEXPORT WTimer::stop()
{
	_timerMap.clearThis( this );
	return KillTimer( NIL, _id ) != 0;
}

void WEXPORT WTimer::tick( DWORD sysTime )
{
	if( isptr( _owner ) && isptr( _notify ) ) {
		(_owner->*_notify)( this, sysTime );
	}
	if( _count != 0 ) {
		_count -= 1;
		if( _count == 0 ) {
			stop();
		}
	}
}
