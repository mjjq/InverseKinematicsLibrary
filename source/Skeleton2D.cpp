#include "Skeleton2D.h"

#include "KinematicAlgorithms.h"


int Skeleton2D::getParentChildPair(std::string const & parent,
                           std::string const & child)
{
    for(int i=0; i<(int)parentTo.size(); ++i)
    {
        if(parentTo[i].first == parent && parentTo[i].second == child)
            return i;
    }

    return PAIR_NOT_FOUND;
}

std::string Skeleton2D::getParent(std::string const & childName)
{
    for(int i=0; i<(int)parentTo.size(); ++i)
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

        updateBaseNodeOrientation(parent, child);

        sf::Vector2f parentStartPos = chains[parent].getNode(0).position;
        chains[child].setTarget(parentStartPos, 0, true);
    }
}

void Skeleton2D::updateBaseNodeOrientation(std::string const & parentName,
                                           std::string const & childName)
{
    if(parentName != NULL_NAME)
    {
        sf::Vector2f parentOrientation = chains[parentName].getOrientation();
        sf::Vector2f parentPosition = chains[parentName].getNode(0).position;

        chains[childName].setParentOrientation(parentOrientation);
        chains[childName].setParentPosition(parentPosition);
    }
}


void Skeleton2D::addChain(std::string const & name,
              Skeleton2DBone chain,
              std::string const & parentName,
              int linkIndex)
{
    chains.insert({name, chain});
    linkParentToChild(parentName, name, linkIndex);
}

void Skeleton2D::addBone(BoneData const & boneData)
{
        sf::Vector2f parentPosition = {0.0f, 0.0f};
        sf::Vector2f parentOrientation = {1.0f, 0.0f};

        if(chains.find(boneData.parent) != chains.end())
        {
            SkeletonNode& parentNode = chains[boneData.parent].getNode(-1);
            parentPosition = parentNode.position;
            parentOrientation = chains[boneData.parent].getOrientation();
        }

        addChain(boneData.name,
                 Skeleton2DBone(boneData, parentPosition, parentOrientation),
                 boneData.parent,
                 -1);
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
    ikGroups.insert({ikData.name, hierarchy});
}

void Skeleton2D::addAnimation(SkeletonAnimation animation)
{
    //animations.insert({animation.getName(), animation});
    animations.addAnimation(animation);
}


void Skeleton2D::setTarget(sf::Vector2f const & target,
               std::string const & chainName,
               int chainNode, bool applyOffset,
               bool inheritOrientation,
               Skeleton2DBone::RelativeTo relativeTo)
{
    if(chains.find(chainName) != chains.end())
    {
        if(inheritOrientation)
            updateBaseNodeOrientation(getParent(chainName), chainName);

        chains[chainName].setTarget(target, chainNode, applyOffset, relativeTo);


        //recursively update targets to children
        for(int i=0; i<(int)parentTo.size(); ++i)
        {
            if(parentTo[i].first == chainName)
            {
                std::string childName = parentTo[i].second;

                sf::Vector2f lastNodePos = chains[chainName].getNode(0).position;
                setTarget(lastNodePos, parentTo[i].second, 0, true);
            }
        }
    }
    else if(ikGroups.find(chainName) != ikGroups.end())
    {
        std::vector<Skeleton2DBone* > ikBones;
        std::string finalName = "";
        for(std::string & boneName : ikGroups[chainName])
        {
            ikBones.push_back(&chains[boneName]);
            finalName = boneName;
        }

        KinematicAlgorithms::inverseK(ikBones, target);

        //recursively update targets to children
        for(int i=0; i<(int)parentTo.size(); ++i)
        {
            if(parentTo[i].first == finalName)
            {
                sf::Vector2f lastNodePos = chains[finalName].getNode(0).position;
                setTarget(lastNodePos, parentTo[i].second, 0, true);
            }
        }
    }
}

void Skeleton2D::setRotation(float angleDegree,
                             std::string const & boneName,
                             Skeleton2DBone::RelativeTo const & relativeTo)
{
    if(chains.find(boneName) == chains.end())
        return;

    if(scale.x < 0.0f) angleDegree = 180.0f - angleDegree;
    if(scale.y < 0.0f) angleDegree = -angleDegree;

    chains[boneName].setRotation(angleDegree, relativeTo);

    setTarget({0.0f, 0.0f}, boneName, 0, false, true, Skeleton2DBone::RelativeTo::Current);
}

void Skeleton2D::resetSkeleton()
{
    for(auto it = chains.begin(); it != chains.end(); ++it)
    {
        setRotation(0.0f, it->first, Skeleton2DBone::RelativeTo::InitialPose);
    }
}

void Skeleton2D::setAnimation(std::string const & animationName,
                              AnimationSet::TransitionType const & transitionType)
{
    if(!animations.exists(animationName))
        return;

    animations.transitionTo(animationName, transitionType, 0.0f);

    resetSkeleton();
}

void Skeleton2D::animate(float time)
{

    std::vector<BoneAnimation > & currAnimation =
            animations.getCurrentAnimationData(time);

    float localAnimTime = animations.getLocalTime();

    for(int i=0; i<(int)currAnimation.size(); ++i)
    {
        std::string boneName = currAnimation[i].getBoneName();
        if(chains.find(boneName) != chains.end())
        {
            float angle = currAnimation[i].getRotation(localAnimTime);
            setRotation(angle, boneName, Skeleton2DBone::RelativeTo::InitialPose);

            sf::Vector2f translation = currAnimation[i].getTranslation(localAnimTime);
            //translation.x *= scale.x;
            //translation.y *= scale.y;
            chains[boneName].setTranslation(translation);

            setTarget({0.0f, 0.0f}, boneName, 0, true, true, Skeleton2DBone::RelativeTo::Parent);
        }
    }
}

void Skeleton2D::draw(sf::RenderWindow& window)
{
    for(auto it = chains.begin(); it != chains.end(); ++it)
        it->second.draw(window);
}

std::map<std::string, Skeleton2DBone > Skeleton2D::getBoneData()
{
    return chains;
}

std::vector<sf::Vector2f > Skeleton2D::getJointPositions()
{
    std::vector<sf::Vector2f > jointPositions;

    for(auto it = chains.begin(); it != chains.end(); ++it)
    {
        jointPositions.push_back(it->second.getNode(0).position);
        jointPositions.push_back(it->second.getNode(-1).position);
    }

    return jointPositions;
}

BoneData Skeleton2D::getBoneData(std::string const & boneName)
{
    return chains[boneName].getData();
}

void Skeleton2D::setScale(sf::Vector2f const & _scale)
{
    scale = _scale;

    sf::Vector2f rootNodePos = (getBoneData("root")).position;

    for(auto it = chains.begin(); it != chains.end(); ++it)
    {
        it->second.setScale(scale, rootNodePos);
    }
}
