#ifndef SKEL_ANIM_H
#define SKEL_ANIM_H

#include "BoneAnimation.h"

class SkeletonAnimation
{
    const std::string name;

    std::vector<BoneAnimation > boneAnimations;

    float animationDuration = 0.0f;

public:
    SkeletonAnimation();

    SkeletonAnimation(std::string const & _name);

    void addBoneAnimation(BoneAnimation const & animation);

    std::string getName();

    std::vector<BoneAnimation > & getAnimations();

    float getAnimationDuration();
};

#endif // SKEL_ANIM_H
