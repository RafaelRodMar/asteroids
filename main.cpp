#include <SFML/Graphics.hpp>
#include <sfml/audio.hpp>
#include <vector>
#include <list>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <cstring>
#include <random>

#include "various.h"
#include "animation.h"
#include "entity.h"
#include "game.h"

int main()
{
    //the game class
    Game game;

    //create the main window
    game.init(1200,800,200,0,"Asteroids");

    ReadHiScores(game.vhiscores);

    sf::Clock clock;
    const sf::Time timePerFrame = sf::seconds(1.f/60.f); //60fps
    sf::Time elapsed = sf::Time::Zero;

    while(game.app.isOpen())
    {
        elapsed += clock.restart();

        game.input();

        while( elapsed >= timePerFrame )
        {
            game.update(elapsed);
            elapsed -= timePerFrame;
        }

        game.draw();
    }

    WriteHiScores(game.vhiscores);

    return EXIT_SUCCESS;
}

