#ifndef wserver_class
#define wserver_class

#include "windows.h"
#include "ddeml.h"

#include "wobject.hpp"
#include "wstring.hpp"

WCLASS WServer : public WObject
{
	public:
		WEXPORT WServer( char* service, HANDLE inst, WObject* owner, sbc notify );
		WEXPORT ~WServer();
		bool WEXPORT xtConnect( HSZ hsz1, HSZ hsz2 );
		HDDEDATA WEXPORT xtRequest( UINT fmt, HSZ hsz1, HSZ hsz2 );
	private:
		HSZ		_service;
		DWORD	_procid;
		WObject* _owner;
		sbc		_notify;
		bool	_ok;
};

#endif