#include "stdafx.h"
#include "quikdraw.h"
#include "shapes.h"
#include <math.h>

IMPLEMENT_SERIAL( CShape, CObject, 1 )

CShape::CShape()
{
}

void CShape::Draw( CDC *pDC )
{
    // This should be overridden in all derived classes.
    UNUSED_ALWAYS( pDC );
}

BOOL CShape::PtInShape( CPoint point ) const
{
    // This should be overridden in all derived classes.
    UNUSED_ALWAYS( point );
    return( FALSE );
}

void CShape::Serialize( CArchive &ar )
{
    CObject::Serialize( ar );

    if( ar.IsStoring() ) {
        ar << m_point1;
        ar << m_point2;
        ar << m_crLine;
        ar << m_crFill;
    } else {
        ar >> m_point1;
        ar >> m_point2;
        ar >> m_crLine;
        ar >> m_crFill;
    }
}

IMPLEMENT_SERIAL( CLine, CShape, 1 )

CLine::CLine()
{
}

void CLine::Draw( CDC *pDC )
{
    CPen    pen( PS_SOLID, 1, m_crLine );
    CPen    *pOldPen = pDC->SelectObject( &pen );
    
    pDC->MoveTo( m_point1 );
    pDC->LineTo( m_point2 );

    pDC->SelectObject( pOldPen );
}

BOOL CLine::PtInShape( CPoint point ) const
{
    // Check that the point is in the bounding box.
    CRect rect( m_point1, m_point2 );
    rect.NormalizeRect();
    if( !rect.PtInRect( point ) ) {
        return( FALSE );
    }

    // Calculate the distance from the line to the point.
    double A = m_point1.y - m_point2.y;
    double B = m_point2.x - m_point1.x;
    double C = m_point1.x * m_point2.y - m_point2.x * m_point1.y;
    double nDistance = fabs( A * point.x + B * point.y + C );
    if( A != 0.0 || B != 0.0 ) {
        nDistance /= sqrt( A * A + B * B );
    }

    // Only register as being inside the shape if the distance from the line is small.
    return( nDistance < 10.0 );
}

IMPLEMENT_SERIAL( CRectangle, CShape, 1 )

CRectangle::CRectangle()
{
}

void CRectangle::Draw( CDC *pDC )
{
    CPen    pen( PS_SOLID, 1, m_crLine );
    CBrush  br( m_crFill );
    CPen    *pOldPen = pDC->SelectObject( &pen );
    CBrush  *pOldBrush = pDC->SelectObject( &br );
    
    pDC->Rectangle( m_point1.x, m_point1.y, m_point2.x, m_point2.y );

    pDC->SelectObject( pOldPen );
    pDC->SelectObject( pOldBrush );
}

BOOL CRectangle::PtInShape( CPoint point ) const
{
    // Check that the point is in the bounding box.
    CRect rect( m_point1, m_point2 );
    rect.NormalizeRect();
    return( rect.PtInRect( point ) );
}

IMPLEMENT_SERIAL( CEllipse, CShape, 1 )

CEllipse::CEllipse()
{
}

void CEllipse::Draw( CDC *pDC )
{
    CPen    pen( PS_SOLID, 1, m_crLine );
    CBrush  br( m_crFill );
    CPen    *pOldPen = pDC->SelectObject( &pen );
    CBrush  *pOldBrush = pDC->SelectObject( &br );
    
    pDC->Ellipse( m_point1.x, m_point1.y, m_point2.x, m_point2.y );

    pDC->SelectObject( pOldPen );
    pDC->SelectObject( pOldBrush );
}

BOOL CEllipse::PtInShape( CPoint point ) const
{
    // Check that the point is in the bounding box.
    CRect rect( m_point1, m_point2 );
    rect.NormalizeRect();
    if( !rect.PtInRect( point ) ) {
        return( FALSE );
    }

    // Check whether the point is inside the ellipse.
    int h = (m_point1.x + m_point2.x) / 2;
    int k = (m_point1.y + m_point2.y) / 2;
    int a = abs(m_point2.x - m_point1.x) / 2;
    int b = abs(m_point2.y - m_point1.y) / 2;
    int nLHS = b * b * (point.x - h) * (point.x - h) +
               a * a * (point.y - k) * (point.y - k);
    int nRHS = a * a * b * b;
    return( nLHS <= nRHS );
}
