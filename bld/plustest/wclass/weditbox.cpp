#include "weditbox.hpp"

WEXPORT WEditBox::WEditBox( WWindow* parent, const WRect& r, char* text, WStyle wstyle )
	: WControl( parent, "EDIT", r, text, wstyle )
{
}

WEXPORT WEditBox::~WEditBox()
{
}

bool WEXPORT WEditBox::processCmd( WORD id, WORD code )
{
	return WControl::processCmd( id, code );
}
