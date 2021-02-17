#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>

#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class Actor;
class GhostRacer;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();

    GhostRacer* getGhostRacer() const;
    bool checkProjectileCollision(Actor* projectile, Actor*& result); // NOTE: Cannot make const for some reason
    bool checkGhostRacerCollision(Actor* a) const;
    Actor* closestCAV(Actor* compare, double y, int direction, int leftEdge);
    int determineLeftEdge(double x) const;

    void addSoul();
    void spawnActor(Actor* a);

private:
    GhostRacer* m_ghostracer;
    std::list<Actor*> m_actors;
    int m_saved;
    int m_whitebordery;
    int m_bonus;

    bool checkCollision(Actor* first, Actor* second) const;
};

#endif // STUDENTWORLD_H_
