#include "BoneAnimation.h"

BoneAnimation::BoneAnimation(BoneAnimationData const & _animationData) :
    animationData{_animationData}
    {}

float BoneAnimation::getRotation(float time)
{

}

sf::Vector2f BoneAnimation::getTranslation(float time)
{

}

std::string BoneAnimation::getBoneName()
{
    return animationData.boneName;
}
