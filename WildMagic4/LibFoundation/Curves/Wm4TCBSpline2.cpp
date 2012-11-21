// Geometric Tools, LLC
// Copyright (c) 1998-2010
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 4.10.0 (2009/11/18)

#include "Wm4FoundationPCH.h"
#include "Wm4TCBSpline2.h"
#include "Wm4Integrate1.h"
#include "Wm4Polynomial1.h"

namespace Wm4
{
//----------------------------------------------------------------------------
template <class Real>
TCBSpline2<Real>::TCBSpline2 (int iSegments, Real* afTime,
    Vector2<Real>* akPoint, Real* afTension, Real* afContinuity, Real* afBias)
    :
    MultipleCurve2<Real>(iSegments,afTime)
{
    // TO DO.  Add 'boundary type' just as in natural splines.
    assert(m_iSegments >= 3);

    // all four of these arrays have m_iSegments+1 elements
    m_akPoint = akPoint;
    m_afTension = afTension;
    m_afContinuity = afContinuity;
    m_afBias = afBias;

    m_akA = WM4_NEW Vector2<Real>[m_iSegments];
    m_akB = WM4_NEW Vector2<Real>[m_iSegments];
    m_akC = WM4_NEW Vector2<Real>[m_iSegments];
    m_akD = WM4_NEW Vector2<Real>[m_iSegments];

    // For now, treat the first point as if it occurred twice.
    ComputePoly(0,0,1,2);

    for (int i = 1; i < m_iSegments-1; i++)
    {
        ComputePoly(i-1,i,i+1,i+2);
    }

    // For now, treat the last point as if it occurred twice.
    ComputePoly(m_iSegments-2,m_iSegments-1,m_iSegments,m_iSegments);
}
//----------------------------------------------------------------------------
template <class Real>
TCBSpline2<Real>::~TCBSpline2 ()
{
    WM4_DELETE[] m_akPoint;
    WM4_DELETE[] m_afTension;
    WM4_DELETE[] m_afContinuity;
    WM4_DELETE[] m_afBias;
    WM4_DELETE[] m_akA;
    WM4_DELETE[] m_akB;
    WM4_DELETE[] m_akC;
    WM4_DELETE[] m_akD;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>* TCBSpline2<Real>::GetPoints () const
{
    return m_akPoint;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* TCBSpline2<Real>::GetTensions () const
{
    return m_afTension;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* TCBSpline2<Real>::GetContinuities () const
{
    return m_afContinuity;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* TCBSpline2<Real>::GetBiases () const
{
    return m_afBias;
}
//----------------------------------------------------------------------------
template <class Real>
void TCBSpline2<Real>::ComputePoly (int i0, int i1, int i2, int i3)
{
    Vector2<Real> kDiff = m_akPoint[i2] - m_akPoint[i1];
    Real fDt = m_afTime[i2] - m_afTime[i1];

    // build multipliers at P1
    Real fOmt0 = (Real)1.0 - m_afTension[i1];
    Real fOmc0 = (Real)1.0 - m_afContinuity[i1];
    Real fOpc0 = (Real)1.0 + m_afContinuity[i1];
    Real fOmb0 = (Real)1.0 - m_afBias[i1];
    Real fOpb0 = (Real)1.0 + m_afBias[i1];
    Real fAdj0 = ((Real)2.0)*fDt/(m_afTime[i2]-m_afTime[i0]);
    Real fOut0 = ((Real)0.5)*fAdj0*fOmt0*fOpc0*fOpb0;
    Real fOut1 = ((Real)0.5)*fAdj0*fOmt0*fOmc0*fOmb0;

    // build outgoing tangent at P1
    Vector2<Real> kTOut = fOut1*kDiff + fOut0*(m_akPoint[i1] - m_akPoint[i0]);

    // build multipliers at point P2
    Real fOmt1 = (Real)1.0 - m_afTension[i2];
    Real fOmc1 = (Real)1.0 - m_afContinuity[i2];
    Real fOpc1 = (Real)1.0 + m_afContinuity[i2];
    Real fOmb1 = (Real)1.0 - m_afBias[i2];
    Real fOpb1 = (Real)1.0 + m_afBias[i2];
    Real fAdj1 = ((Real)2.0)*fDt/(m_afTime[i3] - m_afTime[i1]);
    Real fIn0 = ((Real)0.5)*fAdj1*fOmt1*fOmc1*fOpb1;
    Real fIn1 = ((Real)0.5)*fAdj1*fOmt1*fOpc1*fOmb1;

    // build incoming tangent at P2
    Vector2<Real> kTIn = fIn1*(m_akPoint[i3] - m_akPoint[i2]) + fIn0*kDiff;

    m_akA[i1] = m_akPoint[i1];
    m_akB[i1] = kTOut;
    m_akC[i1] = ((Real)3.0)*kDiff - ((Real)2.0)*kTOut - kTIn;
    m_akD[i1] = ((Real)-2.0)*kDiff + kTOut + kTIn;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> TCBSpline2<Real>::GetPosition (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    fDt /= (m_afTime[iKey+1] - m_afTime[iKey]);

    Vector2<Real> kResult = m_akA[iKey] + fDt*(m_akB[iKey] +
        fDt*(m_akC[iKey] + fDt*m_akD[iKey]));

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> TCBSpline2<Real>::GetFirstDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    fDt /= (m_afTime[iKey+1] - m_afTime[iKey]);

    Vector2<Real> kResult = m_akB[iKey] + fDt*(m_akC[iKey]*((Real)2.0) +
        m_akD[iKey]*(((Real)3.0)*fDt));

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> TCBSpline2<Real>::GetSecondDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    fDt /= (m_afTime[iKey+1] - m_afTime[iKey]);

    Vector2<Real> kResult = m_akC[iKey]*((Real)2.0) +
        m_akD[iKey]*(((Real)6.0)*fDt);

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> TCBSpline2<Real>::GetThirdDerivative (Real fTime) const
{
    int iKey;
    Real fDt;
    GetKeyInfo(fTime,iKey,fDt);

    fDt /= (m_afTime[iKey+1] - m_afTime[iKey]);

    Vector2<Real> kResult = ((Real)6.0)*m_akD[iKey];

    return kResult;
}
//----------------------------------------------------------------------------
template <class Real>
Real TCBSpline2<Real>::GetSpeedKey (int iKey, Real fTime) const
{
    Vector2<Real> kVelocity = m_akB[iKey] + fTime*(m_akC[iKey]*((Real)2.0) +
        m_akD[iKey]*(((Real)3.0)*fTime));

    return kVelocity.Length();
}
//----------------------------------------------------------------------------
template <class Real>
Real TCBSpline2<Real>::GetLengthKey (int iKey, Real fT0, Real fT1) const
{
    ThisPlusKey kData(this,iKey);
    return Integrate1<Real>::RombergIntegral(8,fT0,fT1,GetSpeedWithData,
        (void*)&kData);
}
//----------------------------------------------------------------------------
template <class Real>
Real TCBSpline2<Real>::GetVariationKey (int iKey, Real fT0, Real fT1,
    const Vector2<Real>& rkA, const Vector2<Real>& rkB) const
{
    Polynomial1<Real> kXPoly(3);
    kXPoly[0] = m_akA[iKey].X();
    kXPoly[1] = m_akB[iKey].X();
    kXPoly[2] = m_akC[iKey].X();
    kXPoly[3] = m_akD[iKey].X();

    Polynomial1<Real> kYPoly(3);
    kYPoly[0] = m_akA[iKey].Y();
    kYPoly[1] = m_akB[iKey].Y();
    kYPoly[2] = m_akC[iKey].Y();
    kYPoly[3] = m_akD[iKey].Y();

    // construct line segment A + t*B
    Polynomial1<Real> kLx(1), kLy(1);
    kLx[0] = rkA.X();
    kLx[1] = rkB.X();
    kLy[0] = rkA.Y();
    kLy[1] = rkB.Y();

    // compute |X(t) - L(t)|^2
    Polynomial1<Real> kDx = kXPoly - kLx;
    Polynomial1<Real> kDy = kYPoly - kLy;
    Polynomial1<Real> kNormSqr = kDx*kDx + kDy*kDy;

    // compute indefinite integral of |X(t)-L(t)|^2
    Polynomial1<Real> kIntegral(kNormSqr.GetDegree()+1);
    kIntegral[0] = (Real)0.0;
    for (int i = 1; i <= kIntegral.GetDegree(); i++)
    {
        kIntegral[i] = kNormSqr[i-1]/i;
    }

    // compute definite Integral(t0,t1,|X(t)-L(t)|^2)
    Real fResult = kIntegral(fT1) - kIntegral(fT0);
    return fResult;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
template WM4_FOUNDATION_ITEM
class TCBSpline2<float>;

template WM4_FOUNDATION_ITEM
class TCBSpline2<double>;
//----------------------------------------------------------------------------
}
