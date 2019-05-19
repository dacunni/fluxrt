#include <stdlib.h>
#include <iostream>

#include "base.h"
#include "image.h"
#include "sphere.h"
#include "triangle.h"
#include "trianglemesh.h"
#include "trianglemeshoctree.h"

template<typename OBJ>
void make_intersection_images(const OBJ & obj,
                              const std::string & name)
{
    int w = 256, h = 256;
    const float minDistance = 0.01f;
    Image<float> hitMask(w, h, 1);
    hitMask.setAll(0.0f);
    Image<float> isectMask(w, h, 1);
    isectMask.setAll(0.0f);
    Image<float> isectDist(w, h, 3);
    isectDist.setAll(0.0f);
    Image<float> isectNormal(w, h, 3);
    isectNormal.setAll(0.0f);
    Image<float> isectPos(w, h, 3);
    isectPos.setAll(0.0f);

    Ray ray;
    RayIntersection isect;

    std::cout << "START intersect image " << name << std::endl;
    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            ray.origin = Position3(-1.0f + 2.0f * (float) x / (w - 1),
                                   +1.0f - 2.0f * (float) y / (h - 1),
                                   2.0f);
            ray.direction = Direction3(0.0f, 0.0f, -1.0f);

            // Predicate intersection (hit / no hit)
            bool hit = intersects(ray, obj, minDistance);
            hitMask.set(x, y, 0, hit ? 1.0f : 0.0f);

            // Full intersection
            hit = findIntersection(ray, obj, minDistance, isect);
            isectMask.set(x, y, 0, hit ? 1.0f : 0.0f);
            if(hit) {
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
                    //float v = isect.distance / 4.0;
                    float v = std::log10(isect.distance);
                    isectDist.set3(x, y, v, v, v);
                }
                isectNormal.set3(x, y,
                                 isect.normal.x * 0.5f + 0.5f,
                                 isect.normal.y * 0.5f + 0.5f,
                                 isect.normal.z * 0.5f + 0.5f);
                isectPos.set3(x, y,
                                 isect.position.x * 0.5f + 0.5f,
                                 isect.position.y * 0.5f + 0.5f,
                                 isect.position.z * 0.5f + 0.5f);
            }
        }
    }
    std::cout << "END intersect image " << name << std::endl;

    const std::string prefix = std::string("simple_isect_") + name + "_";

    writePNG(hitMask, prefix + "hit_mask.png");
    writePNG(isectMask, prefix + "isect_mask.png");
    writePNG(isectDist, prefix + "isect_distance.png");
    writePNG(isectNormal, prefix + "isect_normal.png");
    writePNG(isectPos, prefix + "isect_position.png");
}

int main(int argc, char ** argv)
{
    Sphere sphere(Position3(0, 0, 0), 0.5f);
    make_intersection_images(sphere, "sphere");

    Triangle triangle;
    triangle.vertices[0] = Position3(-0.4, -0.3,  0.0f);
    triangle.vertices[1] = Position3( 0.1,  0.6,  0.2f);
    triangle.vertices[2] = Position3( 0.5, -0.5, -0.3f);
    make_intersection_images(triangle, "triangle");

    TriangleMesh mesh;
    if(!loadTriangleMesh(mesh, "models/blender", "sphere.obj")) {
    //if(!loadTriangleMesh(mesh, "models/blender", "monkey2.obj")) {
        std::cerr << "Error loading mesh\n";
        return EXIT_FAILURE;
    }
    make_intersection_images(mesh, "mesh");

    TriangleMeshOctree meshOctree(mesh);
    meshOctree.build();
    make_intersection_images(meshOctree, "mesh_octree");

    return EXIT_SUCCESS;
}

