#include <SFML/Graphics.hpp>
#include <sfml/audio.hpp>
#include <vector>
#include <list>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>

//variables globales
const int screenwidth = 1200;
const int screenheight = 800;

enum game_states {MENU, GAME, END_GAME};
int state = -1;
std::vector<int> vhiscores;
int lives, score;

float DEGTORAD = 0.017453f; //pi/180

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

class Animation
{
public:
	float Frame, speed;
	sf::Sprite sprite;
    std::vector<sf::IntRect> frames;

	Animation(){}

    Animation (sf::Texture &t, int x, int y, int w, int h, int count, float Speed)
	{
	    Frame = 0;
        speed = Speed;

		for (int i=0;i<count;i++)
         frames.push_back( sf::IntRect(x+i*w, y, w, h)  );

		sprite.setTexture(t);
		sprite.setOrigin(w/2,h/2);
        sprite.setTextureRect(frames[0]);
	}


	void update()
	{
    	Frame += speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n>0) sprite.setTextureRect( frames[int(Frame)] );
	}

	bool isEnd()
	{
	  return Frame+speed>=frames.size();
	}
};


class Entity
{
    public:
        float x,y,dx,dy,R,angle;
        bool life;
        bool shield;
        int shieldtime;
        std::string name;
        Animation anim;

    Entity()
    {
        life=1;
        shield=false;
    }

    void settings(Animation &a,int X,int Y,float Angle=0,int radius=1)
    {
        anim = a;
        x=X; y=Y;
        angle = Angle;
        R = radius;
    }

    virtual void update(){};

    void draw(sf::RenderWindow &app)
    {
      anim.sprite.setPosition(x,y);
      anim.sprite.setRotation(angle+90);
      app.draw(anim.sprite);

      if(shield)
      {
          sf::CircleShape circle(R);
          circle.setFillColor(sf::Color(255,0,0,170));
          circle.setPosition(x,y);
          circle.setOrigin(R,R);
          app.draw(circle);
      }
    }

    virtual ~Entity(){};
};

class asteroid: public Entity
{
public:
    asteroid()
    {
        dx=rand()%8-4;
        dy=rand()%8-4;
        name="asteroid";
    }

    void  update()
    {
        x+=dx;
        y+=dy;

        if (x>screenwidth) x=0;
        if (x<0) x=screenwidth;
        if (y>screenheight) y=0;
        if (y<0) y=screenheight;
    }

};

class bullet: public Entity
{
public:
    bullet()
    {
        name="bullet";
    }

void  update()
    {
        dx=cos(angle*DEGTORAD)*6;
        dy=sin(angle*DEGTORAD)*6;
        // angle+=rand()%6-3;
        x+=dx;
        y+=dy;

        if (x>screenwidth || x<0 || y>screenheight || y<0) life=0;
    }

};


class player: public Entity
{
public:
   bool thrust;

   player()
   {
     name="player";
   }

   void update()
   {
     if (thrust)
      { dx+=cos(angle*DEGTORAD)*0.2;
        dy+=sin(angle*DEGTORAD)*0.2; }
     else
      { dx*=0.99;   //reduce thrust
        dy*=0.99; }

    int maxSpeed=15;
    float speed = sqrt(dx*dx+dy*dy);
    if (speed>maxSpeed)
     { dx *= maxSpeed/speed;
       dy *= maxSpeed/speed; }

    x+=dx;
    y+=dy;

    if (x>screenwidth) x=0;
    if (x<0) x=screenwidth;
    if (y>screenheight) y=0;
    if (y<0) y=screenheight;
   }

};


bool isCollide(Entity *a,Entity *b)
{
  return (b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
}

void HandleKeys()
{

}

void Game_cycle()
{

}

void Game_paint()
{

}

void UpdateHiScores(int newscore)
{
    //new score to the end
    vhiscores.push_back(newscore);
    //sort
    sort(vhiscores.rbegin(), vhiscores.rend());
    //remove the last
    vhiscores.pop_back();
}

void ReadHiScores()
{
    std::ifstream in("hiscores.dat");
    if(in.good())
    {
        std::string str;
        getline(in,str);
        std::stringstream ss(str);
        int n;
        for(int i=0;i<5;i++)
        {
            ss >> n;
            vhiscores.push_back(n);
        }
        in.close();
    }
    else
    {
        //if file does not exist fill with 5 scores
        for(int i=0;i<5;i++)
        {
            vhiscores.push_back(0);
        }
    }
}

void WriteHiScores()
{
    std::ofstream out("hiscores.dat");
    for(int i=0;i<5;i++)
    {
        out << vhiscores[i] << " ";
    }
    out.close();
}

void Text(sf::RenderWindow &app, std::string pstr, float px, float py,sf::Color pcolor, int psize, sf::Font pfont)
{
    sf::Text str;
    str.setString(pstr);
    str.setFont(pfont);
    str.setCharacterSize(psize);
    str.setPosition(px, py);
    str.setFillColor(pcolor);
    app.draw(str);
}


int main()
{
    srand(time(0));

    // Create the main window
    sf::RenderWindow app(sf::VideoMode(screenwidth, screenheight), "Asteroids");
    app.setPosition(sf::Vector2i(200,0));

    sf::Texture t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14;
    t1.loadFromFile("images/spaceship.png");
    t2.loadFromFile("images/background-0.jpg");
    t3.loadFromFile("images/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_blue.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/type_B.png");
    t8.loadFromFile("images/splash.png");
    t9.loadFromFile("images/menu.png");
    t10.loadFromFile("images/gameover.png");
    t11.loadFromFile("images/background-1.png");
    t12.loadFromFile("images/background-2.png");
    t13.loadFromFile("images/background-3.png");
    t14.loadFromFile("images/background-4.png");

    t1.setSmooth(true);
    t2.setSmooth(true);
    t11.setSmooth(true);
    t12.setSmooth(true);
    t13.setSmooth(true);
    t14.setSmooth(true);

    //assign textures to sprites
    sf::Sprite background(t2);
    sf::Sprite splash(t8);
    sf::Sprite menu(t9);
    sf::Sprite gameover(t10);
    sf::Sprite backgr[5];
    backgr[0].setTexture(t2);
    backgr[1].setTexture(t11);
    backgr[2].setTexture(t12);
    backgr[3].setTexture(t13);
    backgr[4].setTexture(t14);
    int backgrindex = 0;

    //create animations
    Animation sExplosion(t3, 0,0,171,171, 48, 0.5);
    Animation sRock(t4, 0,0,64,64, 16, 0.2);
    Animation sRock_small(t6, 0,0,64,64, 16, 0.2);
    Animation sBullet(t5, 0,0,32,64, 16, 0.8);
    Animation sPlayer(t1, 40,0,40,40, 1, 0);
    Animation sPlayer_go(t1, 40,40,40,40, 1, 0);
    Animation sExplosion_ship(t7, 0,0,128,128, 64, 0.5);

    // Load the sounds used in the game
    GOBuffer.loadFromFile("sounds/gameover.wav");
    GameOverSound.setBuffer(GOBuffer);
    ShipExpBuffer.loadFromFile("sounds/explosion+6.wav");
    ShipExplosion.setBuffer(ShipExpBuffer);
    ExpBuffer.loadFromFile("sounds/explosion+3.wav");
    Explosion.setBuffer(ExpBuffer);
    LaserBuffer.loadFromFile("sounds/laserblasts.wav");
    Laser.setBuffer(LaserBuffer);

    // Load the text font
    font.loadFromFile("sansation.ttf");

    std::list<Entity*> entities;

    for(int i=0;i<15;i++)
    {
      asteroid *a = new asteroid();
      a->settings(sRock, rand()%screenwidth, rand()%screenheight, rand()%360, 25);
      entities.push_back(a);
    }

    player *p = new player();
    p->settings(sPlayer,200,200,0,20);
    p->shield = true;
    p->shieldtime = 100;
    entities.push_back(p);

    ReadHiScores();

    sf::Clock clock;
    const float timePerFrame = 1.0 / 60.0;  //60fps.

    state = MENU;

    while(app.isOpen())
    {
        sf::Time elapsed = clock.getElapsedTime();

        //HandleKeys();
        if(state==MENU)
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
                    backgrindex = rand()%5;
                    lives = 3;
                    score = 0;
                    p->shield = true;
                    p->shieldtime = 100;
                }
            }
        }

        if(state==GAME && elapsed.asSeconds() > timePerFrame)
        {
            sf::Event event;
            while (app.pollEvent(event))
            {
                if ((event.type == sf::Event::Closed) ||
                    ((event.type == sf::Event::KeyPressed)
                     && (event.key.code == sf::Keyboard::Escape)))
                    app.close();

                // Space is the fire key
                if (event.type == sf::Event::KeyPressed)
                    if (event.key.code == sf::Keyboard::Space && p->shield == false)
                    {
                        bullet *b = new bullet();
                        b->settings(sBullet,p->x,p->y,p->angle,10);
                        entities.push_back(b);
                        Laser.play();
                    }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) p->shield = true;
            if(p->shieldtime > 0)
                {p->shieldtime--;}
            else
            {
                if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) p->shield = false;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) p->angle+=3;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  p->angle-=3;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                p->thrust=true;
            else
                p->thrust=false;
        }

        if(state==END_GAME)
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
        }

        //Game_cycle();
        if(state==GAME && elapsed.asSeconds() > timePerFrame)
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
                     e->settings(sRock_small,a->x,a->y,rand()%360,15);
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
                        UpdateHiScores(score);
                        GameOverSound.play();
                        state=END_GAME;
                    }

                    //relocate the ship
                    p->settings(sPlayer,screenwidth/2,screenheight/2,0,20);
                    p->dx=0; p->dy=0;
                    p->shield = true;
                    p->shieldtime = 100;
                }
            }
         }

            if (p->thrust)  p->anim = sPlayer_go;
            else   p->anim = sPlayer;


            for(auto e:entities)
             if (e->name=="explosion")
              if (e->anim.isEnd()) e->life=0;

            if (rand()%150==0)
             {
               asteroid *a = new asteroid();
               a->settings(sRock, 0,rand()%screenheight, rand()%360, 25);
               entities.push_back(a);
             }

            for(auto i=entities.begin();i!=entities.end();)
            {
              Entity *e = *i;

              e->update();
              e->anim.update();

              if (e->life==false) {i=entities.erase(i); delete e;}
              else i++;
            }

            clock.restart();
        }

        //Game_paint();
        //////draw//////
        app.draw(backgr[backgrindex]);
        if(state==MENU)
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

        if(state==GAME)
        {
            for(auto i:entities)
                i->draw(app);

            // Draw the score
            std::string sc = "Lives: " + std::to_string(lives) + "   Score: " + std::to_string(score);
            Text(app,sc,450.f,0.f,sf::Color::White,20,font);
        }

        if(state==END_GAME)
        {
            app.draw(gameover);

            for(auto i:entities)
                i->draw(app);
        }

        app.display();
    }

    WriteHiScores();

    return EXIT_SUCCESS;
}
