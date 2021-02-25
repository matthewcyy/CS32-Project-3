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
    bonus = 5000;
    neededSouls = getLevel()*2 + 5;
}
// White lines at ROAD_CENTER - ROAD_WIDTH/2 + ROAD_WIDTH/3 and ROAD_CENTER + ROAD_WIDTH/2 - ROAD_WIDTH/3
// Center of white line lanes = ROAD_CENTER, ROAD_CENTER - ROAD_WIDTH/3, ROAD_CENTER + ROAD_WIDTH/3
// Distance from center of lane to left border is ROAD_CENTER - (ROAD_CENTER - ROAD_WIDTH/2 + ROAD_WIDTH/3) = ROAD_WIDTH/2 - ROAD_WIDTH/3 = ROAD_WIDTH/6
// Since the cab will start in the center, and actors are in a lane if its center is on or to the left boundary of lane K, and to the left of but not on the right boundary of lane K...
Actor* StudentWorld::actorInSameLane(Actor *cab)
{
    list<Actor*> :: iterator actorIt;
    actorIt = livingActors.begin();
    double cabX = cab->getX();
    double cabY = cab->getY();
    Actor* closestActorInLane = nullptr;
    double playerX = getPlayerPtr()->getX();
    double playerY = getPlayerPtr()->getY();
    bool isInLane = (cabX - playerX >= 0 && cabX - playerX <= ROAD_WIDTH/6) || (cabX - playerX < 0 && playerY - cabX < ROAD_WIDTH/6);
    if (isInLane)
        closestActorInLane = getPlayerPtr();
    while (actorIt != livingActors.end())
    {
        if ((*actorIt)->isCollisionAvoidanceActor() && cab != *actorIt) // If find collision avoidance actor
        {
            double actorX = (*actorIt)->getX();
            double actorY = (*actorIt)->getY();
            bool isInLane = (cabX - actorX >= 0 && cabX - actorX <= ROAD_WIDTH/6) || (cabX - actorX < 0 && actorX - cabX < ROAD_WIDTH/6); // Distance from center of lane (where cab is) to its borders is ROAD_WIDTH/6. However, it's considered to be in the lane if it's on the left boundary but not the right boundary
            if (isInLane)
            {
                if (closestActorInLane == nullptr)
                    closestActorInLane = (*actorIt);
                if (closestActorInLane != nullptr && abs(closestActorInLane->getY() - cabY) > abs(actorY - cabY))
                    closestActorInLane = (*actorIt);
            }
        }
        actorIt++;
    }
    return closestActorInLane;
}

Actor* StudentWorld::waterOverlap(Actor *projectile)
{
    list<Actor*> :: iterator actorIt;
    actorIt = livingActors.begin();
    Actor* overlapActor = nullptr;
    while (actorIt != livingActors.end())
    {
        if((*actorIt)->isAffectedByWater() && projectile->isOverlap(*actorIt, projectile))
        {
            overlapActor = *actorIt;
            return overlapActor;
        }
        actorIt++;
    }
    return nullptr;
}

int StudentWorld::init()
{
    neededSouls = getLevel()*2 + 5;
    bonus = 5000;
    lastAddedWhiteY = 220; // Have this here b/c otherwise if you lose a life and then it restarts, the first added white border line will not be spaced right
    GhostRacer *playerPtr = new GhostRacer(this);
    setPlayerPtr(playerPtr);
    for (int i = 0; i < VIEW_HEIGHT/(SPRITE_HEIGHT); i++)
    {
        Actor *leftBorder  = new BorderLine(this, playerPtr, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i*SPRITE_HEIGHT);
        Actor *rightBorder  = new BorderLine(this, playerPtr, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i*SPRITE_HEIGHT);
        livingActors.push_back(leftBorder);
        livingActors.push_back(rightBorder); // Creating and adding all border walls necessary on start
    }
    for (int j = 0; j < VIEW_HEIGHT/(4*SPRITE_HEIGHT); j++)
    {
        Actor *leftWhiteBorder = new BorderLine(this, playerPtr, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, j*4*SPRITE_HEIGHT);
        Actor *rightWhiteBorder = new BorderLine(this, playerPtr, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, j*4*SPRITE_HEIGHT);
        livingActors.push_back(leftWhiteBorder);
        livingActors.push_back(rightWhiteBorder); // Creating and adding all border walls necessary on start
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    list<Actor*> :: iterator actorIt;
    actorIt = livingActors.begin();
    getPlayerPtr()->doSomething();
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
            if (neededSouls == 0)
            {
                increaseScore(bonus);
                return GWSTATUS_FINISHED_LEVEL;
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
        Actor *newLeftYellow = new BorderLine(this, getPlayerPtr(), IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y);
        Actor *newRightYellow = new BorderLine(this, getPlayerPtr(), IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y);
        livingActors.push_back(newLeftYellow);
        livingActors.push_back(newRightYellow);
    }
    if (delta_y >= 4*SPRITE_HEIGHT)
    {
        Actor *newLeftWhite = new BorderLine(this, getPlayerPtr(), IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, new_border_y);
        Actor *newRightWhite = new BorderLine(this, getPlayerPtr(), IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, new_border_y);
        livingActors.push_back(newLeftWhite);
        livingActors.push_back(newRightWhite);
        lastAddedWhiteY = 244 + 4;
    }
    lastAddedWhiteY -= (4 + getPlayerPtr()->getVertSpeed());
    
    // Adding zombie cabs
    int ChanceVehicle = max(100 - L*10, 20);
    if (randInt(0, 0) == 0) // CHANGE THIS BACK TO CHANCEVEHICLE - 1
    {
        int cur_lane = randInt(0, 2); // 0 = left lane, 1 = middle lane, 2 = right lane
        cur_lane = 0; // REMOVE THIS AFTER TESTING
        int laneForCab = -1;
        list<Actor*> :: iterator actorIt;
        int countChecks = 0; // Determining how many times we check a lane
        double startX = 0.0;
        double startY = 0.0;
        double startVertSpeed = 0.0;
        while (countChecks < 3)
        {
            int centerX = ROAD_CENTER + (cur_lane - 1)*(ROAD_WIDTH/3);
            actorIt = livingActors.begin();
            Actor *closestActorToBottom = nullptr;
            int minY = VIEW_HEIGHT;
            int actorX;
            int actorY;
            bool isInLane;
            actorX = getPlayerPtr()->getX();
            actorY = getPlayerPtr()->getY();
            isInLane = ((centerX - actorX >= 0 && centerX - actorX <= ROAD_WIDTH/6) || (centerX - actorX < 0 && actorX - centerX < ROAD_WIDTH/6));
            if (isInLane)
            {
                closestActorToBottom = getPlayerPtr();
                minY = actorY;
            }
            while (actorIt != livingActors.end()) // Determining the closest collision avoidance-worthy actor to the bottom of the screen
            {
                actorX = (*actorIt)->getX();
                actorY = (*actorIt)->getY();
                isInLane = ((centerX - actorX >= 0 && centerX - actorX <= ROAD_WIDTH/6) || (centerX - actorX < 0 && actorX - centerX < ROAD_WIDTH/6)); // Determining if actor is in the lane based on the actor's x coordinate
                if ((*actorIt)->isCollisionAvoidanceActor() && isInLane)
                {
                    if (actorY < minY)
                    {
                        closestActorToBottom = *actorIt;
                        minY = actorY;
                    }
                }
                actorIt++;
            }
            if (closestActorToBottom == nullptr || minY > VIEW_HEIGHT/3)
            {
                laneForCab = cur_lane;
                startY = SPRITE_HEIGHT / 2;
                startVertSpeed = getPlayerPtr()->getVertSpeed() + randInt(2,4);
                break;
            }
            
            Actor *closestActorToTop = nullptr;
            int maxY = 0;
            actorIt = livingActors.begin();
            actorX = getPlayerPtr()->getX();
            actorY = getPlayerPtr()->getY();
            isInLane = ((centerX - actorX >= 0 && centerX - actorX <= ROAD_WIDTH/6) || (centerX - actorX < 0 && actorX - centerX < ROAD_WIDTH/6));
            if (isInLane)
            {
                closestActorToTop = getPlayerPtr();
                maxY = actorY;
            }
            while (actorIt != livingActors.end()) // Determining the closest collision avoidance-worthy actor to the top of the screen
            {
                actorX = (*actorIt)->getX();
                actorY = (*actorIt)->getY();
                if (actorIt == livingActors.begin())
                {
                    actorX = getPlayerPtr()->getX();
                    actorY = getPlayerPtr()->getY();
                }
                isInLane = ((centerX - actorX >= 0 && centerX - actorX <= ROAD_WIDTH/6) || (centerX - actorX < 0 && actorX - centerX < ROAD_WIDTH/6)); // Determining if actor is in the lane based on the actor's x coordinate
                if ((*actorIt)->isCollisionAvoidanceActor() && isInLane)
                {
                    if (actorY > maxY)
                    {
                        closestActorToTop = *actorIt;
                        maxY = actorY;
                    }
                }
                actorIt++;
            }
            if (closestActorToTop == nullptr || maxY < VIEW_HEIGHT*2/3)
            {
                laneForCab = cur_lane;
                startY = VIEW_HEIGHT - SPRITE_HEIGHT/2;
                startVertSpeed = getPlayerPtr()->getVertSpeed() - randInt(2,4);
                break;
            }
            if (cur_lane != 2)
                cur_lane++;
            else
                cur_lane = 0;
            countChecks++;
        }
        if (laneForCab != -1)
        {
            startX = ROAD_CENTER + (laneForCab - 1)*(ROAD_WIDTH/3);
            Actor *newCab = new ZombieCab(this, getPlayerPtr(), startVertSpeed, startX, startY);
            livingActors.push_back(newCab);
        }
    }
    
    // Adding oil slicks
    int ChanceOilSlick = max(150 - L * 10, 40);
    if (randInt(0, ChanceOilSlick - 1) == 0)
    {
        int randomX = randInt(LEFT_EDGE, RIGHT_EDGE);
        int oilSize = randInt(2, 5);
        Actor *newOil = new OilSlick(this, getPlayerPtr(), randomX, VIEW_HEIGHT, oilSize);
        livingActors.push_back(newOil);
    }
    
    // Adding zombie Peds
    int pedY = VIEW_HEIGHT;
    
    int ChanceZombiePed = max(100 - L * 10, 20);
    if (randInt(0, ChanceZombiePed - 1) == 0)
    {
        int zombieX = randInt(0, VIEW_WIDTH);
        Actor *newZombiePed = new ZombiePedestrian(this, getPlayerPtr(), zombieX, pedY);
        livingActors.push_back(newZombiePed);
    }
    
    // Adding human Peds
    int ChanceHumanPed = max(200 - L * 10, 30);
    if (randInt(0, ChanceHumanPed - 1) == 0)
    {
        int humanX = randInt(0, VIEW_WIDTH);
        Actor *newHumanPed = new HumanPedestrian(this, getPlayerPtr(), humanX, pedY);
        livingActors.push_back(newHumanPed);
    }
    
    // Adding HolyWater Goodies
    int ChanceOfHolyWater = 100 + 10 * L;
    if (randInt(0, ChanceOfHolyWater-1) == 0)
    {
        int waterGoodieX = randInt(LEFT_EDGE, RIGHT_EDGE);
        Actor *newWaterGoodie = new HolyWaterGoodie(this, getPlayerPtr(), waterGoodieX, VIEW_HEIGHT);
        livingActors.push_back(newWaterGoodie);
    }
    
    // Adding Lost Soul Goodies
    if (randInt(0, 100 - 1) == 0)
    {
        int soulGoodieX = randInt(LEFT_EDGE, RIGHT_EDGE);
        Actor *newSoulGoodie = new SoulGoodie(this, getPlayerPtr(), soulGoodieX, VIEW_HEIGHT);
        livingActors.push_back(newSoulGoodie);
    }
        
    
    // UPDATING THE GAME STATUS LINE
    ostringstream statusBar;
    statusBar << "Score: " << getScore() << "  " << "Lvl: " << getLevel() << "  " << "Souls2Save: " << neededSouls << "  " << "Lives: " << getLives() << "  " << "Health: " << getPlayerPtr()->getHealth() << "  " << "Sprays: " << getPlayerPtr()->getSprays() << "  " << "Bonus: " << bonus--;
    string status = statusBar.str();
    setGameStatText(status);
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete getPlayerPtr();
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
