#include "Firefighter/recoStuff/interface/GeometryBoundary.h"

#include <cmath>
#include <cassert>
#include <iostream>

ff::GeometryBoundary::GeometryBoundary() :
  fMinEta(-99999.), fMaxEta(-fMinEta),
  fMinRadius(0.), fMaxRadius(740.),
  fMinZ(0.), fMaxZ(960.)
{}


ff::GeometryBoundary::GeometryBoundary(float eta, float r) :
  fMinEta(-fabs(eta)), fMaxEta(fabs(eta)),
  fMinRadius(0.), fMaxRadius(r),
  fMinZ(r*sinh(fMinEta)), fMaxZ(r*sinh(fMaxEta))
{}


ff::GeometryBoundary::GeometryBoundary(float eta, float r, float z) :
  fMinEta(-fabs(eta)), fMaxEta(fabs(eta)),
  fMinRadius(0.), fMaxRadius(r),
  fMinZ(-fabs(z)), fMaxZ(fabs(z))
{}


ff::GeometryBoundary::GeometryBoundary(float mineta, float maxeta, float minr, float maxr, float minz, float maxz) :
  fMinEta(mineta), fMaxEta(maxeta),
  fMinRadius(minr), fMaxRadius(maxr),
  fMinZ(minz), fMaxZ(maxz)
{
  assert(
    fMinEta < fMaxEta
    && fMinRadius < fMaxRadius
    && fMinZ < fMaxZ
  );
}


void
ff::GeometryBoundary::setEta(float mineta, float maxeta)
{
  assert(maxeta > mineta);
  fMaxEta = maxeta;
  fMinEta = mineta;
}


void
ff::GeometryBoundary::setEta(float eta)
{
  fMaxEta = fabs(eta);
  fMinEta = -fabs(eta);
}


void
ff::GeometryBoundary::setRadius(float minr, float maxr)
{
  assert(maxr > minr);
  fMaxRadius = maxr;
  fMinRadius = minr;
}


void
ff::GeometryBoundary::setRadius(float r)
{
  assert(r>0);
  fMaxRadius = r;
  fMinRadius = 0.;
}


void
ff::GeometryBoundary::setZ(float minz, float maxz)
{
  assert(maxz > minz);
  fMaxZ = maxz;
  fMinZ = minz;
}


void
ff::GeometryBoundary::setZ(float z)
{
  fMaxZ = fabs(z);
  fMinZ = -fabs(z);
}



bool
ff::GeometryBoundary::inRegionByRZ(float r, float z) const
{
  return (r>=fMinRadius && r<=fMaxRadius)
    && (z>=fMinZ && z<=fMaxZ)
    && (asinhf(z/r)>=fMinEta && asinhf(z/r)<=fMaxEta);
}


bool
ff::GeometryBoundary::inRegionByLTheta(float l, float theta) const
{
  return inRegionByRZ(l*sin(theta), l*cos(theta));
}


void
ff::GeometryBoundary::print() const
{
  std::cout<<"ff::GeometryBoundary => "
           <<"eta: ["<<fMinEta<<", "<<fMaxEta<<"], "
           <<"radius: ["<<fMinRadius<<", "<<fMaxRadius<<"], "
           <<"Z: ["<<fMinZ<<", "<<fMaxZ<<"]."
           <<std::endl;
}