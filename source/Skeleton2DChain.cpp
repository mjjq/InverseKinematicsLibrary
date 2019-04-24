#include "Skeleton2DChain.h"

void Skeleton2DChain::inverseK(sf::Vector2f const & t,
                          int targetIndex)
{
    float dist = sqrtf(Math::square(t - bones[0].getNode(0).position));
    float totalJointDist = 0.0f;
    for(int i=0; i<targetIndex; ++i)
    {
        totalJointDist += bones[i].getNode(0).distanceToNext;
    }

    if(dist > totalJointDist)
}


void Skeleton2DBone::inverseK(sf::Vector2f const & t,
              int baseIndex,
              int targetIndex)
{
    if(targetIndex < 0) targetIndex = nodes.size()-1;

    float dist = sqrtf(Math::square(t - nodes[baseIndex].position));
    float totalJointDist = 0.0f;
    for(int i=baseIndex; i<targetIndex; ++i)
        totalJointDist += nodes[i].distanceToNext;

    if(dist > totalJointDist)
    {
        sf::Vector2f tNew = t;
        if(nodes.size() > 1)
        {
            SkeletonNode tempNode(t, 0, 0.0f, 0.0f, "");
            constrainToAngularRange(nodes[baseIndex], nodes[baseIndex+1]);
            //tNew = nodes[baseIndex+1].position;
        }
        for(int i=baseIndex; i<targetIndex; ++i)
        {
            sf::Vector2f pi = nodes[i].position;
            float ri = sqrtf(Math::square(tNew - pi));
            float lambdai = nodes[i].distanceToNext/ri;
            nodes[i+1].position = (1-lambdai)*pi + lambdai*tNew;

            constrainToAngularRange(nodes[i], nodes[i+1]);

            sf::Vector2f& pip1 = nodes[i+1].position;
            pi   = nodes[i].position;

            ri = sqrtf(Math::square(pip1 - pi));
            lambdai = nodes[i].distanceToNext/ri;

            pip1 = (1-lambdai)*pi + lambdai*pip1;
        }
    }
    else
    {
        sf::Vector2f b = nodes[baseIndex].position;
        sf::Vector2f& pn = nodes[targetIndex].position;

        float difA = sqrtf(Math::square(pn - t));

        float tolerance = 1e-4;
        int noIterations = 0;
        int maxIterations = 100;

        while(difA > tolerance && noIterations < maxIterations)
        {
            pn = t;
            for(int i=targetIndex-1; i>=baseIndex; --i)
            {

                if(i>baseIndex)
                {
                    constrainToAngularRange(nodes[i+1], nodes[i], nodes[i-1]);
                }
                else
                {
                    constrainToAngularRange(nodes[baseIndex], nodes[baseIndex+1]);
                }

                sf::Vector2f& pip1 = nodes[i+1].position;
                sf::Vector2f& pi   = nodes[i].position;

                float ri = sqrtf(Math::square(pip1 - pi));
                float lambdai = nodes[i].distanceToNext/ri;

                pi = (1-lambdai)*pip1 + lambdai*pi;
            }

            nodes[baseIndex].position = b;

            for(int i=baseIndex; i<targetIndex; ++i)
            {

                if(i>baseIndex)
                {
                    constrainToAngularRange(nodes[i-1], nodes[i], nodes[i+1]);
                }
                else
                {
                    constrainToAngularRange(nodes[baseIndex], nodes[baseIndex+1]);
                }
                sf::Vector2f& pip1 = nodes[i+1].position;
                sf::Vector2f& pi   = nodes[i].position;

                float ri = sqrtf(Math::square(pip1 - pi));
                float lambdai = nodes[i].distanceToNext/ri;

                pip1 = (1-lambdai)*pi + lambdai*pip1;
            }

            difA = sqrtf(Math::square(pn - t));
            ++noIterations;
        }
    }

    baseNodeAngle = SkeletonNode::getAngle(nodes[0], nodes[1]);
    nodes[0].angle = baseNodeAngle;

    for(int i=baseIndex; i<targetIndex; ++i)
    {
        if(i < targetIndex - 1)
        {
            nodes[i+1].angle = SkeletonNode::getAngle(nodes[i], nodes[i+1], nodes[i+2]);
        }
        if(i < nodes.size()-1)
        {
            sf::Vector2f newDir = Math::norm(nodes[i+1].position -
                                nodes[i].position);
            nodes[i+1].orientation = newDir;
        }
    }

}*/
