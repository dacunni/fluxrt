#include <exception>
#include "cpptoml.h"

#include "scene.h"
#include "vectortypes.h"
#include "filesystem.h"
#include "constants.h"

Scene::Scene()
    : environmentMap(std::make_unique<EnvironmentMap>())
{
}

void Scene::print() const
{
    std::cout << "Scene: "
        << " spheres " << spheres.size()
        << " slabs " << slabs.size()
        << " meshes " << meshes.size()
        << " mesh_octrees " << meshOctrees.size()
        << " point lights " << pointLights.size()
        << std::endl;
    sensor.print();
}

bool loadSceneFromFile(Scene & scene, const std::string & filename)
{
    if(filesystem::hasExtension(filename, ".toml")) {
        return loadSceneFromTOMLFile(scene, filename);
    }

    throw std::runtime_error("Unable to deduce scene file type");
    return false;
}

