#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "GameConstants.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;

class Actor: public GraphObject
{
public:
    Actor(StudentWorld* sp, Actor* playerPtr, int health, double horizSpeed, double vertSpeed, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual void doSomething() = 0;
    virtual bool isAffectedByWater() = 0;
    void changeHealth(int change) {m_health += change;};
    void commonMove();
    bool isAlive() {return m_isAlive;};
    bool isOverlap(Actor* object, Actor* player);
    bool getHitWater() {return m_waterHit;};
    void setHitWater(bool wasHit) {m_waterHit = wasHit;};
    virtual bool isPedestrian() {return false;};
    virtual bool isCollisionAvoidanceActor() {return false;};
    void setAlive(bool alive) {m_isAlive = alive;};
    int getHealth() {return m_health;};
    double getHorizSpeed() {return m_horizontalSpeed;};
    double getVertSpeed() {return m_verticalSpeed;};
    virtual int getSprays() {return 0;};
    // void changeHorizSpeed(double horizSpeed) {m_horizontalSpeed += horizSpeed;};
    // void changeVertSpeed(double vertSpeed) {m_verticalSpeed += vertSpeed;};
    void setHorizSpeed(double horizSpeed) {m_horizontalSpeed = horizSpeed;};
    void setVertSpeed(double vertSpeed) {m_verticalSpeed = vertSpeed;};
    StudentWorld* getWorld() {return worldPtr;};
    Actor* getPlayer() {return GhostRacerPtr;};
private:
    StudentWorld* worldPtr;
    Actor* GhostRacerPtr;
    bool m_isAlive;
    bool m_waterHit;
    double m_horizontalSpeed;
    double m_verticalSpeed;
    int m_health;
};

class Projectile: public Actor
{
public:
    Projectile(StudentWorld* sp, Actor* playerPtr, double startX, double startY, int dir);
    virtual void doSomething();
    virtual bool isAffectedByWater() {return false;};

private:
    int m_countMoves;
};

class BorderLine: public Actor
{
public:
    BorderLine(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY);
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
    virtual int getSprays() {return m_numSprays;};
    virtual bool isCollisionAvoidanceActor() {return true;};
private:
    int m_numSprays;
};

class ZombieCab: public Actor
{
public:
    ZombieCab(StudentWorld* sp, Actor* playerPtr, double vertSpeed, double startX, double startY);
    virtual void doSomething();
    virtual bool isCollisionAvoidanceActor() {return true;};
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
public:
    Pedestrian(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY, double size);
    bool isAffectedByWater() {return true;};
    virtual bool isPedestrian() {return true;};
    virtual bool isCollisionAvoidanceActor() {return true;};
    int getMovePlan() {return m_movementPlanDist;};
    void setMovePlan(int setMove) {m_movementPlanDist = setMove;};
    void commonMovePlan();

private:
    int m_movementPlanDist;
};

class HumanPedestrian: public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sp, Actor* playerPtr, double startX, double startY);
    // virtual void changeHealth(int change) {return;};
    virtual void doSomething();
};

class ZombiePedestrian: public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sp, Actor* playerPtr, double startX, double startY);
    virtual void doSomething();
private:
    int m_ticks;
};

#endif // ACTOR_H_
