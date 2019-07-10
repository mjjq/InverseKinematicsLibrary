#include "AnimationSet.h"
#include <iostream>

bool AnimationSet::exists(std::string const & animationName)
{
    int index = 0;
    while(index < (int)animations.size())
    {
        if(animations[index].getName() == animationName)
            return true;

        ++index;
    }
    return false;
}

int AnimationSet::getIndex(std::string const & animationName)
{
    int index = 0;
    while(index < (int)animations.size())
    {
        if(animations[index].getName() == animationName)
            return index;


        ++index;
    }
    return -1;
}

void AnimationSet::addAnimation(SkeletonAnimation animation)
{
    animations.push_back(animation);
}

void AnimationSet::setAnimation(std::string const & animationName)
{
    currentAnimation = getIndex(animationName);
}

void AnimationSet::transitionTo(std::string const & animationName,
                      TransitionType const & transitionType,
                      float transitionLength)
{
    nextAnimation = getIndex(animationName);

    switchAnimationState = true;

    animationTransitionType = transitionType;
}

std::vector<BoneAnimation > & AnimationSet::getCurrentAnimationData(float deltaTime)
{
    localTime += deltaTime;

    if(switchAnimationState && animationTransitionType == TransitionType::Immediate)
    {
        currentAnimation = nextAnimation;
        nextAnimation = -1;
        switchAnimationState = false;
        localTime = 0.0f;
    }

    if(localTime >= animations[currentAnimation].getAnimationDuration())
    {
        localTime = 0.0f;

        if(switchAnimationState && animationTransitionType == TransitionType::EndOfAnimation)
        {
            currentAnimation = nextAnimation;
            nextAnimation = -1;
            switchAnimationState = false;
        }
    }
    else if(localTime < 0.0f)
    {
        localTime = animations[currentAnimation].getAnimationDuration();
    }

    return animations[currentAnimation].getAnimations();
}

float AnimationSet::getLocalTime()
{
    return localTime;
}

