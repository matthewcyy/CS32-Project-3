#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"
#include "GameConstants.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;

class Actor: public GraphObject
{
public:
    Actor(StudentWorld* sp, Actor* playerPtr, int health, int horizSpeed, int vertSpeed, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual void doSomething() = 0;
    virtual bool isMortal() = 0;
    void changeHealth(int change) {m_health += change;};
    bool isAlive() {return m_isAlive;};
    void setAlive(bool alive) {m_isAlive = alive;};
    int getHealth() {return m_health;};
    virtual bool isCollisionAvoidanceActor() {return false;};
    int getHorizSpeed() {return m_horizontalSpeed;};
    int getVertSpeed() {return m_verticalSpeed;};
    void changeHorizSpeed(int horizSpeed) {m_horizontalSpeed += horizSpeed;};
    void changeVertSpeed(int vertSpeed) {m_verticalSpeed += vertSpeed;};
    StudentWorld* getWorld() {return worldPtr;};
    Actor* getPlayer() {return GhostRacerPtr;};
private:
    StudentWorld* worldPtr;
    Actor* GhostRacerPtr;
    bool m_isAlive;
    int m_horizontalSpeed;
    int m_verticalSpeed;
    int m_health;
};

class Projectile: public Actor
{
public:
    Projectile(StudentWorld* sp, Actor* playerPtr, double startX, double startY, int dir, double size = 1.0, unsigned int depth = 1);
    virtual void doSomething();
    virtual bool isMortal() {return false;};
    virtual int getSpeed() {return 0;};

private:
};

class BorderLine: public Actor
{
public:
    BorderLine(StudentWorld* sp, Actor* playerPtr, bool isWhite, int imageID, double startX, double startY, int dir = 0, double size = 2.0, unsigned int depth = 1);
    virtual void doSomething();
    virtual bool isMortal() {return false;};
    bool isWhite() {return m_white;};
    
private:
    bool m_white;
};

class GhostRacer: public Actor
{
public:
    GhostRacer(StudentWorld* sp, Actor* playerPtr, double startX = 128, double startY = 32, int dir = 90, double size = 4.0, unsigned int depth = 0);
    virtual void doSomething();
    virtual bool isMortal() {return true;};
private:
    int m_numSprays;
};


#endif // ACTOR_H_
