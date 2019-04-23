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

    float nodeAngle = SkeletonNode::getAngle(node1, node2, node3);

    sf::Vector2f d1hat = Math::norm(node2.position -
                                    node1.position);

    if(nodeAngle < minAngle)
    {
        node3.position = Math::rotate(d1hat, -180.0f*minAngle/Math::PI) + node2.position;
        return true;
    }
    else if(nodeAngle > maxAngle)
    {
        node3.position = Math::rotate(d1hat, -180.0f*maxAngle/Math::PI) + node2.position;
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
