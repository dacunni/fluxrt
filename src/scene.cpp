#include "cpptoml.h"

#include "scene.h"
#include "vectortypes.h"

void Scene::print() const
{
    std::cout << "Scene: "
        << " spheres " << spheres.size()
        << " slabs " << slabs.size()
        << " meshes " << meshes.size()
        << std::endl;
}

static vec3 vectorToVec3(const std::vector<double> & v)
{
    return vec3(v[0], v[1], v[2]);
}

bool loadSceneFromTOML(Scene & scene, const std::string & filename)
{
    try {
        auto top = cpptoml::parse_file(filename);

        auto cameraTable = top->get_table("camera");
        if(cameraTable) {
            auto type = cameraTable->get_as<std::string>("type").value_or("pinhole");
            auto position = Position3(vectorToVec3(cameraTable->get_array_of<double>("position").value_or(std::vector<double>{0.0, 0.0, 0.0})));
            auto lookat = cameraTable->get_array_of<double>("lookat");
            if(lookat) { /* TODO */ }
            auto up = Direction3(vectorToVec3(cameraTable->get_array_of<double>("up").value_or(std::vector<double>{0.0, 1.0, 0.0})));
            auto hfov = cameraTable->get_as<double>("hfov").value_or(45.0);

            std::cout << "Camera: type " << type
                << " position " << position
                << " hfov " << hfov
                << std::endl;

            // TODO - make a camera
        }

        auto sensorTable = top->get_table("sensor");
        if(sensorTable) {
            auto pixelwidth = sensorTable->get_as<uint32_t>("pixelwidth").value_or(100);
            auto pixelheight = sensorTable->get_as<uint32_t>("pixelheight").value_or(100);

            printf("Sensor: %u x %u\n", pixelwidth, pixelheight);
            // TODO - make a sensor
        }

        auto meshTableArray = top->get_table_array("meshes");
        if(meshTableArray) {
            for (const auto & meshTable : *meshTableArray) {
                auto name = meshTable->get_as<std::string>("name").value_or("");
                auto filename = meshTable->get_as<std::string>("file");
                if(!filename) { throw std::runtime_error("Meshes must supply a file name"); }

                std::cout << "Mesh: name " << name << " file " << *filename << std::endl;

                TriangleMesh mesh;

                if(!loadTriangleMesh(mesh, scene.materials, scene.textures, *filename)) {
                    throw std::runtime_error("Error loading mesh");
                }
                scene.meshes.emplace_back(std::move(mesh));
            }
        }

        auto sphereTableArray = top->get_table_array("spheres");
        if(sphereTableArray) {
            for (const auto & sphereTable : *sphereTableArray) {
                auto radius = sphereTable->get_as<double>("radius").value_or(1.0);
                auto position = Position3(vectorToVec3(cameraTable->get_array_of<double>("position").value_or(std::vector<double>{0.0, 0.0, 0.0})));

                std::cout << "Sphere: radius " << radius << " position " << position << std::endl;

                scene.spheres.emplace_back(position, radius);
            }
        }

        auto slabTableArray = top->get_table_array("slabs");
        if(slabTableArray) {
            for (const auto & slabTable : *slabTableArray) {
                auto min = Position3(vectorToVec3(slabTable->get_array_of<double>("min").value_or(std::vector<double>{0.0, 0.0, 0.0})));
                auto max = Position3(vectorToVec3(slabTable->get_array_of<double>("max").value_or(std::vector<double>{0.0, 0.0, 0.0})));

                std::cout << "Slab:  min " << min << " max " << max << std::endl;

                scene.slabs.emplace_back(min, max);
            }
        }

        scene.print();
    }
    catch(cpptoml::parse_exception & e) {
        std::cout << "Caught cpptoml exception: " << e.what() << std::endl;
        return false;
    }
    catch(std::exception & e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
        return false;
    }
    catch(...) {
        std::cout << "Caught unknown exception" << std::endl;
        return false;
    }
    return true;
}

