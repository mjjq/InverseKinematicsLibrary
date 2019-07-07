#include "SkeletonAnimation.h"
#include <iostream>

SkeletonAnimation::SkeletonAnimation() : name{""} {}

SkeletonAnimation::SkeletonAnimation(std::string const & _name) : name{_name}
{}

void SkeletonAnimation::addBoneAnimation(BoneAnimation const & animation)
{
    boneAnimations.push_back(animation);

    float duration =  (boneAnimations.end()-1)->getDuration();
    if(duration > animationDuration) animationDuration = duration;
}

std::string SkeletonAnimation::getName()
{
    return name;
}

std::vector<BoneAnimation > & SkeletonAnimation::getAnimations()
{
    return boneAnimations;
}

float SkeletonAnimation::getAnimationDuration()
{
    return animationDuration;
}
