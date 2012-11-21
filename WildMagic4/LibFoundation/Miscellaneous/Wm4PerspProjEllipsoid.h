// Geometric Tools, LLC
// Copyright (c) 1998-2010
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 4.10.0 (2009/11/18)

#ifndef WM4PERSPPROJELLIPSOID_H
#define WM4PERSPPROJELLIPSOID_H

#include "Wm4FoundationLIB.h"
#include "Wm4Ellipse2.h"
#include "Wm4Ellipsoid3.h"
#include "Wm4Plane3.h"

namespace Wm4
{

// Input
//   ellipsoid:   specified by center, axis directions, and extents
//   eye point:   E
//   view plane:  Dot(N,X) = D, planar vectors U and V so that {U,V,N} is an
//                orthonormal set of vectors
// The ellipsoid should be between the eye point and the view plane in the
// sense that all rays from the eye point that intersect the ellipsoid must
// also intersect the view plane.
//
// Output
//   plane origin:       P = E + (D - Dot(N,E))*N
//   projected ellipse:  The projected ellipse coordinates Y = (y0,y1) are the
//                       view plane coordinates of the actual 3D ellipse
//                       points X = P + y0*U + y1*V.

template <class Real> WM4_FOUNDATION_ITEM
void PerspectiveProjectEllipsoid (const Ellipsoid3<Real>& rkEllipsoid,
    const Vector3<Real>& rkEye, const Plane3<Real>& rkPlane,
    const Vector3<Real>& rkU, const Vector3<Real>& rkV, Vector3<Real>& rkP,
    Ellipse2<Real>& rkEllipse);

}

#endif
