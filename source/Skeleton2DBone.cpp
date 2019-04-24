#include "Skeleton2DBone.h"

Skeleton2DBone::Skeleton2DBone() {}

Skeleton2DBone::Skeleton2DBone(std::vector<SkeletonNode > _nodes,
                                 sf::Vector2f const & _offset) :
                                     nodes{_nodes}, absOffset{_offset}
{
    resetBeams();
}

bool Skeleton2DBone::constrainToAngularRange(SkeletonNode& node1,
                             SkeletonNode& node2,
                             SkeletonNode& node3)
{
    float minAngle = node2.minAngle;
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
        //std::cout << minAngle << " min\n";
        return true;
    }
    else if(nodeAngle > maxAngle)
    {
        node3.position = Math::rotate(d1hat, -180.0f*maxAngle/Math::PI) + node2.position;
        //std::cout << maxAngle << " max\n";
        return true;
    }

    return false;
}

void Skeleton2DBone::constrainToAngularRange(SkeletonNode& node1,
                             SkeletonNode& node2)
{
    SkeletonNode virtualNode(node1.position - node1.orientation,
                             0, 0.0f, 0.0f, "");
    constrainToAngularRange(virtualNode, node1, node2);
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

                pi = (1.0f-lambdai)*pip1 + lambdai*pi;
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

                pip1 = (1.0f-lambdai)*pi + lambdai*pip1;
            }

            difA = sqrtf(Math::square(pn - t));
            ++noIterations;
        }
    }

    baseNodeAngle = SkeletonNode::getAngle(nodes[0], nodes[1]);
    nodes[0].angle = baseNodeAngle;

    for(int i=baseIndex; i<targetIndex; ++i)
    {
        if(i < targetIndex - 1 && !nodes[i+1].lockAngle)
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

}

void Skeleton2DBone::inverseK(sf::Vector2f const & t,
                              std::vector<Skeleton2DBone* > bones)
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
            SkeletonNode& nodeN   = bones[lastIndex-1]->getNode(0);
            SkeletonNode& nodeNM1 = bones[lastIndex-1]->getNode(-1);

            sf::Vector2f& pi   = nodeN.position;
            sf::Vector2f& pip1 = nodeNM1.position;

            pip1 = t;

            float ri = sqrtf(Math::square(pip1 - pi));
            float lambdai = nodeN.distanceToNext/ri;

            pi = (1.0f-lambdai)*pip1 + lambdai*pi;

            nodeNM1.orientation = (pip1-pi)/ri;
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

            SkeletonNode& boneNode1 = bones[i]->getNode(0);
            SkeletonNode& boneNode2 = bones[i]->getNode(-1);

            sf::Vector2f& pi   = boneNode1.position;
            sf::Vector2f& pip1 = boneNode2.position;

            pip1 = bones[i+1]->getNode(0).position;

            float ri = sqrtf(Math::square(pip1 - pi));
            float lambdai = boneNode1.distanceToNext/ri;

            pi = (1.0f-lambdai)*pip1 + lambdai*pi;

            boneNode2.orientation = (pip1-pi)/ri;

            sf::Vector2f offset = bones[i+1]->getOffset() -
                                 sf::Vector2f{boneNode1.distanceToNext, 0.0f};
            offset = offset.x * boneNode2.orientation +
                     offset.y * Math::orthogonal(boneNode2.orientation, 1.0f);

            pi = pi;// - offset;
            pip1 = pip1;// - offset;
        }

        //nodes[baseIndex].position = b;

        {
            SkeletonNode& node0 = bones[0]->getNode(0);
            SkeletonNode& node1 = bones[0]->getNode(-1);

            sf::Vector2f& pi   = node0.position;
            sf::Vector2f& pip1 = node1.position;

            pi = b;

            float ri = sqrtf(Math::square(pip1 - pi));
            float lambdai = node0.distanceToNext/ri;

            pip1 = (1-lambdai)*pi + lambdai*pip1;

            node1.orientation = (pip1-pi)/ri;
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
            SkeletonNode& boneNode1 = bones[i]->getNode(0);
            SkeletonNode& boneNode2 = bones[i]->getNode(-1);

            sf::Vector2f& pi   = boneNode1.position;
            sf::Vector2f& pip1 = boneNode2.position;

            pi = bones[i-1]->getNode(-1).position;

            float ri = sqrtf(Math::square(pip1 - pi));
            float lambdai = boneNode1.distanceToNext/ri;

            pip1 = (1-lambdai)*pi + lambdai*pip1;

            boneNode2.orientation = (pip1-pi)/ri;

            SkeletonNode& prevNode = bones[i-1]->getNode(-1);

            sf::Vector2f offset = bones[i-1]->getOffset() -
                                 sf::Vector2f{bones[i-1]->getNode(0).distanceToNext, 0.0f};
            offset = offset.x * prevNode.orientation +
                     offset.y * Math::orthogonal(prevNode.orientation, 1.0f);

            pi = pi;// - offset;
            pip1 = pip1;// - offset;
        }

        difA = sqrtf(Math::square(pn - t));
        ++noIterations;

        Skeleton2DBone& baseBone = *bones[0];
        baseBone.baseNodeAngle = SkeletonNode::getAngle(baseBone.getNode(0),
                                               baseBone.getNode(-1));
        baseBone.getNode(0).angle = baseBone.baseNodeAngle;

        for(int i=0; i<lastIndex-1; ++i)
        {
            SkeletonNode& node1 = bones[i]->getNode(0);
            SkeletonNode& node2 = bones[i]->getNode(-1);
            SkeletonNode& node3 = bones[i+1]->getNode(0);
            SkeletonNode& node4 = bones[i+1]->getNode(-1);

            node2.angle = SkeletonNode::getAngle(node1, node2, node3, node4);
            node3.angle = node2.angle;
            bones[i+1]->baseNodeAngle = node2.angle;
        }
    }
}

void Skeleton2DBone::forwardK(sf::Vector2f const & t,
              int targetIndex)
{
    sf::Vector2f resultantVector = {0.0f, 0.0f};
    float resultantAngle = 0.0f;

    assert(!std::isnan(t.x) && !std::isnan(t.y));

    if(targetIndex == 0)
    {
        nodes[0].position = t;
    }

    for(int i=targetIndex; i<nodes.size()-1; ++i)
    {

        resultantAngle = 180.0f*nodes[i].angle/Math::PI;
        resultantVector = Math::rotate(nodes[i].orientation * nodes[i].distanceToNext,
                                       -resultantAngle);

        assert(!std::isnan(resultantAngle));
        assert(!std::isnan(nodes[i].orientation.x));

        nodes[i+1].position = nodes[i].position +
                              resultantVector;

        sf::Vector2f newDir = Math::norm(nodes[i+1].position -
                                         nodes[i].position);

        nodes[i+1].orientation = newDir;
    }
}

void Skeleton2DBone::resetBeams()
{
    for(int i=1; i<nodes.size(); ++i)
    {
        //beams.push_back(SkeletonBeam(nodes[i-1], nodes[i], 'y'));
        sf::Vector2f relVector = nodes[i].position - nodes[i-1].position;
        nodes[i-1].distanceToNext = sqrtf(Math::square(relVector));
        sf::Vector2f direction = relVector/nodes[i-1].distanceToNext;

        nodes[i].orientation = direction;
    }
    if(!linkedToParent)
    {
        nodes[0].orientation = {1.0f, 0.0f};
    }

    for(int i=0; i<nodes.size()-1; ++i)
    {
        nodes[i+1].angle = SkeletonNode::getAngle(nodes[i], nodes[i+1], nodes[i+2]);
    }
}

void Skeleton2DBone::draw(sf::RenderWindow& window)
{
    float radius = 5.0f;
    sf::CircleShape shape(radius);
    shape.setOrigin({radius, radius});
    sf::VertexArray line(sf::LineStrip, 2);
    line[0].color = sf::Color::White;
    line[1].color = sf::Color::White;


    shape.setPosition(nodes[0].position);
    window.draw(shape);

    for(int i=0; i<nodes.size()-1; ++i)
    {
        line[0].position = nodes[i].position;
        line[1].position = nodes[i+1].position;
        window.draw(line);

        shape.setPosition(nodes[i+1].position);
        window.draw(shape);
    }
}

void Skeleton2DBone::setTarget(sf::Vector2f const & t, int targetIndex,
                                bool applyOffset, bool setOffset)
{
    sf::Vector2f offset = {0.0f, 0.0f};
    if(applyOffset)
        offset = relOffset.x * nodes[0].orientation +
                 relOffset.y * Math::orthogonal(nodes[0].orientation, 1.0f);

    if(targetIndex < 0 || targetIndex >= nodes.size())
    {
        targetIndex = nodes.size()-1;
        inverseK(t+offset, 0, targetIndex);
    }
    else if(targetIndex==0)
    {
        forwardK(t+offset, targetIndex);
    }
    else
    {
        inverseK(t+offset, 0, targetIndex);
        forwardK(t+offset, targetIndex);
    }
}

void Skeleton2DBone::setParentNode(SkeletonNode& parentNode)
{
    linkedToParent = true;

    resetBeams();

    nodes[0].angle = SkeletonNode::getAngle(nodes[0], nodes[1]);
    baseNodeAngle = nodes[0].angle;

    //relOffset = {Math::dot(absOffset, nodes[0].orientation),
    //             Math::cross(absOffset, nodes[0].orientation)};
    relOffset = absOffset;
}

SkeletonNode& Skeleton2DBone::getBaseNode()
{
    return nodes[0];
}

SkeletonNode& Skeleton2DBone::getNode(int index)
{
    if(index < 0) index = nodes.size()+index;

    assert(index >=0 && index < nodes.size());

    return nodes[index];
}


void Skeleton2DBone::setBaseNodeAngle(float angle)
{
    baseNodeAngle = angle;
}

float Skeleton2DBone::getBaseNodeAngle()
{
    return baseNodeAngle;
}

int Skeleton2DBone::getNumNodes()
{
    return nodes.size();
}

void Skeleton2DBone::updateParentOrientation(sf::Vector2f const & orientation)
{
    nodes[0].orientation = orientation;
}


sf::Vector2f Skeleton2DBone::getOffset()
{
    return absOffset;
}
