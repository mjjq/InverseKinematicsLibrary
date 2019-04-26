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

    sf::Vector2f orientation = {1.0f, 0.0f};
    sf::Vector2f parentOrientation = {1.0f, 0.0f};

    const BoneData initialBoneData;
    BoneData boneData;


public:
    Skeleton2DBone();
    Skeleton2DBone(BoneData const & _boneData,
                   sf::Vector2f const & _parentPosition = {0.0f, 0.0f},
                   sf::Vector2f const & _parentOrientation = {1.0f, 0.0f});

    void draw(sf::RenderWindow& window);

    enum RelativeTo
    {
        InitialPose,
        Parent,
        Current,
        World
    };

    void setTarget(sf::Vector2f const & t, int targetIndex = -1,
                   bool applyOffset = false,
                   RelativeTo const & relativeTo = RelativeTo::World);

    void setRotation(float angleDegree,
                     RelativeTo const & relativeTo);

    SkeletonNode& getNode(int index);

    sf::Vector2f getOrientation();
    void setOrientation(sf::Vector2f const & o);

    sf::Vector2f getParentOrientation();
    void setParentOrientation(sf::Vector2f const & po);

    BoneData getInitialData();
    BoneData getData();

    void setAngle(float angleDegree);
};

#endif // SKEL2DBONE_H

