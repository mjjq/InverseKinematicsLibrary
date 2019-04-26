#ifndef KIN_ALGORITHMS_H
#define KIN_ALGORITHMS_H

#include "SkeletonNode.h"
#include "Skeleton2DBone.h"

class KinematicAlgorithms
{

    enum Direction
    {
        Forward,
        Backward
    };

    static bool constrainToAngularRange(SkeletonNode& node1,
                                 SkeletonNode& node2,
                                 SkeletonNode& node3);

    static void constrainToAngularRange(SkeletonNode& node1,
                                 SkeletonNode& node2);
public:
    static void inverseK(std::vector<Skeleton2DBone* > bones,
                         sf::Vector2f const & t);

    static void forwardK(std::vector<Skeleton2DBone* > bones,
                         sf::Vector2f const & t);
};

#endif // KIN_ALGORITHMS_H
