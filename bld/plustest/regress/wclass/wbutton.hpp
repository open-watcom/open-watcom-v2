#ifndef wbutton_class
#define wbutton_class

#include "wcontrol.hpp"

WCLASS WButton : public WControl
{
	public:
		WEXPORT WButton( WWindow* parent, const WRect& r, char* text, WStyle wstyle );
		WEXPORT ~WButton();
		void WEXPORT onClick( WObject* obj, cb click );
		void WEXPORT onDblClick( WObject* obj, cb dblClick );
		virtual bool WEXPORT processCmd( WORD id, WORD code );
	private:
		WObject*	_clickClient;
		cb		_click;
		WObject*	_dblClickClient;
		cb		_dblClick;
};

#endif //wbutton_class
