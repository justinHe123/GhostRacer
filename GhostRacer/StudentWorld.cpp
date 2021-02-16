#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <list>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath) :
    GameWorld(assetPath),
    m_saved(0),
    m_whitebordery(0),
    m_bonus(5000)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

void StudentWorld::setWhiteBorderY(int y)
{
    m_whitebordery = y;
}

void StudentWorld::addSoul()
{
    m_saved++;
}

// Initialize data structures
int StudentWorld::init()
{
    m_whitebordery = 0;
    m_saved = 0;
    m_bonus = 5000;

    // add ghost racer
    m_ghostracer = new GhostRacer(this);
    
    int n = VIEW_HEIGHT / SPRITE_HEIGHT;
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    
    // add yellow border lines
    int y = 0;
    for (int i = 0; i < n; i++) {
        m_actors.push_back(new YellowBorderLine(this, LEFT_EDGE, y));
        m_actors.push_back(new YellowBorderLine(this, RIGHT_EDGE, y));
        y += SPRITE_HEIGHT;
    }

    // add white border lines
    int m = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    y = 0;
    for (int i = 0; i < m; i++) {
        m_actors.push_back(new WhiteBorderLine(this, LEFT_EDGE + ROAD_WIDTH/3, y));
        m_actors.push_back(new WhiteBorderLine(this, RIGHT_EDGE - ROAD_WIDTH/3, y));
        y += 4 * SPRITE_HEIGHT;
    }

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.

    // Move all actors
    m_ghostracer->doSomething();
    if (!m_ghostracer->isAlive()) { return GWSTATUS_PLAYER_DIED; }

    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* a = *it;
        if (a->isAlive()) {
            a->doSomething();
            if (!m_ghostracer->isAlive()) { return GWSTATUS_PLAYER_DIED; }
            if (m_saved >= GameWorld::getLevel() * 2 + 5) {
                // Advance to next level
                // Award bonus points to player
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }

    // delete dead actors (WATCH OUT FOR DELETION GOTCHA)
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ) {
        Actor* a = *it;
        if(!a->isAlive()) { 
            delete a;
            it = m_actors.erase(it); 
        }
        else { it++; }
    }

    // TODO: Potentially add new actors 
    // Add new road markers
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    int delta_y = new_border_y - m_whitebordery;
    std::cerr << delta_y << std::endl;
    if (delta_y >= SPRITE_HEIGHT)
    {
        m_actors.push_back(new YellowBorderLine(this, LEFT_EDGE, new_border_y));
        m_actors.push_back(new YellowBorderLine(this, RIGHT_EDGE, new_border_y));
    }
    if (delta_y >= 4 * SPRITE_HEIGHT)
    {
        m_actors.push_back(new WhiteBorderLine(this, LEFT_EDGE + ROAD_WIDTH / 3, new_border_y));
        m_actors.push_back(new WhiteBorderLine(this, RIGHT_EDGE - ROAD_WIDTH / 3, new_border_y));
    }

    // Update game status
    // TODO: Use StringStream
    ostringstream oss;
    oss << setw(7) << "Score: " << setw(5) << getScore();
    oss << setw(12) << " Souls2Save: " << setw(3) << GameWorld::getLevel() * 2 + 5 - m_saved;
    oss << setw(8) << " Lives: " << setw(1) << getLives();
    oss << setw(9) << " Health: " << setw(3) << m_ghostracer->getHealth();
    oss << setw(9) << " Sprays: " << setw(3) << m_ghostracer->getSprays();
    oss << setw(8) << " Bonus: " << setw(4) << m_bonus;
    setGameStatText(oss.str());

    m_bonus--;

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ) {
        delete *it;
        it = m_actors.erase(it);
    }
    delete m_ghostracer;
}

GhostRacer* StudentWorld::getGhostRacer() const
{
    return m_ghostracer;
}