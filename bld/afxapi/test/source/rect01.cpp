#include "stdafx.h"

int main()
{
    CRect rect( 20, 40, 10, 20 );
    rect.NormalizeRect();
    if( rect.left != 10 ) _fail;
    if( rect.top != 20 ) _fail;
    if( rect.right != 20 ) _fail;
    if( rect.bottom != 40 ) _fail;
    if( rect.TopLeft().x != 10 ) _fail;
    if( rect.TopLeft().y != 20 ) _fail;
    if( rect.BottomRight().x != 20 ) _fail;
    if( rect.BottomRight().y != 40 ) _fail;
    if( rect.Width() != 10 ) _fail;
    if( rect.Height() != 20 ) _fail;
    if( rect.Size().cx != 10 ) _fail;
    if( rect.Size().cy != 20 ) _fail;
    if( rect.CenterPoint().x != 15 ) _fail;
    if( rect.CenterPoint().y != 30 ) _fail;

    rect.MoveToX( 0 );
    if( rect.left != 0 ) _fail;
    if( rect.top != 20 ) _fail;
    if( rect.right != 10 ) _fail;
    if( rect.bottom != 40 ) _fail;

    rect.MoveToY( 0 );
    if( rect.left != 0 ) _fail;
    if( rect.top != 0 ) _fail;
    if( rect.right != 10 ) _fail;
    if( rect.bottom != 20 ) _fail;

    rect.MoveToXY( 10, 20 );
    if( rect.left != 10 ) _fail;
    if( rect.top != 20 ) _fail;
    if( rect.right != 20 ) _fail;
    if( rect.bottom != 40 ) _fail;

    _PASS;
}
