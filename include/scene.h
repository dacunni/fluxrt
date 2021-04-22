#ifndef __SCENE_H__
#define __SCENE_H__

#include <string>

#include "Sphere.h"
#include "slab.h"
#include "TriangleMesh.h"
#include "TriangleMeshOctree.h"
#include "PointLight.h"
#include "DiskLight.h"
#include "EnvironmentMap.h"
#include "sensor.h"
#include "camera.h"
#include "Ray.h"
#include "HeapManager.h"
#include "traceable.h"

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

    // Lights
    std::vector<PointLight> pointLights;
    std::vector<DiskLight> diskLights;

    // Always points to a valid envionment map
    std::unique_ptr<EnvironmentMap> environmentMap;

    MaterialArray materials;
    TextureCache textureCache;

    Sensor sensor;
    std::shared_ptr<Camera> camera;

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
inline bool intersectsWorldRay(const Ray & rayWorld, const Scene & scene, float minDistance, float maxDistance = std::numeric_limits<float>::max());
inline bool findIntersectionWorldRay(const Ray & rayWorld, const Sphere & sphere, float minDistance, RayIntersection & intersection);

#include "scene.hpp"
#endif
