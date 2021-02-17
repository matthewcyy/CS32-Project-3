#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <list>
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    
}

int StudentWorld::init()
{
    Actor *playerPtr = new GhostRacer(this, nullptr, 128, 32, 90, 4.0, 0);
    livingActors.push_back(playerPtr);
    for (int i = 0; i < VIEW_HEIGHT/(SPRITE_HEIGHT); i++)
    {
        Actor *leftBorder  = new BorderLine(this, playerPtr, false, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i*SPRITE_HEIGHT);
        Actor *rightBorder  = new BorderLine(this, playerPtr, false, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i*SPRITE_HEIGHT);
        livingActors.push_back(leftBorder);
        livingActors.push_back(rightBorder); // Creating and adding all border walls necessary on start
    }
    for (int j = 0; j < VIEW_HEIGHT/(4*SPRITE_HEIGHT); j++)
    {
        Actor *leftWhiteBorder = new BorderLine(this, playerPtr, true, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, j*4*SPRITE_HEIGHT);
        Actor *rightWhiteBorder = new BorderLine(this, playerPtr, true, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, j*4*SPRITE_HEIGHT);
        livingActors.push_back(leftWhiteBorder);
        livingActors.push_back(rightWhiteBorder); // Creating and adding all border walls necessary on start
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    list<Actor*> :: iterator actorIt;
    actorIt = livingActors.begin();
    while (actorIt != livingActors.end())
    {
        (*actorIt)->doSomething(); // Having all actors do something
        actorIt++;
    }
    actorIt = livingActors.begin();
    while (actorIt != livingActors.end())
    {
        if (!((*actorIt)->isAlive()))
        {
            delete (*actorIt);
            actorIt = livingActors.erase(actorIt); // Erasing all dead actors
        }
        else
            actorIt++;
    }
    list<Actor*> :: iterator lastWhite;
    lastWhite = livingActors.end();
    lastWhite--;
    while (lastWhite != livingActors.begin())
    {
        if ((*lastWhite)->canMoveDownScreenAndWhite())
            break; // Finding the last added white border
        else
            lastWhite--;
    }
    double lastAdded_y = (*lastWhite)->getY();
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - lastAdded_y;
    if (delta_y >= SPRITE_HEIGHT)
    {
        Actor *newLeftYellow = new BorderLine(this, (*lastWhite)->getPlayer(), false, IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y);
        Actor *newRightYellow = new BorderLine(this, (*lastWhite)->getPlayer(), false, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y);
        livingActors.push_back(newLeftYellow);
        livingActors.push_back(newRightYellow);
    }
    if (delta_y >= 4*SPRITE_HEIGHT)
    {
        Actor *newLeftWhite = new BorderLine(this, (*lastWhite)->getPlayer(),true, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, new_border_y);
        Actor *newRightWhite = new BorderLine(this, (*lastWhite)->getPlayer(), true, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, new_border_y);
        livingActors.push_back(newLeftWhite);
        livingActors.push_back(newRightWhite);
    }
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    list<Actor*> :: iterator actorIt;
    actorIt = livingActors.begin();
    while (actorIt != livingActors.end())
    {
        delete (*actorIt);
        actorIt = livingActors.erase(actorIt); // Deleting all actors
    }
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}
