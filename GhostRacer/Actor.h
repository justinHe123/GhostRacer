#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// Types of Actors:
// Ghost Racer
// Human pedestrians
// Zombie pedestrians
// Zombie cabs
// Oil slicks
// Holy water projectiles(fired by Aisha from Ghost Racer)
// Border lines(yellowand white)
// Lost souls
// Healing goodies
// Holy water bottle(refill) goodies

class StudentWorld;

class Actor : public GraphObject
{
public:
	// Constructor
	Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, StudentWorld* world, double xSpeed, double ySpeed);
	virtual ~Actor();

	// move method: pure virtual
	virtual void doSomething() = 0;

	// methods to determine type of actor
	virtual bool isCollisionAvoidanceWorthy() const;
	virtual bool isProjectileVulnerable() const;

	StudentWorld* getWorld() const;
	bool isAlive() const;
	double getXSpeed() const;
	virtual double getYSpeed() const;

	void die();
	void setXSpeed(double speed);
	void setYSpeed(double speed);
	virtual void move();

private:
	bool m_alive;
	StudentWorld* m_world;
	double m_xspeed;
	double m_yspeed;
};

class LivingActor : public Actor
{
public:
	LivingActor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, 
		int startHealth, StudentWorld* world, double xSpeed, double ySpeed);
	virtual ~LivingActor();

	virtual bool isCollisionAvoidanceWorthy() const;
	bool LivingActor::isProjectileVulnerable() const;

	int getHealth() const;

	void heal(int amount);
	void damage(int amount);

private:
	int m_health;
	virtual void makeHurtSound() const = 0;
	virtual void makeDieSound() const = 0;
};

class GhostRacer : public LivingActor
{
public:
	GhostRacer(StudentWorld* world);
	virtual ~GhostRacer();

	int getSprays() const;
	virtual double getYSpeed() const;

	virtual void doSomething();
	virtual void move();

private:
	int m_sprays;
	double m_forwardSpeed;
	virtual void makeHurtSound() const;
	virtual void makeDieSound() const;
};

class BorderLine : public Actor
{
public:
	BorderLine(int imageID, StudentWorld* world, double startX, double startY);
	virtual ~BorderLine();
};

class WhiteBorderLine : public BorderLine
{
public:
	WhiteBorderLine(StudentWorld* world, double startX, double startY);
	virtual ~WhiteBorderLine();

	virtual void doSomething();
};

class YellowBorderLine : public BorderLine
{
public:
	YellowBorderLine(StudentWorld* world, double startX, double startY);
	virtual ~YellowBorderLine();

	virtual void doSomething();
};

#endif // ACTOR_H_
