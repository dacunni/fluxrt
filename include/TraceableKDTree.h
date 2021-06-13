#ifndef __TRACEABLE_KDTREE_H__
#define __TRACEABLE_KDTREE_H__

#include <memory>
#include "traceable.h"
#include "slab.h"

class TraceableKDTree : public Traceable
{
    public:
        TraceableKDTree() = default;
        ~TraceableKDTree() = default;

        void build(std::vector<TraceablePtr> & objects);

        void print() const;

        // Ray intersection implementation
        virtual bool intersects(const Ray & ray, float minDistance, float maxDistance) const override;
        virtual bool findIntersection(const Ray & ray, float minDistance, RayIntersection & intersection) const override;

        // Bounding volume
        Slab boundingBox() override { return bounds; }

    protected:
        // KD Tree node
        //   Only leaf nodes store objects. Objects straddling a split
        //   plane are stored in both children. Left-right orientation
        //   is such that right is more positive.
        struct KDNode {
            std::unique_ptr<KDNode> left, right; 
            std::vector<TraceablePtr> objects;
            Slab bounds;

            enum SplitDirection { SPLIT_X, SPLIT_Y, SPLIT_Z, LEAF };
            SplitDirection splitDirection;
            float splitOffset = 0.0f;
        };
        KDNode root;

        enum SplitStrategy {
            STRATEGY_MIDPOINT_X,
            STRATEGY_MIDPOINT_Y,
            STRATEGY_MIDPOINT_Z
        };

        struct BuildContext;

        void buildNode(KDNode & node, BuildContext & context, unsigned int depth = 0);
        void splitNode(KDNode & node, SplitStrategy strategy, BuildContext & context, unsigned int depth);

        void printNode(const KDNode & node, unsigned int depth = 0) const;

        bool intersectsNode(const KDNode & node, const Ray & ray, float minDistance, float maxDistance) const;
        bool findIntersectionNode(const KDNode & node, const Ray & ray, float minDistance, RayIntersection & intersection) const;

        void getRayComponent(const Ray & ray, KDNode::SplitDirection splitDirection,
                             float & originComponent, float & directionComponent) const;

        Slab bounds;
};


#endif
