#include "stdafx.h"
#include "textedit.h"
#include "textdoc.h"
#include "textview.h"

IMPLEMENT_DYNCREATE( CTextDoc, CDocument )

CTextDoc::CTextDoc()
{
}

void CTextDoc::Serialize( CArchive &ar )
{
    CDocument::Serialize( ar );

    // Call the SerializeRaw method on the view to load or save the document.
    POSITION position = GetFirstViewPosition();
    ASSERT( position != NULL );
    CTextView *pView = (CTextView *)GetNextView( position );
    ASSERT_VALID( pView );
    ASSERT( pView->IsKindOf( RUNTIME_CLASS( CTextView ) ) );
    pView->SerializeRaw( ar );
}
