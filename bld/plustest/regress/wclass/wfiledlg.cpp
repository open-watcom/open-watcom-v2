#include "wfiledlg.hpp"
#include "wwindow.hpp"

extern "C" {
	#include <string.h>
	#ifdef __WATCOM_CPLUSPLUS__
		#include <direct.h>
	#else
		#include <dir.h>
	#endif
}

#ifndef CPPV
WEXPORT WFileDialog::WFileDialog( WWindow* parent, char* filter )
{
	makeDialog( parent->handle(), filter );
}
#else
WEXPORT WFileDialog::WFileDialog( HWND hwnd, char* filter )
{
	makeDialog( hwnd, filter );
}
#endif

void WFileDialog::makeDialog( HWND hwnd, char* filter )
{
	static char filts[] = { "All files(*.*)\0*.*\0\0" };
	ifnil( filter ) filter = filts;
	memset( &_ofn, 0, sizeof( _ofn ) );
	_ofn.lStructSize = sizeof( OPENFILENAME );
	_ofn.hwndOwner = hwnd;
	_ofn.lpstrFilter = filter;
	_ofn.nFilterIndex = 1;
	_ofn.lpstrFile = _fileName;
	_ofn.nMaxFile = sizeof( _fileName );
	_fileName[0] = '\0';
	_ofn.lpstrFileTitle = _titleName;
	_ofn.nMaxFileTitle = sizeof( _titleName );
	_ofn.lpstrInitialDir = _dirName;
	_dirName[0] ='\0';
	getcwd( _cwd, sizeof( _cwd ) );
}

WEXPORT WFileDialog::~WFileDialog()
{
	chdir( _cwd );
}

void WFileDialog::init( char* filename, char* title )
{
	ifptr( title ) {
		_ofn.lpstrTitle = title;
	}
	ifptr( filename ) {
		strcpy( _fileName, filename );
	}
}

char* WEXPORT WFileDialog::getOpenFileName( char* filename, char* title, int style )
{
	init( filename, title );
	_ofn.Flags = style;
	if( GetOpenFileName( &_ofn ) ) {
		return AnsiLower(_fileName);
	}
	return NIL;
}

char* WEXPORT WFileDialog::getSaveFileName( char* filename, char* title, int style )
{
	_ofn.Flags = style;
	init( filename, title );
	if( GetSaveFileName( &_ofn ) ) {
		return AnsiLower(_fileName);
	}
	return NIL;
}

