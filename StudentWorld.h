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
    void addActorToContainer(Actor* newActor) {livingActors.push_back(newActor);};
    Actor* actorInSameLane(Actor* cab); // Determine if there's an actor in the same lane as a cab and return the actor
    Actor* waterOverlap(Actor* projectile);
    ~StudentWorld();

private:
    std::list<Actor*> livingActors;
    Actor* playerPtr;
    int lastAddedWhiteY;
    unsigned int bonus;
    bool hitHuman;
};

#endif // STUDENTWORLD_H_
