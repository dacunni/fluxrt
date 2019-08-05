#ifndef __SCENE_H__
#define __SCENE_H__

#include <string>

#include "sphere.h"
#include "slab.h"
#include "trianglemesh.h"
#include "sensor.h"
#include "camera.h"
#include "ray.h"

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
    PinholeCamera camera;
};

// Load scene from a file, deducing the type from the extension
bool loadSceneFromFile(Scene & scene, const std::string & filename);

bool loadSceneFromTOMLString(Scene & scene, const std::string & toml);
bool loadSceneFromTOMLFile(Scene & scene, const std::string & filename);

// Ray intersection
inline bool intersects(const Ray & ray, const Scene & scene, float minDistance);

#include "scene.hpp"
#endif
