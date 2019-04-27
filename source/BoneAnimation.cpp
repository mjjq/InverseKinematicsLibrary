#include "BoneAnimation.h"

#include <cmath>
#include <iostream>

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

float BoneAnimation::getRotation(float time)
{
    int lowerBoundIndex = animationData.rotationData.size()-1;
    int upperBoundIndex = 0;

    float maxTime = animationData.rotationData[lowerBoundIndex].first;
    time = fmod(time, maxTime);

    while(animationData.rotationData[lowerBoundIndex].first >= time)
        --lowerBoundIndex;

    while(animationData.rotationData[upperBoundIndex].first < time)
        ++upperBoundIndex;

    float rotation = interpolate(animationData.rotationData[lowerBoundIndex],
                                 animationData.rotationData[upperBoundIndex],
                                 time);

    std::cout << time << " time\n";
    std::cout << maxTime << " maxtime\n";
    std::cout << rotation << " angle\n";
    return rotation;
}

sf::Vector2f BoneAnimation::getTranslation(float time)
{
    int lowerBoundIndex = animationData.rotationData.size()-1;
    int upperBoundIndex = 0;

    float maxTime = animationData.translationData[lowerBoundIndex].first;
    time = fmod(time, maxTime);

    while(animationData.translationData[lowerBoundIndex].first >= time)
        --lowerBoundIndex;

    while(animationData.translationData[upperBoundIndex].first < time)
        ++upperBoundIndex;

    sf::Vector2f translation = interpolate(animationData.translationData[lowerBoundIndex],
                                           animationData.translationData[upperBoundIndex],
                                           time);
    return translation;
}

std::string BoneAnimation::getBoneName()
{
    return animationData.boneName;
}
