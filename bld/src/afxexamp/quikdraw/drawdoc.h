#ifndef _DRAWDOC_H_INCLUDED
#define _DRAWDOC_H_INCLUDED

#include "shapes.h"

class CDrawingDoc : public CDocument {
protected:
    CSize                               m_sizeDrawing;
    CTypedPtrList< CObList, CShape * >  m_listShapes;
    
    DECLARE_DYNCREATE( CDrawingDoc )

public:
    CDrawingDoc();

    virtual BOOL    OnNewDocument();
    virtual void    Serialize( CArchive &ar );

    void        AddShape( CShape *pShape );
    CSize       GetDrawingSize() const;
    POSITION    GetFirstShapePosition() const;
    CShape      *GetNextShape( POSITION &rPosition ) const;
    CShape      *HitTest( CPoint point ) const;
};

inline void CDrawingDoc::AddShape( CShape *pShape )
{
    m_listShapes.AddTail( pShape );
}

inline CSize CDrawingDoc::GetDrawingSize() const
{
    return( m_sizeDrawing );
}

inline POSITION CDrawingDoc::GetFirstShapePosition() const
{
    return( m_listShapes.GetHeadPosition() );
}

inline CShape *CDrawingDoc::GetNextShape( POSITION &rPosition ) const
{
    return( m_listShapes.GetNext( rPosition ) );
}

#endif /* _DRAWDOC_H_INCLUDED */
