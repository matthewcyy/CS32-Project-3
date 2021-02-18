#include "Actor.h"
#include "StudentWorld.h"
#include "SoundFX.h"
#include "GameWorld.h"
#include <math.h>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(StudentWorld* sp, Actor* playerPtr, int health, int vertSpeed, int horizSpeed, int imageID, double startX, double startY, int dir, double size, unsigned int depth) : GraphObject(imageID, startX, startY, dir, size, depth)
{
    worldPtr = sp;
    GhostRacerPtr = playerPtr;
    m_health = health;
    m_isAlive = true;
    m_verticalSpeed = vertSpeed;
    m_horizontalSpeed = horizSpeed;
}

bool Actor::isOverlap(Actor* object, Actor* player)
{
    double object_y = object->getY();
    double object_x = object->getX();
    double player_y = player->getY();
    double player_x = player->getX();
    double delta_x = abs(object_x - player_x);
    double delta_y = abs(object_y - player_y);
    double radiusSum = (object->getRadius() + player->getRadius());
    if (delta_x < radiusSum*0.25 && delta_y < radiusSum*0.6)
        return true;
    else
        return false;
}

void Actor::commonMove() // Setting up a general movement function since all actors move like this except for goodies and GhostRacer
{
    double vert_speed = getVertSpeed() - getPlayer()->getVertSpeed();
    double horiz_speed = getHorizSpeed();
    double new_y = getY() + vert_speed;
    double new_x = getX() + horiz_speed;
    this->moveTo(new_x, new_y);
    if (new_x <= 0 || new_y <= 0 || new_x > VIEW_WIDTH || new_y > VIEW_HEIGHT)
    {
        setAlive(false);
        return;
    }
}

BorderLine::BorderLine(StudentWorld* sp, Actor* playerPtr, bool isWhite, int imageID, double startX, double startY) : Actor(sp, playerPtr, 0, -4, 0, imageID, startX, startY, 0, 2.0, 1)
{
    m_white = isWhite;
}

void BorderLine::doSomething()
{
    commonMove();
}

Projectile::Projectile(StudentWorld* sp, Actor* playerPtr, double startX, double startY, int dir, double size, unsigned int depth) : Actor(sp, playerPtr, 0, 0, 0, IID_HOLY_WATER_PROJECTILE, startX, startY, dir, size, depth)
{
    
}

Pedestrian::Pedestrian(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY, double size) : Actor(sp, playerPtr, 2, -4, 0, imageID, startX, startY, 0, size, 0)
{
    m_movementPlanDist = 0;
}

HumanPedestrian::HumanPedestrian(StudentWorld* sp, Actor* playerPtr, double startX, double startY) : Pedestrian(sp, playerPtr, IID_HUMAN_PED, startX, startY, 2)
{
    
}

void HumanPedestrian::doSomething()
{
    if (!isAlive())
        return;
    if (isOverlap(this, getPlayer()))
    {
        getWorld()->setHitHuman(true);
        return;
    }
    commonMove();
    setMovePlan(getMovePlan() - 1);
    if (getMovePlan() > 0)
        return;
    else
    {
        int newHorizSpeed = 0;
        while (newHorizSpeed == 0)
            newHorizSpeed = randInt(-3,3);
        setHorizSpeed(newHorizSpeed);
        setMovePlan(randInt(4, 32));
        if (getHorizSpeed() < 0)
            setDirection(180);
        else
            setDirection(0);
    }
}

ZombiePedestrian::ZombiePedestrian(StudentWorld* sp, Actor* playerPtr, double startX, double startY) : Pedestrian(sp, playerPtr, IID_ZOMBIE_PED, startX, startY, 3)
{
    
}

void ZombiePedestrian::doSomething()
{
    
}

GhostRacer::GhostRacer(StudentWorld* sp, Actor* playerPtr, double startX, double startY, int dir, double size, unsigned int depth) : Actor(sp, playerPtr, 100, 0, 0, IID_GHOST_RACER, startX, startY, dir, size, depth)
{
    m_numSprays = 10;
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
                    Actor* newSpray = new Projectile(this->getWorld(), this, (this->getX() + delta_x), (this->getY() + delta_y), this->getDirection());
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
                if (getVertSpeed() < 5)
                    setVertSpeed(getVertSpeed() + 1);
                break;
            case KEY_PRESS_DOWN:
                if (getVertSpeed() > -1)
                    setVertSpeed(getVertSpeed() - 1);
        }
    }
    double max_shift_per_tick = 4.0;
    double direction = getDirection();
    double delta_x = cos(direction*M_PI/180)*max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

void Projectile::doSomething()
{
    if (!(this->isAlive()))
        return;
}
