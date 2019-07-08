#include <SFML/Graphics.hpp>
#include "Skeleton2D.h"
#include "JSONSkeletonReader.h"

int main()
{
    sf::RenderWindow window;
    sf::VideoMode mode(800, 800);
    window.create(mode, "test");
    sf::View currView;
    currView.setCenter(0.0f, 0.0f);
    window.setView(currView);

    Skeleton2D skeletonJ = JSONSkeletonReader::readFromFile("example3.json");

    int selector = 2;
    std::string sstring = "root";
    std::string animationString = "walk";

    sf::Clock clock;
    clock.restart();

    float time = 0.01f;
    bool paused = false;

    skeletonJ.setAnimation(animationString, AnimationSet::TransitionType::Immediate);

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
                case(sf::Event::KeyPressed) :
                {
                    if(currEvent.key.code == sf::Keyboard::L)
                    {
                        time += 0.001f;
                        skeletonJ.animate(time);
                        std::cout << "time: " << time << "\n\n";
                    }
                    else if(currEvent.key.code == sf::Keyboard::K)
                    {
                        time -= 0.001f;
                        skeletonJ.animate(time);
                        std::cout << "time: " << time << "\n\n";
                    }
                    else if(currEvent.key.code == sf::Keyboard::Space)
                    {
                        paused = !paused;
                    }
                    else if(currEvent.key.code == sf::Keyboard::R)
                    {
                        if(animationString == "walk") animationString = "jump";
                        else if(animationString == "jump") animationString = "walk";

                        skeletonJ.setAnimation(animationString, AnimationSet::TransitionType::Immediate);
                    }
                }
                default:
                    break;
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) selector = 0;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) selector = 1;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) selector = 2;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) selector = 3;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num9)) selector = -1;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::F)) sstring = "root";
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)) sstring = "left limb";
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::U)) sstring = "right limb";
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::T)) sstring = "torso";
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::H)) sstring = "hip";



        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Comma))
        {
            skeletonJ.setRotation(1.0f, sstring, Skeleton2DBone::RelativeTo::Current);
        }

        sf::sleep(sf::milliseconds(16));
        if(!paused)
        {
            skeletonJ.animate(time);
            skeletonJ.setTarget({-200.0f + fmod(150.0f*time, 400.0f), 0.0f}, "root", 0);
        }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Middle))
        {
            skeletonJ.setTarget(window.mapPixelToCoords(sf::Mouse::getPosition(window)),
                               sstring, selector);

        }

        window.clear(sf::Color::Black);

        skeletonJ.draw(window);

        window.display();
    }
}
