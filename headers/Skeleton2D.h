#ifndef SKELETON_H
#define SKELETON_H

#include "SkeletonNode.h"
#include "Skeleton2DBone.h"
#include "SkeletonAnimation.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <assert.h>
#include "Math.h"

struct IKConstraintData
{
    std::string name;
    std::string firstBone;
    std::string lastBone = "";
    std::string target;
};



class Skeleton2D
{
    std::map<std::string, Skeleton2DBone > chains;
    std::vector<std::pair<std::string, std::string> > parentTo;
    std::map<std::string, std::vector<std::string> > ikGroups;
    std::map<std::string, SkeletonAnimation > animations;

    const int PAIR_NOT_FOUND = -1;

    int getParentChildPair(std::string const & parent,
                           std::string const & child);

    const std::string NULL_NAME = "";

    std::string getParent(std::string const & childName);

    void linkParentToChild(std::string const & parent,
                           std::string const & child,
                           int linkIndex = -1);

    void updateBaseNodeOrientation(std::string const & parentName,
                                   std::string const & childName);

    std::vector<std::string > getHierarchy(std::string const & firstNode,
                                           std::string const & lastNode);

public:
    Skeleton2D();

    void addBone(BoneData const & boneData);

    void addChain(std::string const & name,
                  Skeleton2DBone chain,
                  std::string const & parentName,
                  int linkIndex = -1);

    void addIKConstraint(IKConstraintData const & ikData);

    void addAnimation(SkeletonAnimation animation);

    void setTarget(sf::Vector2f const & target,
                   std::string const & chainName,
                   int chainNode,
                   bool applyOffset = false,
                   bool inheritOrientation = true,
                   Skeleton2DBone::RelativeTo relativeTo = Skeleton2DBone::RelativeTo::World);

    void setRotation(float angleDegree,
                     std::string const & boneName,
                     Skeleton2DBone::RelativeTo const & relativeTo);

    void animate(std::string const & animationName,
                 float time);
    void draw(sf::RenderWindow& window);
};


#endif // SKELETON_H
