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
	// TODO: Consider moving xSpeed and ySpeed to a derived class
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
	double getYSpeed() const;

	void die();
	void setXSpeed(double speed);
	void setYSpeed(double speed);
	virtual void move();
	virtual void damage(int amount);

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
	virtual bool isProjectileVulnerable() const;

	int getHealth() const;

	virtual void damage(int amount);

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

	virtual void doSomething();
	virtual void move();

	int getSprays() const;

	void addSprays(int amount);
	void heal(int amount);
	void spin();

private:
	int m_sprays;
	virtual void makeHurtSound() const;
	virtual void makeDieSound() const;
};

class BorderLine : public Actor
{
public:
	BorderLine(int imageID, StudentWorld* world, double startX, double startY);
	virtual ~BorderLine();

	virtual void doSomething();
};

class HolyWaterProjectile : public Actor
{
public:
	HolyWaterProjectile(StudentWorld* world, double startX, double startY, int dir);
	virtual ~HolyWaterProjectile();

	virtual void doSomething();
	virtual void move();
private:
	int m_traveled;
};

class Hostile : public LivingActor
{
public:
	Hostile(int imageID, int dir, int size, int startHealth, StudentWorld* world, 
		double startX, double startY, double xSpeed, double ySpeed);
	virtual ~Hostile();

	virtual void doSomething();

	int getPlanDistance() const;

	void setPlanDistance(int amount);

private:
	int m_plandistance;
	virtual void interactWithGhostRacer() = 0;
	virtual void changeMovement() = 0;
};

class Pedestrian : public Hostile
{
public:
	Pedestrian(int imageID, int size, StudentWorld* world, double startX, double startY);
	virtual ~Pedestrian();

private:
	virtual void changeMovement();
	virtual void makeHurtSound() const;
	virtual void makeDieSound() const;
};

class HumanPedestrian : public Pedestrian
{
public:
	HumanPedestrian(StudentWorld* world, double startX, double startY);
	virtual ~HumanPedestrian();

	virtual void damage(int amount);

private:
	virtual void interactWithGhostRacer();
};

class ZombiePedestrian : public Pedestrian
{
public:
	ZombiePedestrian(StudentWorld* world, double startX, double startY);
	virtual ~ZombiePedestrian();

	virtual void damage(int amount);

private:
	int m_tickstogrunt;
	virtual void interactWithGhostRacer();
};

class ZombieCab : public Hostile
{
public:
	ZombieCab(StudentWorld* world, double startX, double startY, double startSpeed);
	virtual ~ZombieCab();

	virtual void damage(int amount);

private:
	bool m_hit;
	virtual void interactWithGhostRacer();
	virtual void changeMovement();
	virtual void makeHurtSound() const;
	virtual void makeDieSound() const;
};

class Interactable : public Actor
{
public:
	Interactable(int imageID, int dir, int size, StudentWorld* world, double startX, double startY);
	virtual ~Interactable();

	virtual void doSomething();

private:
	virtual void interactWithGhostRacer() = 0;
};

class DestructibleGoodie : public Interactable
{
public:
	DestructibleGoodie(int imageID, int dir, int size, StudentWorld* world, double startX, double startY);
	virtual ~DestructibleGoodie();

	virtual bool isProjectileVulnerable() const;
};

class HolyWaterGoodie : public DestructibleGoodie
{
public:
	HolyWaterGoodie(StudentWorld* world, double startX, double startY);
	virtual ~HolyWaterGoodie();

private:
	virtual void interactWithGhostRacer();
};

class HealingGoodie : public DestructibleGoodie
{
public:
	HealingGoodie(StudentWorld* world, double startX, double startY);
	virtual ~HealingGoodie();

private: 
	virtual void interactWithGhostRacer();
};

class SoulGoodie : public Interactable
{
public:
	SoulGoodie(StudentWorld* world, double startX, double startY);
	virtual ~SoulGoodie();

private:
	virtual void interactWithGhostRacer();
};

class OilSlick : public Interactable
{
public:
	OilSlick(int size, StudentWorld* world, double startX, double startY);
	virtual ~OilSlick();

private:
	virtual void interactWithGhostRacer();
};

#endif // ACTOR_H_
