#ifndef _SHAPES_H_INCLUDED
#define _SHAPES_H_INCLUDED

class CShape : public CObject {
protected:
    CPoint      m_point1;
    CPoint      m_point2;
    COLORREF    m_crLine;
    COLORREF    m_crFill;
    
    DECLARE_SERIAL( CShape )

protected:
    CShape();

public:
    virtual void    Draw( CDC *pDC );
    virtual BOOL    PtInShape( CPoint point ) const;

    virtual void    Serialize( CArchive &ar );

    void    Offset( CSize size );
    void    SetFillColor( COLORREF cr );
    void    SetLineColor( COLORREF cr );
    void    SetPoint1( CPoint point );
    void    SetPoint2( CPoint point );
};


class CLine : public CShape {
    DECLARE_SERIAL( CLine )

public:
    CLine();

    virtual void    Draw( CDC *pDC );
    virtual BOOL    PtInShape( CPoint point ) const;
};

class CRectangle : public CShape {
    DECLARE_SERIAL( CRectangle )

public:
    CRectangle();

    virtual void    Draw( CDC *pDC );
    virtual BOOL    PtInShape( CPoint point ) const;
};

class CEllipse : public CShape {
    DECLARE_SERIAL( CEllipse )

public:
    CEllipse();

    virtual void    Draw( CDC *pDC );
    virtual BOOL    PtInShape( CPoint point ) const;
};

inline void CShape::Offset( CSize size )
{
    m_point1 += size;
    m_point2 += size;
}

inline void CShape::SetFillColor( COLORREF cr )
{
    m_crFill = cr;
}

inline void CShape::SetLineColor( COLORREF cr )
{
    m_crLine = cr;
}

inline void CShape::SetPoint1( CPoint point )
{
    m_point1 = point;
}

inline void CShape::SetPoint2( CPoint point )
{
    m_point2 = point;
}

#endif /* _SHAPES_H_INCLUDED */
