#include "wbrush.hpp"

WEXPORT WBrush::WBrush()
{
	setHandle( (HANDLE)CreateSolidBrush( RGB( 255, 0, 0 ) ) );
}

WEXPORT WBrush::~WBrush()
{
}

