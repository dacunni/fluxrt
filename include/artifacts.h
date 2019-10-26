#ifndef __ARTIFACTS_H__
#define __ARTIFACTS_H__

#include "scene.h"
#include "image.h"
#include "constants.h"
#include "brdf.h"

class Artifacts
{
    public:
        Artifacts();
        Artifacts(int w, int h);

        void writeAll();
        void writePixelColor();

        inline void accumPixelRadiance(int x, int y, const radiometry::RadianceRGB & rad)
        {
            color::ColorRGB color = { rad.r, rad.g, rad.b };
            accumPixelColor(x, y, color);
        }
        inline void accumPixelColor(int x, int y, const color::ColorRGB & color)
        {
            // TODO: Put a flag around this
            if(std::isinf(color.r) || std::isinf(color.g) || std::isinf(color.b)) {
                std::cerr << "WARNING: Pixel color has Inf value : " << color.string() << '\n';;
            }
            if(std::isnan(color.r) || std::isnan(color.g) || std::isnan(color.b)) {
                std::cerr << "WARNING: Pixel color has NaN value : " << color.string() << '\n';;
            }
            if(color.r < 0.0f || color.g < 0.0f || color.b < 0.0f) {
                std::cerr << "WARNING: Pixel color has Negative value : " << color.string() << '\n';;
            }
            pixelColor.accum(x, y, color);
            samplesPerPixel.accum(x, y, 0, 1);
        }

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
            Direction3 L = Direction3(1.0f, 1.0f, 1.0f).normalized();
            float NdL = clampedDot(isect.normal, L);

            ReflectanceRGB D = { 0.9, 0.9, 0.9 };
            ReflectanceRGB S = { 0.0, 0.0, 0.0 };
            //float ambient = 0.2;
            float ambient = 0.0;
            float r, g, b;
            r = g = b = ambient;

            // Simple diffuse/specular model
            if(isect.material != NoMaterial) {
                auto & m = materials[isect.material];
                D = m.diffuse(textures, isect.texcoord);
                S = m.specular(textures, isect.texcoord);
            }

            // Specular exponential response
            Direction3 wi(isect.ray.direction.negated());
            Direction3 wo(mirror(wi, isect.normal));
            float Sin = brdf::phong(wi, Direction3(L), isect.normal, 10.0);

            r += NdL * ((1.0f - S.r) * D.r + S.r * Sin);
            g += NdL * ((1.0f - S.g) * D.g + S.g * Sin);
            b += NdL * ((1.0f - S.b) * D.b + S.b * Sin);

            isectBasicLighting.set3(x, y, r, g, b);
        }

        inline void setAmbientOcclusion(int x, int y, float ao) { setAmbientOcclusionColor(isectAO, x, y, ao); }
        inline void setTime(int x, int y, float tm) { setTimeColor(isectTime, x, y, tm); }

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

        inline color::ColorRGB positionColor(const Position3 & position)
        {
            return color::ColorRGB(position.x * 0.5f + 0.5f,
                                   position.y * 0.5f + 0.5f,
                                   position.z * 0.5f + 0.5f);
        }

        inline color::ColorRGB directionColor(const Direction3 & direction)
        {
            return color::ColorRGB(direction.x * 0.5f + 0.5f,
                                   direction.y * 0.5f + 0.5f,
                                   direction.z * 0.5f + 0.5f);
        }

        inline void setPositionColor(Image<float> & image, int x, int y, const Position3 & position)
        {
            image.set3(x, y, positionColor(position));
        }

        inline void setDirectionColor(Image<float> & image, int x, int y, const Direction3 & direction)
        {
            image.set3(x, y, directionColor(direction));
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

        inline void setAmbientOcclusionColor(Image<float> & image, int x, int y, float ao)
        {
            image.set3(x, y, ao, ao, ao);
            hasAO = true;
        }

        inline void setTimeColor(Image<float> & image, int x, int y, float tm)
        {
            image.set3(x, y, tm, tm, tm);
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
        Image<float> isectAO;
        Image<float> isectTime;

        Image<float> pixelColor;
        Image<uint32_t> samplesPerPixel;

        bool hasAO = false;

    protected:
        int w = 0, h = 0;
        std::string prefix = "trace_";
};


#endif
