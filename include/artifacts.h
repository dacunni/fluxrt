#ifndef __ARTIFACTS_H__
#define __ARTIFACTS_H__

#include "scene.h"
#include "image.h"

class Artifacts
{
    public:
        Artifacts();
        Artifacts(int w, int h);

        void writeAll();

        inline void setIntersection(int x, int y, float minDistance, const Scene & scene, const RayIntersection & intersection)
        {
            setHit(x, y, true);
            setDistance(x, y, minDistance, intersection.distance);
            setPosition(x, y, intersection.position);
            setNormal(x, y, intersection.normal);
            setTangent(x, y, intersection.tangent);
            setBitangent(x, y, intersection.bitangent);
            setTexCoord(x, y, intersection.texcoord);
            setMatDiffuseColor(x, y, intersection, scene.materials, scene.textures);
            setMatSpecularColor(x, y, intersection, scene.materials, scene.textures);
            setBasicLighting(x, y, intersection, scene.materials, scene.textures);
        }

        inline void setHit(int x, int y, bool hit) { hitMask.set(x, y, 0, 1.0f); }
        inline void setDistance(int x, int y, float minDistance, float distance) { setDistColor(isectDist, x, y, minDistance, distance); }
        inline void setPosition(int x, int y, const Position3 & position) { setPositionColor(isectPos, x, y, position); }
        inline void setNormal(int x, int y, const Direction3 & direction) { setDirectionColor(isectNormal, x, y, direction); }
        inline void setTangent(int x, int y, const Direction3 & direction) { setDirectionColor(isectTangent, x, y, direction); }
        inline void setBitangent(int x, int y, const Direction3 & direction) { setDirectionColor(isectBitangent, x, y, direction); }
        inline void setTexCoord(int x, int y, const TextureCoordinate & texcoord) { setTexCoordColor(isectTexCoord, x, y, texcoord); }

        inline void setMatDiffuseColor(int x, int y, const RayIntersection & isect,
                                       const MaterialArray & materials, const TextureArray & textures)
        { setMatDiffuseColor(isectMatDiffuse, x, y, isect, materials, textures); }

        inline void setMatSpecularColor(int x, int y, const RayIntersection & isect,
                                        const MaterialArray & materials, const TextureArray & textures)
        { setMatSpecularColor(isectMatSpecular, x, y, isect, materials, textures); }

        inline void setBasicLighting(int x, int y, const RayIntersection & isect,
                                     const MaterialArray & materials, const TextureArray & textures)
        {
            vec3 L = vec3(1.0f, 1.0f, 1.0f).normalized();
            float NdL = clampedDot(isect.normal, L);

            float r, g, b;
            //r = g = b = NdL; // diffuse
            r = g = b = NdL + 0.2; // diffuse + some ambient

            // Modulate by diffuse material
            if(isect.material != NoMaterial) {
                auto & m = materials[isect.material];
                auto D = m.diffuse(textures, isect.texcoord);
                r *= D.r;
                g *= D.g;
                b *= D.b;
            }

            isectBasicLighting.set3(x, y, r, g, b);
        }

    protected:
        inline void setDistColor(Image<float> & isectDist, int x, int y, float minDistance, float distance)
        {
            if(distance >= std::numeric_limits<float>::max())
                isectDist.set3(x, y, 1.0f, 1.0f, 0.0f);
            else if(std::isinf(distance))
                isectDist.set3(x, y, 1.0f, 0.0f, 0.0f);
            else if(std::isnan(distance))
                isectDist.set3(x, y, 1.0f, 0.0f, 1.0f);
            else if(distance < minDistance)
                isectDist.set3(x, y, 0.0f, 1.0f, 1.0f);
            else if(distance == FLT_MAX)
                isectDist.set3(x, y, 1.0f, 0.5f, 0.0f);
            else {
                float v = std::log10(distance);
                isectDist.set3(x, y, v, v, v);
            }
        }

        inline void setPositionColor(Image<float> & isectPos, int x, int y, const Position3 & position)
        {
            isectPos.set3(x, y,
                          position.x * 0.5f + 0.5f,
                          position.y * 0.5f + 0.5f,
                          position.z * 0.5f + 0.5f);
        }

        inline void setDirectionColor(Image<float> & image, int x, int y, const Direction3 & v)
        {
            image.set3(x, y,
                       v.x * 0.5f + 0.5f,
                       v.y * 0.5f + 0.5f,
                       v.z * 0.5f + 0.5f);
        }

        inline void setTexCoordColor(Image<float> & image, int x, int y, const TextureCoordinate & texcoord)
        {
            image.set3(x, y, texcoord.u, texcoord.v, 0.0f);
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

        Image<float> hitMask;
        Image<float> isectDist;
        Image<float> isectNormal;
        Image<float> isectTangent;
        Image<float> isectBitangent;
        Image<float> isectTexCoord;
        Image<float> isectPos;
        Image<float> isectBasicLighting;
        Image<float> isectMatDiffuse;
        Image<float> isectMatSpecular;

    protected:
        int w = 0, h = 0;
        std::string prefix = "trace_";
};


#endif
