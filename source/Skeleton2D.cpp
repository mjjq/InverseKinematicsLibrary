#include "Skeleton.h"


Skeleton2D::Skeleton2D(sf::Vector2f const & basePos) : basePosition{basePos}
{}

int Skeleton2D::getParentChildPair(std::string const & parent,
                           std::string const & child)
{
    for(int i=0; i<parentTo.size(); ++i)
    {
        if(parentTo[i].first == parent && parentTo[i].second == child)
            return i;
    }

    return PAIR_NOT_FOUND;
}

std::string Skeleton2D::getParent(std::string const & childName)
{
    for(int i=0; i<parentTo.size(); ++i)
        if(parentTo[i].second == childName)
            return parentTo[i].first;

    return NULL_NAME;
}

void Skeleton2D::linkParentToChild(std::string const & parent,
                       std::string const & child,
                       int linkIndex)
{
    if(chains.find(parent) != chains.end() &&
       chains.find(child) != chains.end())
    {
        parentTo.push_back({parent, child});

        chains[child].setParentNode(chains[parent].getNode(linkIndex));
        updateBaseNodeOrientation(child);

        sf::Vector2f parentStartPos = chains[parent].getNode(0).position;
        chains[child].setTarget(parentStartPos, 0, true);
    }
}

void Skeleton2D::updateBaseNodeOrientation(std::string const & childName)
{
    std::string parentName = getParent(childName);
    if(parentName != NULL_NAME)
    {
        SkeletonNode& parentEndNode = chains[parentName].getNode(-1);

        chains[childName].updateParentOrientation(parentEndNode.orientation);
    }
}


void Skeleton2D::addChain(std::string const & name,
              Skeleton2DChain chain,
              std::string const & parentName,
              int linkIndex)
{
    chains.insert({name, chain});
    linkParentToChild(parentName, name, linkIndex);
}

void Skeleton2D::addBone(BoneData const & boneData)
{
    if(chains.find(boneData.parent) != chains.end())
    {
        SkeletonNode& parentNode = chains[boneData.parent].getNode(-1);

        sf::Vector2f firstNodePos = parentNode.position;
        SkeletonNode firstNode(firstNodePos, 0, -Math::PI, Math::PI, "");

        if(boneData.length > 0.0f)
        {
            sf::Vector2f relDir = Math::rotate({1.0f, 0.0f}, boneData.rotation);

            sf::Vector2f secondNodePos = firstNodePos + boneData.length * relDir;
            SkeletonNode secondNode(secondNodePos, 0, -Math::PI, Math::PI, "");
            secondNode.orientation = relDir;

            addChain(boneData.name, Skeleton2DChain({firstNode, secondNode}, boneData.offset), boneData.parent);
        }
        else
        {
            addChain(boneData.name, Skeleton2DChain({firstNode}, boneData.offset), boneData.parent);
        }
    }
    else
    {
        sf::Vector2f firstNodePos = {0.0f, 0.0f};
        SkeletonNode firstNode(firstNodePos, 0, -Math::PI, Math::PI, "");

        if(boneData.length > 0.0f)
        {
            sf::Vector2f relDir = Math::rotate({1.0f, 0.0f}, boneData.rotation);
            sf::Vector2f secondNodePos = firstNodePos + boneData.length * relDir;
            SkeletonNode secondNode(secondNodePos, 0, -Math::PI, Math::PI, "");
            secondNode.orientation = relDir;

            addChain(boneData.name, Skeleton2DChain({firstNode, secondNode}, boneData.offset), NULL_NAME);
        }
        else
        {
            addChain(boneData.name, Skeleton2DChain({firstNode}, boneData.offset), NULL_NAME);
        }
    }
}

void Skeleton2D::setTarget(sf::Vector2f const & target,
               std::string const & chainName,
               int chainNode, bool applyOffset)
{
    if(chains.find(chainName) != chains.end())
    {
        if(getParent(chainName) != NULL_NAME)
        {
            SkeletonNode& baseNode  = chains[chainName].getBaseNode();
            baseNode.angle = chains[chainName].getBaseNodeAngle();
        }

        chains[chainName].setTarget(target, chainNode, applyOffset);

        updateBaseNodeOrientation(chainName);

        //recursively update targets to children
        for(int i=0; i<parentTo.size(); ++i)
        {
            if(parentTo[i].first == chainName)
            {
                sf::Vector2f lastNodePos = chains[chainName].getNode(0).position;
                setTarget(lastNodePos, parentTo[i].second, 0, true);
            }
        }
    }
}

void Skeleton2D::draw(sf::RenderWindow& window)
{
    for(auto it = chains.begin(); it != chains.end(); ++it)
        it->second.draw(window);
}
