#include <exception>
#include "cpptoml.h"

#include "scene.h"
#include "vectortypes.h"
#include "filesystem.h"
#include "constants.h"
#include "transform.h"

static vec3 vectorToVec3(const std::vector<double> & v)
{
    return vec3(v[0], v[1], v[2]);
}

static color::ColorRGB vectorToColorRGB(const std::vector<double> & v)
{
    return color::ColorRGB(v[0], v[1], v[2]);
}

static radiometry::RadianceRGB vectorToRadianceRGB(const std::vector<double> & v)
{
    return radiometry::RadianceRGB(v[0], v[1], v[2]);
}

static ReflectanceRGB vectorToReflectanceRGB(const std::vector<double> & v)
{
    return ReflectanceRGB(v[0], v[1], v[2]);
}

template<typename OBJ>
void loadMaterialForObject(const std::shared_ptr<cpptoml::table> & table, OBJ & obj, Scene & scene)
{
    auto materialTable = table->get_table("material");
    if(materialTable) {
        auto type = materialTable->get_as<std::string>("type");
        if(!type) { throw std::runtime_error("Material must supply a type"); }

        if(*type == "diffuse") {
            auto diffuseRGB = vectorToReflectanceRGB(materialTable->get_array_of<double>("diffuse").value_or(std::vector<double>{0.0, 0.0, 0.0}));
            Material material = Material::makeDiffuse(diffuseRGB);
            obj.material = scene.materials.size();
            scene.materials.push_back(material);
        }
        else if(*type == "diffusespecular") {
            auto diffuseRGB = vectorToReflectanceRGB(materialTable->get_array_of<double>("diffuse").value_or(std::vector<double>{0.0, 0.0, 0.0}));
            auto specularRGB = vectorToReflectanceRGB(materialTable->get_array_of<double>("specular").value_or(std::vector<double>{0.0, 0.0, 0.0}));
            Material material = Material::makeDiffuseSpecular(diffuseRGB, specularRGB);
            obj.material = scene.materials.size();
            scene.materials.push_back(material);
        }
        else if(*type == "mirror") {
            Material material = Material::makeMirror();
            obj.material = scene.materials.size();
            scene.materials.push_back(material);
        }
        else if(*type == "refractive") {
            auto indexOfRefraction = materialTable->get_as<double>("ior").value_or(1.333);
            Material material = Material::makeRefractive(indexOfRefraction);
            obj.material = scene.materials.size();
            scene.materials.push_back(material);
        }
        else {
            throw std::runtime_error(std::string("Unknown material type : ") + *type);
        }
    }
}

template<typename OBJ>
void loadTransformsForObject(const std::shared_ptr<cpptoml::table> & table, OBJ & obj, Scene & scene)
{
    auto transformTableArray = table->get_table_array("transform");

    if(transformTableArray) {
        Transform transform;

        // Transforms are composed such that they are applied in order of appearance
        for(const auto & transformTable : *transformTableArray) {
            int numTypesOfTransform = 0;

            auto translate = transformTable->get_array_of<double>("translate");
            if(translate) {
                auto value = vectorToVec3(*translate);
                transform = compose(Transform::translation(value), transform);
                ++numTypesOfTransform;
            }

            auto rotateAxis = transformTable->get_array_of<double>("rotate_axis");
            auto rotateAngle = transformTable->get_as<double>("rotate_angle");

            if(rotateAxis && rotateAngle) {
                vec3 axis = vectorToVec3(*rotateAxis);
                float angle = *rotateAngle;
                transform = compose(Transform::rotation(axis, angle), transform);
                ++numTypesOfTransform;
            }

            auto scale = transformTable->get_array_of<double>("scale");
            if(scale) {
                auto value = vectorToVec3(*scale);
                transform = compose(Transform::scale(value.x, value.y, value.z), transform);
                ++numTypesOfTransform;
            }

            // Limit to one transform per transform block so composition is well defined
            if(numTypesOfTransform != 1) {
                throw std::runtime_error("Transform should be exactly 1 of: scale, rotate, translate");
            }
        }

        std::cout << "Transform: " << transform.fwd.string() << '\n';

        obj.transform = transform;
    }
}

bool loadSceneFromParsedTOML(Scene & scene, std::shared_ptr<cpptoml::table> & top)
{
    try {
        const char * meshPath = getenv("MESH_PATH");
        const char * envMapPath = getenv("ENV_MAP_PATH");

        auto applyPathPrefix = [](const char * prefix, const std::string & path) {
            // If prefix is defined and path is not absolute
            if(prefix && path.front() != '/') {
                return std::string(prefix) + '/' + path;
            }
            return path;
        };

        if(meshPath) { std::cout << "Mesh path = " << meshPath << '\n'; }
        if(envMapPath) { std::cout << "Env map path = " << envMapPath << '\n'; }

        auto sensorTable = top->get_table("sensor");
        if(sensorTable) {
            auto pixelwidth = sensorTable->get_as<uint32_t>("pixelwidth").value_or(100);
            auto pixelheight = sensorTable->get_as<uint32_t>("pixelheight").value_or(100);
            scene.sensor = Sensor(pixelwidth, pixelheight);
        }
        float aspect = scene.sensor.aspectRatio();

        std::cout << "Aspect ratio " <<  aspect << '\n';

        auto cameraTable = top->get_table("camera");
        if(cameraTable) {
            auto type = cameraTable->get_as<std::string>("type").value_or("pinhole");
            auto position = Position3(vectorToVec3(cameraTable->get_array_of<double>("position").value_or(std::vector<double>{0.0, 0.0, 0.0})));
            auto direction = Direction3(vectorToVec3(cameraTable->get_array_of<double>("direction").value_or(std::vector<double>{0.0, 0.0, -1.0})));
            direction.normalize();
            auto lookat = cameraTable->get_array_of<double>("lookat");
            if(lookat) {
                // TODO
            }
            auto up = Direction3(vectorToVec3(cameraTable->get_array_of<double>("up").value_or(std::vector<double>{0.0, 1.0, 0.0})));
            up.normalize();

            if(type == "pinhole") {
                auto hfov = cameraTable->get_as<double>("hfov").value_or(45.0);
                hfov = DegreesToRadians(hfov);
                auto vfov = std::atan(std::tan(hfov) / aspect);
                auto focusDistance = cameraTable->get_as<double>("focus_distance");
                auto focusDivergence = cameraTable->get_as<double>("focus_divergence");
                bool applyLensBlur = bool(focusDistance || focusDivergence);

                std::cout << "Camera: type " << type
                    << " position " << position
                    << " direction " << direction
                    << " hfov " << hfov << " vfov " << vfov
                    << " applyLensBlur " << (applyLensBlur ? "YES" : "NO");
                if(applyLensBlur) {
                    std::cout << " focus_distance ";
                    if(focusDistance) std::cout << *focusDistance; else std::cout << "default";
                    std::cout << " focus_divergence ";
                    if(focusDivergence) std::cout << *focusDivergence; else std::cout << "default";
                }
                std::cout << std::endl;

                auto pinholeCamera = std::make_unique<PinholeCamera>(hfov, vfov);
                if(applyLensBlur) {
                    pinholeCamera->applyLensBlur = true;
                    if(focusDistance) pinholeCamera->focusDistance = *focusDistance;
                    if(focusDivergence) pinholeCamera->focusDivergence = *focusDivergence;
                }
                scene.camera = std::move(pinholeCamera);
            }
            else if(type == "ortho") {
                auto hsize = cameraTable->get_as<double>("hsize").value_or(2.0);
                auto vsize = hsize / aspect;

                std::cout << "Camera: type " << type
                    << " position " << position
                    << " direction " << direction
                    << " hsize " << hsize << " vsize " << vsize
                    << std::endl;

                scene.camera = std::make_unique<OrthoCamera>(hsize, vsize);
            }

            scene.camera->setPositionDirectionUp(position, direction, up);
        }

        auto envmapTable = top->get_table("envmap");
        if(envmapTable) {
            auto type = envmapTable->get_as<std::string>("type").value_or("none");
            if(type == "cubemap") {
                std::string xneg = applyPathPrefix(envMapPath, *envmapTable->get_as<std::string>("xneg"));
                std::string xpos = applyPathPrefix(envMapPath, *envmapTable->get_as<std::string>("xpos"));
                std::string yneg = applyPathPrefix(envMapPath, *envmapTable->get_as<std::string>("yneg"));
                std::string ypos = applyPathPrefix(envMapPath, *envmapTable->get_as<std::string>("ypos"));
                std::string zneg = applyPathPrefix(envMapPath, *envmapTable->get_as<std::string>("zneg"));
                std::string zpos = applyPathPrefix(envMapPath, *envmapTable->get_as<std::string>("zpos"));
                auto envmap = std::make_unique<CubeMapEnvironmentMap>();
                envmap->loadFromDirectionFiles(xneg, xpos, yneg, ypos, zneg, zpos);
                envmap->setScaleFactor(envmapTable->get_as<double>("scalefactor").value_or(1.0));
                scene.environmentMap = std::move(envmap);
            }
            else if(type == "gradient") {
                auto low = vectorToRadianceRGB(envmapTable->get_array_of<double>("low").value_or(std::vector<double>{0.0, 0.0, 0.0}));
                auto high = vectorToRadianceRGB(envmapTable->get_array_of<double>("high").value_or(std::vector<double>{1.0, 1.0, 1.0}));
                auto direction = Direction3(vectorToVec3(envmapTable->get_array_of<double>("direction").value_or(std::vector<double>{0.0, 1.0, 0.0})));
                auto envmap = std::make_unique<GradientEnvironmentMap>(low, high, direction);
                scene.environmentMap = std::move(envmap);
            }
        }

        auto meshTableArray = top->get_table_array("meshes");
        if(meshTableArray) {
            for (const auto & meshTable : *meshTableArray) {
                auto name = meshTable->get_as<std::string>("name").value_or("");
                auto filePath = meshTable->get_as<std::string>("file");
                if(!filePath) { throw std::runtime_error("Meshes must supply a file name"); }
                std::string fullFilePath = applyPathPrefix(meshPath, *filePath);
                std::cout << "Mesh: name " << name << " file " << fullFilePath << std::endl;

                auto scaletocube = meshTable->get_as<double>("scaletocube");
                auto accelerator = meshTable->get_as<std::string>("accelerator").value_or("octree");

                auto loadMeshAux = [&](TriangleMesh * mesh) {
                    loadMaterialForObject(meshTable, *mesh, scene);

                    if(scaletocube) {
#if 1
                        printf("**** WARNING - encountered deprecated 'scaletocube'\n");
#else
                        mesh->scaleToFit(Slab::centeredCube(*scaletocube));
                        Slab bounds = boundingBox(mesh->sharedData->vertices);
                        printf("Scaled mesh bounds: "); bounds.print();
#endif
                    }

                    if(accelerator == "octree") {
                        std::cout << "Building octree" << std::endl;
                        scene.heapManager.add(mesh);
                        TriangleMeshOctree meshOctree(*mesh);
                        meshOctree.build();
                        scene.meshOctrees.emplace_back(std::move(meshOctree));
                        auto & obj = scene.meshOctrees.back();
                        loadTransformsForObject(meshTable, obj, scene);
                    }
                    else {
                        scene.meshes.emplace_back(std::move(*mesh));
                        auto & obj = scene.meshes.back();
                        loadTransformsForObject(meshTable, obj, scene);
                        delete(mesh);
                    }
                };

                std::vector<TriangleMesh*> meshes;

                if(!loadTriangleMeshes(meshes, scene.materials, scene.textureCache, fullFilePath)) {
                    throw std::runtime_error("Error loading mesh");
                }

                std::cout << "Loaded " << meshes.size() << " meshes from " << fullFilePath << '\n';

                for(auto & mesh : meshes) {
                    loadMeshAux(mesh);
                }
            }
        }

        auto sphereTableArray = top->get_table_array("spheres");
        if(sphereTableArray) {
            for (const auto & sphereTable : *sphereTableArray) {
                auto radius = sphereTable->get_as<double>("radius").value_or(1.0);
                auto position = Position3(vectorToVec3(sphereTable->get_array_of<double>("position").value_or(std::vector<double>{0.0, 0.0, 0.0})));

                std::cout << "Sphere: radius " << radius << " position " << position << std::endl;

                scene.spheres.emplace_back(Sphere(position, radius));
                auto & obj = scene.spheres.back();
                loadMaterialForObject(sphereTable, obj.shape, scene);
                loadTransformsForObject(sphereTable, obj, scene);
            }
        }

        auto slabTableArray = top->get_table_array("slabs");
        if(slabTableArray) {
            for (const auto & slabTable : *slabTableArray) {
                auto min = Position3(vectorToVec3(slabTable->get_array_of<double>("min").value_or(std::vector<double>{0.0, 0.0, 0.0})));
                auto max = Position3(vectorToVec3(slabTable->get_array_of<double>("max").value_or(std::vector<double>{0.0, 0.0, 0.0})));

                std::cout << "Slab:  min " << min << " max " << max << std::endl;

                scene.slabs.emplace_back(Slab(min, max));
                auto & obj = scene.slabs.back();
                loadMaterialForObject(slabTable, obj.shape, scene);
                loadTransformsForObject(slabTable, obj, scene);
            }
        }

        auto pointLightTableArray = top->get_table_array("pointlights");
        if(pointLightTableArray) {
            for (const auto & pointLightTable : *pointLightTableArray) {
                auto position = Position3(vectorToVec3(pointLightTable->get_array_of<double>("position").value_or(std::vector<double>{0.0, 0.0, 0.0})));
                auto intensity = vectorToRadianceRGB(pointLightTable->get_array_of<double>("intensity").value_or(std::vector<double>{1.0, 1.0, 1.0}));

                scene.pointLights.emplace_back(position, intensity);
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

bool loadSceneFromTOMLFile(Scene & scene, const std::string & filename)
{
    try {
        auto top = cpptoml::parse_file(filename);
        return loadSceneFromParsedTOML(scene, top);
    }
    catch(cpptoml::parse_exception & e) {
        std::cout << "Caught cpptoml exception: " << e.what() << std::endl;
        return false;
    }
}

bool loadSceneFromTOMLString(Scene & scene, const std::string & toml)
{
    std::istringstream iss(toml);
    cpptoml::parser parser(iss);
    try {
        auto top = parser.parse();
        return loadSceneFromParsedTOML(scene, top);
    }
    catch(cpptoml::parse_exception & e) {
        std::cout << "Caught cpptoml exception: " << e.what() << std::endl;
        return false;
    }
}

