
inline bool intersectsWorldRay(const Ray & rayWorld, const Scene & scene,
                               float minDistance, float maxDistance)
{
    for(const auto & o : scene.objects) {
        if(o->intersectsWorldRay(rayWorld, minDistance, maxDistance)) {
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
            assert(intersection.distance >= minDistance);
        }
    };

    // Iterate over scene objects

    if(scene.useKDTreeAccelerator) {
        if(scene.objectsKDTree.findIntersectionWorldRay(rayWorld, minDistance, nextIntersection)) {
            updateBestHit();
        }
    }
    else {
        for(const auto & o : scene.objects) {
            if(o->findIntersectionWorldRay(rayWorld, minDistance, nextIntersection)) {
                updateBestHit();
            }
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

