// Geometric Tools, LLC
// Copyright (c) 1998-2010
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 4.10.0 (2009/11/18)

#include "Wm4FoundationPCH.h"
#include "Wm4IntrSegment2Segment2.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrSegment2Segment2<Real>::IntrSegment2Segment2 (
    const Segment2<Real>& rkSegment0, const Segment2<Real>& rkSegment1)
    :
    m_pkSegment0(&rkSegment0),
    m_pkSegment1(&rkSegment1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment2<Real>& IntrSegment2Segment2<Real>::GetSegment0 () const
{
    return *m_pkSegment0;
}
//----------------------------------------------------------------------------
template <class Real>
const Segment2<Real>& IntrSegment2Segment2<Real>::GetSegment1 () const
{
    return *m_pkSegment1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment2Segment2<Real>::Test ()
{
    Vector2<Real> kDiff;
    Real afParameter[2];
    m_iIntersectionType = Classify(afParameter,&kDiff,0);

    if (m_iIntersectionType == IT_POINT)
    {
        // Test whether the line-line intersection is on the segments.
        if (Math<Real>::FAbs(afParameter[0]) <= m_pkSegment0->Extent
        &&  Math<Real>::FAbs(afParameter[1]) <= m_pkSegment1->Extent)
        {
            m_iQuantity = 1;
        }
        else
        {
            m_iQuantity = 0;
            m_iIntersectionType = IT_EMPTY;
        }
    }
    else if (m_iIntersectionType == IT_SEGMENT)
    {
        m_iQuantity = INT_MAX;
    }
    else
    {
        m_iQuantity = 0;
    }

    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrSegment2Segment2<Real>::Find ()
{
    Vector2<Real> kDiff;
    Real afParameter[2];
    m_iIntersectionType = Classify(afParameter,&kDiff,0);

    if (m_iIntersectionType == IT_POINT)
    {
        // Test whether the line-line intersection is on the segments.
        if (Math<Real>::FAbs(afParameter[0]) <= m_pkSegment0->Extent
        &&  Math<Real>::FAbs(afParameter[1]) <= m_pkSegment1->Extent)
        {
            m_iQuantity = 1;
            m_kPoint = m_pkSegment0->Origin +
                afParameter[0]*m_pkSegment0->Direction;
        }
        else
        {
            m_iQuantity = 0;
            m_iIntersectionType = IT_EMPTY;
        }
    }
    else if (m_iIntersectionType == IT_SEGMENT)
    {
        m_iQuantity = INT_MAX;
    }
    else
    {
        m_iQuantity = 0;
    }

    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrSegment2Segment2<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrSegment2Segment2<Real>::GetPoint () const
{
    return m_kPoint;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrSegment2Segment2<Real>::Classify (Real* afS, Vector2<Real>* pkDiff,
    Vector2<Real>* pkDiffN)
{
    // The intersection of two lines is a solution to P0+s0*D0 = P1+s1*D1.
    // Rewrite this as s0*D0 - s1*D1 = P1 - P0 = Q.  If D0.Dot(Perp(D1)) = 0,
    // the lines are parallel.  Additionally, if Q.Dot(Perp(D1)) = 0, the
    // lines are the same.  If D0.Dot(Perp(D1)) is not zero, then
    //   s0 = Q.Dot(Perp(D1))/D0.Dot(Perp(D1))
    // produces the point of intersection.  Also,
    //   s1 = Q.Dot(Perp(D0))/D0.Dot(Perp(D1))

    Vector2<Real> kDiff = m_pkSegment1->Origin - m_pkSegment0->Origin;
    if (pkDiff)
    {
        *pkDiff = kDiff;
    }

    Real fD0DotPerpD1 = m_pkSegment0->Direction.DotPerp(
        m_pkSegment1->Direction);
    if (Math<Real>::FAbs(fD0DotPerpD1) > Math<Real>::ZERO_TOLERANCE)
    {
        // lines intersect in a single point
        if (afS)
        {
            Real fInvD0DotPerpD1 = ((Real)1.0)/fD0DotPerpD1;
            Real fDiffDotPerpD0 = kDiff.DotPerp(m_pkSegment0->Direction);
            Real fDiffDotPerpD1 = kDiff.DotPerp(m_pkSegment1->Direction);
            afS[0] = fDiffDotPerpD1*fInvD0DotPerpD1;
            afS[1] = fDiffDotPerpD0*fInvD0DotPerpD1;
        }
        return IT_POINT;
    }

    // lines are parallel
    kDiff.Normalize();
    if (pkDiffN)
    {
        *pkDiffN = kDiff;
    }

    Real fDiffNDotPerpD1 = kDiff.DotPerp(m_pkSegment1->Direction);
    if (Math<Real>::FAbs(fDiffNDotPerpD1) <= Math<Real>::ZERO_TOLERANCE)
    {
        // lines are colinear
        return IT_SEGMENT;
    }

    // lines are parallel, but distinct
    return IT_EMPTY;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrSegment2Segment2<float>;

template WM4_FOUNDATION_ITEM
class IntrSegment2Segment2<double>;
//----------------------------------------------------------------------------
}
