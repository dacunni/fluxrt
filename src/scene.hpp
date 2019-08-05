
inline bool intersects(const Ray & ray, const Scene & scene, float minDistance)
{
    for(const auto & o : scene.spheres) {
        if(intersects(ray, o, minDistance)) {
            return true;
        }
    }

    for(const auto & o : scene.slabs) {
        if(intersects(ray, o, minDistance)) {
            return true;
        }
    }

    for(const auto & o : scene.meshes) {
        if(intersects(ray, o, minDistance)) {
            return true;
        }
    }

    return false;
}

