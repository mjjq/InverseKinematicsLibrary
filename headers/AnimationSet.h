#ifndef ANIMATION_SET_H
#define ANIMATION_SET_H

#include "SkeletonAnimation.h"

class AnimationSet
{
    std::vector<SkeletonAnimation > animations;
    int currentAnimation = 0;
    int nextAnimation = 0;

    bool switchAnimationState = false;

    int getIndex(std::string const & animationName);

    float localTime = 0.0f;

public:
    bool exists(std::string const & animationName);

    void addAnimation(SkeletonAnimation animation);

    void setAnimation(std::string const & animationName);

    void transitionTo(std::string const & animationName,
                      float transitionLength);
    std::vector<BoneAnimation > & getCurrentAnimationData(float deltaTime);

    float getLocalTime();
};

#endif // ANIMATION_SET_H
