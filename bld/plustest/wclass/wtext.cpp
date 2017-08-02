#include "wtext.hpp"

WEXPORT WText::WText( WWindow* parent, const WRect& r, char* text, DWORD style )
	: WControl( parent, "STATIC", r, text, style )
{
}

WEXPORT WText::~WText()
{
}
