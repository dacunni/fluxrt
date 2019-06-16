#include <stdlib.h>
#include <iostream>
#include <chrono>

#include "base.h"
#include "image.h"
#include "sphere.h"
#include "triangle.h"
#include "trianglemesh.h"
#include "trianglemeshoctree.h"

inline void setDistColor(Image<float> & isectDist, int x, int y, float minDistance, const RayIntersection & isect)
{
    if(isect.distance >= std::numeric_limits<float>::max())
        isectDist.set3(x, y, 1.0f, 1.0f, 0.0f);
    else if(std::isinf(isect.distance))
        isectDist.set3(x, y, 1.0f, 0.0f, 0.0f);
    else if(std::isnan(isect.distance))
        isectDist.set3(x, y, 1.0f, 0.0f, 1.0f);
    else if(isect.distance < minDistance)
        isectDist.set3(x, y, 0.0f, 1.0f, 1.0f);
    else if(isect.distance == FLT_MAX)
        isectDist.set3(x, y, 1.0f, 0.5f, 0.0f);
    else {
        float v = std::log10(isect.distance);
        isectDist.set3(x, y, v, v, v);
    }
}

inline void setPositionColor(Image<float> & isectPos, int x, int y, const RayIntersection & isect)
{
    isectPos.set3(x, y,
                  isect.position.x * 0.5f + 0.5f,
                  isect.position.y * 0.5f + 0.5f,
                  isect.position.z * 0.5f + 0.5f);
}

inline void setDirectionColor(Image<float> & image, int x, int y, const vec3 & v)
{
    image.set3(x, y,
               v.x * 0.5f + 0.5f,
               v.y * 0.5f + 0.5f,
               v.z * 0.5f + 0.5f);
}

inline void setNormalColor(Image<float> & image, int x, int y, const RayIntersection & isect)
{
    setDirectionColor(image, x, y, isect.normal);
}

inline void setTangentColor(Image<float> & image, int x, int y, const RayIntersection & isect)
{
    setDirectionColor(image, x, y, isect.tangent);
}

inline void setBitangentColor(Image<float> & image, int x, int y, const RayIntersection & isect)
{
    setDirectionColor(image, x, y, isect.bitangent);
}

inline void setTexCoordColor(Image<float> & image, int x, int y, const RayIntersection & isect)
{
    image.set3(x, y, isect.texcoord.u, isect.texcoord.v, 0.0f);
}

inline void setBasicLightingColor(Image<float> & image, int x, int y, const RayIntersection & isect)
{
    vec3 L = vec3(1.0f, 1.0f, 1.0f).normalized();
    float NdL = clampedDot(isect.normal, L);
    image.set3(x, y, NdL, NdL, NdL);
}

inline void setMatDiffuseColor(Image<float> & image, int x, int y, const RayIntersection & isect,
                               const MaterialArray & materials, const TextureArray & textures)
{
    if(isect.material == NoMaterial) { image.set3(x, y, 0.0f, 0.0f, 0.0f); }
    else {
        auto & m = materials[isect.material];
        auto D = m.diffuse(textures, isect.texcoord);
        image.set3(x, y, D.r, D.g, D.b);
    }
}

inline void setMatSpecularColor(Image<float> & image, int x, int y, const RayIntersection & isect,
                               const MaterialArray & materials, const TextureArray & textures)
{
    if(isect.material == NoMaterial) { image.set3(x, y, 0.0f, 0.0f, 0.0f); }
    else {
        auto & m = materials[isect.material];
        auto S = m.specular(textures, isect.texcoord);
        image.set3(x, y, S.r, S.g, S.b);
    }
}

template<typename OBJ>
void make_intersection_images(const OBJ & obj, const MaterialArray & materials, const TextureArray & textures,
                              const std::string & name)
{
    int w = 256, h = 256;
    //int w = 1024, h = 1024;
    const float minDistance = 0.01f;
    Image<float> hitMask(w, h, 1); hitMask.setAll(0.0f);
    Image<float> isectMask(w, h, 1); isectMask.setAll(0.0f);
    Image<float> isectDist(w, h, 3); isectDist.setAll(0.0f);
    Image<float> isectNormal(w, h, 3); isectNormal.setAll(0.0f);
    Image<float> isectTangent(w, h, 3); isectTangent.setAll(0.0f);
    Image<float> isectBitangent(w, h, 3); isectBitangent.setAll(0.0f);
    Image<float> isectTexCoord(w, h, 3); isectTexCoord.setAll(0.0f);
    Image<float> isectPos(w, h, 3); isectPos.setAll(0.0f);
    Image<float> isectBasicLighting(w, h, 3); isectBasicLighting.setAll(0.0f);
    Image<float> isectMatDiffuse(w, h, 3); isectMatDiffuse.setAll(0.0f);
    Image<float> isectMatSpecular(w, h, 3); isectMatDiffuse.setAll(0.0f);

    using clock = std::chrono::system_clock;

    RayIntersection isect;

    auto pixelRay = [&](int x, int y) {
#if 0   // zoom in on part of the model
        float xd = 0.25f, yd = 0.25f;
        float cx = -0.75f, cy = -0.0f;
        return Ray(Position3(cx + xd * ((float) x / (w - 1) - 0.5f),
                             cy - yd * ((float) y / (h - 1) - 0.5f),
                             2.0f),
#elif 0
        // living room
        float xd = 0.25f, yd = 0.25f;
        float cx = 0.0f, cy = -0.15f;
        return Ray(Position3(cx + xd * ((float) x / (w - 1) - 0.5f),
                             cy - yd * ((float) y / (h - 1) - 0.5f),
                             0.0f),
#else
        return Ray(Position3(-1.0f + 2.0f * (float) x / (w - 1),
                             +1.0f - 2.0f * (float) y / (h - 1),
                             2.0f),
#endif
                   Direction3(0.0f, 0.0f, -1.0f));
    };

    auto timeCall = [&](const std::string & testLabel, std::function<void()> fn) {
        std::cout << testLabel << " : " << name << " ... " << std::flush;
        auto start = clock::now();
        fn();
        auto end = clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << elapsed.count() << " sec "
                  << (w * h / elapsed.count()) << " obj/sec"
                  << std::endl;
    };

    timeCall("hit      ", [&]() {
        hitMask.forEachPixel(
            [&](Image<float> &, int x, int y) {
                // Predicate intersection (hit / no hit)
                bool hit = intersects(pixelRay(x, y), obj, minDistance);
                hitMask.set(x, y, 0, hit ? 1.0f : 0.0f);
            });
        });

    timeCall("intersect", [&]() {
        isectMask.forEachPixel(
            [&](Image<float> &, int x, int y) {
                // Full intersection
                bool hit = findIntersection(pixelRay(x, y), obj, minDistance, isect);
                isectMask.set(x, y, 0, hit ? 1.0f : 0.0f);
                if(hit) {
                    setDistColor(isectDist, x, y, minDistance, isect);
                    setPositionColor(isectPos, x, y, isect);
                    setNormalColor(isectNormal, x, y, isect);
                    setTangentColor(isectTangent, x, y, isect);
                    setBitangentColor(isectBitangent, x, y, isect);
                    setTexCoordColor(isectTexCoord, x, y, isect);
                    setBasicLightingColor(isectBasicLighting, x, y, isect);
                    setMatDiffuseColor(isectMatDiffuse, x, y, isect, materials, textures);
                    setMatSpecularColor(isectMatSpecular, x, y, isect, materials, textures);
                }
            });
        });

    const std::string prefix = std::string("simple_isect_") + name + "_";

    writePNG(hitMask, prefix + "hit_mask.png");
    writePNG(isectMask, prefix + "isect_mask.png");
    writePNG(isectDist, prefix + "isect_distance.png");
    writePNG(isectNormal, prefix + "isect_normal.png");
    writePNG(isectTangent, prefix + "isect_tangent.png");
    writePNG(isectBitangent, prefix + "isect_bitangent.png");
    writePNG(isectTexCoord, prefix + "isect_texcoord.png");
    writePNG(isectPos, prefix + "isect_position.png");
    writePNG(isectBasicLighting, prefix + "isect_basic_lighting.png");
    writePNG(isectMatDiffuse, prefix + "isect_mat_diffuse.png");
    writePNG(isectMatSpecular, prefix + "isect_mat_specular.png");
}

int main(int argc, char ** argv)
{
    MaterialArray materials;
    TextureArray textures;

    Sphere sphere(Position3(0, 0, 0), 0.5f);
    make_intersection_images(sphere, materials, textures, "sphere");

    Triangle triangle;
    triangle.vertices[0] = Position3(-0.4, -0.3,  0.0f);
    triangle.vertices[1] = Position3( 0.1,  0.6,  0.2f);
    triangle.vertices[2] = Position3( 0.5, -0.5, -0.3f);
    make_intersection_images(triangle, materials, textures, "triangle");

    TriangleMesh mesh;
    //if(!loadTriangleMesh(mesh, materials, textures, "models/blender", "sphere.obj")) {
    //if(!loadTriangleMesh(mesh, materials, textures, "models/blender", "monkey2.obj")) {
    //if(!loadTriangleMesh(mesh, materials, textures, "models/blender", "monkey3.obj")) {
    //if(!loadTriangleMesh(mesh, materials, textures, "models/blender", "monkey_simple_flat.obj")) {
    //if(!loadTriangleMesh(mesh, materials, textures, "models/blender", "monkey_simple_smooth.obj")) {
    //if(!loadTriangleMesh(mesh, materials, textures, "models/casual-effects.com/sportsCar", "sportsCar.obj")) {
    if(!loadTriangleMesh(mesh, materials, textures, "models/casual-effects.com/bmw", "bmw.obj")) {
    //if(!loadTriangleMesh(mesh, materials, textures, "models/casual-effects.com/mitsuba", "mitsuba-sphere.obj")) {
    //if(!loadTriangleMesh(mesh, materials, textures, "models/casual-effects.com/mori_knob", "testObj.obj")) {
    //if(!loadTriangleMesh(mesh, materials, textures, "models/casual-effects.com/living_room", "living_room.obj")) {
        std::cerr << "Error loading mesh\n";
        return EXIT_FAILURE;
    }
    Slab meshBounds = boundingBox(mesh.vertices);
    printf("Mesh bounds: "); meshBounds.print();
    printf("Scaling mesh to visible area\n");
    mesh.scaleToFit(Slab::centeredCube(2.0f));
    meshBounds = boundingBox(mesh.vertices);
    printf("Mesh bounds: "); meshBounds.print();
    //make_intersection_images(mesh, materials, textures, "mesh";

    TriangleMeshOctree meshOctree(mesh);
    meshOctree.build();
    printf("Octree covers all triangles: %d\n", (int) meshOctree.nodesCoverAllTriangles());
    make_intersection_images(meshOctree, materials, textures, "mesh_octree");

    return EXIT_SUCCESS;
}

