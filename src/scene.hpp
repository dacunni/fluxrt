
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

    for(const auto & o : scene.meshOctrees) {
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

inline bool findIntersection(const Ray & ray, const Scene & scene,
                             float minDistance, RayIntersection & intersection)
{
    RayIntersection nextIntersection;
    bool hit = false;

    const auto updateBestHit = [&]() {
        if(!hit || nextIntersection.distance < intersection.distance) {
            intersection = nextIntersection;
            hit = true;
        }
    };

    // Iterate over scene objects

    for(const auto & o : scene.spheres) {
        if(findIntersection(ray, o, minDistance, nextIntersection)) {
            updateBestHit();
        }
    }

    for(const auto & o : scene.slabs) {
        if(findIntersection(ray, o, minDistance, nextIntersection)) {
            updateBestHit();
        }
    }

    for(const auto & o : scene.meshOctrees) {
        if(findIntersection(ray, o, minDistance, nextIntersection)) {
            updateBestHit();
        }
    }

    for(const auto & o : scene.meshes) {
        if(findIntersection(ray, o, minDistance, nextIntersection)) {
            updateBestHit();
        }
    }

    return hit;
}

