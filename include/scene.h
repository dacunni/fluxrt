#ifndef __SCENE_H__
#define __SCENE_H__

#include <string>

#include "sphere.h"
#include "slab.h"
#include "trianglemesh.h"
#include "trianglemeshoctree.h"
#include "environmentmap.h"
#include "sensor.h"
#include "camera.h"
#include "ray.h"
#include "heapmanager.h"

struct Scene
{
	Scene();
	~Scene() = default;

    void print() const;

    // Objects
    std::vector<Sphere> spheres;
    std::vector<Slab> slabs;
    std::vector<TriangleMesh> meshes;
    std::vector<TriangleMeshOctree> meshOctrees;

    // Always points to a valid envionment map
    std::unique_ptr<EnvironmentMap> environmentMap;

    MaterialArray materials;
    TextureCache textureCache;

    Sensor sensor;
    std::unique_ptr<Camera> camera;

    // Heap manager for holding pointers to any objects that must
    // live for the lifetime of the scene, but are otherwise unowned.
    HeapManager heapManager;
};

// Load scene from a file, deducing the type from the extension
bool loadSceneFromFile(Scene & scene, const std::string & filename);

bool loadSceneFromTOMLString(Scene & scene, const std::string & toml);
bool loadSceneFromTOMLFile(Scene & scene, const std::string & filename);

// Ray intersection
inline bool intersects(const Ray & ray, const Scene & scene, float minDistance);
inline bool findIntersection(const Ray & ray, const Sphere & sphere, float minDistance, RayIntersection & intersection);

#include "scene.hpp"
#endif
