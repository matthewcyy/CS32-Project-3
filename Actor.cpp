#include "Actor.h"
#include "StudentWorld.h"
#include "SoundFX.h"
#include "GameWorld.h"
#include <math.h>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(StudentWorld* sp, Actor* playerPtr, int health, double vertSpeed, double horizSpeed, int imageID, double startX, double startY, int dir, double size, unsigned int depth, int numSprays) : GraphObject(imageID, startX, startY, dir, size, depth)
{
    worldPtr = sp;
    GhostRacerPtr = playerPtr;
    m_health = health;
    m_isAlive = true;
    m_waterHit = false;
    m_oilHit = false;
    m_verticalSpeed = vertSpeed;
    m_horizontalSpeed = horizSpeed;
    m_numSprays = numSprays;
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
    if (new_x < 0 || new_y < 0 || new_x > VIEW_WIDTH || new_y > VIEW_HEIGHT)
    {
        setAlive(false);
        return;
    }
}

BorderLine::BorderLine(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY) : Actor(sp, playerPtr, 0, -4, 0, imageID, startX, startY, 0, 2.0, 2)
{
}

void BorderLine::doSomething()
{
    commonMove();
}

Projectile::Projectile(StudentWorld* sp, Actor* playerPtr, double startX, double startY, int dir) : Actor(sp, playerPtr, 0, 0, 0, IID_HOLY_WATER_PROJECTILE, startX, startY, dir, 1.0, 1)
{
    m_countMoves = 0;
}

void Projectile::doSomething()
{
    if (!isAlive())
        return;
    Actor *hitActor = getWorld()->waterOverlap(this);
    if (hitActor != nullptr)
    {
        hitActor->setHitWater(true);
        setAlive(false);
        return;
    }
    moveForward(SPRITE_HEIGHT);
    double projX = getX();
    double projY = getY();
    if (projX < 0 || projY < 0 || projX > VIEW_WIDTH || projY > VIEW_HEIGHT)
    {
        setAlive(false);
        return;
    }
    m_countMoves++;
    if (m_countMoves == 19)
        setAlive(false);
}

ZombieCab::ZombieCab(StudentWorld* sp, Actor* playerPtr, double vertSpeed, double startX, double startY) : Actor(sp, playerPtr, 3, vertSpeed, 0, IID_ZOMBIE_CAB, startX, startY, 90, 4.0, 0)
{
    m_hasDamagedPlayer = false;
    m_planLength = 0;
}

void ZombieCab::doSomething()
{
    if (!isAlive())
        return;
    if (getHitWater())
    {
        changeHealth(-1);
        setHitWater(false);
    }
    if (getHealth() <= 0)
    {
        setAlive(false);
        getWorld()->increaseScore(200);
        getWorld()->playSound(SOUND_VEHICLE_DIE);
        if(randInt(1, 5) == 5)
        {
            int oilSize = randInt(2, 5);
            Actor* newOil = new OilSlick(getWorld(), getPlayer(), getX(), getY(), oilSize); // MAKE OIL SLICK ONCE IMPLEMENTED
            getWorld()->addActorToContainer(newOil);
        }
    }
    double cabX = getX();
    double cabY = getY();
    double playerX = getPlayer()->getX();
    if (isOverlap(this, getPlayer()))
    {
        if (!m_hasDamagedPlayer)
        {
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            getPlayer()->changeHealth(-20);
            if (cabX <= playerX)
            {
                setHorizSpeed(-5);
                setDirection(120 + randInt(0, 19));
            }
            else if (cabX > playerX)
            {
                setHorizSpeed(5);
                setDirection(60 - randInt(0, 19));
            }
            setDamagedPlayer(true);
        }
    }
    commonMove();
    Actor* actorInLane = getWorld()->actorInSameLane(this);
    double actorInLaneY = -1;
    if (actorInLane != nullptr)
        actorInLaneY = actorInLane->getY();
    if (getVertSpeed() > getPlayer()->getVertSpeed() && actorInLane != nullptr && actorInLaneY > cabY)
        if (actorInLaneY - cabY < 96)
        {
            setVertSpeed(getVertSpeed() - 0.5);
            return;
        }
    if (getVertSpeed() <= getPlayer()->getVertSpeed() && actorInLane != nullptr && actorInLaneY < cabY)
        if (cabY - actorInLaneY < 96)
        {
            setVertSpeed(getVertSpeed() + 0.5);
            return;
        }
    setPlanLength(getPlanLength() - 1);
    if (getPlanLength() > 0)
        return;
    else
    {
        setPlanLength(randInt(4, 32));
        setVertSpeed(getVertSpeed() + randInt(-2, 2));
    }
}

Pedestrian::Pedestrian(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY, double size) : Actor(sp, playerPtr, 2, -4, 0, imageID, startX, startY, 0, size, 0)
{
    m_movementPlanDist = 0;
}

void Pedestrian::commonMovePlan()
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
    if (getHitWater())
    {
        setHorizSpeed(-1*getHorizSpeed());
        if (getDirection() == 180)
            setDirection(0);
        else
            setDirection(180);
        setHitWater(false);
    }
    commonMove();
    setMovePlan(getMovePlan() - 1);
    if (getMovePlan() > 0)
        return;
    else
        commonMovePlan();
}

ZombiePedestrian::ZombiePedestrian(StudentWorld* sp, Actor* playerPtr, double startX, double startY) : Pedestrian(sp, playerPtr, IID_ZOMBIE_PED, startX, startY, 3)
{
    m_ticks = 0;
}

void ZombiePedestrian::doSomething()
{
    if (!isAlive())
        return;
    if (isOverlap(this, getPlayer()))
    {
        getPlayer()->changeHealth(-5);
        changeHealth(-2);
    }
    if (getHitWater())
    {
        changeHealth(-1);
        if (getHealth() <= 0)
        {
            if(randInt(1, 5) == 5)
            {
                Actor *newHeal = new HealingGoodie(getWorld(), getPlayer(), getX(), getY());
                getWorld()->addActorToContainer(newHeal);
            }
        }
        setHitWater(false);
    }
    if (getHealth() <= 0)
    {
        setAlive(false);
        getWorld()->increaseScore(150);
        getWorld()->playSound(SOUND_PED_DIE);
    }
    int playerX = getPlayer()->getX();
    int playerY = getPlayer()->getY();
    int zombieX = getX();
    int zombieY = getY();
    if (abs(playerX - zombieX) <= 30 && zombieY > playerY)
    {
        setDirection(270);
        if (zombieX < playerX)
            setHorizSpeed(1);
        else if (zombieX > playerX)
            setHorizSpeed(-1);
        else
            setHorizSpeed(0);
        m_ticks--;
        if (m_ticks <= 0)
        {
            getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
            m_ticks = 20;
        }
    }
    commonMove();
    if (getMovePlan() > 0)
    {
        setMovePlan(getMovePlan() - 1);
        return;
    }
    else
        commonMovePlan();
}

Goodie::Goodie(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY, double size, int dir) : Actor(sp, playerPtr, 1, -4, 0, imageID, startX, startY, dir, size, 2)
{
    
}

bool Goodie::commonGoodieAndOverlap()
{
    commonMove();
    return isOverlap(this, getPlayer());
}

OilSlick::OilSlick(StudentWorld* sp, Actor* playerPtr, double startX, double startY, double size) : Goodie(sp, playerPtr, IID_OIL_SLICK, startX, startY, size)
{
    
}

void OilSlick::doSomething()
{
    if(commonGoodieAndOverlap())
    {
        getWorld()->playSound(SOUND_OIL_SLICK);
        getPlayer()->setHitOilSlick(true);
    }
}

HealingGoodie::HealingGoodie(StudentWorld* sp, Actor* playerPtr, double startX, double startY) : Goodie(sp, playerPtr, IID_HEAL_GOODIE, startX, startY, 1)
{
    
}

void HealingGoodie::doSomething()
{
    if (commonGoodieAndOverlap())
    {
        int playerHealth = getPlayer()->getHealth();
        if (playerHealth > 90)
            getPlayer()->changeHealth(100 - playerHealth);
        else
            getPlayer()->changeHealth(10);
        setAlive(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(250);
    }
    if (getHitWater())
    {
        changeHealth(-1);
        setAlive(false);
    }
}

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sp, Actor* playerPtr, double startX, double startY) : Goodie(sp, playerPtr, IID_HOLY_WATER_GOODIE, startX, startY, 2, 90)
{
    
}

void HolyWaterGoodie::doSomething()
{
    if (commonGoodieAndOverlap())
    {
        getPlayer()->changeSprays(10);
        setAlive(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(50);
    }
    if (getHitWater())
    {
        changeHealth(-1);
        setAlive(false);
    }
}

SoulGoodie::SoulGoodie(StudentWorld* sp, Actor* playerPtr, double startX, double startY) : Goodie(sp, playerPtr, IID_SOUL_GOODIE, startX, startY, 4)
{
    
}

void SoulGoodie::doSomething()
{
    if (commonGoodieAndOverlap())
    {
        getWorld()->decrementNumSoulsNeeded();
        setAlive(false);
        getWorld()->playSound(SOUND_GOT_SOUL);
        getWorld()->increaseScore(100);
    }
    setDirection(getDirection() - 10);
}

GhostRacer::GhostRacer(StudentWorld* sp) : Actor(sp, nullptr, 100, 0, 0, IID_GHOST_RACER, 128, 32, 90, 4.0, 0, 10)
{
    
}

void GhostRacer::doSomething()
{
    if (!isAlive())
        return;
    if (getHealth() <= 0)
    {
        setAlive(false);
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
    if(getHitOil())
    {
        int multiplier = 0;
        while (multiplier == 0)
            multiplier = randInt(-1, 1);
        int randomAngle = multiplier*randInt(5, 20);
        int newAngle = getDirection() + randomAngle;
        if (newAngle > 120)
            newAngle = 120;
        else if (newAngle < 60)
            newAngle = 60;
        setDirection(newAngle);
        setHitOilSlick(false);
    }
    bool hitSide = false;
    if (getX() <= LEFT_EDGE)
    {
        if (getDirection() > 90)
        {
            changeHealth(-10);
            setDirection(82);
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            hitSide = true;
        }
    }
    else if (getX() >= RIGHT_EDGE)
    {
        if (getDirection() < 90)
        {
            changeHealth(-10);
            setDirection(98);
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            hitSide = true;
        }
    }
    int ch;
    if (getWorld()->getKey(ch) && !hitSide)
    {
        switch(ch)
        {
            case KEY_PRESS_SPACE:
                if (getSprays() >= 1)
                {
                    double cur_y = getY();
                    double cur_x = getX();
                    double delta_x = cos(this->getDirection()*M_PI/180)*SPRITE_HEIGHT;
                    double delta_y = sin(this->getDirection()*M_PI/180)*SPRITE_HEIGHT;
                    Actor* newSpray = new Projectile(this->getWorld(), this, (cur_x + delta_x), (cur_y + delta_y), this->getDirection());
                    getWorld()->addActorToContainer(newSpray);
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                    changeSprays(-1);
                }
                break;
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
    hitSide = false;
}
