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

    sf::Vector2f position;
    sf::Vector2f parentPosition;
};



class Skeleton2DBone
{
    std::vector<SkeletonNode > nodes;

    const BoneData initialBoneData;
    BoneData boneData;

    sf::Vector2f orientation = {1.0f, 0.0f};
    sf::Vector2f parentOrientation = {1.0f, 0.0f};
    sf::Vector2f parentPosition = {0.0f, 0.0f};
    sf::Vector2f translation = {0.0f, 0.0f};

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
        World,
        Orthogonal
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

    sf::Vector2f getParentPosition();
    void setParentPosition(sf::Vector2f const & pp);

    void setTranslation(sf::Vector2f const & tr);

    BoneData getInitialData();
    BoneData getData();

    void setAngle(float angleDegree);

    void setScale(sf::Vector2f const & scale,
                  sf::Vector2f const & rootNodePos);
};

#endif // SKEL2DBONE_H

