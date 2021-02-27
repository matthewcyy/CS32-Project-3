#include "Actor.h"
#include "StudentWorld.h"
#include "SoundFX.h"
#include "GameWorld.h"
#include <math.h>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp
Actor::Actor(StudentWorld* sp, GhostRacer* playerPtr, int health, double vertSpeed, double horizSpeed, int imageID, double startX, double startY, int dir, double size, unsigned int depth) : GraphObject(imageID, startX, startY, dir, size, depth)
{
    worldPtr = sp;
    GhostRacerPtr = playerPtr;
    m_health = health;
    m_isAlive = true;
    m_waterHit = false;
    m_verticalSpeed = vertSpeed;
    m_horizontalSpeed = horizSpeed;
}

bool Actor::isOverlap(Actor* object, Actor* player) const
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

void Actor::commonMove() // Setting up a general movement function since all actors move like this, based off of GhostRacer
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

BorderLine::BorderLine(StudentWorld* sp, GhostRacer* playerPtr, int imageID, double startX, double startY) : Actor(sp, playerPtr, 0, -4, 0, imageID, startX, startY, 0, 2.0, 2)
{
}

void BorderLine::doSomething()
{
    commonMove(); // BorderLines just move up the screen according to GhostRacer's speed
}

Projectile::Projectile(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY, int dir) : Actor(sp, playerPtr, 0, 0, 0, IID_HOLY_WATER_PROJECTILE, startX, startY, dir, 1.0, 1)
{
    m_countMoves = 0;
}

void Projectile::doSomething()
{
    if (!isAlive())
        return;
    Actor *hitActor = getWorld()->waterOverlap(this); // Retrieve the actor hit by the projectile
    if (hitActor != nullptr) // If hits such an actor, must dissipate and set itself to not alive
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
    if (m_countMoves == 19) // Used to make sure it travels the right amount of distance before terminating
        setAlive(false);
}

ZombieCab::ZombieCab(StudentWorld* sp, GhostRacer* playerPtr, double vertSpeed, double startX, double startY) : Actor(sp, playerPtr, 3, vertSpeed, 0, IID_ZOMBIE_CAB, startX, startY, 90, 4.0, 0)
{
    m_hasDamagedPlayer = false;
    m_planLength = 0;
}

void ZombieCab::doSomething()
{
    if (!isAlive())
        return;
    if (getHitWater()) // If hit by water, just decrement health
    {
        changeHealth(-1);
        if (getHealth() > 0)
            getWorld()->playSound(SOUND_VEHICLE_HURT);
        setHitWater(false);
    }
    if (getHealth() <= 0) // If health is less than or equal to 0, set it to be dead and increase score
    {
        setAlive(false);
        getWorld()->increaseScore(200);
        getWorld()->playSound(SOUND_VEHICLE_DIE);
        if(randInt(1, 5) == 5) // Has a 20% chance of creating an oil slick where zombieCab was
        {
            int oilSize = randInt(2, 5);
            Actor* newOil = new OilSlick(getWorld(), getPlayer(), getX(), getY(), oilSize);
            getWorld()->addActorToContainer(newOil);
        }
        return;
    }
    double cabX = getX();
    double cabY = getY();
    double playerX = getPlayer()->getX();
    if (isOverlap(this, getPlayer())) // If overlaps with GhostRacer, then damage GhostRacer and adjust direction based off of GhostRacer's relative position
    {
        if (!m_hasDamagedPlayer) // Only do the following if hasn't damaged player yet
        {
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            getPlayer()->changeHealth(-20);
            if (cabX <= playerX) // Turn based off of player's X coordinate
            {
                setHorizSpeed(-5);
                setDirection(120 + randInt(0, 19));
            }
            else
            {
                setHorizSpeed(5);
                setDirection(60 - randInt(0, 19));
            }
            setDamagedPlayer(true);
        }
    }
    commonMove();
    Actor* frontActorInLane = getWorld()->actorInSameLaneInFront(this); // Find the closest actor in front of zombieCab
    Actor* behindActorInLane = getWorld()->actorInSameLaneBehind(this); // Find the closest actor behind zombieCab
    double frontActorInLaneY = -1;
    double behindActorInLaneY = -1;

    if (frontActorInLane != nullptr) // Make sure not accessing a nullpointer
        frontActorInLaneY = frontActorInLane->getY();
    if (behindActorInLane != nullptr)
        behindActorInLaneY = behindActorInLane->getY();
    if (getVertSpeed() > getPlayer()->getVertSpeed() && frontActorInLane != nullptr && frontActorInLaneY - cabY < 96) // If there's a collision avoidance worthy actor within 96 pixels ahead of the cab, then slow down
    {
        setVertSpeed(getVertSpeed() - 0.5);
        return;
    }
    if (getVertSpeed() <= getPlayer()->getVertSpeed() && behindActorInLane != nullptr && cabY - behindActorInLaneY < 96 && behindActorInLane != getPlayer()) // If there's a collision avoidance worthy actor within 96 pixels behind the cab, then speed up
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

Pedestrian::Pedestrian(StudentWorld* sp, GhostRacer* playerPtr, int imageID, double startX, double startY, double size) : Actor(sp, playerPtr, 2, -4, 0, imageID, startX, startY, 0, size, 0)
{
    m_movementPlanDist = 0;
}

void Pedestrian::commonMovePlan() // Common pedestrian moveplan, setting new horizontal speeds and new move plans
{
    int newHorizSpeed = 0;
    while (newHorizSpeed == 0)
        newHorizSpeed = randInt(-3,3); // Continues to pick a new horizontal speed until it is not 0
    setHorizSpeed(newHorizSpeed);
    setMovePlan(randInt(4, 32));
    if (getHorizSpeed() < 0)
        setDirection(180);
    else
        setDirection(0);
}

HumanPedestrian::HumanPedestrian(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY) : Pedestrian(sp, playerPtr, IID_HUMAN_PED, startX, startY, 2)
{
    
}

void HumanPedestrian::doSomething()
{
    if (!isAlive())
        return;
    if (isOverlap(this, getPlayer()))
    {
        getWorld()->setHitHuman(true); // If overlaps with GhostRacer, communicates this to StudentWorld and it will then lose a life
        return;
    }
    if (getHitWater()) // When hit by water, it will reverse its direction and horizontal speed
    {
        setHorizSpeed(-1*getHorizSpeed());
        if (getDirection() == 180)
            setDirection(0);
        else
            setDirection(180);
        getWorld()->playSound(SOUND_PED_HURT);
        setHitWater(false);
    }
    commonMove();
    setMovePlan(getMovePlan() - 1);
    if (getMovePlan() > 0)
        return;
    else
        commonMovePlan();
}

ZombiePedestrian::ZombiePedestrian(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY) : Pedestrian(sp, playerPtr, IID_ZOMBIE_PED, startX, startY, 3)
{
    m_ticks = 0;
}

void ZombiePedestrian::doSomething()
{
    if (!isAlive())
        return;
    if (isOverlap(this, getPlayer())) // If overlaps with GhostRacer, damage GhostRacer by 5 and damage itself by 2 and die
    {
        getPlayer()->changeHealth(-5);
        changeHealth(-2);
    }
    if (getHitWater()) // If hit by water and it also dies, then it has a 20% chance of creating a healing goodie where zombiePed once stood
    {
        changeHealth(-1);
        if (getHealth() <= 0)
        {
            if(randInt(1, 5) == 5)
            {
                Actor *newHeal = new HealingGoodie(getWorld(), getPlayer(), getX(), getY()); // Creating new healing goodie where zombie ped was
                getWorld()->addActorToContainer(newHeal);
            }
        }
        else
            getWorld()->playSound(SOUND_PED_HURT);
        setHitWater(false);
    }
    if (getHealth() <= 0) // If a zombiePedestrian is killed, play proper sound and increase score
    {
        setAlive(false);
        getWorld()->increaseScore(150);
        getWorld()->playSound(SOUND_PED_DIE);
        return;
    }
    int playerX = getPlayer()->getX();
    int playerY = getPlayer()->getY();
    int zombieX = getX();
    int zombieY = getY();
    if (abs(playerX - zombieX) <= 30 && zombieY > playerY) // If GhostRacer is close enough to zombie pedestrian, then it will try to move towards it and play sound
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

Goodie::Goodie(StudentWorld* sp, GhostRacer* playerPtr, int imageID, double startX, double startY, double size, int dir) : Actor(sp, playerPtr, 1, -4, 0, imageID, startX, startY, dir, size, 2) // Common Goodie constructor; all goodies will have a depth of 2, 0 horizontal speed, and -4 vert speed
{
    
}

bool Goodie::commonGoodieAndOverlap() // Common function that all goodies will do is move and check to see if GhostRacer overlaps with it, then do something if that's the case
{
    commonMove();
    return isOverlap(this, getPlayer());
}

OilSlick::OilSlick(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY, double size) : Goodie(sp, playerPtr, IID_OIL_SLICK, startX, startY, size)
{
    
}

void OilSlick::doSomething()
{
    if(commonGoodieAndOverlap()) // If overlaps with GhostRacer, then make GhostRacer spin and play sound
    {
        getWorld()->playSound(SOUND_OIL_SLICK);
        getPlayer()->setHitOilSlick(true);
    }
}

HealingGoodie::HealingGoodie(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY) : Goodie(sp, playerPtr, IID_HEAL_GOODIE, startX, startY, 1)
{
    
}

void HealingGoodie::doSomething()
{
    if (commonGoodieAndOverlap()) // If overlaps with Ghostracer, attempt to heal it but make sure not to heal it above 100 health
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
    if (getHitWater()) // If hit by projectile, set health to 0 and die
    {
        changeHealth(-1);
        setAlive(false);
    }
}

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY) : Goodie(sp, playerPtr, IID_HOLY_WATER_GOODIE, startX, startY, 2, 90)
{
    
}

void HolyWaterGoodie::doSomething()
{
    if (commonGoodieAndOverlap()) // If overlaps with GhostRacer, increase number of sprays by 10
    {
        getPlayer()->changeSprays(10);
        setAlive(false);
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(50);
    }
    if (getHitWater()) // If hit by projectile, set health to 0 and die
    {
        changeHealth(-1);
        setAlive(false);
    }
}

SoulGoodie::SoulGoodie(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY) : Goodie(sp, playerPtr, IID_SOUL_GOODIE, startX, startY, 4)
{
    
}

void SoulGoodie::doSomething()
{
    if (commonGoodieAndOverlap()) // If GhostRacer overlaps with Soul, increase score and decrement number of souls needed
    {
        getWorld()->decrementNumSoulsNeeded();
        setAlive(false);
        getWorld()->playSound(SOUND_GOT_SOUL);
        getWorld()->increaseScore(100);
    }
    setDirection(getDirection() - 10);
}

GhostRacer::GhostRacer(StudentWorld* sp) : Actor(sp, nullptr, 100, 0, 0, IID_GHOST_RACER, 128, 32, 90, 4.0, 0)
{
    m_numSprays = 10;
    m_oilHit = false;
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
    if(getHitOil()) // When overlaps with oil slick, then spins
    {
        int multiplier = 0;
        while (multiplier == 0)
            multiplier = randInt(-1, 1); // Runs until get a multiplier which is -1 or 1
        int randomAngle = multiplier*randInt(5, 20);
        int newAngle = getDirection() + randomAngle;
        if (newAngle > 120)
            newAngle = 120;
        else if (newAngle < 60)
            newAngle = 60; // GhostRacer new angle can't be greater than 120 or less than 60
        setDirection(newAngle);
        setHitOilSlick(false); // Set to false otherwise it will continue to spin
    }
    bool hitSide = false;
    if (getX() <= LEFT_EDGE) // If hits the left side of the road, then take damage and change angle
    {
        if (getDirection() > 90)
        {
            changeHealth(-10);
            setDirection(82);
            getWorld()->playSound(SOUND_VEHICLE_CRASH);
            hitSide = true;
        }
    }
    else if (getX() >= RIGHT_EDGE) // If hits the right side of the road then take damage and change angle
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
    if (getWorld()->getKey(ch) && !hitSide) // If player pressed something and didn't hit a wall then get its input
    {
        switch(ch)
        {
            case KEY_PRESS_SPACE:
                if (getSprays() >= 1) // Creating new water projectile and adding it to studentWorld's container
                {
                    double cur_y = getY();
                    double cur_x = getX();
                    double delta_x = cos(this->getDirection()*M_PI/180)*SPRITE_HEIGHT;
                    double delta_y = sin(this->getDirection()*M_PI/180)*SPRITE_HEIGHT;
                    Actor* newSpray = new Projectile(this->getWorld(), this, (cur_x + delta_x), (cur_y + delta_y), this->getDirection()); // The new coordinates for the projectile must be ahead of GhostRacer, which means it's GhostRacer's coordinates plus delta_x and delta_y, where the two are based on the angle of GhostRacer
                    getWorld()->addActorToContainer(newSpray);
                    getWorld()->playSound(SOUND_PLAYER_SPRAY);
                    changeSprays(-1);
                }
                break;
            case KEY_PRESS_LEFT: // Turning, but making sure it can't turn past certain angles
                if (getDirection() < 114)
                    setDirection(getDirection() + 8);
                break;
            case KEY_PRESS_RIGHT:
                if (getDirection() > 66)
                    setDirection(getDirection() - 8);
                break;
            case KEY_PRESS_UP: // Speeding up or slowing down, but can't exceed certain speeds
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
