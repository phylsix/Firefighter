#ifndef recoStuff_GeometryBoundary_h
#define recoStuff_GeometryBoundary_h  
  

namespace ff {  
  class GeometryBoundary
  {
    public:
      GeometryBoundary();
      ~GeometryBoundary() {}

      GeometryBoundary(float eta, float r);
      GeometryBoundary(float eta, float r, float z);
      GeometryBoundary(float mineta, float maxeta, float minr, float maxr, float minz, float maxz);

      void setEta(float mineta, float maxeta);
      void setEta(float eta);
      void setRadius(float minr, float maxr);
      void setRadius(float r);
      void setZ(float minz, float maxz);
      void setZ(float z);

      bool inRegionByRZ(float r, float z) const;
      bool inRegionByLTheta(float l, float theta) const;

      void print() const;

    private:
      float fMinEta;
      float fMaxEta;
      float fMinRadius;
      float fMaxRadius;
      float fMinZ;
      float fMaxZ;

  };
}

#endif