#include "wbrush.hpp"

WEXPORT WBrush::WBrush()
{
	setHandle( CreateSolidBrush( RGB( 255, 0, 0 ) ) );
}

WEXPORT WBrush::~WBrush()
{
}

