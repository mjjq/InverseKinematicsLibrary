#include "Skeleton2DBone.h"

#include "KinematicAlgorithms.h"

Skeleton2DBone::Skeleton2DBone() : initialBoneData{BoneData()} {}

Skeleton2DBone::Skeleton2DBone(BoneData const & _boneData,
                               sf::Vector2f const & _parentPosition,
                               sf::Vector2f const & _parentOrientation) :
    initialBoneData{_boneData},
    boneData{_boneData},
    parentOrientation{_parentOrientation}
{
    SkeletonNode firstNode(_parentPosition);
    nodes.push_back(firstNode);

    if(boneData.length > 0.0f)
    {
        sf::Vector2f relDir = Math::rotate(_parentOrientation, boneData.rotation);

        sf::Vector2f secondNodePos = _parentPosition + boneData.length * relDir;
        SkeletonNode secondNode(secondNodePos);

        nodes.push_back(secondNode);
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
                                bool applyOffset,
                                RelativeTo const & relativeTo)
{

    sf::Vector2f offset = {0.0f, 0.0f};
    if(applyOffset)
        offset = initialBoneData.offset.x * parentOrientation +
                 initialBoneData.offset.y * Math::orthogonal(parentOrientation, 1.0f);

    sf::Vector2f finalPosition = t + translation;
    switch(relativeTo)
    {
        case RelativeTo::Current:
        {
            finalPosition += nodes[0].position;
            break;
        }
        case RelativeTo::Parent:
        {
            finalPosition += parentPosition;
            //std::cout << "finalpos: " << finalPosition.x << ", " << finalPosition.y << "\n";
            //std::cout << "offset: " << offset.x << ", " << offset.y << "\n";
            break;
        }
        default:
            break;
    }

    finalPosition += offset;

    if(targetIndex < 0 || targetIndex >= nodes.size())
        targetIndex = nodes.size()-1;

    if(targetIndex==0)
    {
        KinematicAlgorithms::forwardK({this}, finalPosition);
    }
    else
    {
        KinematicAlgorithms::inverseK({this}, finalPosition);
    }
}

void Skeleton2DBone::setRotation(float angleDegree,
                                 RelativeTo const & relativeTo)
{
    float currRotation;

    switch(relativeTo)
    {
        case RelativeTo::InitialPose:
        {
            currRotation = initialBoneData.rotation;
            break;
        }
        case RelativeTo::Parent:
        {
            currRotation = 0.0f;
            break;
        }
        case RelativeTo::Current:
        {
            currRotation = boneData.rotation;
            break;
        }
    }

    setAngle(currRotation + angleDegree);
}


SkeletonNode& Skeleton2DBone::getNode(int index)
{
    if(index < 0) index = nodes.size()+index;
    return nodes[index];
}

sf::Vector2f Skeleton2DBone::getOrientation()
{
    return orientation;
}

void Skeleton2DBone::setOrientation(sf::Vector2f const & o)
{
    orientation = o;
}

sf::Vector2f Skeleton2DBone::getParentOrientation()
{
    return parentOrientation;
}

void Skeleton2DBone::setParentOrientation(sf::Vector2f const & po)
{
    parentOrientation = po;
}

sf::Vector2f Skeleton2DBone::getParentPosition()
{
    return parentPosition;
}

void Skeleton2DBone::setParentPosition(sf::Vector2f const & pp)
{
    parentPosition = pp;
}

void Skeleton2DBone::setTranslation(sf::Vector2f const & tr)
{
    translation = tr;
}

BoneData Skeleton2DBone::getInitialData()
{
    return initialBoneData;
}

BoneData Skeleton2DBone::getData()
{
    return boneData;
}

void Skeleton2DBone::setAngle(float angleDegree)
{
    boneData.rotation = angleDegree;
}
