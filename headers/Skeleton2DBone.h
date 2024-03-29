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
    sf::Vector2f scale = {1.0f, 1.0f};
    float length;
    float rotation;

    sf::Vector2f position;

    sf::Vector2f orientation = {1.0f, 0.0f};
    sf::Vector2f parentOrientation = {1.0f, 0.0f};
    sf::Vector2f parentPosition = {0.0f, 0.0f};
    sf::Vector2f translation = {0.0f, 0.0f};
};



class Skeleton2DBone
{
    std::vector<SkeletonNode > nodes;

    const BoneData initialBoneData;
    BoneData boneData;


public:
    Skeleton2DBone();
    Skeleton2DBone(BoneData const & _boneData);

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

    void setData(BoneData _data);

    void setAngle(float angleDegree);

    void setScale(sf::Vector2f const & scale);
};

#endif // SKEL2DBONE_H

