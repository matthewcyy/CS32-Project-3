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
    playerPtr = nullptr; //Initialize playerPtr to be nullptre
    lastAddedWhiteY = 220; //Used to keep track of last added white border's y coordinate
    hitHuman = false;
    bonus = 5000;
    neededSouls = getLevel()*2 + 5;
}

Actor* StudentWorld::isInSameLane(Actor* cab, Actor* other)
{
    double cabX = cab->getX();
    double actorX = other->getX();
    bool isInLane = (cabX - actorX >= 0 && cabX - actorX <= ROAD_WIDTH/6) || (cabX - actorX < 0 && actorX - cabX < ROAD_WIDTH/6); // Distance from center of lane (where cab is) to its borders is ROAD_WIDTH/6. However, it's considered to be in the lane if it's on the left boundary but not the right boundary
    if (isInLane && other->isCollisionAvoidanceActor())
        return other; // Returning isCollisionAvoidanceActor that's in the same lane
    else
        return nullptr; // If no such actor is found, return nullptr
}

Actor* StudentWorld::actorInSameLaneInFront(Actor *cab) // Searching for closest actor in front of the cab
{
    list<Actor*> :: iterator actorIt;
    actorIt = livingActors.begin();
    Actor* closestActorInLane = nullptr;
    double cabY = cab->getY();
    Actor* actorInLane = isInSameLane(cab, getPlayerPtr());
    double actorInLaneY = -1;
    if (actorInLane != nullptr)
        closestActorInLane = actorInLane; // Since GhostRacer is not in the actor container, have to check if it's in the same lane separately from the rest
    while (actorIt != livingActors.end())
    {
        actorInLane = isInSameLane(cab, *(actorIt));
        if (actorInLane != nullptr)
            actorInLaneY = actorInLane->getY(); // Making sure don't access a nullptr
        if (actorInLane != nullptr && cab != *actorIt) // Making sure the actor we found in the same lane as the cab isn't the cab itself
        {
            if (closestActorInLane == nullptr && actorInLaneY > cabY)
                    closestActorInLane = actorInLane; // If no other closestActors yet, first one will be closestActor
            if (closestActorInLane != nullptr && actorInLaneY > cabY && closestActorInLane->getY() - cabY > actorInLaneY - cabY) // If this new actorInLane's Y has a smaller difference with the cab's Y, then it should be the new closestActor
                    closestActorInLane = (*actorIt);
        }
        actorIt++;
    }
    return closestActorInLane;
}

Actor* StudentWorld::actorInSameLaneBehind(Actor *cab) // Similar logic as above, but instead searching for closest Actor which is behind the cab
{
    list<Actor*> :: iterator actorIt;
    actorIt = livingActors.begin();
    Actor* closestActorInLane = nullptr;
    double cabY = cab->getY();
    Actor* actorInLane = isInSameLane(cab, getPlayerPtr());
    double actorInLaneY = -1;
    if (actorInLane != nullptr)
        closestActorInLane = actorInLane;
    while (actorIt != livingActors.end())
    {
        actorInLane = isInSameLane(cab, *(actorIt));
        if (actorInLane != nullptr)
            actorInLaneY = actorInLane->getY();
        if (actorInLane != nullptr && cab != *actorIt)
        {
            if (closestActorInLane == nullptr && actorInLaneY < cabY)
                    closestActorInLane = actorInLane;
            if (closestActorInLane != nullptr && actorInLaneY < cabY && cabY - closestActorInLane->getY() > cabY - actorInLaneY) // Instead searching for actor in lane which is behind the cab and seeing if their difference is greater than or less than the current closest actor in lane
                    closestActorInLane = (*actorIt);
        }
        actorIt++;
    }
    return closestActorInLane;
}

Actor* StudentWorld::waterOverlap(Actor *projectile) // Returning an actor which overlaps with projectile and is affected by water
{
    list<Actor*> :: iterator actorIt;
    actorIt = livingActors.begin();
    Actor* overlapActor = nullptr;
    while (actorIt != livingActors.end())
    {
        if((*actorIt)->isAffectedByWater() && projectile->isOverlap(*actorIt, projectile))
        {
            overlapActor = *actorIt;
            return overlapActor; // Going through all actors in the list, and if there is an actor which is affectedByWater and overlaps with the water projectile then return such actor
        }
        actorIt++;
    }
    return nullptr;
}

int StudentWorld::init()
{
    neededSouls = getLevel()*2 + 5; // Reset sould needed when world is created
    bonus = 5000; // Initialize the bonus to be 5000 when world is created
    lastAddedWhiteY = 220; // Have this here b/c otherwise if you lose a life and then it restarts, the first added white border line will not be spaced right
    GhostRacer *playerPtr = new GhostRacer(this);
    setPlayerPtr(playerPtr); // Creating and setting our playerPtr
    for (int i = 0; i < VIEW_HEIGHT/(SPRITE_HEIGHT); i++)
    {
        Actor *leftBorder  = new BorderLine(this, playerPtr, IID_YELLOW_BORDER_LINE, LEFT_EDGE, i*SPRITE_HEIGHT);
        Actor *rightBorder  = new BorderLine(this, playerPtr, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, i*SPRITE_HEIGHT);
        livingActors.push_back(leftBorder);
        livingActors.push_back(rightBorder); // Creating and adding all yellow borders necessary on start
    }
    for (int j = 0; j < VIEW_HEIGHT/(4*SPRITE_HEIGHT); j++)
    {
        Actor *leftWhiteBorder = new BorderLine(this, playerPtr, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, j*4*SPRITE_HEIGHT);
        Actor *rightWhiteBorder = new BorderLine(this, playerPtr, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, j*4*SPRITE_HEIGHT);
        livingActors.push_back(leftWhiteBorder);
        livingActors.push_back(rightWhiteBorder); // Creating and adding all white borders necessary on start
        // Last added whiteBorder's initial Y value comes from the last added white line here on initialization
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    list<Actor*> :: iterator actorIt;
    actorIt = livingActors.begin();
    getPlayerPtr()->doSomething(); // Since playerPtr not in the container for the actors, since it's a GhostRacer ptr and not an actor pointer
    while (actorIt != livingActors.end()) // Having all actors do something or ending the game early if something happend
    {
        if ((*actorIt)->isAlive())
        {
            (*actorIt)->doSomething(); // Having all actors do something
            if (playerPtr->getHealth() <= 0) // If player's health is less than or equal to 0 then set it to be not alive
            {
                playerPtr->setAlive(false);
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (neededSouls == 0) // If we've found all the required souls to complete the level, add bonus points and complete level
            {
                increaseScore(bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
            if (hitHuman) // If the GhostRacer hit a human pedestrian, immediately end the game
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
    if (delta_y >= SPRITE_HEIGHT) // Adding yellow lines when appropriate
    {
        Actor *newLeftYellow = new BorderLine(this, getPlayerPtr(), IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y);
        Actor *newRightYellow = new BorderLine(this, getPlayerPtr(), IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y);
        livingActors.push_back(newLeftYellow);
        livingActors.push_back(newRightYellow);
    }
    if (delta_y >= 4*SPRITE_HEIGHT) // Adding whtie lines when appropriate
    {
        Actor *newLeftWhite = new BorderLine(this, getPlayerPtr(), IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, new_border_y);
        Actor *newRightWhite = new BorderLine(this, getPlayerPtr(), IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, new_border_y);
        livingActors.push_back(newLeftWhite);
        livingActors.push_back(newRightWhite);
        lastAddedWhiteY = 244 + 4; // Must properly adjust lastAddedWhiteY
    }
    lastAddedWhiteY -= (4 + getPlayerPtr()->getVertSpeed()); // Must properly adjust lastAddedWhiteY which depends on the player's speed
    
    // Adding zombie cabs
    int ChanceVehicle = max(100 - L*10, 20);
    if (randInt(0, ChanceVehicle - 1) == 0)
    {
        int cur_lane = randInt(0, 2); // 0 = left lane, 1 = middle lane, 2 = right lane. Picking random lane to start iterating
        int laneForCab = -1;
        list<Actor*> :: iterator actorIt;
        int countChecks = 0; // Determining how many times we check a lane
        double startX = 0.0;
        double startY = 0.0;
        double startVertSpeed = 0.0;
        while (countChecks < 3)
        {
            int centerX = ROAD_CENTER + (cur_lane - 1)*(ROAD_WIDTH/3); // Calculation for center of lane based on which lane it is
            actorIt = livingActors.begin();
            Actor *closestActorToBottom = nullptr;
            int minY = VIEW_HEIGHT;
            int actorX;
            int actorY;
            bool isInLane;
            actorX = getPlayerPtr()->getX();
            actorY = getPlayerPtr()->getY();
            isInLane = ((centerX - actorX >= 0 && centerX - actorX <= ROAD_WIDTH/6) || (centerX - actorX < 0 && actorX - centerX < ROAD_WIDTH/6)); // First checking to see if ghostRacer is in the lane, since ghostRacer is not in the actor container
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
                    if (actorY < minY) // If the current actor's Y coordinate is less than the previous lowest actor then set it to be the new closest actor to bottom
                    {
                        closestActorToBottom = *actorIt;
                        minY = actorY;
                    }
                }
                actorIt++;
            }
            if (closestActorToBottom == nullptr || minY > VIEW_HEIGHT/3) // If there is no actor in the lane or the closest actor to the bottom is far away enough from the bottom, then select this lane to add the zombie cab to
            {
                laneForCab = cur_lane;
                startY = SPRITE_HEIGHT / 2;
                startVertSpeed = getPlayerPtr()->getVertSpeed() + randInt(2,4);
                break; // Skipping to setp 3
            }
            
            Actor *closestActorToTop = nullptr;
            int maxY = 0;
            actorIt = livingActors.begin();
            actorX = getPlayerPtr()->getX();
            actorY = getPlayerPtr()->getY();
            isInLane = ((centerX - actorX >= 0 && centerX - actorX <= ROAD_WIDTH/6) || (centerX - actorX < 0 && actorX - centerX < ROAD_WIDTH/6)); // Again checking to see if the ghostRacer is in the lane since it is not in the container
            if (isInLane)
            {
                closestActorToTop = getPlayerPtr();
                maxY = actorY;
            }
            while (actorIt != livingActors.end()) // Determining the closest collision avoidance-worthy actor to the top of the screen. Similar process as above but instead using a maxY var
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
                    if (actorY > maxY) // If find an actor which is closer to the top than the previous closestActorToTop, then set it to be the new closestActorToTop
                    {
                        closestActorToTop = *actorIt;
                        maxY = actorY;
                    }
                }
                actorIt++;
            }
            if (closestActorToTop == nullptr || maxY < VIEW_HEIGHT*2/3) // If no actor in lane or it's y value is not large, then assign this lane to add the zombieCab
            {
                laneForCab = cur_lane;
                startY = VIEW_HEIGHT - SPRITE_HEIGHT/2;
                startVertSpeed = getPlayerPtr()->getVertSpeed() - randInt(2,4);
                break; // Skipping to step 3
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
            Actor *newCab = new ZombieCab(this, getPlayerPtr(), startVertSpeed, startX, startY); // Creating new cab and adding it to the container
            livingActors.push_back(newCab);
        }
    }
    int newAddY = VIEW_HEIGHT;
    
    // Adding oil slicks
    int ChanceOilSlick = max(150 - L * 10, 40);
    if (randInt(0, ChanceOilSlick - 1) == 0)
    {
        int randomX = randInt(LEFT_EDGE, RIGHT_EDGE);
        int oilSize = randInt(2, 5); // Generating random size and X coordinate for new oil slick
        Actor *newOil = new OilSlick(this, getPlayerPtr(), randomX, newAddY, oilSize);
        livingActors.push_back(newOil);
    }
    
    // Adding zombie Peds
    
    int ChanceZombiePed = max(100 - L * 10, 20);
    if (randInt(0, ChanceZombiePed - 1) == 0)
    {
        int zombieX = randInt(0, VIEW_WIDTH);
        Actor *newZombiePed = new ZombiePedestrian(this, getPlayerPtr(), zombieX, newAddY);
        livingActors.push_back(newZombiePed); // Creating new zombiePed and adding it to container
    }
    
    // Adding human Peds
    int ChanceHumanPed = max(200 - L * 10, 30);
    if (randInt(0, ChanceHumanPed - 1) == 0)
    {
        int humanX = randInt(0, VIEW_WIDTH);
        Actor *newHumanPed = new HumanPedestrian(this, getPlayerPtr(), humanX, newAddY);
        livingActors.push_back(newHumanPed); // Creating new humanPed and adding it to container
    }
    
    // Adding HolyWater Goodies
    int ChanceOfHolyWater = 100 + 10 * L;
    if (randInt(0, ChanceOfHolyWater-1) == 0)
    {
        int waterGoodieX = randInt(LEFT_EDGE, RIGHT_EDGE);
        Actor *newWaterGoodie = new HolyWaterGoodie(this, getPlayerPtr(), waterGoodieX, newAddY); // Creating new holy water goodie and adding it to container
        livingActors.push_back(newWaterGoodie);
    }
    
    // Adding Lost Soul Goodies
    if (randInt(0, 100 - 1) == 0)
    {
        int soulGoodieX = randInt(LEFT_EDGE, RIGHT_EDGE);
        Actor *newSoulGoodie = new SoulGoodie(this, getPlayerPtr(), soulGoodieX, newAddY);
        livingActors.push_back(newSoulGoodie); // Creating new souls and adding to container
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
    delete getPlayerPtr(); // Deleting ghostRacer since not in container
    list<Actor*> :: iterator actorIt;
    actorIt = livingActors.begin();
    while (actorIt != livingActors.end())
    {
        delete (*actorIt);
        actorIt = livingActors.erase(actorIt); // Deleting all actors in container
    }
}

StudentWorld::~StudentWorld()
{
    cleanUp(); // Simply calling cleanUp
}
