#include "TraceableKDTree.h"
#include "rng.h"

struct TraceableKDTree::BuildContext {
    RNG rng;
};

void TraceableKDTree::build(std::vector<TraceablePtr> & objects)
{
    // Reset state
    root = {};

    // Build starting from the root
    root.objects = objects;
    BuildContext context;
    buildNode(root, context);
    bounds = root.bounds;

    print();
}

void TraceableKDTree::buildNode(KDNode & node, BuildContext & context, unsigned int depth)
{
    // Get bounding box
    if(!node.objects.empty()) {
        node.bounds = node.objects[0]->boundingBoxTransformed();
        for(unsigned int i = 1; i < node.objects.size(); ++i) {
            node.bounds = merge(node.bounds, node.objects[i]->boundingBoxTransformed());
        }
    }
    else {
        node.bounds = {};
    }

    const unsigned int minSplitSize = 4; // TODO
    const unsigned int maxDepth = 4; // TODO

    // Do not split further if we the number of objects in this node
    // is small or the max depth has been reached
    if(node.objects.size() < minSplitSize || depth > maxDepth) {
        node.splitDirection = KDNode::LEAF;
        return;
    }

    switch(depth % 3) {
        case 0: splitNode(node, STRATEGY_MIDPOINT_X, context, depth); break;
        case 1: splitNode(node, STRATEGY_MIDPOINT_Y, context, depth); break;
        case 2: splitNode(node, STRATEGY_MIDPOINT_Z, context, depth); break;
    }
}

void TraceableKDTree::splitNode(KDNode & node, SplitStrategy strategy, BuildContext & context, unsigned int depth)
{
    // Choose a split plane
    switch(strategy) {
    case STRATEGY_MIDPOINT_X:
        node.splitDirection = KDNode::SPLIT_X;
        node.splitOffset = 0.5f * (node.bounds.xmin + node.bounds.xmax);
        break;
    case STRATEGY_MIDPOINT_Y:
        node.splitDirection = KDNode::SPLIT_Y;
        node.splitOffset = 0.5f * (node.bounds.ymin + node.bounds.ymax);
        break;
    case STRATEGY_MIDPOINT_Z:
        node.splitDirection = KDNode::SPLIT_Z;
        node.splitOffset = 0.5f * (node.bounds.zmin + node.bounds.zmax);
        break;
    default:
        printf("Unsupported split strategy\n");
    }
    // TODO
    //node.splitOffset = context.rng.uniformRange(node.bounds.xmin, node.bounds.xmax);
    printf("Combined bounds: "); node.bounds.print(); // TEMP
    printf("splitOffset %f\n", node.splitOffset); // TEMP

    // Sort 
    auto compareAxis = [&](float minCoord, float maxCoord) {
        if     (maxCoord < node.splitOffset) { return -1; } // left
        else if(minCoord > node.splitOffset) { return +1; } // right
        else                                 { return  0; } // overlap
    };

    auto compareX = [&](const Slab & bb) { return compareAxis(bb.xmin, bb.xmax); };
    auto compareY = [&](const Slab & bb) { return compareAxis(bb.ymin, bb.ymax); };
    auto compareZ = [&](const Slab & bb) { return compareAxis(bb.zmin, bb.zmax); };

    std::function<int(const Slab &)> compareFunc;

    switch(node.splitDirection) {
        case KDNode::SPLIT_X: compareFunc = compareX; break;
        case KDNode::SPLIT_Y: compareFunc = compareY; break;
        case KDNode::SPLIT_Z: compareFunc = compareZ; break;
        default: printf("Invalid split direction\n");
    }

    std::vector<TraceablePtr> leftObjects, rightObjects;
    for(auto & obj : node.objects) {
        auto bb = obj->boundingBoxTransformed();
        //bb.print(); // TEMP
        int compare = compareFunc(bb);
        if(compare <= 0) { leftObjects.push_back(obj); }
        if(compare >= 0) { rightObjects.push_back(obj); }
    }

    // If no progress was made, stop and make this a leaf node
    if(leftObjects.empty() || rightObjects.empty()) {
        printf("Abandoning split due to no progress - internal %u\n",
               (unsigned) node.objects.size()); // TEMP
        node.splitDirection = KDNode::LEAF;
        return;
    }

    float maxBloat = 1.5; // TODO - tune this

    // Bloat factor is the ratio of the total number of objects after the split
    // to the original number of objects (due to duplication)
    float bloatFactor = float(leftObjects.size() + rightObjects.size())
                        / float(node.objects.size());

    printf("before split %u left %u right %u bloat %f\n",
           (unsigned) node.objects.size(), (unsigned) leftObjects.size(),
           (unsigned) rightObjects.size(), bloatFactor); // TEMP

    // Abandon split if too much bloat (dupes in children)
    if(bloatFactor > maxBloat) {
        printf("Abandoning split due to bloat\n");
        node.splitDirection = KDNode::LEAF;
        return;
    }

    // Internal nodes do not hold objects
    node.objects = {};

    // Create left child node
    node.left = std::make_unique<KDNode>();
    node.left->objects = leftObjects;
    buildNode(*node.left, context, depth + 1);

    // Create right child node
    node.right = std::make_unique<KDNode>();
    node.right->objects = rightObjects;
    buildNode(*node.right, context, depth + 1);
}

void TraceableKDTree::print() const
{
    printNode(root); 
}

void TraceableKDTree::printNode(const KDNode & node, unsigned int depth) const
{
    auto printTabs = [&]() {
        for(unsigned int i = 0; i < depth; ++i) {
            printf("  ");
        }
    };
    printTabs();
    printf("KDNode split=%s @ %f objects=%u\n",
           node.splitDirection == KDNode::SPLIT_X
               ? "X" : node.splitDirection == KDNode::SPLIT_Y
                   ? "Y" : node.splitDirection == KDNode::SPLIT_Z
                       ? "Z" : "LEAF",
           node.splitOffset,
           (unsigned int) node.objects.size());
    printTabs();
    printf("  bounds="); node.bounds.print();
    if(node.left)
        printNode(*node.left, depth + 1);
    if(node.right)
        printNode(*node.right, depth + 1);
}

bool TraceableKDTree::intersects(const Ray & ray, float minDistance, float maxDistance) const
{
    return intersectsNode(root, ray, minDistance, maxDistance);
}

bool TraceableKDTree::findIntersection(const Ray & ray, float minDistance, RayIntersection & intersection) const
{
    return findIntersectionNode(root, ray, minDistance, intersection);
}

bool TraceableKDTree::intersectsNode(const KDNode & node, const Ray & ray, float minDistance, float maxDistance) const
{
    // Leaf node
    if(!node.objects.empty()) {
        for(auto object : node.objects) {
            if(object->intersectsWorldRay(ray, minDistance, maxDistance)) {
                return true;
            }
        }
    }

    // Internal node
    //   Descend depth-first through children until a hit is found

    // Extract relevant components from ray for comparison with split plane
    float originComponent;
    float directionComponent;
    getRayComponent(ray, node.splitDirection, originComponent, directionComponent);

    // Determine order based on ray direction component
    bool leftFirst = directionComponent >= 0.0f;

    // Descend down the tree
    if(leftFirst) {
        // Skip near side if origin on far side already
        if(originComponent + minDistance < node.splitOffset) {
            if(intersectsNode(*node.left, ray, minDistance, maxDistance)) {
                return true;
            }
        }
        if(intersectsNode(*node.right, ray, minDistance, maxDistance)) {
            return true;
        }
    }
    else { // right first
        // Skip near side if origin on far side already
        if(originComponent - minDistance > node.splitOffset) {
            if(intersectsNode(*node.right, ray, minDistance, maxDistance)) {
                return true;
            }
        }
        if(intersectsNode(*node.left, ray, minDistance, maxDistance)) {
            return true;
        }
    }

    return false;
}

bool TraceableKDTree::findIntersectionNode(const KDNode & node, const Ray & ray, float minDistance, RayIntersection & intersection) const
{
    // Leaf node
    //   Find best intersection among leaf node objects
    if(!node.objects.empty()) {
        RayIntersection tempIntersection;
        bool anyHit = false;

        for(auto object : node.objects) {
            bool hit = object->findIntersectionWorldRay(ray, minDistance, tempIntersection);
            if(hit && (!anyHit || tempIntersection.distance < intersection.distance)) {
                intersection = tempIntersection;;
                anyHit = true;
            }
        }

        return anyHit;
    }

    // Internal node
    //   Descend depth-first through children until a hit is found

    // Extract relevant components from ray for comparison with split plane
    float originComponent;
    float directionComponent;
    getRayComponent(ray, node.splitDirection, originComponent, directionComponent);

    // Determine order based on ray direction component
    bool leftFirst = directionComponent >= 0.0f;

    // Descend down the tree
    bool hit = false;
    if(leftFirst) {
        // Skip near side if origin on far side already
        if(originComponent + minDistance < node.splitOffset) {
            hit = findIntersectionNode(*node.left, ray, minDistance, intersection);
            if(hit) { return true; }
        }
        hit = findIntersectionNode(*node.right, ray, minDistance, intersection);
        if(hit) { return true; }
    }
    else { // right first
        // Skip near side if origin on far side already
        if(originComponent - minDistance > node.splitOffset) {
            hit = findIntersectionNode(*node.right, ray, minDistance, intersection);
            if(hit) { return true; }
        }
        hit = findIntersectionNode(*node.left, ray, minDistance, intersection);
        if(hit) { return true; }
    }

    return false;
}

void TraceableKDTree::getRayComponent(const Ray & ray, KDNode::SplitDirection splitDirection,
                                      float & originComponent, float & directionComponent) const
{
    // Extract relevant components from ray for comparison with split plane
    switch(splitDirection) {
        case KDNode::SPLIT_X:
            originComponent = ray.origin.x;
            directionComponent = ray.direction.x;
            break;
        case KDNode::SPLIT_Y:
            originComponent = ray.origin.y;
            directionComponent = ray.direction.y;
            break;
        case KDNode::SPLIT_Z: // fallthrough
        default:
            originComponent = ray.origin.z;
            directionComponent = ray.direction.z;
    }
}

