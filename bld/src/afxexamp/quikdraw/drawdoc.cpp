#include "stdafx.h"
#include "quikdraw.h"
#include "drawdoc.h"

IMPLEMENT_DYNCREATE( CDrawingDoc, CDocument )

CDrawingDoc::CDrawingDoc()
{
}

BOOL CDrawingDoc::OnNewDocument()
{
    if( !CDocument::OnNewDocument() ) {
        return( FALSE );
    }

    m_sizeDrawing = CSize( 500, 500 );
    return( TRUE );
}

void CDrawingDoc::Serialize( CArchive &ar )
{
    CDocument::Serialize( ar );

    // Serialize the drawing size.
    if( ar.IsStoring() ) {
        ar << m_sizeDrawing;
    } else {
        ar >> m_sizeDrawing;
    }

    // Serialize the shapes in the drawing.
    m_listShapes.Serialize( ar );
}

CShape *CDrawingDoc::HitTest( CPoint point ) const
{
    POSITION position = m_listShapes.GetHeadPosition();
    while( position != NULL ) {
        CShape *pShape = m_listShapes.GetNext( position );
        if( pShape->PtInShape( point ) ) {
            return( pShape );
        }
    }
    return( NULL );
}
