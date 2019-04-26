#ifndef ANGLE_CALC_H
#define ANGLE_CALC_H

#include "Skeleton2DBone.h"

class AngleCalculations
{
public:
    enum Quadrant
    {
        UL,
        UR,
        LL,
        LR
    };

    static int convertQuadrantToSgn(Quadrant q)
    {
        if(q==Quadrant::UL || q==Quadrant::LL)
            return -1;

        return 1;
    }

    static Quadrant getVectorQuadrant(sf::Vector2f const & xAxis,
                               sf::Vector2f const & yAxis,
                               sf::Vector2f const & direction)
    {
        float xProjection = Math::dot(xAxis, direction);
        float yProjection = Math::dot(yAxis, direction);

        bool isInUpperHalf = yProjection > 0.0f;
        bool isInRightHalf = xProjection > 0.0f;

        if(isInRightHalf && isInUpperHalf)
            return Quadrant::UR;
        if(isInRightHalf && !isInUpperHalf)
            return Quadrant::LR;
        if(!isInRightHalf && isInUpperHalf)
            return Quadrant::UL;

        return Quadrant::LL;
    }

    static float getAngle(SkeletonNode& node1,
                          SkeletonNode& node2,
                          SkeletonNode& node3)
    {
        sf::Vector2f d1 = node2.position -
                          node1.position;
        sf::Vector2f d2 = node3.position -
                          node2.position;
        sf::Vector2f d1hat  = Math::norm(d1);
        sf::Vector2f od1hat = Math::orthogonal(d1hat, 1.0f);
        sf::Vector2f d2hat  = Math::norm(d2);

        Quadrant nodeQuadrant = getVectorQuadrant(od1hat, d1hat, d2hat);

        float angleCosine = Math::dot(d1hat, d2hat);

        if(angleCosine > 1.0f) angleCosine = 1.0f;
        if(angleCosine < -1.0f) angleCosine = -1.0f;

        return 180.0f*convertQuadrantToSgn(nodeQuadrant)*acosf(angleCosine)/Math::PI;
    }

    static float calcAngleToParent(Skeleton2DBone& bone)
    {
        sf::Vector2f d1hat = bone.getParentOrientation();
        sf::Vector2f d2hat = bone.getOrientation();
        sf::Vector2f od1hat = Math::orthogonal(d1hat, 1.0f);

        Quadrant boneQuadrant = getVectorQuadrant(od1hat, d1hat, d2hat);

        float angleCosine = Math::dot(d1hat, d2hat);

        if(angleCosine > 1.0f) angleCosine = 1.0f;
        if(angleCosine < -1.0f) angleCosine = -1.0f;

        return 180.0f*convertQuadrantToSgn(boneQuadrant)*acosf(angleCosine)/Math::PI;
    }
    //static float getAngle(SkeletonNode& node1,
    //                      SkeletonNode& node2)
    //{
        /*sf::Vector2f tempPos = node1.position; - node1.orientation;
        SkeletonNode virtualNode(tempPos, 0, 0.0f, 0.0f, "");
        return getAngle(virtualNode, node1, node2);*/
    //}


    static float getAngle(SkeletonNode& nodeA1,
                          SkeletonNode& nodeA2,
                          SkeletonNode& nodeB1,
                          SkeletonNode& nodeB2)
    {
        sf::Vector2f d1 = nodeA2.position - nodeA1.position;
        sf::Vector2f d2 = nodeB2.position - nodeB1.position;

        sf::Vector2f d1hat  = Math::norm(d1);
        sf::Vector2f od1hat = Math::orthogonal(d1hat, 1.0f);
        sf::Vector2f d2hat  = Math::norm(d2);

        Quadrant nodeQuadrant = getVectorQuadrant(od1hat, d1hat, d2hat);

        float angleCosine = Math::dot(d1hat, d2hat);

        if(angleCosine > 1.0f) angleCosine = 1.0f;
        if(angleCosine < -1.0f) angleCosine = -1.0f;

        return 180.0f*convertQuadrantToSgn(nodeQuadrant)*acosf(angleCosine)/Math::PI;
    }
};

#endif // ANGLE_CALC_H
