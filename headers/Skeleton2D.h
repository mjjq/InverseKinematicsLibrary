#ifndef SKELETON_H
#define SKELETON_H

#include "SkeletonNode.h"
#include "Skeleton2DBone.h"
#include "SkeletonAnimation.h"
#include "AnimationSet.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <assert.h>
#include "Math.h"

struct GeneralData
{
    std::string imageSubfolder;
    std::string audioSubfolder;
};

struct IKConstraintData
{
    std::string name;
    std::string firstBone;
    std::string lastBone = "";
    std::string target;
};

struct SlotData
{
    std::string name;
    std::string bone;
    std::string attachment;
};

struct SkinData
{
    std::string name;
    std::string attachment;
    sf::Vector2f offset;
    float rotation;
    sf::Vector2f size;
};

class Skeleton2D
{
    std::string NULL_NAME = "";
    int PAIR_NOT_FOUND = -1;

    std::map<std::string, Skeleton2DBone > chains;
    std::vector<std::pair<std::string, std::string> > parentTo;
    std::map<std::string, std::vector<std::string> > ikGroups;
    AnimationSet animations;
    std::vector<SlotData > slots;
    std::vector<SkinData > skins;
    GeneralData generalData;

    sf::Vector2f scale = {1.0f, 1.0f};

    int getParentChildPair(std::string const & parent,
                           std::string const & child);


    std::string getParent(std::string const & childName);

    void linkParentToChild(std::string const & parent,
                           std::string const & child,
                           int linkIndex = -1);

    void updateBaseNodeOrientation(std::string const & parentName,
                                   std::string const & childName);

    std::vector<std::string > getHierarchy(std::string const & firstNode,
                                           std::string const & lastNode);

    void resetSkeleton();

    void setTarget(sf::Vector2f const & target,
                   std::string const & chainName,
                   int chainNode,
                   bool applyOffset = false,
                   bool inheritOrientation = true,
                   Skeleton2DBone::RelativeTo relativeTo = Skeleton2DBone::RelativeTo::World);

public:

    void addBone(BoneData boneData);

    void addChain(std::string const & name,
                  Skeleton2DBone chain,
                  std::string const & parentName,
                  int linkIndex = -1);

    void addIKConstraint(IKConstraintData const & ikData);

    void addAnimation(SkeletonAnimation animation);

    void addSlot(SlotData slotData) {slots.push_back(slotData);}

    void addSkin(SkinData skin) {skins.push_back(skin);}


    void setRotation(float angleDegree,
                     std::string const & boneName,
                     Skeleton2DBone::RelativeTo const & relativeTo);

    void setAnimation(std::string const & animationName,
                      AnimationSet::TransitionType const & transitionType);

    void animate(float time);
    void draw(sf::RenderWindow& window);
    std::map<std::string, BoneData > getBoneData();
    BoneData getBoneData(std::string const & boneName);
    std::vector<SlotData > getSlotData() { return slots; }
    std::vector<SkinData > getSkinData() { return skins; }

    std::vector<sf::Vector2f > getJointPositions();

    void setScale(sf::Vector2f const & _scale);
    sf::Vector2f getScale() { return scale; }

    void externalSetTarget(sf::Vector2f const & target,
                   std::string const & chainName,
                   int chainNode,
                   bool applyOffset = false,
                   bool inheritOrientation = true,
                   Skeleton2DBone::RelativeTo relativeTo = Skeleton2DBone::RelativeTo::World);

    void setGeneralData(GeneralData _generalData) { generalData = _generalData; };
    GeneralData getGeneralData() { return generalData; };
};


#endif // SKELETON_H
