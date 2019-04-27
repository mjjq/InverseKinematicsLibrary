#include "SkeletonAnimation.h"

SkeletonAnimation::SkeletonAnimation() : name{""} {}

SkeletonAnimation::SkeletonAnimation(std::string const & _name) : name{_name}
{}

void SkeletonAnimation::addBoneAnimation(BoneAnimation const & animation)
{
    boneAnimations.push_back(animation);
}

std::string SkeletonAnimation::getName()
{
    return name;
}

std::vector<BoneAnimation > & SkeletonAnimation::getAnimations()
{
    return boneAnimations;
}
