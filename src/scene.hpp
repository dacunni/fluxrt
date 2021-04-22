
inline bool intersectsWorldRay(const Ray & rayWorld, const Scene & scene,
                               float minDistance, float maxDistance)
{
    for(const auto & o : scene.spheres) {
        if(o.intersectsWorldRay(rayWorld, minDistance, maxDistance)) {
            return true;
        }
    }

    for(const auto & o : scene.slabs) {
        if(o.intersectsWorldRay(rayWorld, minDistance, maxDistance)) {
            return true;
        }
    }

    for(const auto & o : scene.meshOctrees) {
        if(o.intersectsWorldRay(rayWorld, minDistance, maxDistance)) {
            return true;
            return true;
        }
    }

    for(const auto & o : scene.meshes) {
        if(o.intersectsWorldRay(rayWorld, minDistance, maxDistance)) {
            return true;
        }
    }

    for(const auto & o : scene.diskLights) {
        if(o.intersectsWorldRay(rayWorld, minDistance, maxDistance)) {
            return true;
        }
    }

    return false;
}

inline bool findIntersectionWorldRay(const Ray & rayWorld, const Scene & scene,
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
        if(o.findIntersectionWorldRay(rayWorld, minDistance, nextIntersection)) {
            updateBestHit();
        }
    }

    for(const auto & o : scene.slabs) {
        if(o.findIntersectionWorldRay(rayWorld, minDistance, nextIntersection)) {
            updateBestHit();
        }
    }

    for(const auto & o : scene.meshOctrees) {
        if(o.findIntersectionWorldRay(rayWorld, minDistance, nextIntersection)) {
            updateBestHit();
        }
    }

    for(const auto & o : scene.meshes) {
        if(o.findIntersectionWorldRay(rayWorld, minDistance, nextIntersection)) {
            updateBestHit();
        }
    }

    for(const auto & o : scene.diskLights) {
        if(o.findIntersectionWorldRay(rayWorld, minDistance, nextIntersection)) {
            updateBestHit();
        }
    }

    if(hit) {
        intersection.ray = rayWorld;
    }

    return hit;
}

