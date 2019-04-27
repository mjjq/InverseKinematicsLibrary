#ifndef SKEL_ANIM_H
#define SKEL_ANIM_H

#include "BoneAnimation.h"

class SkeletonAnimation
{
    const std::string name;

    std::vector<BoneAnimation > boneAnimations;

public:
    SkeletonAnimation();

    SkeletonAnimation(std::string const & _name);

    void addBoneAnimation(BoneAnimation const & animation);

    std::string getName();

    std::vector<BoneAnimation > & getAnimations();
};

#endif // SKEL_ANIM_H
