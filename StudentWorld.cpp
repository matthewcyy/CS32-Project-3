#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <iostream>
#include <sstream>
#include <iomanip>
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
    playerPtr = nullptr;
    lastAddedWhiteY = 220;
    hitHuman = false;
}

int StudentWorld::init()
{
    lastAddedWhiteY = 220; // Have this here b/c otherwise if you lose a life and then it restarts, the first added white border line will not be spaced right
    Actor *playerPtr = new GhostRacer(this);
    setPlayerPtr(playerPtr);
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
    list<Actor*> :: iterator actorIt;
    actorIt = livingActors.begin();
    while (actorIt != livingActors.end()) // Having all actors do something or ending the game early if something happend
    {
        if ((*actorIt)->isAlive())
        {
            (*actorIt)->doSomething(); // Having all actors do something
            if (playerPtr->getHealth() <= 0)
            {
                playerPtr->setAlive(false);
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (hitHuman)
            {
                playerPtr->setAlive(false);
                decLives();
                setHitHuman(false);
                return GWSTATUS_PLAYER_DIED;
            }
            actorIt++;
        }
        else
            actorIt++;
    }
    actorIt = livingActors.begin();
    while (actorIt != livingActors.end()) // Removing all dead actors
    {
        if (!((*actorIt)->isAlive()))
        {
            delete (*actorIt);
            actorIt = livingActors.erase(actorIt); // Erasing all dead actors
        }
        else
            actorIt++;
    }
    
    // ADDING NEW ACTORS BELOW:
    
    int L = getLevel();
    // Adding borderlines when necessary
    double lastAdded_y = lastAddedWhiteY;
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    double delta_y = new_border_y - lastAdded_y;
    if (delta_y >= SPRITE_HEIGHT)
    {
        Actor *newLeftYellow = new BorderLine(this, getPlayerPtr(), false, IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y);
        Actor *newRightYellow = new BorderLine(this, getPlayerPtr(), false, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y);
        livingActors.push_back(newLeftYellow);
        livingActors.push_back(newRightYellow);
    }
    if (delta_y >= 4*SPRITE_HEIGHT)
    {
        Actor *newLeftWhite = new BorderLine(this, getPlayerPtr(),true, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, new_border_y);
        Actor *newRightWhite = new BorderLine(this, getPlayerPtr(), true, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, new_border_y);
        livingActors.push_back(newLeftWhite);
        livingActors.push_back(newRightWhite);
        lastAddedWhiteY = 244 + 4;
    }
    lastAddedWhiteY -= (4 + getPlayerPtr()->getVertSpeed());
    
    // Adding zombie Peds
    int pedY = VIEW_HEIGHT;
    
    int ChanceZombiePed = max(100 - L * 10, 20);
    if (randInt(0, ChanceZombiePed) == 0)
    {
        int zombieX = randInt(0, VIEW_WIDTH);
        Actor *newZombiePed = new ZombiePedestrian(this, getPlayerPtr(), zombieX, pedY);
        livingActors.push_back(newZombiePed);
    }
    
    // Adding human Peds
    int ChanceHumanPed = max(200 - L * 10, 30);
    if (randInt(0, ChanceHumanPed) == 0)
    {
        int humanX = randInt(0, VIEW_WIDTH);
        Actor *newHumanPed = new HumanPedestrian(this, getPlayerPtr(), humanX, pedY);
        livingActors.push_back(newHumanPed);
    }
    
    // UPDATING THE GAME STATUS LINE
    ostringstream statusBar;
    statusBar << "Score: " << getScore() << "  " << "Lvl: " << getLevel() << "  " << "Souls2Save: " << "Lives: " << getLives() << "  " << "Health: " << getPlayerPtr()->getHealth() << "  " << "Sprays: " << getPlayerPtr()->getSprays() << "  " << "Bonus: " << 5000;
    string status = statusBar.str();
    setGameStatText(status);
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
