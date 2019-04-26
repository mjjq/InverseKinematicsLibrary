#ifndef SKELNODE_H
#define SKELNODE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include "Math.h"

struct SkeletonNode
{
    sf::Vector2f position;

    SkeletonNode() {};
    SkeletonNode(sf::Vector2f _position) :
                     position{_position}
    {
    };
};

#endif // SKELNODE_H
