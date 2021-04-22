#include <exception>
#include "cpptoml.h"

#include "scene.h"
#include "vectortypes.h"
#include "filesystem.h"
#include "constants.h"
#include "transform.h"
#include "timer.h"
#include "GradientEnvironmentMap.h"
#include "LatLonEnvironmentMap.h"
#include "CubeMapEnvironmentMap.h"

static vec3 vectorToVec3(const std::vector<double> & v)
{
    return vec3(v[0], v[1], v[2]);
}

static color::ColorRGB vectorToColorRGB(const std::vector<double> & v)
{
    return color::ColorRGB(v[0], v[1], v[2]);
}

static RadianceRGB vectorToRadianceRGB(const std::vector<double> & v)
{
    return RadianceRGB(v[0], v[1], v[2]);
}

static ReflectanceRGB vectorToReflectanceRGB(const std::vector<double> & v)
{
    return ReflectanceRGB(v[0], v[1], v[2]);
}

static ParameterRGB vectorToParameterRGB(const std::vector<double> & v)
{
    return ParameterRGB(v[0], v[1], v[2]);
}

void loadMaterialDiffuseComponent(const std::shared_ptr<cpptoml::table> & table, Scene & scene, Material & material)
{
    // Try to load as either a single color or a texture
    auto rgb = table->get_array_of<double>("diffuse");
    auto tex = table->get_as<std::string>("diffuse");

    if(rgb) {
        material.diffuseColor = vectorToReflectanceRGB(*rgb);
    }
    else if(tex) {
        material.diffuseTexture = scene.textureCache.loadTextureFromFile("", *tex);
    }
    else {
        material.diffuseColor = {0.0f, 0.0f, 0.0};
    }
}

void loadMaterialSpecularComponent(const std::shared_ptr<cpptoml::table> & table, Scene & scene, Material & material)
{
    auto rgb = table->get_array_of<double>("specular");
    auto tex = table->get_as<std::string>("specular");
    auto specularExponent = table->get_as<double>("specular_exponent").value_or(0.0);

    material.specularExponent = specularExponent;

    if(rgb) {
        material.specularColor = vectorToReflectanceRGB(*rgb);
    }
    else if(tex) {
        material.specularTexture = scene.textureCache.loadTextureFromFile("", *tex);
    }
    else {
        material.specularColor = {0.0f, 0.0f, 0.0};
    }
}

MaterialID loadMaterial(const std::shared_ptr<cpptoml::table> & materialTable, Scene & scene,
                        const std::map<std::string, MaterialID> & namedMaterials)
{
    auto include = materialTable->get_as<std::string>("include");
    if(include) {
        std::cout << "Including named material: " << *include << '\n';
        auto it = namedMaterials.find(*include);
        if(it == namedMaterials.end()) {
            throw std::runtime_error("Material include references non-existent named material " + *include);
        }
        return it->second;
    }

    auto type = materialTable->get_as<std::string>("type");
    if(!type) { throw std::runtime_error("Material must supply a type"); }

    Material material;

    if(*type == "diffuse") {
        loadMaterialDiffuseComponent(materialTable, scene, material);
    }
    else if(*type == "diffusespecular") {
        loadMaterialDiffuseComponent(materialTable, scene, material);
        loadMaterialSpecularComponent(materialTable, scene, material);
    }
    else if(*type == "mirror") {
        material = Material::makeMirror();
    }
    else if(*type == "refractive") {
        auto indexOfRefraction = materialTable->get_as<double>("ior").value_or(1.333);
        material = Material::makeRefractive(indexOfRefraction);
        auto beersLawAtt = vectorToParameterRGB(materialTable->get_array_of<double>("beer").value_or(std::vector<double>{0.0, 0.0, 0.0}));
        material.innerMedium.beersLawAttenuation = beersLawAtt;
    }
    else if(*type == "emissive") {
        auto rgb = materialTable->get_array_of<double>("emissive");
        auto emissive = vectorToRadianceRGB(*rgb);
        material = Material::makeEmissive(emissive);
    }
    else {
        throw std::runtime_error(std::string("Unknown material type : ") + *type);
    }

    auto normalMapTex = materialTable->get_as<std::string>("normalmap");
    if(normalMapTex) {
        material.normalMapTexture = scene.textureCache.loadTextureFromFile("", *normalMapTex);
        auto & texture = scene.textureCache.textures[material.normalMapTexture];
        // Convert normal map color to normal [0, 1] -> [-1, 1]
        texture->forEachPixelChannel(
                                     [](Texture & image, size_t x, size_t y, int c) {
                                     image.set(x, y, c, image.get(x, y, c) * 2.0f - 1.0f);
                                     });
    }

    MaterialID id = scene.materials.size();
    scene.materials.push_back(material);
    return id;
}

template<typename OBJ>
void loadMaterialForObject(const std::shared_ptr<cpptoml::table> & table, OBJ & obj, Scene & scene,
                           std::map<std::string, MaterialID> & namedMaterials)
{
    auto materialTable = table->get_table("material");
    if(materialTable) {
        obj.material = loadMaterial(materialTable, scene, namedMaterials);
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

bool loadSceneFromParsedTOML(Scene & scene, std::shared_ptr<cpptoml::table> & top,
                             std::map<std::string, MaterialID> & namedMaterials)
{
    try {
        auto getEnvVar = [](const char * name) {
            const char * value = getenv(name);
            if(value)
                return std::string(value);
            return std::string("");
        };
        std::string meshPath = getEnvVar("MESH_PATH");
        std::string envMapPath = getEnvVar("ENV_MAP_PATH");
        std::string scenePath = getEnvVar("SCENE_PATH");

        auto applyPathPrefix = [](const std::string & prefix, const std::string & path) {
            // If prefix is defined and path is not absolute
            if(!prefix.empty() && path.front() != '/') {
                return std::string(prefix) + '/' + path;
            }
            return path;
        };

        std::cout << "Mesh path = " << meshPath << '\n';
        std::cout << "Env map path = " << envMapPath << '\n';
        std::cout << "Scene path = " << scenePath << '\n';

        auto includeTableArray = top->get_table_array("include");
        if(includeTableArray) {
            for (const auto & includeTable : *includeTableArray) {
                auto source = includeTable->get_as<std::string>("source");
                if(!source) { throw std::runtime_error("Includes must supply a file name"); }
                std::string fullFilePath = applyPathPrefix(scenePath, *source);
                std::cout << "Include: source " << fullFilePath << std::endl;
                if(!loadSceneFromTOMLFile(scene, fullFilePath, namedMaterials)) {
                    throw std::runtime_error("Error including file " + fullFilePath);
                }
            }
        }

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
                auto lookAtPos = Position3(vectorToVec3(*lookat));
                direction = (lookAtPos - position).normalized();
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

                auto pinholeCamera = std::make_shared<PinholeCamera>(hfov, vfov);
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

                scene.camera = std::make_shared<OrthoCamera>(hsize, vsize);
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
            else if(type == "latlon") {
                std::string file = applyPathPrefix(envMapPath, *envmapTable->get_as<std::string>("file"));
                auto envmap = std::make_unique<LatLonEnvironmentMap>();
                envmap->loadFromFile(file);
                envmap->setScaleFactor(envmapTable->get_as<double>("scalefactor").value_or(1.0));
                scene.environmentMap = std::move(envmap);
            }
        }

        auto namedMaterialTableArray = top->get_table_array("namedmaterials");
        if(namedMaterialTableArray) {
            for (const auto & namedMaterialTable : *namedMaterialTableArray) {
                auto name = namedMaterialTable->get_as<std::string>("name");
                if(!name) { throw std::runtime_error("Named material requires a name"); }
                std::cout << "Named material: " << *name << '\n';
                namedMaterials[*name] = loadMaterial(namedMaterialTable, scene, namedMaterials);
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

                TriangleMesh * mesh = new TriangleMesh();

                if(!loadTriangleMesh(*mesh, scene.materials, scene.textureCache, fullFilePath)) {
                    throw std::runtime_error("Error loading mesh");
                }

                loadMaterialForObject(meshTable, *mesh, scene, namedMaterials);

                auto scaletocube = meshTable->get_as<double>("scaletocube");
                if(scaletocube) {
                    mesh->scaleToFit(Slab::centeredCube(*scaletocube));
                    Slab bounds = boundingBox(mesh->vertices);
                    printf("Scaled mesh bounds: "); bounds.print();
                }

                auto accelerator = meshTable->get_as<std::string>("accelerator").value_or("octree");

                if(accelerator == "octree") {
                    std::cout << "Building octree" << std::endl;
                    scene.heapManager.add(mesh);
                    TriangleMeshOctree meshOctree(*mesh);
                    auto writeTimer = WallClockTimer::makeRunningTimer();
                    meshOctree.build();
                    auto writeTime = writeTimer.elapsed();
                    printf("Octree built in %f sec\n", writeTime);
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
                loadMaterialForObject(sphereTable, obj, scene, namedMaterials);
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
                loadMaterialForObject(slabTable, obj, scene, namedMaterials);
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

        auto diskLightTableArray = top->get_table_array("disklights");
        if(diskLightTableArray) {
            for(const auto & diskLightTable : *diskLightTableArray) {
                auto position = Position3(vectorToVec3(diskLightTable->get_array_of<double>("position").value_or(std::vector<double>{0.0, 0.0, 0.0})));
                auto direction = Direction3(vectorToVec3(diskLightTable->get_array_of<double>("direction").value_or(std::vector<double>{0.0, -1.0, 0.0})));
                direction.normalize();
                float radius = (float) diskLightTable->get_as<double>("radius").value_or(1.0);
                auto intensity = vectorToRadianceRGB(diskLightTable->get_array_of<double>("intensity").value_or(std::vector<double>{1.0, 1.0, 1.0}));
                Material material = Material::makeEmissive(intensity);
                MaterialID id = scene.materials.size();
                scene.materials.push_back(material);
                scene.diskLights.emplace_back(position, direction, radius, id);
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

bool loadSceneFromTOMLFile(Scene & scene, const std::string & filename,
                           std::map<std::string, MaterialID> & namedMaterials)
{
    try {
        auto top = cpptoml::parse_file(filename);
        return loadSceneFromParsedTOML(scene, top, namedMaterials);
    }
    catch(cpptoml::parse_exception & e) {
        std::cout << "Caught cpptoml exception: " << e.what() << std::endl;
        return false;
    }
}

bool loadSceneFromTOMLFile(Scene & scene, const std::string & filename)
{
    std::map<std::string, MaterialID> namedMaterials;
    return loadSceneFromTOMLFile(scene, filename, namedMaterials);
}

bool loadSceneFromTOMLString(Scene & scene, const std::string & toml,
                             std::map<std::string, MaterialID> & namedMaterials)
{
    std::istringstream iss(toml);
    cpptoml::parser parser(iss);
    try {
        auto top = parser.parse();
        return loadSceneFromParsedTOML(scene, top, namedMaterials);
    }
    catch(cpptoml::parse_exception & e) {
        std::cout << "Caught cpptoml exception: " << e.what() << std::endl;
        return false;
    }
}

bool loadSceneFromTOMLString(Scene & scene, const std::string & toml)
{
    std::map<std::string, MaterialID> namedMaterials;
    return loadSceneFromTOMLString(scene, toml, namedMaterials);
}

