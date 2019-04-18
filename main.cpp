#include <iostream>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "Math.h"
#include <assert.h>


struct SkeletonNode
{
    sf::Vector2f position;
    float minAngle;
    float maxAngle;
    float minCosine;
    float maxCosine;
    sf::Vector2f orientation;
    float angle = 0.0f;
    int z;
    std::string name;

    SkeletonNode() {};
    SkeletonNode(sf::Vector2f _position,
                 int _z, float _minAngle, float _maxAngle,
                 std::string _name) :
                     position{_position},
                     z{_z}, minAngle{_minAngle}, maxAngle{_maxAngle},
                     name{_name}
    {
        minCosine = cosf(minAngle);
        maxCosine = cosf(maxAngle);
    };
};

struct SkeletonBeam
{
    SkeletonNode& node1;
    SkeletonNode& node2;

    char spriteAxis;
    float distance;

    SkeletonBeam(SkeletonNode& _n1,
                 SkeletonNode& _n2,
                 char const & _spriteAxis) :
                node1{_n1}, node2{_n2}, spriteAxis{_spriteAxis}
    {
        distance = sqrtf(Math::square((node1.position - node2.position)));
    }


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

    static float getBeamAngle(SkeletonNode& node1,
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

        return convertQuadrantToSgn(nodeQuadrant)*acosf(angleCosine);
    }

    static float getBeamAngle(SkeletonBeam& beam1,
                              SkeletonBeam& beam2)
    {
        return getBeamAngle(beam1.node1, beam2.node1, beam2.node2);
    }
};

class Skeleton2DChain
{
    std::vector<SkeletonNode > nodes;
    std::vector<SkeletonBeam > beams;
    float baseNodeAngle = 0.0f;

    enum Direction
    {
        Forward,
        Backward
    };

    void constrainToAngularRange(SkeletonBeam& firstBeam,
                                 SkeletonBeam& secondBeam,
                                 Direction dir = Direction::Forward)
    {
        if(dir == Direction::Forward)
            constrainToAngularRange(firstBeam.node1,
                                    secondBeam.node1,
                                    secondBeam.node2);
        else
            constrainToAngularRange(secondBeam.node2,
                                    secondBeam.node1,
                                    firstBeam.node1);
    }

    bool constrainToAngularRange(SkeletonNode& node1,
                                 SkeletonNode& node2,
                                 SkeletonNode& node3)
    {
        float minAngle = node2.minAngle;
        float maxAngle = node2.maxAngle;

        float nodeAngle = SkeletonBeam::getBeamAngle(node1, node2, node3);

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

    void constrainToAngularRange(SkeletonNode& node1,
                                 SkeletonNode& node2)
    {
        SkeletonNode virtualNode(node1.position - node1.orientation,
                                 0, 0.0f, 0.0f, "");
        constrainToAngularRange(virtualNode, node1, node2);
    }

    void constrainToAngularRange(SkeletonBeam& beam)
    {
        constrainToAngularRange(beam.node1, beam.node2);
    }

    void inverseK(sf::Vector2f const & t,
                  int baseIndex,
                  int targetIndex)
    {
        if(targetIndex < 0) targetIndex = nodes.size()-1;

        float dist = sqrtf(Math::square(t - beams[baseIndex].node1.position));
        float totalJointDist = 0.0f;
        for(int i=baseIndex; i<targetIndex; ++i)
            totalJointDist += beams[i].distance;

        if(dist > totalJointDist)
        {
            sf::Vector2f tNew = t;
            if(beams.size() > 0)
            {
                SkeletonNode tempNode(t, 0, 0.0f, 0.0f, "");
                constrainToAngularRange(beams[baseIndex]);
                tNew = tempNode.position;
            }
            for(int i=baseIndex; i<targetIndex; ++i)
            {
                sf::Vector2f pi = beams[i].node1.position;
                float ri = sqrtf(Math::square(tNew - pi));
                float lambdai = beams[i].distance/ri;
                beams[i].node2.position = (1-lambdai)*pi + lambdai*tNew;
            }
        }
        else
        {
            sf::Vector2f b = beams[baseIndex].node1.position;
            sf::Vector2f& pn = beams[targetIndex-1].node2.position;

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
                        constrainToAngularRange(beams[i-1], beams[i], Direction::Backward);
                    }
                    else
                    {
                        constrainToAngularRange(beams[baseIndex]);
                    }

                    sf::Vector2f& pip1 = beams[i].node2.position;
                    sf::Vector2f& pi   = beams[i].node1.position;

                    float ri = sqrtf(Math::square(pip1 - pi));
                    float lambdai = beams[i].distance/ri;

                    pi = (1-lambdai)*pip1 + lambdai*pi;
                }

                beams[baseIndex].node1.position = b;

                for(int i=baseIndex; i<targetIndex; ++i)
                {

                    if(i>baseIndex)
                    {
                        constrainToAngularRange(beams[i-1], beams[i], Direction::Forward);
                    }
                    else
                    {
                        constrainToAngularRange(beams[baseIndex]);
                    }
                    sf::Vector2f& pip1 = beams[i].node2.position;
                    sf::Vector2f& pi   = beams[i].node1.position;

                    float ri = sqrtf(Math::square(pip1 - pi));
                    float lambdai = beams[i].distance/ri;

                    pip1 = (1-lambdai)*pi + lambdai*pip1;
                }

                difA = sqrtf(Math::square(pn - t));
                ++noIterations;
            }
        }


        sf::Vector2f newDir = Math::norm(beams[0].node2.position -
                                         beams[0].node1.position);
        beams[0].node1.angle = atan2(-newDir.y, newDir.x);

        for(int i=baseIndex; i<targetIndex; ++i)
        {
            if(i < targetIndex - 1)
            {
                beams[i].node2.angle = SkeletonBeam::getBeamAngle(beams[i], beams[i+1]);
            }
            if(i < beams.size())
            {
                newDir = Math::norm(beams[i].node2.position -
                                    beams[i].node1.position);
                beams[i].node2.orientation = newDir;
            }
        }

    }

    void forwardK(sf::Vector2f const & t,
                  int targetIndex)
    {
        sf::Vector2f resultantVector = {0.0f, 0.0f};
        float resultantAngle = 0.0f;

        assert(!std::isnan(t.x) && !std::isnan(t.y));

        if(targetIndex == 0)
        {
            beams[0].node1.position = t;
        }

        for(int i=targetIndex; i<beams.size(); ++i)
        {

            resultantAngle = 180.0f*beams[i].node1.angle/Math::PI;
            resultantVector = Math::rotate(beams[i].node1.orientation * beams[i].distance,
                                           -resultantAngle);

            assert(!std::isnan(resultantAngle));
            assert(!std::isnan(beams[i].node1.orientation.x));

            beams[i].node2.position = beams[i].node1.position +
                                  resultantVector;

            sf::Vector2f newDir = Math::norm(beams[i].node2.position -
                                      beams[i].node1.position);

            beams[i].node2.orientation = newDir;
        }
    }

    void resetBeams()
    {
        for(int i=1; i<nodes.size(); ++i)
        {
            beams.push_back(SkeletonBeam(nodes[i-1], nodes[i], 'y'));

            sf::Vector2f direction = (nodes[i].position - nodes[i-1].position)/beams[i-1].distance;
            nodes[i-1].orientation = direction;
        }
    }

public:
    Skeleton2DChain() {}
    Skeleton2DChain(std::vector<SkeletonNode > _nodes) : nodes{_nodes}
    {
        resetBeams();
    }

    void draw(sf::RenderWindow& window)
    {
        float radius = 5.0f;
        sf::CircleShape shape(radius);
        shape.setOrigin({radius, radius});
        sf::VertexArray line(sf::LineStrip, 2);
        line[0].color = sf::Color::White;
        line[1].color = sf::Color::White;


        for(int i=0; i<beams.size(); ++i)
        {
            line[0].position = beams[i].node1.position;
            line[1].position = beams[i].node2.position;
            window.draw(line);

            shape.setPosition(beams[i].node2.position);
            window.draw(shape);
        }
    }

    void setTarget(sf::Vector2f const & t, int targetIndex = -1)
    {
        if(targetIndex < 0 || targetIndex >= beams.size())
        {
            targetIndex = beams.size();
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

    void setParentNode(SkeletonNode& parentNode)
    {
        beams.clear();
        beams.push_back(SkeletonBeam(parentNode, nodes[0], 'y'));
        parentNode.orientation = Math::norm(nodes[0].position - parentNode.position);

        resetBeams();

        SkeletonBeam& firstBeam = beams[0];
        SkeletonBeam& secondBeam = beams[1];

        beams[0].node2.angle = SkeletonBeam::getBeamAngle(firstBeam, secondBeam);
    }

    SkeletonNode& getNode(int index)
    {
        if(index < 0) index = beams.size()+index;

        if(index % 2 == 0)
            return beams[index].node1;

        return beams[index].node2;
    }

    SkeletonBeam& getBeam(int index)
    {
        if(index < 0) index = beams.size()+index;

        return beams[index];
    }

    void setBaseNodeAngle(float angle)
    {
        baseNodeAngle = angle;
    }

    float getBaseNodeAngle()
    {
        return baseNodeAngle;
    }
};


class Skeleton2D
{
    sf::Vector2f basePosition = {0.0f, 0.0f};
    std::map<std::string, Skeleton2DChain > chains;
    std::vector<std::pair<std::string, std::string> > parentTo;

    const int PAIR_NOT_FOUND = -1;

    int getParentChildPair(std::string const & parent,
                           std::string const & child)
    {
        for(int i=0; i<parentTo.size(); ++i)
        {
            if(parentTo[i].first == parent && parentTo[i].second == child)
                return i;
        }

        return PAIR_NOT_FOUND;
    }

    const std::string NULL_NAME = "";

    std::string getParent(std::string const & childName)
    {
        for(int i=0; i<parentTo.size(); ++i)
            if(parentTo[i].second == childName)
                return parentTo[i].first;

        return NULL_NAME;
    }

    void linkParentToChild(std::string const & parent,
                           std::string const & child,
                           int linkIndex = -1)
    {
        if(chains.find(parent) != chains.end() &&
           chains.find(child) != chains.end())
        {
            parentTo.push_back({parent, child});

            chains[child].setParentNode(chains[parent].getNode(linkIndex));
            updateBaseNodeAngle(child);
        }
    }

    void updateBaseNodeAngle(std::string const & childName)
    {
        std::string parentName = getParent(childName);
        if(parentName != NULL_NAME)
        {
            SkeletonBeam& childBeam  = chains[childName].getBeam(0);
            SkeletonBeam& parentBeam = chains[parentName].getBeam(-1);

            float angle = SkeletonBeam::getBeamAngle(parentBeam, childBeam);
            assert(!std::isnan(angle));
            chains[childName].setBaseNodeAngle(angle);
            parentBeam.node2.angle = angle;
            childBeam.node1.angle = angle;
        }
    }

public:
    Skeleton2D(sf::Vector2f const & basePos = {0.0f, 0.0f}) : basePosition{basePos}
    {}

    void addChain(std::string const & name,
                  Skeleton2DChain chain,
                  std::string const & parentName,
                  int linkIndex = -1)
    {
        chains.insert({name, chain});
        linkParentToChild(parentName, name, linkIndex);
    }

    void setTarget(sf::Vector2f const & target,
                   std::string const & chainName,
                   int chainNode)
    {
        if(getParent(chainName) != NULL_NAME)
        {
            SkeletonBeam& childBeam  = chains[chainName].getBeam(0);
            childBeam.node1.angle = chains[chainName].getBaseNodeAngle();
        }

        chains[chainName].setTarget(target, chainNode);

        updateBaseNodeAngle(chainName);

        //recursively update targets to children
        for(int i=0; i<parentTo.size(); ++i)
        {
            if(parentTo[i].first == chainName)
            {
                sf::Vector2f lastNodePos = chains[chainName].getNode(-1).position;
                setTarget(lastNodePos, parentTo[i].second, 0);
            }
        }
    }

    void draw(sf::RenderWindow& window)
    {
        for(auto it = chains.begin(); it != chains.end(); ++it)
            it->second.draw(window);
    }
};


int main()
{
    sf::RenderWindow window;
    sf::VideoMode mode(800, 800);
    window.create(mode, "test");

    std::vector<SkeletonNode > nodes1 = {
        SkeletonNode{{0.0f, 0.0f},    0, -Math::PI, Math::PI, ""},
        SkeletonNode{{50.0f,  0.0f},     0, -Math::PI, Math::PI, ""},
        SkeletonNode{{100.0f, 0.0f},   0, -Math::PI, Math::PI, ""},
    };
    std::vector<SkeletonNode > nodes2 = {
        SkeletonNode{{150.0f, 0.0f},   0, -Math::PI, Math::PI, ""},
        SkeletonNode{{200.0f, 0.0f},   0, -Math::PI, Math::PI, ""},
        SkeletonNode{{250.0f, 0.0f},   0, -Math::PI, Math::PI, ""},
        SkeletonNode{{300.0f, 0.0f},   0, -Math::PI, Math::PI, ""},
    };
    std::vector<SkeletonNode > nodes3 = {
        SkeletonNode{{150.0f, 20.0f},   0, -Math::PI, Math::PI, ""},
        SkeletonNode{{200.0f, 20.0f},   0, -Math::PI, Math::PI, ""},
        SkeletonNode{{250.0f, 20.0f},   0, -Math::PI, Math::PI, ""},
        SkeletonNode{{300.0f, 20.0f},   0, -Math::PI, Math::PI, ""},
    };

    Skeleton2DChain skeleton1(nodes1);
    Skeleton2DChain skeleton2(nodes2);
    Skeleton2DChain skeleton3(nodes3);
    Skeleton2DChain skeleton4(nodes3);

    Skeleton2D skeleton({300.0f, 0.0f});
    skeleton.addChain("first", skeleton1, "");
    skeleton.addChain("second", skeleton2, "first");
    skeleton.addChain("third", skeleton3, "first");
    //skeleton.addChain("fourth", skeleton4, "third");
    skeleton.setTarget({400.0f, 400.0f}, "first", 0);

    int selector = 2;
    std::string sstring = "first";

    sf::Clock clock;
    clock.restart();

    while(window.isOpen())
    {
        sf::Event currEvent;

        while(window.pollEvent(currEvent))
        {
            switch(currEvent.type)
            {
                case(sf::Event::Closed):
                    window.close();
                    break;
                default:
                    break;
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) selector = 0;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) selector = 1;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) selector = 2;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) selector = 3;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num9)) selector = -1;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::F)) sstring = "first";
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) sstring = "second";
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::T)) sstring = "third";


        if(sf::Mouse::isButtonPressed(sf::Mouse::Middle))
        {
            skeleton.setTarget(window.mapPixelToCoords(sf::Mouse::getPosition(window)),
                               sstring, selector);

        }

        float time = clock.getElapsedTime().asSeconds()/1.0f;
        skeleton.setTarget({300.0f, 200.0f+50.0f*sin(time)}, "second", -1);
        skeleton.setTarget({300.0f, 300.0f-50.0f*sin(time)}, "third", -1);

        window.clear(sf::Color::Black);

        skeleton.draw(window);

        window.display();
    }
}
