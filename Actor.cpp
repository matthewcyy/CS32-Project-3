#include "Actor.h"
#include "StudentWorld.h"
#include "SoundFX.h"
#include "GameWorld.h"
#include <math.h>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY, int dir, double size, unsigned int depth) : GraphObject(imageID, startX, startY, dir, size, depth)
{
    worldPtr = sp;
    GhostRacerPtr = playerPtr;
    m_isAlive = true;
}

BorderLine::BorderLine(StudentWorld* sp, Actor* playerPtr, bool isWhite, int imageID, double startX, double startY, int dir, double size, unsigned int depth) : Actor(sp, playerPtr, imageID, startX, startY, dir, size, depth)
{
    m_verticalSpeed = -4;
    m_horizontalSpeed = 0;
    m_white = isWhite;
}

bool BorderLine::canMoveDownScreenAndWhite()
{
    if (m_white)
        return true;
    else
        return false;
}

Projectile::Projectile(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY, int dir, double size, unsigned int depth) : Actor(sp, playerPtr, imageID, startX, startY, dir, size, depth)
{
    
}

void Projectile::doSomething()
{
    
}

void BorderLine::doSomething()
{
    double vert_speed = m_verticalSpeed - getPlayer()->getSpeed();
    double horiz_speed = m_horizontalSpeed;
    double new_y = (this->getY() + vert_speed);
    double new_x = (this->getX() + horiz_speed);
    this->moveTo(new_x, new_y);
    if (new_x <= 0 || new_y <= 0 || new_x > VIEW_WIDTH || new_y > VIEW_HEIGHT)
    {
        setAlive(false);
        return;
    }        
}

mortalActor::mortalActor(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY, int dir, double size, unsigned int depth, int health, bool alive) : Actor(sp, playerPtr, imageID, startX, startY, dir, size, depth)
{
    m_alive = true;
    m_health = health;
}

GhostRacer::GhostRacer(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY, int dir, double size, unsigned int depth) : mortalActor(sp, playerPtr, imageID, startX, startY, dir, size, depth, 100, true)
{
    m_numSprays = 10;
    m_forwardSpeed = 0;
}

void GhostRacer::doSomething()
{
    if (!isAlive())
        return;
    if (getX() <= LEFT_EDGE)
    {
        if (getDirection() > 90)
        {
            changeHealth(-10);
            setDirection(82);
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
        }
    }
    else if (getX() >= RIGHT_EDGE)
    {
        if (getDirection() < 90)
        {
            changeHealth(-10);
            setDirection(98);
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
        }
    }
    int ch;
    if (getWorld()->getKey(ch))
    {
        switch(ch)
        {
            /*
            case KEY_PRESS_SPACE:
                if (m_numSprays >= 1)
                {
                    double cur_y = getY();
                    double cur_x = getX();
                    double delta_x = cos(this->getDirection()*M_PI/180)*SPRITE_HEIGHT;
                    double delta_y = sin(this->getDirection()*M_PI/180)*SPRITE_HEIGHT;
                    Actor* newSpray = new Projectile(this->getWorld(), this, IID_HOLY_WATER_PROJECTILE, (this->getX() + delta_x), (this->getY() + delta_y), this->getDirection());
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                    m_numSprays--;
                }
                break;
             */
            case KEY_PRESS_LEFT:
                if (getDirection() < 114)
                    setDirection(getDirection() + 8);
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() > 66)
                    setDirection(getDirection() - 8);
                break;
            case KEY_PRESS_UP:
                if (m_forwardSpeed < 5)
                    m_forwardSpeed++;
                break;
            case KEY_PRESS_DOWN:
                if (m_forwardSpeed > -1)
                    m_forwardSpeed--;
        }
    }
    double max_shift_per_tick = 4.0;
    double direction = getDirection();
    double delta_x = cos(direction*M_PI/180)*max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}


