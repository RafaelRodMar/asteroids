class Game{
public:
    int screenwidth = -1;
    int screenheight = -1;
    sf::RenderWindow app;

    enum game_states {MENU, GAME, END_GAME};
    int state = MENU;
    std::vector<int> vhiscores;
    int lives = -1, score = -1;

    //keyboard handling
    // The keyboard's state in the current- and the previous frame
    bool CurrentKeyState[sf::Keyboard::KeyCount];
    bool PreviousKeyState[sf::Keyboard::KeyCount];

    //sound buffers
    sf::SoundBuffer GOBuffer;
    sf::SoundBuffer ExpBuffer;
    sf::SoundBuffer ShipExpBuffer;
    sf::SoundBuffer LaserBuffer;
    //sound play
    sf::Sound GameOverSound;
    sf::Sound Explosion;
    sf::Sound ShipExplosion;
    sf::Sound Laser;

    //fonts
    sf::Font font;
    sf::Text showScore;

    sf::Texture t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14;

    //sprites
    sf::Sprite background,splash,menu,gameover;

    //create animations
    Animation sExplosion;
    Animation sRock;
    Animation sRock_small;
    Animation sBullet;
    Animation sPlayer;
    Animation sPlayer_go;
    Animation sExplosion_ship;

    sf::Sprite backgr[5];
    int backgrindex = 0;

    //random numbers
    std::default_random_engine random_engine;

    //player, asteroids and bullets
    std::list<Entity*> entities;

    player *p;

    void init(int pscrw, int pscrh, int pposx, int pposy, std::string pname);
    void input();
    void update(sf::Time delta);
    void draw();

    //keyboard functions
    bool KeyPressed(sf::Keyboard::Key Key)
        { return (CurrentKeyState[Key] && !PreviousKeyState[Key]); }

    bool KeyReleased(sf::Keyboard::Key Key)
        { return (!CurrentKeyState[Key] && PreviousKeyState[Key]); }

    bool KeyHeld(sf::Keyboard::Key Key)
        { return CurrentKeyState[Key]; }
};

void Game::init(int pscrw, int pscrh, int pposx, int pposy, std::string pname)
{
    screenwidth = pscrw;
    screenheight = pscrh;
    app.create(sf::VideoMode(pscrw,pscrh),pname);
    app.setPosition(sf::Vector2i(pposx,pposy));

    t1.loadFromFile("assets/images/spaceship.png");
    t2.loadFromFile("assets/images/background-0.jpg");
    t3.loadFromFile("assets/images/type_C.png");
    t4.loadFromFile("assets/images/rock.png");
    t5.loadFromFile("assets/images/fire_blue.png");
    t6.loadFromFile("assets/images/rock_small.png");
    t7.loadFromFile("assets/images/type_B.png");
    t8.loadFromFile("assets/images/splash.png");
    t9.loadFromFile("assets/images/menu.png");
    t10.loadFromFile("assets/images/gameover.png");
    t11.loadFromFile("assets/images/background-1.png");
    t12.loadFromFile("assets/images/background-2.png");
    t13.loadFromFile("assets/images/background-3.png");
    t14.loadFromFile("assets/images/background-4.png");

    t1.setSmooth(true);
    t2.setSmooth(true);
    t11.setSmooth(true);
    t12.setSmooth(true);
    t13.setSmooth(true);
    t14.setSmooth(true);

    //assign textures to sprites
    background.setTexture(t2);
    splash.setTexture(t8);
    menu.setTexture(t9);
    gameover.setTexture(t10);
    backgr[0].setTexture(t2);
    backgr[1].setTexture(t11);
    backgr[2].setTexture(t12);
    backgr[3].setTexture(t13);
    backgr[4].setTexture(t14);

    // Load the text font
    font.loadFromFile("assets/fonts/sansation.ttf");

    // Load the sounds used in the game
    GOBuffer.loadFromFile("assets/sounds/gameover.wav");
    GameOverSound.setBuffer(GOBuffer);
    ShipExpBuffer.loadFromFile("assets/sounds/explosion+6.wav");
    ShipExplosion.setBuffer(ShipExpBuffer);
    ExpBuffer.loadFromFile("assets/sounds/explosion+3.wav");
    Explosion.setBuffer(ExpBuffer);
    LaserBuffer.loadFromFile("assets/sounds/laserblasts.wav");
    Laser.setBuffer(LaserBuffer);

    //create animations
    sExplosion.init(t3, 0,0,171,171, 48, 0.5);
    sRock.init(t4, 0,0,64,64, 16, 0.2);
    sRock_small.init(t6, 0,0,64,64, 16, 0.2);
    sBullet.init(t5, 0,0,32,64, 16, 0.8);
    sPlayer.init(t1, 40,0,40,40, 1, 0);
    sPlayer_go.init(t1, 40,40,40,40, 1, 0);
    sExplosion_ship.init(t7, 0,0,128,128, 64, 0.5);

    //init random numbers
    random_engine.seed(time(0));

    //keyboard buffers initialization
    memset(CurrentKeyState,     false, sizeof(CurrentKeyState));
    memset(PreviousKeyState,    false, sizeof(PreviousKeyState));

    p = new player();
    p->settings(sPlayer,200,200,0,20);
    p->shield = true;
    p->shieldtime = 100;
    entities.push_back(p);
}

void Game::input()
{
    switch(state)
        {
        case MENU:
            {
                sf::Event event;
                while (app.pollEvent(event))
                {
                    if ((event.type == sf::Event::Closed) ||
                        ((event.type == sf::Event::KeyPressed)
                         && (event.key.code == sf::Keyboard::Escape)))
                        app.close();

                    // S key pressed: change state to GAME
                    if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::S))
                    {
                        state=GAME;
                        std::uniform_int_distribution<> randomNum(0,4);
                        backgrindex = randomNum(random_engine);
                        lives = 3;
                        score = 0;
                        p->shield = true;
                        p->shieldtime = 3000;
                    }
                }
                break;
            }
        case GAME:
            {
                sf::Event event;
                while (app.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        app.close();
                }

                // Save the state of each keyboard key (must be done before any Key* function is executed)
                for(unsigned int i = 0; i < sf::Keyboard::KeyCount; ++i)
                {
                    // Save the keyboard's state from the previous frame
                    PreviousKeyState[i] = CurrentKeyState[i];

                    // And save the keyboard's state in the current frame
                    CurrentKeyState[i] = sf::Keyboard::isKeyPressed((sf::Keyboard::Key)i);
                }

                if(KeyPressed(sf::Keyboard::Escape))
                    app.close();

                // Space is the fire key
                if (KeyPressed(sf::Keyboard::Space) && p->shield == false)
                {
                    bullet *b = new bullet();
                    b->settings(sBullet,p->x,p->y,p->angle,10);
                    entities.push_back(b);
                    Laser.play();
                }

                if (KeyHeld(sf::Keyboard::Z)) p->shield = true;
                if(p->shieldtime > 0)
                    {p->shieldtime--;}
                else
                {
                    if (!KeyHeld(sf::Keyboard::Z)) p->shield = false;
                }
                if (KeyHeld(sf::Keyboard::Right)) p->angle+=0.03;
                if (KeyHeld(sf::Keyboard::Left))  p->angle-=0.03;
                if (KeyHeld(sf::Keyboard::Up))
                    p->thrust=true;
                else
                    p->thrust=false;
                break;
            }
        case END_GAME:
            {
                sf::Event event;
                while (app.pollEvent(event))
                {
                    if ((event.type == sf::Event::Closed) ||
                        ((event.type == sf::Event::KeyPressed)
                         && (event.key.code == sf::Keyboard::Escape)))
                        app.close();

                    // Any key pressed: change state to MENU
                    if (event.type == sf::Event::KeyPressed)
                    {
                        state=MENU;
                    }
                }
                break;
            }
        default:
            break;
        }
}

void Game::update(sf::Time delta)
{
    //Game_cycle();
    if(state==GAME)
    {
     for(auto a:entities)
     {
        for(auto b:entities)
        {
          if (a->name=="asteroid" && b->name=="bullet")
           if ( isCollide(a,b) )
            {
                a->life=false;
                b->life=false;

                //explosion
                Entity *e = new Entity();
                e->settings(sExplosion,a->x,a->y);
                e->name="explosion";
                entities.push_back(e);
                Explosion.play();
                score += 10;

                //create two little ones
                for(int i=0;i<2;i++)
                {
                 if (a->R==15) continue;
                 Entity *e = new asteroid();
                 std::uniform_int_distribution<> randomNum(0,360);
                 e->settings(sRock_small,a->x,a->y,randomNum(random_engine),15);
                 entities.push_back(e);
                }

            }

          if (a->name=="player" && b->name=="asteroid")
           if ( isCollide(a,b) && a->shield==false)
            {
                b->life=false;

                //ship explosion
                Entity *e = new Entity();
                e->settings(sExplosion_ship,a->x,a->y);
                e->name="explosion";
                entities.push_back(e);
                ShipExplosion.play();
                lives--;
                if(lives<=0)
                {
                    UpdateHiScores(vhiscores, score);
                    GameOverSound.play();
                    state=END_GAME;
                }

                //relocate the ship
                p->settings(sPlayer,screenwidth/2,screenheight/2,0,20);
                p->dx=0; p->dy=0;
                p->shield = true;
                p->shieldtime = 3000;
            }
        }
     }

        if (p->thrust)  p->anim = sPlayer_go;
        else   p->anim = sPlayer;


        for(auto e:entities)
         if (e->name=="explosion")
          if (e->anim.isEnd()) e->life=0;

        int numasteroids = 0;
        for(auto e:entities)
            if(e->name=="asteroid") numasteroids++;

        if ( numasteroids < 20 )
         {
           asteroid *a = new asteroid();
           std::uniform_int_distribution<> randomNum(0,360);
           std::uniform_int_distribution<> randomHeight(0,screenheight);
           a->settings(sRock, 0,randomHeight(random_engine), randomNum(random_engine), 25);
           entities.push_back(a);
         }

        for(auto i=entities.begin();i!=entities.end();)
        {
          Entity *e = *i;

          e->update(delta);
          e->anim.update();

          if (e->life==false) {i=entities.erase(i); delete e;}
          else i++;
        }
    }
}

void Game::draw()
{
    //Game_paint();
    //////draw//////
    app.clear();
    app.draw(backgr[backgrindex]);

    switch(state)
    {
    case MENU:
        {
        app.draw(menu);

        for(auto i:entities)
            i->draw(app);

        //Show hi scores
        std::string histr="HiScores\n";
        for(int i=0;i<5;i++)
        {
            histr = histr + "    " + std::to_string(vhiscores[i]) + "\n";
        }
        Text(app,histr,580.f,350.f,sf::Color::White,20,font);
        }
        break;
    case GAME:
        {
        for(auto i:entities)
            i->draw(app);

        // Draw the score
        std::string sc = "Lives: " + std::to_string(lives) + "   Score: " + std::to_string(score);
        Text(app,sc,450.f,0.f,sf::Color::White,20,font);
        }
        break;
    case END_GAME:
        {
        app.draw(gameover);

        for(auto i:entities)
            i->draw(app);
        }
        break;
    default:
        break;
    }
    app.display();
}
