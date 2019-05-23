class Entity
{
    public:
        float x,y,dx,dy,R,angle;
        bool life;
        bool shield;
        int shieldtime;
        std::string name;
        Animation anim;
        int scrw=1200, scrh=800;
        float DEGTORAD = 0.017453f; //pi/180

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

    virtual void update(sf::Time dt){};

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
        dx = rnd.getRndInt(-40,40);
        dy = rnd.getRndInt(-40,40);
        name="asteroid";
    }

    void  update(sf::Time dt)
    {
        x+=dx * dt.asSeconds();
        y+=dy * dt.asSeconds();

        if (x>scrw) x=0;
        if (x<0) x=scrw;
        if (y>scrh) y=0;
        if (y<0) y=scrh;
    }

};

class bullet: public Entity
{
public:
    bullet()
    {
        name="bullet";
    }

void  update(sf::Time dt)
    {
        dx=cos(angle*DEGTORAD)*600;
        dy=sin(angle*DEGTORAD)*600;
        // angle+=rand()%6-3;
        x+=dx * dt.asSeconds();
        y+=dy * dt.asSeconds();

        if (x>scrw || x<0 || y>scrh || y<0) life=0;
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

   void update(sf::Time dt)
   {
     if (thrust)
      { dx+=cos(angle*DEGTORAD)*200;
        dy+=sin(angle*DEGTORAD)*200;}
     else
      { dx*=0.99;   //reduce thrust
        dy*=0.99; }

    int maxSpeed=200;
    float speed = sqrt(dx*dx+dy*dy);
    if (speed>maxSpeed)
     { dx *= maxSpeed/speed;
       dy *= maxSpeed/speed; }

    x+=dx * dt.asSeconds();
    y+=dy * dt.asSeconds();

    if (x>scrw) x=0;
    if (x<0) x=scrw;
    if (y>scrh) y=0;
    if (y<0) y=scrh;
   }

};


bool isCollide(Entity *a,Entity *b)
{
  return (b->x - a->x)*(b->x - a->x)+
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
}
