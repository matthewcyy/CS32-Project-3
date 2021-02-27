#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;
class GhostRacer;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;

class Actor: public GraphObject
{
public:
    Actor(StudentWorld* sp, GhostRacer* playerPtr, int health, double horizSpeed, double vertSpeed, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual void doSomething() = 0; // What each actor will do
    virtual bool isAffectedByWater() = 0; // Is actor affected by water projectile
    void changeHealth(int change) {m_health += change;};
    void commonMove(); // Movement pattern common to all actors, which makes them move vertically relative to GhostRacer's speed
    bool isAlive() const {return m_isAlive;};
    bool isOverlap(Actor* object, Actor* player) const;
    bool getHitWater() const {return m_waterHit;};
    void setHitWater(bool wasHit) {m_waterHit = wasHit;}; // Used to determine if an actor was hit by water and if so, do something relevant to it
    virtual bool isCollisionAvoidanceActor() const {return false;}; // Used to determine if ZombieCab can be created in a specific lane
    void setAlive(bool alive) {m_isAlive = alive;};
    int getHealth() const {return m_health;};
    double getHorizSpeed() const {return m_horizontalSpeed;};
    double getVertSpeed() const {return m_verticalSpeed;};
    void setHorizSpeed(double horizSpeed) {m_horizontalSpeed = horizSpeed;};
    void setVertSpeed(double vertSpeed) {m_verticalSpeed = vertSpeed;};
    StudentWorld* getWorld() const {return worldPtr;};
    GhostRacer* getPlayer() const {return GhostRacerPtr;};
private:
    StudentWorld* worldPtr;
    GhostRacer* GhostRacerPtr;
    bool m_isAlive;
    bool m_waterHit;
    double m_horizontalSpeed;
    double m_verticalSpeed;
    int m_health;
};

class Projectile: public Actor
{
public:
    Projectile(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY, int dir);
    virtual void doSomething();
    virtual bool isAffectedByWater() {return false;};

private:
    int m_countMoves;
};

class BorderLine: public Actor
{
public:
    BorderLine(StudentWorld* sp, GhostRacer* playerPtr, int imageID, double startX, double startY);
    virtual void doSomething();
    virtual bool isAffectedByWater() {return false;};
    
private:
};

class GhostRacer: public Actor
{
public:
    GhostRacer(StudentWorld* sp);
    virtual void doSomething();
    virtual bool isAffectedByWater() {return false;};
    virtual bool isCollisionAvoidanceActor() const {return true;};
    void changeSprays(int change) {m_numSprays += change;}; // Used to change number of sprays (increment or decrement)
    void setHitOilSlick(bool hitOil) {m_oilHit = hitOil;};
    int getSprays() {return m_numSprays;};
    bool getHitOil() {return m_oilHit;};

private:
    int m_numSprays; // Move this to Ghost Racer class
    bool m_oilHit; // Can remove this or move to GhostRacer class
};

class ZombieCab: public Actor
{
public:
    ZombieCab(StudentWorld* sp, GhostRacer* playerPtr, double vertSpeed, double startX, double startY);
    virtual void doSomething();
    virtual bool isCollisionAvoidanceActor() const {return true;};
    virtual bool isAffectedByWater() {return true;};
    void setDamagedPlayer(bool hasDamaged) {m_hasDamagedPlayer = hasDamaged;};
    int getPlanLength() {return m_planLength;};
    void setPlanLength(int plan) {m_planLength = plan;};
private:
    bool m_hasDamagedPlayer;
    int m_planLength;
};

class Pedestrian: public Actor
{
public: // Common subclass for pedestrians, based around their moveplans
    Pedestrian(StudentWorld* sp, GhostRacer* playerPtr, int imageID, double startX, double startY, double size);
    virtual bool isAffectedByWater() {return true;};
    virtual bool isCollisionAvoidanceActor() const {return true;};
    int getMovePlan() {return m_movementPlanDist;};
    void setMovePlan(int setMove) {m_movementPlanDist = setMove;};
    void commonMovePlan();

private:
    int m_movementPlanDist;
};

class HumanPedestrian: public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY);
    // virtual void changeHealth(int change) {return;};
    virtual void doSomething();
};

class ZombiePedestrian: public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY);
    virtual void doSomething();
private:
    int m_ticks;
};

class Goodie: public Actor // Common subclass for goodie, based around actors that do something to GhostRacer when they overlap
{
public:
    Goodie(StudentWorld* sp, GhostRacer* playerPtr, int imageID, double startX, double startY, double size, int dir = 0);
    virtual bool isAffectedByWater() {return false;};
    bool commonGoodieAndOverlap();
};

class OilSlick: public Goodie
{
public:
    OilSlick(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY, double size);
    virtual void doSomething();
};

class HealingGoodie: public Goodie
{
public:
    HealingGoodie(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY);
    virtual bool isAffectedByWater() {return true;};
    virtual void doSomething();
};

class HolyWaterGoodie: public Goodie
{
public:
    HolyWaterGoodie(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY);
    virtual bool isAffectedByWater() {return true;};
    virtual void doSomething();
};

class SoulGoodie: public Goodie
{
public:
    SoulGoodie(StudentWorld* sp, GhostRacer* playerPtr, double startX, double startY);
    virtual void doSomething();
};

#endif // ACTOR_H_
