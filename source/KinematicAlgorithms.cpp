#include "KinematicAlgorithms.h"

#include "AngleCalculations.h"

bool KinematicAlgorithms::constrainToAngularRange(SkeletonNode& node1,
                             SkeletonNode& node2,
                             SkeletonNode& node3)
{
    /*float minAngle = node2.minAngle;
    float maxAngle = node2.maxAngle;

    if(node2.lockAngle)
    {
        minAngle = node2.angle;
        maxAngle = node2.angle;
    }

    float nodeAngle = SkeletonNode::getAngle(node1, node2, node3);

    sf::Vector2f d1hat = Math::norm(node2.position -
                                    node1.position);

    if(nodeAngle < minAngle)
    {
        node3.position = Math::rotate(d1hat, -180.0f*minAngle/Math::PI) + node2.position;
        std::cout << minAngle << " min\n";
        return true;
    }
    else if(nodeAngle > maxAngle)
    {
        node3.position = Math::rotate(d1hat, -180.0f*maxAngle/Math::PI) + node2.position;
        std::cout << maxAngle << " max\n";
        return true;
    }

    return false;*/
}

//void KinematicAlgorithms::constrainToAngularRange(SkeletonNode& node1,
//                             SkeletonNode& node2)
//{
    /*SkeletonNode virtualNode(node1.position - node1.orientation,
                             0, 0.0f, 0.0f, "");
    constrainToAngularRange(virtualNode, node1, node2);*/
//}



void KinematicAlgorithms::inverseK(std::vector<Skeleton2DBone* > bones,
                                   sf::Vector2f const & t)
{
    int lastIndex = bones.size();
    sf::Vector2f b = bones[0]->getNode(0).position;
    sf::Vector2f pn = bones[lastIndex-1]->getNode(-1).position;

    float difA = sqrtf(Math::square(pn - t));

    float tolerance = 1e-4;
    int noIterations = 0;
    int maxIterations = 1;

    while(difA > tolerance && noIterations < maxIterations)
    {
        {
            BoneData lastIndData  = bones[lastIndex-1]->getData();
            SkeletonNode& nodeN   = bones[lastIndex-1]->getNode(0);
            SkeletonNode& nodeNM1 = bones[lastIndex-1]->getNode(-1);

            sf::Vector2f& pi   = nodeN.position;
            sf::Vector2f& pip1 = nodeNM1.position;

            pip1 = t;

            float ri = sqrtf(Math::square(pip1 - pi));
            float lambdai = lastIndData.length/ri;

            pi = (1.0f-lambdai)*pip1 + lambdai*pi;

            bones[lastIndex-1]->setOrientation(Math::norm(pip1-pi));
        }
        //pn = t;
        for(int i=lastIndex-2; i>=0; --i)
        {

            /*if(i>0)
            {
                constrainToAngularRange(nodes[i+1], nodes[i], nodes[i-1]);
            }
            else
            {
                constrainToAngularRange(nodes[baseIndex], nodes[baseIndex+1]);
            }*/
            BoneData boneData  = bones[i]->getData();
            SkeletonNode& boneNode1 = bones[i]->getNode(0);
            SkeletonNode& boneNode2 = bones[i]->getNode(-1);

            sf::Vector2f& pi   = boneNode1.position;
            sf::Vector2f& pip1 = boneNode2.position;

            pip1 = bones[i+1]->getNode(0).position;

            float ri = sqrtf(Math::square(pip1 - pi));
            float lambdai = boneData.length/ri;

            pi = (1.0f-lambdai)*pip1 + lambdai*pi;

            bones[i]->setOrientation(Math::norm(pip1-pi));

            /*sf::Vector2f offset = bones[i+1]->getOffset() -
                                 sf::Vector2f{boneNode1.distanceToNext, 0.0f};
            offset = offset.x * boneNode2.orientation +
                     offset.y * Math::orthogonal(boneNode2.orientation, 1.0f);*/

            //pi = pi + offset;
            //pip1 = pip1 + offset;
        }

        //nodes[baseIndex].position = b;

        {
            BoneData boneData  = bones[0]->getData();
            SkeletonNode& node0 = bones[0]->getNode(0);
            SkeletonNode& node1 = bones[0]->getNode(-1);

            sf::Vector2f& pi   = node0.position;
            sf::Vector2f& pip1 = node1.position;

            pi = b;

            float ri = sqrtf(Math::square(pip1 - pi));
            float lambdai = boneData.length/ri;

            pip1 = (1-lambdai)*pi + lambdai*pip1;

            bones[0]->setOrientation(Math::norm(pip1-pi));
        }

        for(int i=1; i<lastIndex; ++i)
        {

            /*if(i>baseIndex)
            {
                constrainToAngularRange(nodes[i-1], nodes[i], nodes[i+1]);
            }
            else
            {
                constrainToAngularRange(nodes[baseIndex], nodes[baseIndex+1]);
            }*/
            BoneData boneData  = bones[i]->getData();
            SkeletonNode& boneNode1 = bones[i]->getNode(0);
            SkeletonNode& boneNode2 = bones[i]->getNode(-1);

            sf::Vector2f& pi   = boneNode1.position;
            sf::Vector2f& pip1 = boneNode2.position;

            pi = bones[i-1]->getNode(-1).position;

            float ri = sqrtf(Math::square(pip1 - pi));
            float lambdai = boneData.length/ri;

            pip1 = (1-lambdai)*pi + lambdai*pip1;

            bones[i]->setOrientation(Math::norm(pip1-pi));

            SkeletonNode& prevNode = bones[i-1]->getNode(-1);

            /*sf::Vector2f offset = bones[i-1]->getOffset() -
                                 sf::Vector2f{bones[i-1]->getNode(0).distanceToNext, 0.0f};
            offset = offset.x * prevNode.orientation +
                     offset.y * Math::orthogonal(prevNode.orientation, 1.0f);*/

            //pi = pi + offset;
            //pip1 = pip1 +  offset;
        }

        difA = sqrtf(Math::square(pn - t));
        ++noIterations;

    }

    Skeleton2DBone& baseBone = *bones[0];
    baseBone.setAngle( -AngleCalculations::calcAngleToParent(baseBone) );

    for(int i=0; i<lastIndex-1; ++i)
    {
        SkeletonNode& node1 = bones[i]->getNode(0);
        SkeletonNode& node2 = bones[i]->getNode(-1);
        SkeletonNode& node3 = bones[i+1]->getNode(0);
        SkeletonNode& node4 = bones[i+1]->getNode(-1);

        float angle = -AngleCalculations::getAngle(node1, node2, node3, node4);
        bones[i+1]->setAngle(angle);
    }
}

void KinematicAlgorithms::forwardK(std::vector<Skeleton2DBone* > bones,
              sf::Vector2f const & t)
{
    sf::Vector2f resultantVector = {0.0f, 0.0f};
    float resultantAngle = 0.0f;

    assert(!std::isnan(t.x) && !std::isnan(t.y));

    bones[0]->getNode(0).position = t;

    for(int i=0; i<bones.size(); ++i)
    {
        Skeleton2DBone& currBone = *bones[i];
        BoneData currBoneData = currBone.getData();

        resultantAngle = currBoneData.rotation;
        resultantVector = Math::rotate(currBone.getParentOrientation(),
                                       resultantAngle);
        assert(!std::isnan(resultantAngle));
        assert(!std::isnan(bones[i]->getOrientation().x));

        currBone.getNode(-1).position = currBone.getNode(0).position +
                                        resultantVector * currBoneData.length;

        sf::Vector2f newDir = resultantVector;

        currBone.setOrientation(newDir);

        if(i < bones.size()-1)
            bones[i+1]->setParentOrientation(newDir);
    }
}
