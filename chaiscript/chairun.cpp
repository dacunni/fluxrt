#include <iostream>
#include <cstdlib>
#include <chaiscript/chaiscript.hpp>

#include "micromath.h"

int main(int argc, char ** argv)
{
    using chaiscript::fun;
    using chaiscript::constructor;
    using chaiscript::user_type;
    using chaiscript::base_class;

    chaiscript::ChaiScript chai;

    // Vector Types

    chai.add(user_type<vec3>(), "vec3");
    chai.add(constructor<vec3()>(), "vec3");
    chai.add(constructor<vec3(float, float, float)>(), "vec3");
    chai.add(fun(&vec3::x), "x");
    chai.add(fun(&vec3::y), "y");
    chai.add(fun(&vec3::z), "z");

    chai.add(user_type<Position3>(), "Position3");
    chai.add(base_class<vec3, Position3>());
    chai.add(constructor<Position3()>(), "Position3");
    chai.add(constructor<Position3(const Position3 &)>(), "Position3");
    chai.add(constructor<Position3(float, float, float)>(), "Position3");

    chai.add(user_type<Direction3>(), "Direction3");
    chai.add(base_class<vec3, Direction3>());
    chai.add(constructor<Direction3()>(), "Direction3");
    chai.add(constructor<Direction3(const Direction3 &)>(), "Direction3");
    chai.add(constructor<Direction3(float, float, float)>(), "Direction3");
    chai.add(fun(&Direction3::normalized), "normalized");

    // Ray

    chai.add(user_type<Ray>(), "Ray");
    chai.add(constructor<Ray()>(), "Ray");
    chai.add(constructor<Ray(const Position3 &, const Direction3 &)>(), "Ray");
    chai.add(fun(&Ray::origin), "origin");
    chai.add(fun(&Ray::direction), "direction");

    // RayIntersection

    chai.add(user_type<RayIntersection>(), "RayIntersection");
    chai.add(constructor<RayIntersection()>(), "RayIntersection");
    chai.add(fun(&RayIntersection::position), "position");
    chai.add(fun(&RayIntersection::normal), "normal");
    chai.add(fun(&RayIntersection::tangent), "tangent");
    chai.add(fun(&RayIntersection::bitangent), "bitangent");
    chai.add(fun(&RayIntersection::distance), "distance");
    chai.add(fun(&RayIntersection::material), "material");
    chai.add(fun(&RayIntersection::texcoord), "texcoord");
    chai.add(fun(&RayIntersection::hasTexCoord), "hasTexCoord");

    // Texture

    chai.add(user_type<Texture>(), "Texture");
    chai.add(constructor<Texture(size_t, size_t, int)>(), "Texture");

    chai.add(user_type<TextureArray>(), "TextureArray");
    chai.add(constructor<TextureArray()>(), "TextureArray");

    // Material

    chai.add(user_type<Material>(), "Material");
    chai.add(constructor<Material()>(), "Material");
    chai.add(fun(&Material::makeDiffuse), "makeDiffuse");
    chai.add(fun(&Material::makeDiffuseSpecular), "makeDiffuseSpecular");

    chai.add(user_type<MaterialArray>(), "MaterialArray");
    chai.add(constructor<MaterialArray()>(), "MaterialArray");

    // Sphere

    chai.add(user_type<Sphere>(), "Sphere");
    chai.add(constructor<Sphere()>(), "Sphere");
    chai.add(constructor<Sphere(const Sphere &)>(), "Sphere");
    chai.add(constructor<Sphere(const Position3 &, float)>(), "Sphere");
    chai.add(fun(&Sphere::center), "center");
    chai.add(fun(&Sphere::radius), "radius");

    // Slab

    chai.add(user_type<Slab>(), "Slab");
    chai.add(constructor<Slab()>(), "Slab");
    chai.add(constructor<Slab(float, float, float, float, float, float)>(), "Slab");
    chai.add(constructor<Slab(const Position3 &, const Position3 &)>(), "Slab");
    chai.add(fun(&Slab::xmin), "xmin");
    chai.add(fun(&Slab::ymin), "ymin");
    chai.add(fun(&Slab::zmin), "zmin");
    chai.add(fun(&Slab::xmax), "xmax");
    chai.add(fun(&Slab::ymax), "ymax");
    chai.add(fun(&Slab::zmax), "zmax");

    chai.add(fun(&boundingBox), "boundingBox");

    // TriangleMesh

    chai.add(user_type<TriangleMesh>(), "TriangleMesh");
    chai.add(constructor<TriangleMesh()>(), "TriangleMesh");
    chai.add(fun(&TriangleMesh::vertices), "vertices");
    chai.add(fun(&TriangleMesh::normals), "normals");
    chai.add(fun(&TriangleMesh::texcoords), "texcoords");

    chai.add(fun(&loadTriangleMesh), "loadTriangleMesh");

    // TriangleMeshOctree

    chai.add(user_type<TriangleMeshOctree>(), "TriangleMeshOctree");
    chai.add(constructor<TriangleMeshOctree(TriangleMesh &)>(), "TriangleMeshOctree");
    chai.add(fun(&TriangleMeshOctree::build), "build");

    // Ray-Object Intersection

    // Sphere
    auto intersects_Sphere = [](const Ray & ray, const Sphere & obj, float minDistance) {
        return intersects(ray, obj, minDistance);
    };
    auto findIntersection_Sphere = [](const Ray & ray, const Sphere & obj, float minDistance, RayIntersection & intersection) {
        return findIntersection(ray, obj, minDistance, intersection);
    };
    chai.add(fun(intersects_Sphere), "intersects");
    chai.add(fun(findIntersection_Sphere), "findIntersection");

    // Slab
    auto intersects_Slab = [](const Ray & ray, const Slab & obj, float minDistance) {
        return intersects(ray, obj, minDistance);
    };
    auto findIntersection_Slab = [](const Ray & ray, const Slab & obj, float minDistance, RayIntersection & intersection) {
        return findIntersection(ray, obj, minDistance, intersection);
    };
    chai.add(fun(intersects_Slab), "intersects");
    chai.add(fun(findIntersection_Slab), "findIntersection");

    // TriangleMesh
    auto intersects_TriangleMesh = [](const Ray & ray, const TriangleMesh & obj, float minDistance) {
        return intersects(ray, obj, minDistance);
    };
    auto findIntersection_TriangleMesh = [](const Ray & ray, const TriangleMesh & obj, float minDistance, RayIntersection & intersection) {
        return findIntersection(ray, obj, minDistance, intersection);
    };
    chai.add(fun(intersects_TriangleMesh), "intersects");
    chai.add(fun(findIntersection_TriangleMesh), "findIntersection");

    // TriangleMeshOctree
    auto intersects_TriangleMeshOctree = [](const Ray & ray, const TriangleMeshOctree & obj, float minDistance) {
        return intersects(ray, obj, minDistance);
    };
    auto findIntersection_TriangleMeshOctree = [](const Ray & ray, const TriangleMeshOctree & obj, float minDistance, RayIntersection & intersection) {
        return findIntersection(ray, obj, minDistance, intersection);
    };
    chai.add(fun(intersects_TriangleMeshOctree), "intersects");
    chai.add(fun(findIntersection_TriangleMeshOctree), "findIntersection");

    if(argc > 1) {
        std::cout << "Running ChaiScript file '" << argv[1] << "'\n";
        chai.eval_file(argv[1]);
    }

    return EXIT_SUCCESS;
}

