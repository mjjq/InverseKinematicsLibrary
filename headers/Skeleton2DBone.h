#ifndef SKEL2DBONE_H
#define SKEL2DBONE_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include <assert.h>

#include "SkeletonNode.h"

struct BoneData
{
    std::string name;
    std::string parent;
    sf::Vector2f offset;
    float length;
    float rotation;
};



class Skeleton2DBone
{
    std::vector<SkeletonNode > nodes;

    sf::Vector2f absOffset = {0.0f, 0.0f};
    sf::Vector2f relOffset = {0.0f, 0.0f};
    bool linkedToParent = false;
    float baseNodeAngle = 0.0f;
    sf::Vector2f parentOrientation = {1.0f, 0.0f};

    enum Direction
    {
        Forward,
        Backward
    };

    bool constrainToAngularRange(SkeletonNode& node1,
                                 SkeletonNode& node2,
                                 SkeletonNode& node3);

    void constrainToAngularRange(SkeletonNode& node1,
                                 SkeletonNode& node2);

    void inverseK(sf::Vector2f const & t,
                  int baseIndex,
                  int targetIndex);

    void forwardK(sf::Vector2f const & t,
                  int targetIndex);

    void resetBeams();

public:
    Skeleton2DBone();
    Skeleton2DBone(std::vector<SkeletonNode > _nodes,
                    sf::Vector2f const & _offset = {0.0f, 0.0f});
    void draw(sf::RenderWindow& window);

    void setTarget(sf::Vector2f const & t, int targetIndex = -1,
                   bool applyOffset = false, bool setOffset = false);

    void setParentNode(SkeletonNode& parentNode);

    SkeletonNode& getBaseNode();

    SkeletonNode& getNode(int index);

    void updateParentOrientation(sf::Vector2f const & orientation);

    void setBaseNodeAngle(float angle);

    float getBaseNodeAngle();

    int getNumNodes();

    sf::Vector2f getOffset();

    static void inverseK(sf::Vector2f const & t,
                              std::vector<Skeleton2DBone* > bones);
};

#endif // SKEL2DBONE_H

