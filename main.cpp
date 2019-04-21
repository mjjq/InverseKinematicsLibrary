#include <SFML/Graphics.hpp>
#include "Skeleton.h"
#include "JSONSkeletonReader.h"

int main()
{
    sf::RenderWindow window;
    sf::VideoMode mode(800, 800);
    window.create(mode, "test");

    std::vector<SkeletonNode > nodes1 = {
        SkeletonNode{{0.0f, 0.0f},    0, -Math::PI/2.0f, Math::PI, ""},
        //SkeletonNode{{50.0f,  0.0f},     0, -Math::PI, Math::PI, ""},
        //SkeletonNode{{100.0f, 0.0f},   0, -Math::PI/2.0f, Math::PI/2.0f, ""},
    };
    std::vector<SkeletonNode > nodes2 = {
        SkeletonNode{{150.0f, 100.0f},   0, -Math::PI, Math::PI, ""},
        //SkeletonNode{{200.0f, 0.0f},   0, -Math::PI, Math::PI, ""},
        //SkeletonNode{{250.0f, 0.0f},   0, -Math::PI, Math::PI, ""},
        //SkeletonNode{{300.0f, 0.0f},   0, -Math::PI, Math::PI, ""},
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
    skeleton.addChain("root", skeleton1, "");
    skeleton.addChain("second", skeleton2, "root");
    //skeleton.addChain("third", skeleton3, "first");
    //skeleton.addChain("fourth", skeleton4, "third");

    //Skeleton2D skeletonJ = JSONSkeletonReader::readFromFile("example2.json");
    skeleton.setTarget({400.0f, 400.0f}, "root", 0);

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
        //skeleton.setTarget({300.0f, 200.0f+50.0f*sin(time)}, "second", -1);
        //skeleton.setTarget({300.0f, 300.0f-50.0f*sin(time)}, "third", -1);

        window.clear(sf::Color::Black);

        skeleton.draw(window);

        window.display();
    }
}
