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
    m_ghostracer(nullptr),
    m_saved(0),
    m_bonus(5000),
    m_whitebordery(0)
{

}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

// Initialize data structures
int StudentWorld::init()
{
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
        spawnActor(new BorderLine(IID_YELLOW_BORDER_LINE, this, LEFT_EDGE, y));
        spawnActor(new BorderLine(IID_YELLOW_BORDER_LINE, this, RIGHT_EDGE, y));
        y += SPRITE_HEIGHT;
    }

    // add white border lines
    int m = VIEW_HEIGHT / (4 * SPRITE_HEIGHT);
    y = 0;
    for (int i = 0; i < m; i++) {
        spawnActor(new BorderLine(IID_WHITE_BORDER_LINE, this, LEFT_EDGE + ROAD_WIDTH/3.0, y));
        spawnActor(new BorderLine(IID_WHITE_BORDER_LINE, this, RIGHT_EDGE - ROAD_WIDTH/3.0, y));
        y += 4 * SPRITE_HEIGHT;
    }

    m_whitebordery = y - 4 * SPRITE_HEIGHT;

    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // Move all actors
    m_ghostracer->doSomething();
    if (!m_ghostracer->isAlive()) { 
        decLives();
        return GWSTATUS_PLAYER_DIED; 
    }

    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* a = *it;
        if (a->isAlive()) {
            a->doSomething();
            if (!m_ghostracer->isAlive()) { 
                decLives();
                return GWSTATUS_PLAYER_DIED; 
            }
            if (m_saved >= getLevel() * 2 + 5) {
                increaseScore(m_bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }

    // delete dead actors
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); ) {
        Actor* a = *it;
        if(!a->isAlive()) { 
            delete a;
            it = m_actors.erase(it); 
        }
        else { it++; }
    }

    // Potentially add new actors 
    // Add new road markers
    m_whitebordery += -4 - m_ghostracer->getYSpeed();
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    int delta_y = new_border_y - m_whitebordery;
    if (delta_y >= SPRITE_HEIGHT)
    {
        spawnActor(new BorderLine(IID_YELLOW_BORDER_LINE, this, LEFT_EDGE, new_border_y));
        spawnActor(new BorderLine(IID_YELLOW_BORDER_LINE, this, RIGHT_EDGE, new_border_y));
    }
    if (delta_y >= 4 * SPRITE_HEIGHT)
    {
        spawnActor(new BorderLine(IID_WHITE_BORDER_LINE, this, LEFT_EDGE + ROAD_WIDTH / 3.0, new_border_y));
        spawnActor(new BorderLine(IID_WHITE_BORDER_LINE, this, RIGHT_EDGE - ROAD_WIDTH / 3.0, new_border_y));
        m_whitebordery = new_border_y;
    }

    // Add zombie cab
    int chance = max(100 - getLevel() * 10, 20);

    if (randInt(0, chance - 1) == 0) {
        // unholy code to determine spawn lane
        int edges[3] = { LEFT_EDGE, LEFT_EDGE + ROAD_WIDTH / 3, LEFT_EDGE + ROAD_WIDTH * 2 / 3 };
        int i = randInt(1, 3);
        int sum = 0;
        int edge;
        Actor* a;
        while (sum != 6) {
            edge = edges[i - 1];
            a = closestCAV(nullptr, 0, 1, edge); // Closest CAV above y = 0 in the chosen lane
            if (a == nullptr || a->getY() > VIEW_HEIGHT / 3) {
                spawnActor(new ZombieCab(this, edge + ROAD_WIDTH / 6.0, 
                    SPRITE_HEIGHT / 2.0, m_ghostracer->getYSpeed() + randInt(2, 4)));
                break;
            }
            a = closestCAV(nullptr, VIEW_HEIGHT, -1, edge); // Closest CAV below y = VIEW_HEIGHT in the chosen lane
            if (a == nullptr || a->getY() < 2 * VIEW_HEIGHT / 3) {
                spawnActor(new ZombieCab(this, edge + ROAD_WIDTH / 6.0,
                    VIEW_HEIGHT - SPRITE_HEIGHT / 2.0, m_ghostracer->getYSpeed() - randInt(2, 4)));
                break;
            }

            sum += i;
            if (sum == i) {
                i = randExcept(i);
            }
            else if (6 - sum > 0) {
                i = 6 - sum;
            }
        }
        
    }

    // Add oil slick
    chance = max(150 - getLevel() * 10, 40);
    if (randInt(0, chance - 1) == 0) {
        spawnActor(new OilSlick(randInt(2, 5), this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }

    // Add zombie ped
    chance = max(100 - getLevel() * 10, 20);
    if (randInt(0, chance - 1) == 0) {
        spawnActor(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }

    // Add human ped
    chance = max(200 - getLevel() * 10, 30);
    if (randInt(0, chance - 1) == 0) {
        spawnActor(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
    }

    // Add holy water refill
    chance = 100 + 10 * getLevel();
    if (randInt(0, chance - 1) == 0) {
        spawnActor(new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }

    // Add lost soul
    chance = 100;
    if (randInt(0, chance - 1) == 0) {
        spawnActor(new SoulGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
    }


    // Update game status
    ostringstream oss;
    oss << setw(7) << "Score: " << setw(5) << getScore();
    oss << setw(6) << " Lvl: " << setw(1) << getLevel();
    oss << setw(12) << " Souls2Save: " << setw(3) << getLevel() * 2 + 5 - m_saved;
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

bool StudentWorld::checkCollision(Actor* a, Actor* b) const
{
    int delta_x = a->getX() - b->getX();
    if (delta_x < 0) delta_x *= -1;
    int delta_y = a->getY() - b->getY();
    if (delta_y < 0) delta_y *= -1;
    int radius_sum = a->getRadius() + b->getRadius();
    return delta_x < radius_sum * 0.25 && delta_y < radius_sum * 0.6;
}

bool StudentWorld::checkProjectileCollision(Actor* projectile, Actor* &result)
{
    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* a = *it;
        if (a->isProjectileVulnerable() && checkCollision(projectile, a)) {
            result = a; 
            return true;
        }
    }
    return false;
}

bool StudentWorld::checkGhostRacerCollision(Actor* a) const
{
    return checkCollision(a, m_ghostracer);
}

Actor* StudentWorld::closestCAV(Actor* compare, double y, int direction, int leftEdge)
// direction = 1 means above, direction = -1 means below
{
    double minimum = VIEW_HEIGHT;
    Actor* a = nullptr; // in case no CAVs
    int rightEdge = leftEdge + ROAD_WIDTH / 3;

    double dy = direction*(m_ghostracer->getY() - y);
    if (dy > 0 && m_ghostracer->getX() > leftEdge && m_ghostracer->getX() < rightEdge) { 
        a = m_ghostracer;
        minimum = dy; 
    }

    for (list<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++) {
        Actor* b = *it;
        if (b->isCollisionAvoidanceWorthy() && b != compare && b->getX() > leftEdge && b->getX() < rightEdge) {
            dy = direction*(b->getY() - y);
            if (dy >= 0 && dy < minimum) {
                a = b;
                minimum = dy;
            }
        }
    }
    return a;
}

int StudentWorld::determineLeftEdge(double x) const
{
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH / 2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH / 2;
    int MID_LEFT_EDGE = LEFT_EDGE + ROAD_WIDTH / 3;
    int MID_RIGHT_EDGE = RIGHT_EDGE - ROAD_WIDTH / 3;
    if (x >= LEFT_EDGE && x < MID_LEFT_EDGE) return LEFT_EDGE; // Left lane
    if (x >= MID_LEFT_EDGE && x < MID_RIGHT_EDGE) return MID_LEFT_EDGE; // Center lane
    if (x >= MID_RIGHT_EDGE && x < RIGHT_EDGE) return MID_RIGHT_EDGE; // Right lane
    return -1; // not within the lanes
}

void StudentWorld::addSoul()
{
    m_saved++;
}

void StudentWorld::spawnActor(Actor* a)
{
    m_actors.push_back(a);
}

int StudentWorld::randExcept(int i) const
{
    if (i == 1) {
        return randInt(2, 3);
    }
    else if (i == 2) {
        int possible[2] = { 1, 3 };
        return possible[randInt(0, 1)];
    }
    else {
        return randInt(1, 2);
    }
}