#include "Skeleton2D.h"


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
              Skeleton2DBone chain,
              std::string const & parentName,
              int linkIndex)
{
    chains.insert({name, chain});
    std::cout << name << ": "  << chain.getNumNodes() << "\n";
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
            SkeletonNode secondNode(secondNodePos, 0, -Math::PI, Math::PI, "", true);
            secondNode.orientation = relDir;

            addChain(boneData.name, Skeleton2DBone({firstNode, secondNode}, boneData.offset), boneData.parent);
        }
        else
        {
            addChain(boneData.name, Skeleton2DBone({firstNode}, boneData.offset), boneData.parent);
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
            SkeletonNode secondNode(secondNodePos, 0, -Math::PI, Math::PI, "", true);
            secondNode.orientation = relDir;

            addChain(boneData.name, Skeleton2DBone({firstNode, secondNode}, boneData.offset), NULL_NAME);
        }
        else
        {
            addChain(boneData.name, Skeleton2DBone({firstNode}, boneData.offset), NULL_NAME);
        }
    }
}

std::vector<std::string > Skeleton2D::getHierarchy(std::string const & firstNode,
                                                   std::string const & lastNode)
{
    std::vector<std::string > hierarchy = {lastNode};
    std::string parentBone = "";
    std::string currentBone = lastNode;
    while(parentBone != firstNode)
    {
        parentBone = getParent(currentBone);
        hierarchy.insert(hierarchy.begin(), parentBone);
        currentBone = parentBone;

        if(currentBone == chains.begin()->first)
            return {};
    }

    return hierarchy;
}


void Skeleton2D::addIKConstraint(IKConstraintData const & ikData)
{
    std::vector<std::string > hierarchy = getHierarchy(ikData.firstBone,
                                                       ikData.lastBone);

    std::vector<SkeletonNode > ikNodes;

    sf::Vector2f baseOffset = chains[ikData.firstBone].getOffset();

    for(std::string &boneName : hierarchy)
    {
        Skeleton2DBone& bone = chains[boneName];

        for(int i=0; i<bone.getNumNodes(); ++i)
        {
            ikNodes.push_back(bone.getNode(i));
            if(i == bone.getNumNodes()-1)
            {
               /*ikNodes[i].minAngle = 0.0f;
                ikNodes[i].maxAngle = 0.0f;
                ikNodes[i].minCosine = 0.0f;
                ikNodes[i].maxCosine = 0.0f;*/
                //std::cout << i << "reset cosine\n";
            }
        }
    }

    //update parentTo References with new name
    /*for(auto &pCPair : parentTo)
    {
        for(std::string &boneName : hierarchy)
        {
            if(pCPair.first == boneName)
                pCPair.first = ikData.name;
            if(pCPair.second == boneName)
                pCPair.second = ikData.name;
        }
    }*/

    //ikConstraints.insert({ikData.name, Skeleton2DBone(ikNodes, baseOffset)});
    ikGroups.insert({ikData.name, hierarchy});
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
    else if(ikGroups.find(chainName) != ikGroups.end())
    {
        //ikConstraints[chainName].setTarget(target, chainNode, true);
        //std::cout << chainName << " is manipulated\n";
        std::vector<Skeleton2DBone* > ikBones;
        for(std::string & boneName : ikGroups[chainName])
        {
            ikBones.push_back(&chains[boneName]);
        }

        Skeleton2DBone::inverseK(target, ikBones);
    }
}

void Skeleton2D::draw(sf::RenderWindow& window)
{
    for(auto it = chains.begin(); it != chains.end(); ++it)
        it->second.draw(window);
    //for(auto it = ikConstraints.begin(); it != ikConstraints.end(); ++it)
    //    it->second.draw(window);
}
