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
    ~StudentWorld();

private:
    std::list<Actor*> livingActors;
    Actor* playerPtr;
    int lastAddedWhiteY;
};

#endif // STUDENTWORLD_H_
