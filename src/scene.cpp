#include <exception>
#include "cpptoml.h"

#include "scene.h"
#include "vectortypes.h"
#include "filesystem.h"
#include "constants.h"
#include "Logger.h"

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

void Scene::logSummary(Logger & logger) const
{
    logger.normal() << "Scene: ";
    logger.normal() << "Number of objects: " << objects.size();
    logger.normal() << "Use objects KD-tree accelerator: " << Logger::yesno(useKDTreeAccelerator);
    logger.normal() << "Number of point lights: " << pointLights.size();
    logger.normal() << "Number of disk lights: " << diskLights.size();
    logger.normal() << "Has environment map: " << Logger::yesno(bool(environmentMap));
    logger.normal() << "Number of materials: " << materials.size();
    logger.normal() << "Number of textures: " << textureCache.textures.size();
    logger.normal() << "Mesh data cache size: " << meshDataCache.fileToMeshData.size();

    sensor.logSummary(logger);

    if(camera) { camera->logSummary(logger); } else { logger.normal() << "Camera: none"; }
}

bool loadSceneFromFile(Scene & scene, const std::string & filename)
{
    if(filesystem::hasExtension(filename, ".toml")) {
        bool ok = loadSceneFromTOMLFile(scene, filename);
        if(ok) {
            scene.logSummary(getLogger());
        }
        return ok;
    }

    std::stringstream ss;
    ss << "Unable to deduce scene file type (" << filename <<")";

    throw std::runtime_error(ss.str());
    return false;
}

