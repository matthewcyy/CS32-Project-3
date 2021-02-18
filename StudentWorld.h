#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

class Actor;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    Actor* getPlayerPtr() {return playerPtr;};
    void setPlayerPtr(Actor* racerPtr) {playerPtr = racerPtr;};
    void setHitHuman(bool hit) {hitHuman = hit;};
    ~StudentWorld();

private:
    std::list<Actor*> livingActors;
    Actor* playerPtr;
    int lastAddedWhiteY;
    bool hitHuman;
};

#endif // STUDENTWORLD_H_
