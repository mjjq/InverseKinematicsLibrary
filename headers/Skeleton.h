#ifndef SKELETON_H
#define SKELETON_H


#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <assert.h>
#include "Math.h"


struct BoneData
{
    std::string name;
    std::string parent;
    sf::Vector2f offset;
    float length;
    float rotation;
};

struct SkeletonNode
{
    sf::Vector2f position;
    float minAngle;
    float maxAngle;
    float minCosine;
    float maxCosine;
    sf::Vector2f orientation;
    float angle = 0.0f;
    float distanceToNext = 0.0f;
    int z;
    std::string name;

    SkeletonNode() {};
    SkeletonNode(sf::Vector2f _position,
                 int _z, float _minAngle, float _maxAngle,
                 std::string _name) :
                     position{_position},
                     z{_z}, minAngle{_minAngle}, maxAngle{_maxAngle},
                     name{_name}
    {
        minCosine = cosf(minAngle);
        maxCosine = cosf(maxAngle);
    };

    enum Quadrant
    {
        UL,
        UR,
        LL,
        LR
    };

    static int convertQuadrantToSgn(Quadrant q)
    {
        if(q==Quadrant::UL || q==Quadrant::LL)
            return -1;

        return 1;
    }

    static Quadrant getVectorQuadrant(sf::Vector2f const & xAxis,
                               sf::Vector2f const & yAxis,
                               sf::Vector2f const & direction)
    {
        float xProjection = Math::dot(xAxis, direction);
        float yProjection = Math::dot(yAxis, direction);

        bool isInUpperHalf = yProjection > 0.0f;
        bool isInRightHalf = xProjection > 0.0f;

        if(isInRightHalf && isInUpperHalf)
            return Quadrant::UR;
        if(isInRightHalf && !isInUpperHalf)
            return Quadrant::LR;
        if(!isInRightHalf && isInUpperHalf)
            return Quadrant::UL;

        return Quadrant::LL;
    }

    static float getAngle(SkeletonNode& node1,
                          SkeletonNode& node2,
                          SkeletonNode& node3)
    {
        sf::Vector2f d1 = node2.position -
                          node1.position;
        sf::Vector2f d2 = node3.position -
                          node2.position;
        sf::Vector2f d1hat  = Math::norm(d1);
        sf::Vector2f od1hat = Math::orthogonal(d1hat, 1.0f);
        sf::Vector2f d2hat  = Math::norm(d2);

        Quadrant nodeQuadrant = getVectorQuadrant(od1hat, d1hat, d2hat);

        float angleCosine = Math::dot(d1hat, d2hat);

        if(angleCosine > 1.0f) angleCosine = 1.0f;
        if(angleCosine < -1.0f) angleCosine = -1.0f;

        return convertQuadrantToSgn(nodeQuadrant)*acosf(angleCosine);
    }

    static float getAngle(SkeletonNode& node1,
                          SkeletonNode& node2)
    {
        sf::Vector2f tempPos = node1.position - node1.orientation;
        SkeletonNode virtualNode(tempPos, 0, 0.0f, 0.0f, "");
        return getAngle(virtualNode, node1, node2);
    }
};

class Skeleton2DChain
{
    std::vector<SkeletonNode > nodes;

    sf::Vector2f absOffset = {0.0f, 0.0f};
    sf::Vector2f relOffset = {0.0f, 0.0f};
    bool linkedToParent = false;
    float baseNodeAngle = 0.0f;

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
    Skeleton2DChain();
    Skeleton2DChain(std::vector<SkeletonNode > _nodes,
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
};


class Skeleton2D
{
    sf::Vector2f basePosition = {0.0f, 0.0f};
    std::map<std::string, Skeleton2DChain > chains;
    std::vector<std::pair<std::string, std::string> > parentTo;

    const int PAIR_NOT_FOUND = -1;

    int getParentChildPair(std::string const & parent,
                           std::string const & child);

    const std::string NULL_NAME = "";

    std::string getParent(std::string const & childName);

    void linkParentToChild(std::string const & parent,
                           std::string const & child,
                           int linkIndex = -1);

    void updateBaseNodeOrientation(std::string const & childName);

public:
    Skeleton2D(sf::Vector2f const & basePos = {0.0f, 0.0f});

    void addBone(BoneData const & boneData);

    void addChain(std::string const & name,
                  Skeleton2DChain chain,
                  std::string const & parentName,
                  int linkIndex = -1);

    void setTarget(sf::Vector2f const & target,
                   std::string const & chainName,
                   int chainNode,
                   bool applyOffset = false);

    void draw(sf::RenderWindow& window);
};


#endif // SKELETON_H
