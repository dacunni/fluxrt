#ifndef __SCENE_H__
#define __SCENE_H__

#include <string>

#include "sphere.h"
#include "slab.h"
#include "trianglemesh.h"
#include "trianglemeshoctree.h"
#include "light.h"
#include "EnvironmentMap.h"
#include "sensor.h"
#include "camera.h"
#include "ray.h"
#include "heapmanager.h"
#include "traceable.h"

struct Scene
{
	Scene();
	~Scene() = default;

    void print() const;

    // Objects
    std::vector<Traceable<Sphere>> spheres;
    std::vector<Traceable<Slab>> slabs;
    std::vector<Traceable<TriangleMesh>> meshes;
    std::vector<Traceable<TriangleMeshOctree>> meshOctrees;

    // Lights
    std::vector<PointLight> pointLights;
    std::vector<DiskLight> diskLights;

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

bool loadSceneFromTOMLString(Scene & scene, const std::string & toml,
                             std::map<std::string, MaterialID> & namedMaterials);
bool loadSceneFromTOMLFile(Scene & scene, const std::string & filename,
                           std::map<std::string, MaterialID> & namedMaterials);

// Ray intersection
inline bool intersects(const Ray & ray, const Scene & scene, float minDistance, float maxDistance = std::numeric_limits<float>::max());
inline bool findIntersection(const Ray & ray, const Sphere & sphere, float minDistance, RayIntersection & intersection);

#include "scene.hpp"
#endif
