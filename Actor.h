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
    Actor(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY, int dir, double size, unsigned int depth);
    virtual void doSomething() = 0;
    virtual bool isMortal() {return false;};
    bool isAlive() {return m_isAlive;};
    void setAlive(bool alive) {m_isAlive = alive;};
    virtual bool isCollisionAvoidanceActor() {return false;};
    virtual bool canMoveDownScreenAndWhite() {return false;}; // Is false for all non-borderlines
    virtual int getSpeed() = 0;
    StudentWorld* getWorld() {return worldPtr;};
    Actor* getPlayer() {return GhostRacerPtr;};
private:
    StudentWorld* worldPtr;
    Actor* GhostRacerPtr;
    bool m_isAlive;
};

class Projectile: public Actor
{
public:
    Projectile(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY, int dir, double size = 1.0, unsigned int depth = 1);
    virtual void doSomething();
    virtual int getSpeed() {return 0;};

private:
};

class BorderLine: public Actor
{
public:
    BorderLine(StudentWorld* sp, Actor* playerPtr, bool isWhite, int imageID, double startX, double startY, int dir = 0, double size = 2.0, unsigned int depth = 1);
    virtual void doSomething();
    virtual int getSpeed() {return m_verticalSpeed;};
    bool isWhite() {return m_white;};
    virtual bool canMoveDownScreenAndWhite(); // Is true for all borderlines
    
private:
    double m_verticalSpeed;
    double m_horizontalSpeed;
    bool m_white;
};

class mortalActor: public Actor
{
public:
    mortalActor(StudentWorld* sp, Actor* playerPtr, int imageID, double startX, double startY, int dir, double size, unsigned int depth, int health, bool alive = true);
    virtual bool isMortal() {return true;};
    void changeHealth(int change) {m_health += change;};
    virtual bool isCollisionAvoidanceActor() {return true;};
    virtual void doSomething() = 0;
    virtual int getSpeed() = 0;
private:
    int m_health;
    bool m_alive;
};

class GhostRacer: public mortalActor
{
public:
    GhostRacer(StudentWorld* sp, Actor* playerPtr, int imageID = IID_GHOST_RACER, double startX = 128, double startY = 32, int dir = 90, double size = 4.0, unsigned int depth = 0);
    virtual void doSomething();
    int getSpeed() {return m_forwardSpeed;};
private:
    int m_numSprays;
    double m_forwardSpeed;
};


#endif // ACTOR_H_
