#include "wgroupbx.hpp"

WEXPORT WGroupBox::WGroupBox( WWindow* parent, const WRect& r, char* text )
	: WButton( parent, r, text, BS_GROUPBOX )
{
}

WEXPORT WGroupBox::~WGroupBox()
{
}
