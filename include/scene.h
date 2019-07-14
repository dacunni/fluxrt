#ifndef __SCENE_H__
#define __SCENE_H__

#include <string>

#include "sphere.h"
#include "slab.h"
#include "trianglemesh.h"
#include "sensor.h"

struct Scene
{
	inline Scene() = default;
	inline ~Scene() = default;

    void print() const;

    // Objects
    std::vector<Sphere> spheres;
    std::vector<Slab> slabs;
    std::vector<TriangleMesh> meshes;

    MaterialArray materials;
    TextureArray textures;

    Sensor sensor;
};

bool loadSceneFromTOML(Scene & scene, const std::string & filename);

#endif
