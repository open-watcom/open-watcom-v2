#include "stdafx.h"
#include "textedit.h"
#include "textview.h"

IMPLEMENT_DYNCREATE( CTextView, CEditView )

BEGIN_MESSAGE_MAP( CTextView, CEditView )
    // Handle the following commands with the framework-provided handlers.
    ON_COMMAND( ID_FILE_PRINT, OnFilePrint )
    ON_COMMAND( ID_FILE_PRINT_DIRECT, OnFilePrint )
    ON_COMMAND( ID_FILE_PRINT_PREVIEW, OnFilePrintPreview )
END_MESSAGE_MAP()

CTextView::CTextView()
{
}
