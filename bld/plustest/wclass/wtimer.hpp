#ifndef wtimer_class
#define wtimer_class

#include "wwindow.hpp"
#include "wobjmap.hpp"

WCLASS WTimer;
typedef void (WObject::*cbt)( WTimer* o, DWORD sysTime );

WCLASS WTimer : public WObject
{
	public:
		WEXPORT WTimer( WObject* owner, cbt notify );
		WEXPORT ~WTimer();
		bool WEXPORT start( WORD interval, int count=0 );
		bool WEXPORT stop();
		void WEXPORT tick( DWORD sysTime );
		static WObjectMap WEXPORT _timerMap;
	private:
		WObject* _owner;
		cbt		_notify;
		WORD	_id;
		int		_count;
};

#endif