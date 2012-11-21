// Geometric Tools, LLC
// Copyright (c) 1998-2010
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 4.10.0 (2009/11/18)

#include "Wm4FoundationPCH.h"
#include "Wm4IntrLine3Cone3.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
IntrLine3Cone3<Real>::IntrLine3Cone3 (const Line3<Real>& rkLine,
    const Cone3<Real>& rkCone)
    :
    m_pkLine(&rkLine),
    m_pkCone(&rkCone)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Line3<Real>& IntrLine3Cone3<Real>::GetLine () const
{
    return *m_pkLine;
}
//----------------------------------------------------------------------------
template <class Real>
const Cone3<Real>& IntrLine3Cone3<Real>::GetCone () const
{
    return *m_pkCone;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLine3Cone3<Real>::Find ()
{
    // Set up the quadratic Q(t) = c2*t^2 + 2*c1*t + c0 that corresponds to
    // the cone.  Let the vertex be V, the unit-length direction vector be A,
    // and the angle measured from the cone axis to the cone wall be Theta,
    // and define g = cos(Theta).  A point X is on the cone wall whenever
    // Dot(A,(X-V)/|X-V|) = g.  Square this equation and factor to obtain
    //   (X-V)^T * (A*A^T - g^2*I) * (X-V) = 0
    // where the superscript T denotes the transpose operator.  This defines
    // a double-sided cone.  The line is L(t) = P + t*D, where P is the line
    // origin and D is a unit-length direction vector.  Substituting
    // X = L(t) into the cone equation above leads to Q(t) = 0.  Since we
    // want only intersection points on the single-sided cone that lives in
    // the half-space pointed to by A, any point L(t) generated by a root of
    // Q(t) = 0 must be tested for Dot(A,L(t)-V) >= 0.
    Real fAdD = m_pkCone->Axis.Dot(m_pkLine->Direction);
    Real fCosSqr = m_pkCone->CosAngle*m_pkCone->CosAngle;
    Vector3<Real> kE = m_pkLine->Origin - m_pkCone->Vertex;
    Real fAdE = m_pkCone->Axis.Dot(kE);
    Real fDdE = m_pkLine->Direction.Dot(kE);
    Real fEdE = kE.Dot(kE);
    Real fC2 = fAdD*fAdD - fCosSqr;
    Real fC1 = fAdD*fAdE - fCosSqr*fDdE;
    Real fC0 = fAdE*fAdE - fCosSqr*fEdE;
    Real fDot;

    // Solve the quadratic.  Keep only those X for which Dot(A,X-V) >= 0.
    if (Math<Real>::FAbs(fC2) >= Math<Real>::ZERO_TOLERANCE)
    {
        // c2 != 0
        Real fDiscr = fC1*fC1 - fC0*fC2;
        if (fDiscr < (Real)0.0)
        {
            // Q(t) = 0 has no real-valued roots.  The line does not
            // intersect the double-sided cone.
            m_iIntersectionType = IT_EMPTY;
            m_iQuantity = 0;
        }
        else if (fDiscr > Math<Real>::ZERO_TOLERANCE)
        {
            // Q(t) = 0 has two distinct real-valued roots.  However, one or
            // both of them might intersect the portion of the double-sided
            // cone "behind" the vertex.  We are interested only in those
            // intersections "in front" of the vertex.
            Real fRoot = Math<Real>::Sqrt(fDiscr);
            Real fInvC2 = ((Real)1.0)/fC2;
            m_iQuantity = 0;

            Real fT = (-fC1 - fRoot)*fInvC2;
            m_akPoint[m_iQuantity] = m_pkLine->Origin + fT*m_pkLine->Direction;
            kE = m_akPoint[m_iQuantity] - m_pkCone->Vertex;
            fDot = kE.Dot(m_pkCone->Axis);
            if (fDot > (Real)0.0)
            {
                m_iQuantity++;
            }

            fT = (-fC1 + fRoot)*fInvC2;
            m_akPoint[m_iQuantity] = m_pkLine->Origin + fT*m_pkLine->Direction;
            kE = m_akPoint[m_iQuantity] - m_pkCone->Vertex;
            fDot = kE.Dot(m_pkCone->Axis);
            if (fDot > (Real)0.0)
            {
                m_iQuantity++;
            }

            if (m_iQuantity == 2)
            {
                // The line intersects the single-sided cone in front of the
                // vertex twice.
                m_iIntersectionType = IT_SEGMENT;
            }
            else if (m_iQuantity == 1)
            {
                // The line intersects the single-sided cone in front of the
                // vertex once.  The other intersection is with the
                // single-sided cone behind the vertex.
                m_iIntersectionType = IT_RAY;
                m_akPoint[m_iQuantity++] = m_pkLine->Direction;
            }
            else
            {
                // The line intersects the single-sided cone behind the vertex
                // twice.
                m_iIntersectionType = IT_EMPTY;
            }
        }
        else
        {
            // one repeated real root (line is tangent to the cone)
            m_akPoint[0] = m_pkLine->Origin - (fC1/fC2)*m_pkLine->Direction;
            kE = m_akPoint[0] - m_pkCone->Vertex;
            if (kE.Dot(m_pkCone->Axis) > (Real)0.0)
            {
                m_iIntersectionType = IT_POINT;
                m_iQuantity = 1;
            }
            else
            {
                m_iIntersectionType = IT_EMPTY;
                m_iQuantity = 0;
            }
        }
    }
    else if (Math<Real>::FAbs(fC1) >= Math<Real>::ZERO_TOLERANCE)
    {
        // c2 = 0, c1 != 0 (D is a direction vector on the cone boundary)
        m_akPoint[0] = m_pkLine->Origin - 
            (((Real)0.5)*fC0/fC1)*m_pkLine->Direction;
        kE = m_akPoint[0] - m_pkCone->Vertex;
        fDot = kE.Dot(m_pkCone->Axis);
        if (fDot > (Real)0.0)
        {
            m_iIntersectionType = IT_RAY;
            m_iQuantity = 2;
            m_akPoint[1] = m_pkLine->Direction;
        }
        else
        {
            m_iIntersectionType = IT_EMPTY;
            m_iQuantity = 0;
        }
    }
    else if (Math<Real>::FAbs(fC0) >= Math<Real>::ZERO_TOLERANCE)
    {
        // c2 = c1 = 0, c0 != 0
        m_iIntersectionType = IT_EMPTY;
        m_iQuantity = 0;
    }
    else
    {
        // c2 = c1 = c0 = 0, cone contains ray V+t*D where V is cone vertex
        // and D is the line direction.
        m_iIntersectionType = IT_RAY;
        m_iQuantity = 2;
        m_akPoint[0] = m_pkCone->Vertex;
        m_akPoint[1] = m_pkLine->Direction;
    }

    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrLine3Cone3<Real>::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector3<Real>& IntrLine3Cone3<Real>::GetPoint (int i) const
{
    assert(0 <= i && i < m_iQuantity);
    return m_akPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class IntrLine3Cone3<float>;

template WM4_FOUNDATION_ITEM
class IntrLine3Cone3<double>;
//----------------------------------------------------------------------------
}
