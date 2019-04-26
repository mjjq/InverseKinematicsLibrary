#ifndef BONE_ANIM_H
#define BONE_ANIM_H

#include <vector>
#include <SFML/Graphics.hpp>

struct BoneAnimationData
{
    typedef std::pair<float, float> TimeRotationPair;
    typedef std::pair<float, sf::Vector2f> TimeTranslationPair;

    std::string boneName;

    std::vector<TimeRotationPair > rotationData;
    std::vector<TimeTranslationPair > translationData;
};

class BoneAnimation
{
    typedef std::pair<float, float> TimeRotationPair;
    typedef std::pair<float, sf::Vector2f> TimeTranslationPair;

    const BoneAnimationData animationData;

    template <typename T>
    T interpolate(std::pair<float, T> const & lowerBound,
                  std::pair<float, T> const & upperBound,
                  float time);
public:
    BoneAnimation(BoneAnimationData const & _animationData);

    float getRotation(float time);
    sf::Vector2f getTranslation(float time);
    std::string getBoneName();
};

#endif // BONE_ANIM_H
