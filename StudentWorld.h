#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>

class Actor;
class GhostRacer;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    GhostRacer* getPlayerPtr() const {return playerPtr;};
    void setPlayerPtr(GhostRacer* racerPtr) {playerPtr = racerPtr;}; // Assigning the GhostRacer pointer after initialization
    void setHitHuman(bool hit) {hitHuman = hit;}; // Determining if a human was hit so that the game may end
    void addActorToContainer(Actor* newActor) {livingActors.push_back(newActor);}; // Here to add actors that are created by other actors, such as projectilves, oil slicks, and healing goodies
    void decrementNumSoulsNeeded() {neededSouls--;};
    Actor* actorInSameLaneInFront(Actor* cab); // Determine if there's an actor in the same lane as a cab and return the actor
    Actor* actorInSameLaneBehind(Actor* cab); // Determine if there's an actor in the same lane as a cab and return the actor
    Actor* waterOverlap(Actor* projectile); // Determine if any actors overlap with water
    ~StudentWorld();

private:
    std::list<Actor*> livingActors; // Container of actors
    GhostRacer* playerPtr;
    int lastAddedWhiteY;
    int neededSouls;
    unsigned int bonus;
    bool hitHuman;
    Actor* isInSameLane(Actor* cab, Actor* other);
};

#endif // STUDENTWORLD_H_
