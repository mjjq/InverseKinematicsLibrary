#include "Skeleton.h"

Skeleton2DChain::Skeleton2DChain() {}

Skeleton2DChain::Skeleton2DChain(std::vector<SkeletonNode > _nodes,
                                 sf::Vector2f const & _offset) :
                                     nodes{_nodes}, offset{_offset}
{
    resetBeams();
}


/*void Skeleton2DChain::constrainToAngularRange(SkeletonBeam& firstBeam,
                                 SkeletonBeam& secondBeam,
                                 Direction dir)
{
    if(dir == Direction::Forward)
        constrainToAngularRange(firstBeam.node1,
                                secondBeam.node1,
                                secondBeam.node2);
    else
        constrainToAngularRange(secondBeam.node2,
                                secondBeam.node1,
                                firstBeam.node1);
}*/

bool Skeleton2DChain::constrainToAngularRange(SkeletonNode& node1,
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

void Skeleton2DChain::constrainToAngularRange(SkeletonNode& node1,
                             SkeletonNode& node2)
{
    SkeletonNode virtualNode(node1.position - node1.orientation,
                             0, 0.0f, 0.0f, "");
    constrainToAngularRange(virtualNode, node1, node2);
}

/*void Skeleton2DChain::constrainToAngularRange(SkeletonBeam& beam)
{
    constrainToAngularRange(beam.node1, beam.node2);
}*/

void Skeleton2DChain::inverseK(sf::Vector2f const & t,
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


    /*sf::Vector2f newDir = Math::norm(nodes[1].position -
                                     nodes[0].position);
    nodes[0].angle = atan2(-newDir.y, newDir.x);*/

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

void Skeleton2DChain::forwardK(sf::Vector2f const & t,
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

void Skeleton2DChain::resetBeams()
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

void Skeleton2DChain::draw(sf::RenderWindow& window)
{
    float radius = 5.0f;
    sf::CircleShape shape(radius);
    shape.setOrigin({radius, radius});
    sf::VertexArray line(sf::LineStrip, 2);
    line[0].color = sf::Color::White;
    line[1].color = sf::Color::White;


    for(int i=0; i<nodes.size()-1; ++i)
    {
        line[0].position = nodes[i].position + offset;
        line[1].position = nodes[i+1].position + offset;
        window.draw(line);

        shape.setPosition(nodes[i+1].position + offset);
        window.draw(shape);
    }
}

void Skeleton2DChain::setTarget(sf::Vector2f const & t, int targetIndex)
{
    if(targetIndex < 0 || targetIndex >= nodes.size())
    {
        targetIndex = nodes.size()-1;
        inverseK(t, 0, targetIndex);
    }
    else if(targetIndex==0)
    {
        forwardK(t, targetIndex);
    }
    else
    {
        inverseK(t, 0, targetIndex);
        forwardK(t, targetIndex);
    }
}

void Skeleton2DChain::setParentNode(SkeletonNode& parentNode)
{
    //beams.clear();
    //beams.push_back(SkeletonBeam(parentNode, nodes[0], 'y'));
    //parentNode.orientation = Math::norm(nodes[0].position - parentNode.position);
    nodes.insert(nodes.begin(), parentNode);
    linkedToParent = true;

    resetBeams();

    if(nodes.size() > 2)
    {
        nodes[1].angle = SkeletonNode::getAngle(nodes[0], nodes[1], nodes[2]);
    }
    else
        nodes[1].angle = 0.0f;


    nodes[0].angle = SkeletonNode::getAngle(nodes[0], nodes[1]);
    baseNodeAngle = nodes[0].angle;
}

/*void Skeleton2DChain::updateParentNode(SkeletonNode& parentNode)
{
    if(linkedToParent)
    {
        nodes[0].position = parentNode.position;
        nodes[0].orientation = parentNode.orientation;
    }
}*/

SkeletonNode& Skeleton2DChain::getBaseNode()
{
    return nodes[0];
}

SkeletonNode& Skeleton2DChain::getNode(int index)
{
    if(index < 0) index = nodes.size()+index;

    assert(index >=0 && index < nodes.size());

    return nodes[index];
}

/*SkeletonBeam& Skeleton2DChain::getBeam(int index)
{
    if(index < 0) index = beams.size()+index;

    return beams[index];
}*/

void Skeleton2DChain::setBaseNodeAngle(float angle)
{
    baseNodeAngle = angle;
}

float Skeleton2DChain::getBaseNodeAngle()
{
    return baseNodeAngle;
}

int Skeleton2DChain::getNumNodes()
{
    return nodes.size();
}
