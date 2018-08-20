
inline Slab::Slab(float xmind, float ymind, float zmind,
                  float xmaxd, float ymaxd, float zmaxd) :
    xmin(xmind), ymin(ymind), zmin(zmind),
    xmax(xmaxd), ymax(ymaxd), zmax(zmaxd)
{
    correctMinMax();
}

inline Slab::Slab(float xmind, float ymind, float zmind, float sz) :
    xmin(xmind), ymin(ymind), zmin(zmind), 
    xmax(xmind + sz), ymax(ymind + sz), zmax(zmind + sz)
{
    correctMinMax();
}
