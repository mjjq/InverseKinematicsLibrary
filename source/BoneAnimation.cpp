#include "BoneAnimation.h"

#include <cmath>
#include <iostream>
#include <assert.h>

BoneAnimation::BoneAnimation(BoneAnimationData const & _animationData) :
    animationData{_animationData}
    {}

template <typename T>
T BoneAnimation::interpolate(std::pair<float, T> const & lowerBound,
                             std::pair<float, T> const & upperBound,
                             float time)
{
    float fraction = (time - lowerBound.first)/
                     (upperBound.first - lowerBound.first);

    if(fraction < 0.0f) fraction = 0.0f;
    if(fraction > 1.0f) fraction = 1.0f;

    return lowerBound.second +
           fraction * (upperBound.second - lowerBound.second);
}

float BoneAnimation::getDuration()
{
    return animationData.duration;
}

float BoneAnimation::getRotation(float time)
{
    if(animationData.rotationData.size() == 0)
        return 0.0f;

    int lowerBoundIndex = animationData.rotationData.size()-1;
    int upperBoundIndex = 0;

    float maxTime = animationData.rotationData[lowerBoundIndex].first;
    time = fmod(time, maxTime);

    while(animationData.rotationData[lowerBoundIndex].first > time)
        --lowerBoundIndex;

    while(animationData.rotationData[upperBoundIndex].first < time)
        ++upperBoundIndex;


    float rotation = 0.0f;
    if(lowerBoundIndex == upperBoundIndex)
        rotation = animationData.rotationData[lowerBoundIndex].second;
    else
        rotation = interpolate(animationData.rotationData[lowerBoundIndex],
                                 animationData.rotationData[upperBoundIndex],
                                 time);
    return rotation;
}

sf::Vector2f BoneAnimation::getTranslation(float time)
{
    if(animationData.translationData.size() == 0)
        return {0.0f, 0.0f};

    int lowerBoundIndex = animationData.translationData.size()-1;
    int upperBoundIndex = 0;

    float maxTime = animationData.translationData[lowerBoundIndex].first;
    time = fmod(time, maxTime);

    while(animationData.translationData[lowerBoundIndex].first > time)
        --lowerBoundIndex;

    while(animationData.translationData[upperBoundIndex].first < time)
        ++upperBoundIndex;

    sf::Vector2f translation = {0.0f, 0.0f};

    if(lowerBoundIndex == upperBoundIndex)
        translation = animationData.translationData[lowerBoundIndex].second;
    else
        translation = interpolate(animationData.translationData[lowerBoundIndex],
                                           animationData.translationData[upperBoundIndex],
                                           time);
    return translation;
}

std::string BoneAnimation::getBoneName()
{
    return animationData.boneName;
}
