#include "stdafx.h"

#define TEST_HGDIOBJ    ((HGDIOBJ)0x12345678)
#define TEST_HDC        ((HDC)0x12345678)
#define TEST_HMENU      ((HMENU)0x12345678)
#define TEST_HWND       ((HWND)0x12345678)
#define TEST_HIMAGELIST ((HIMAGELIST)0x12345678)

int main()
{
    CGdiObject  testGdiObject;
    CDC         testDC;
    CMenu       testMenu;
    CWnd        testWnd;
    CImageList  testImageList;

    if( !testGdiObject.Attach( TEST_HGDIOBJ ) ) _fail;
    if( CGdiObject::FromHandle( TEST_HGDIOBJ ) != &testGdiObject ) _fail;
    if( testGdiObject.Detach() != TEST_HGDIOBJ ) _fail;
    if( CGdiObject::FromHandle( TEST_HGDIOBJ ) == &testGdiObject ) _fail;
    CGdiObject::DeleteTempMap();

    if( !testDC.Attach( TEST_HDC ) ) _fail;
    if( CDC::FromHandle( TEST_HDC ) != &testDC ) _fail;
    if( testDC.Detach() != TEST_HDC ) _fail;
    if( CDC::FromHandle( TEST_HDC ) == &testDC ) _fail;
    CDC::DeleteTempMap();

    if( !testMenu.Attach( TEST_HMENU ) ) _fail;
    if( CMenu::FromHandle( TEST_HMENU ) != &testMenu ) _fail;
    if( CMenu::FromHandlePermanent( TEST_HMENU ) != &testMenu ) _fail;
    if( testMenu.Detach() != TEST_HMENU ) _fail;
    if( CMenu::FromHandle( TEST_HMENU ) == &testMenu ) _fail;
    if( CMenu::FromHandlePermanent( TEST_HMENU ) != NULL ) _fail;
    CMenu::DeleteTempMap();

    if( !testWnd.Attach( TEST_HWND ) ) _fail;
    if( CWnd::FromHandle( TEST_HWND ) != &testWnd ) _fail;
    if( CWnd::FromHandlePermanent( TEST_HWND ) != &testWnd ) _fail;
    if( testWnd.Detach() != TEST_HWND ) _fail;
    if( CWnd::FromHandle( TEST_HWND ) == &testWnd ) _fail;
    if( CWnd::FromHandlePermanent( TEST_HWND ) != NULL ) _fail;
    CWnd::DeleteTempMap();

    if( !testImageList.Attach( TEST_HIMAGELIST ) ) _fail;
    if( CImageList::FromHandle( TEST_HIMAGELIST ) != &testImageList ) _fail;
    if( CImageList::FromHandlePermanent( TEST_HIMAGELIST ) != &testImageList ) _fail;
    if( testImageList.Detach() != TEST_HIMAGELIST ) _fail;
    if( CImageList::FromHandle( TEST_HIMAGELIST ) == &testImageList ) _fail;
    if( CImageList::FromHandlePermanent( TEST_HIMAGELIST ) != NULL ) _fail;
    CImageList::DeleteTempMap();

    _PASS;
}
