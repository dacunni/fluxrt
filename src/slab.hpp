#include <iostream>

inline Slab::Slab(float xmind, float ymind, float zmind,
                  float xmaxd, float ymaxd, float zmaxd) :
    xmin(xmind), ymin(ymind), zmin(zmind),
    xmax(xmaxd), ymax(ymaxd), zmax(zmaxd)
{
    correctMinMax();
}

inline Slab::Slab(float xmind, float ymind, float zmind, float sz) :
    Slab(xmind, ymind, zmind, xmind + sz, ymind + sz, zmind + sz)
{
    correctMinMax();
}

inline Slab::Slab(const Position3 & minpos, const Position3 & maxpos)
    : Slab(minpos.x, minpos.y, minpos.z, maxpos.x, maxpos.y, maxpos.z)
{ }

inline bool Slab::contains(const Position3 & P) const
{
    return
        xmin <= P.x && xmax >= P.x &&
        ymin <= P.y && ymax >= P.y &&
        zmin <= P.z && zmax >= P.z;
}

inline bool intersects(const Ray & ray, const Slab & slab, float minDistance)
{
    vec3 dinv(1.0f / ray.direction.x, 1.0f / ray.direction.y, 1.0f / ray.direction.z);

    float tx1 = (slab.xmin - ray.origin.x) * dinv.x;
    float tx2 = (slab.xmax - ray.origin.x) * dinv.x;

    float tmin = std::min(tx1, tx2);
    float tmax = std::max(tx1, tx2);

    float ty1 = (slab.ymin - ray.origin.y) * dinv.y;
    float ty2 = (slab.ymax - ray.origin.y) * dinv.y;

    tmin = std::max(tmin, std::min(ty1, ty2));
    tmax = std::min(tmax, std::max(ty1, ty2));

    float tz1 = (slab.zmin - ray.origin.z) * dinv.z;
    float tz2 = (slab.zmax - ray.origin.z) * dinv.z;

    tmin = std::max(tmin, std::min(tz1, tz2));
    tmax = std::min(tmax, std::max(tz1, tz2));

    return tmax >= tmin && (tmin > minDistance || tmax > minDistance);
}

static const Direction3 boxNormals[6] = {
    Direction3(-1.0,  0.0,  0.0),
    Direction3( 1.0,  0.0,  0.0),
    Direction3( 0.0, -1.0,  0.0),
    Direction3( 0.0,  1.0,  0.0),
    Direction3( 0.0,  0.0, -1.0),
    Direction3( 0.0,  0.0,  1.0)
};

static const Direction3 boxTangents[6] = {
    Direction3( 0.0, -1.0,  0.0),
    Direction3( 0.0,  1.0,  0.0),
    Direction3( 0.0,  0.0, -1.0),
    Direction3( 0.0,  0.0,  1.0),
    Direction3(-1.0,  0.0,  0.0),
    Direction3( 1.0,  0.0,  0.0)
};

static const Direction3 boxBitangents[6] = {
    Direction3( 0.0,  0.0, -1.0),
    Direction3( 0.0,  0.0,  1.0),
    Direction3(-1.0,  0.0,  0.0),
    Direction3( 1.0,  0.0,  0.0),
    Direction3( 0.0,  1.0,  0.0),
    Direction3( 0.0, -1.0,  0.0)
};

const int MINUS_X_NORMAL_INDEX = 0;
const int PLUS_X_NORMAL_INDEX  = 1;
const int MINUS_Y_NORMAL_INDEX = 2;
const int PLUS_Y_NORMAL_INDEX  = 3;
const int MINUS_Z_NORMAL_INDEX = 4;
const int PLUS_Z_NORMAL_INDEX  = 5;

const int X_NORMAL_BASE_INDEX = 0;
const int Y_NORMAL_BASE_INDEX = 2;
const int Z_NORMAL_BASE_INDEX = 4;

const int LOOK_POSITIVE_NORMAL_OFFSET = 0;
const int LOOK_NEGATIVE_NORMAL_OFFSET = 1;

//
// Reference: http://www.scratchapixel.com/lessons/3d-basic-lessons/lesson-7-intersecting-simple-shapes/ray-box-intersection/
//
// Note: This method assumes that slab min/max values are ordered correctly.
inline bool findIntersection(const Ray & ray, const Slab & slab, float minDistance, RayIntersection & intersection)
{
    float xn, xf, yn, yf, zn, zf;       // near and far planes for the box
    int nin, nif, nix, niy, niz;        // indices into normal table (near plane, far plane, x, y, z)
    
    // Determine which sides of the box to label "near" or "far" depending on the
    // ray direction
    if(ray.direction.x >= 0.0f) {
        xn = slab.xmin;
        xf = slab.xmax;
        nix = X_NORMAL_BASE_INDEX + LOOK_POSITIVE_NORMAL_OFFSET;
    }
    else {
        xn = slab.xmax;
        xf = slab.xmin;
        nix = X_NORMAL_BASE_INDEX + LOOK_NEGATIVE_NORMAL_OFFSET;
    }
    
    if(ray.direction.y >= 0.0f) {
        yn = slab.ymin;
        yf = slab.ymax;
        niy = Y_NORMAL_BASE_INDEX + LOOK_POSITIVE_NORMAL_OFFSET;
    }
    else {
        yn = slab.ymax;
        yf = slab.ymin;
        niy = Y_NORMAL_BASE_INDEX + LOOK_NEGATIVE_NORMAL_OFFSET;
    }

    if(ray.direction.z >= 0.0f) {
        zn = slab.zmin;
        zf = slab.zmax;
        niz = Z_NORMAL_BASE_INDEX + LOOK_POSITIVE_NORMAL_OFFSET;
    }
    else {
        zn = slab.zmax;
        zf = slab.zmin;
        niz = Z_NORMAL_BASE_INDEX + LOOK_NEGATIVE_NORMAL_OFFSET;
    }
    
    // Find t values for intersection into each of the planes that
    // define the sides of the axis-aligned slab
    float tnx = (xn - ray.origin.x) / ray.direction.x;
    float tfx = (xf - ray.origin.x) / ray.direction.x;
    float tny = (yn - ray.origin.y) / ray.direction.y;
    float tfy = (yf - ray.origin.y) / ray.direction.y;
    float tnz = (zn - ray.origin.z) / ray.direction.z;
    float tfz = (zf - ray.origin.z) / ray.direction.z;
    
    // Reject if farthest y is closer than closest x, or
    // if farthest x is closer than closest y
    if(tfy < tnx || tfx < tny)
        return false;

    // Find first potential intersection point as the farthest of the t0s, ignoring z
    float tn = tnx;
    nin = nix;
    if(tny > tn) {
        tn = tny;
        nin = niy;
    }
    // Find second potential intersection point as the closest of the t1s, ignoring z
    float tf = tfx;
    nif = nix;
    if(tfy < tf) {
        tf = tfy;
        nif = niy;
    }

    // Reject if farthest z is closer than closest x,y, or
    // if farthest x,y is closer than closest z
    if(tfz < tn || tnz > tf)
        return false;
    
    // Update closest and farthest intersection with z
    if(tnz > tn) {
        tn = tnz;
        nin = niz;
    }
    if(tfz < tf) {            // FIXME - is this the right or is it < ?
        tf = tfz;
        nif = niz;
    }

    if(tf < minDistance) {
        return false;
    }

    if(tn > minDistance) {
        intersection.normal = boxNormals[nin];
        intersection.tangent = boxTangents[nin];
        intersection.bitangent = boxBitangents[nin];
        intersection.distance = tn;
    }
    else {
        intersection.normal = boxNormals[nif];
        intersection.tangent = boxTangents[nif];
        intersection.bitangent = boxBitangents[nif];
        intersection.distance = tf;
    }
    intersection.position = add(ray.origin, scale(ray.direction, intersection.distance));

    return true;
}

