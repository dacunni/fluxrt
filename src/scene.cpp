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

void Scene::buildAccelerators()
{
    objectsKDTree.build(objects);
}

void Scene::print() const
{
    std::cout << "Scene: "
        << " objects " << objects.size()
        << " point lights " << pointLights.size()
        << std::endl;
    sensor.print();
}

bool loadSceneFromFile(Scene & scene, const std::string & filename)
{
    if(filesystem::hasExtension(filename, ".toml")) {
        return loadSceneFromTOMLFile(scene, filename);
    }

    std::stringstream ss;
    ss << "Unable to deduce scene file type (" << filename <<")";

    throw std::runtime_error(ss.str());
    return false;
}

