#ifndef BONE_ANIM_H
#define BONE_ANIM_H

#include <vector>
#include <SFML/Graphics.hpp>

struct BoneAnimationData
{
    typedef std::pair<float, float> TimeRotationPair;
    typedef std::pair<float, sf::Vector2f> TimeTranslationPair;

    std::string boneName;

    std::vector<TimeRotationPair > angleFuncOfTime;
    std::vector<TimeTranslationPair > translationFuncOfTime;
};

class BoneAnimation
{
    const BoneAnimationData animationData;


public:
    BoneAnimation(BoneAnimationData const & _animationData);

    float getRotation(float time);
    sf::Vector2f getTranslation(float time);
    std::string getBoneName();
};

#endif // BONE_ANIM_H
