// Geometric Tools, LLC
// Copyright (c) 1998-2010
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 4.10.0 (2009/11/18)

#include "Wm4FoundationPCH.h"
#include "Wm4IntrPlane3Plane3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrPlane3Plane3<Real>::IntrPlane3Plane3 (
    const Plane3<Real>& rkPlane0, const Plane3<Real>& rkPlane1)
    :
    m_pkPlane0(&rkPlane0),
    m_pkPlane1(&rkPlane1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Plane3<Real>::GetPlane0 () const
{
    return *m_pkPlane0;
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Plane3<Real>::GetPlane1 () const
{
    return *m_pkPlane1;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Plane3<Real>::Test ()
{
    // If Cross(N0,N1) is zero, then either planes are parallel and separated
    // or the same plane.  In both cases, 'false' is returned.  Otherwise, the
    // planes intersect.  To avoid subtle differences in reporting between
    // Test() and Find(), the same parallel test is used.  Mathematically,
    //   |Cross(N0,N1)|^2 = Dot(N0,N0)*Dot(N1,N1)-Dot(N0,N1)^2
    //                    = 1 - Dot(N0,N1)^2
    // The last equality is true since planes are required to have unit-length
    // normal vectors.  The test |Cross(N0,N1)| = 0 is the same as
    // |Dot(N0,N1)| = 1.  I test the latter condition in Test() and Find().

    Real fDot = m_pkPlane0->Normal.Dot(m_pkPlane1->Normal);
    return Math<Real>::FAbs(fDot) < (Real)1.0 - Math<Real>::ZERO_TOLERANCE;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Plane3<Real>::Find ()
{
    // If N0 and N1 are parallel, either the planes are parallel and separated
    // or the same plane.  In both cases, 'false' is returned.  Otherwise,
    // the intersection line is
    //   L(t) = t*Cross(N0,N1)/|Cross(N0,N1)| + c0*N0 + c1*N1
    // for some coefficients c0 and c1 and for t any real number (the line
    // parameter).  Taking dot products with the normals,
    //   d0 = Dot(N0,L) = c0*Dot(N0,N0) + c1*Dot(N0,N1) = c0 + c1*d
    //   d1 = Dot(N1,L) = c0*Dot(N0,N1) + c1*Dot(N1,N1) = c0*d + c1
    // where d = Dot(N0,N1).  These are two equations in two unknowns.  The
    // solution is
    //   c0 = (d0 - d*d1)/det
    //   c1 = (d1 - d*d0)/det
    // where det = 1 - d^2.

    Real fDot = m_pkPlane0->Normal.Dot(m_pkPlane1->Normal);
    if (Math<Real>::FAbs(fDot) >= (Real)1.0 - Math<Real>::ZERO_TOLERANCE)
    {
        // The planes are parallel.  Check if they are coplanar.
        Real fCDiff;
        if (fDot >= (Real)0.0)
        {
            // normals are in same direction, need to look at c0-c1
            fCDiff = m_pkPlane0->Constant - m_pkPlane1->Constant;
        }
        else
        {
            // normals are in opposite directions, need to look at c0+c1
            fCDiff = m_pkPlane0->Constant + m_pkPlane1->Constant;
        }

        if (Math<Real>::FAbs(fCDiff) < Math<Real>::ZERO_TOLERANCE)
        {
            // planes are coplanar
            m_iIntersectionType = IT_PLANE;
            m_kIntrPlane = *m_pkPlane0;
            return true;
        }

        // planes are parallel, but distinct
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    Real fInvDet = ((Real)1.0)/((Real)1.0 - fDot*fDot);
    Real fC0 = (m_pkPlane0->Constant - fDot*m_pkPlane1->Constant)*fInvDet;
    Real fC1 = (m_pkPlane1->Constant - fDot*m_pkPlane0->Constant)*fInvDet;
    m_iIntersectionType = IT_LINE;
    m_kIntrLine.Origin = fC0*m_pkPlane0->Normal + fC1*m_pkPlane1->Normal;
    m_kIntrLine.Direction = m_pkPlane0->Normal.UnitCross(m_pkPlane1->Normal);
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Plane3<Real>::Test (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Real fDot = m_pkPlane0->Normal.Dot(m_pkPlane1->Normal);
    if (Math<Real>::FAbs(fDot) < (Real)1.0 - Math<Real>::ZERO_TOLERANCE)
    {
        // The planes are initially intersecting.  Linear velocities will
        // not change the fact that they are intersecting.
        m_fContactTime = (Real)0.0;
        m_iIntersectionType = IT_LINE;
        return true;
    }

    // check if planes are already coplanar
    Real fCDiff;
    if (fDot >= (Real)0.0)
    {
        // normals are in same direction, need to look at c0-c1
        fCDiff = m_pkPlane0->Constant - m_pkPlane1->Constant;
    }
    else
    {
        // normals are in opposite directions, need to look at c0+c1
        fCDiff = m_pkPlane0->Constant + m_pkPlane1->Constant;
    }

    if (Math<Real>::FAbs(fCDiff) < Math<Real>::ZERO_TOLERANCE)
    {
        // planes are initially the same
        m_fContactTime = (Real)0.0;
        m_iIntersectionType = IT_PLANE;
        return true;
    }

    // The planes are parallel and separated.  Determine when they will
    // become coplanar.
    Vector3<Real> kVDiff = rkVelocity1 - rkVelocity0;
    fDot = m_pkPlane0->Normal.Dot(kVDiff);
    if (Math<Real>::FAbs(fDot) < Math<Real>::ZERO_TOLERANCE)
    {
        // The relative motion of the planes keeps them parallel.
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    m_fContactTime = fCDiff/fDot;
    if ((Real)0.0 <= m_fContactTime && m_fContactTime <= fTMax)
    {
        // The planes are moving towards each other and will meet within the
        // specified time interval.
        m_iIntersectionType = IT_PLANE;
        return true;
    }

    m_iIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrPlane3Plane3<Real>::Find (Real fTMax,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Real fDot = m_pkPlane0->Normal.Dot(m_pkPlane1->Normal);
    if (Math<Real>::FAbs(fDot) < (Real)1.0 - Math<Real>::ZERO_TOLERANCE)
    {
        // The planes are initially intersecting.  Linear velocities will
        // not change the fact that they are intersecting.
        m_fContactTime = (Real)0.0;

        Real fInvDet = ((Real)1.0)/((Real)1.0 - fDot*fDot);
        Real fC0 = (m_pkPlane0->Constant - fDot*m_pkPlane1->Constant)*fInvDet;
        Real fC1 = (m_pkPlane1->Constant - fDot*m_pkPlane0->Constant)*fInvDet;
        m_iIntersectionType = IT_LINE;
        m_kIntrLine.Origin = fC0*m_pkPlane0->Normal + fC1*m_pkPlane1->Normal;
        m_kIntrLine.Direction = m_pkPlane0->Normal.UnitCross(
            m_pkPlane1->Normal);
        return true;
    }

    // check if planes are already coplanar
    Real fCDiff;
    if (fDot >= (Real)0.0)
    {
        // normals are in same direction, need to look at c0-c1
        fCDiff = m_pkPlane0->Constant - m_pkPlane1->Constant;
    }
    else
    {
        // normals are in opposite directions, need to look at c0+c1
        fCDiff = m_pkPlane0->Constant + m_pkPlane1->Constant;
    }

    if (Math<Real>::FAbs(fCDiff) < Math<Real>::ZERO_TOLERANCE)
    {
        // planes are initially the same
        m_fContactTime = (Real)0.0;
        m_iIntersectionType = IT_PLANE;
        m_kIntrPlane = *m_pkPlane0;
        return true;
    }

    // The planes are parallel and separated.  Determine when they will
    // become coplanar.
    Vector3<Real> kVDiff = rkVelocity1 - rkVelocity0;
    fDot = m_pkPlane0->Normal.Dot(kVDiff);
    if (Math<Real>::FAbs(fDot) < Math<Real>::ZERO_TOLERANCE)
    {
        // The relative motion of the planes keeps them parallel.
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    m_fContactTime = fCDiff/fDot;
    if ((Real)0.0 <= m_fContactTime && m_fContactTime <= fTMax)
    {
        // The planes are moving towards each other and will meet within the
        // specified time interval.
        m_iIntersectionType = IT_PLANE;
        m_kIntrPlane.Normal = m_pkPlane0->Normal;
        m_kIntrPlane.Constant = m_pkPlane0->Constant +
            m_fContactTime*m_pkPlane0->Normal.Dot(rkVelocity0);
        return true;
    }

    m_iIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrPlane3Plane3<Real>::GetIntersectionLine () const
{
    return m_kIntrLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Plane3<Real>& IntrPlane3Plane3<Real>::GetIntersectionPlane () const
{
    return m_kIntrPlane;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrPlane3Plane3<float>;

template WM4_FOUNDATION_ITEM
class IntrPlane3Plane3<double>;
//----------------------------------------------------------------------------
}
