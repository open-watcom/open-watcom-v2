#ifndef wfiledialog_class
#define wfiledialog_class

extern "C" {
	#include <windows.h>
	#include "commdlg.h"
	#include "cderr.h"
}

#define WFOpenNew OFN_HIDEREADONLY|OFN_PATHMUSTEXIST
#define WFOpenExisting OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST
#define WFSaveDefault OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT

#include "wdialog.hpp"

WCLASS WWindow;
WCLASS WFileDialog  : public WDialog
{
	public:
#ifndef CPPV
		WEXPORT WFileDialog( WWindow* parent, char* filter=NIL );
#else
		WEXPORT WFileDialog( HWND hwnd, char* filter=NIL );
#endif
		WEXPORT ~WFileDialog();
		char* WEXPORT getOpenFileName( char* filename=NIL, char* title=NIL, int style=WFOpenExisting );
		char* WEXPORT getSaveFileName( char* filename=NIL, char* title=NIL, int style=WFSaveDefault );
	private:
		OPENFILENAME	_ofn;
		char	_fileName[200];
		char	_titleName[200];
		char	_dirName[200];
		char	_cwd[200];
		void	makeDialog( HWND hwnd, char* filter );
		void	init( char* filename, char* title );
};

#endif
