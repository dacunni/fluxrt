
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
    bool hit = false;
    RayIntersection nextIntersection;

    for(const auto & o : scene.spheres) {
        if(findIntersection(ray, o, minDistance, nextIntersection)) {
            intersection = nextIntersection;
            hit = true;
        }
    }

    for(const auto & o : scene.slabs) {
        if(findIntersection(ray, o, minDistance, nextIntersection)) {
            intersection = nextIntersection;
            hit = true;
        }
    }

    for(const auto & o : scene.meshOctrees) {
        if(findIntersection(ray, o, minDistance, nextIntersection)) {
            intersection = nextIntersection;
            hit = true;
        }
    }

    for(const auto & o : scene.meshes) {
        if(findIntersection(ray, o, minDistance, nextIntersection)) {
            intersection = nextIntersection;
            hit = true;
        }
    }

    return hit;
}

