#include "wpen.hpp"

WEXPORT WPen::WPen()
{
	setHandle( (HANDLE)CreatePen( PS_SOLID, 1, RGB(0,0,0) ) );
}

WEXPORT WPen::~WPen()
{
}

