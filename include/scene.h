#ifndef __SCENE_H__
#define __SCENE_H__

#include "sphere.h"
#include "slab.h"
#include "trianglemesh.h"

struct Scene
{
	inline Scene() = default;
	inline ~Scene() = default;

    // Objects
    std::vector<Sphere> spheres;
    std::vector<Slab> slabs;
    std::vector<TriangleMesh> meshes;
};

#endif
