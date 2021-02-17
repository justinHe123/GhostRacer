#include "Actor.h"
#include "StudentWorld.h"

#include "GameConstants.h"
#include <cmath>
// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

/////////////////////////////////////////////////////////////////////////////
//////////////////////// ACTOR CLASS  IMPLEMENTATION ////////////////////////
/////////////////////////////////////////////////////////////////////////////

Actor::Actor(int imageID, double startX, double startY, int dir, double size, unsigned int depth, StudentWorld* world, double xSpeed, double ySpeed) :
	GraphObject(imageID, startX, startY, dir, size, depth),
	m_alive(true),
	m_world(world),
	m_xspeed(xSpeed),
	m_yspeed(ySpeed)
{

}

Actor::~Actor()
{

}

StudentWorld* Actor::getWorld() const
{
	return m_world;
}

bool Actor::isCollisionAvoidanceWorthy() const
{
	return false;
}

bool Actor::isProjectileVulnerable() const
{
	return false;
}

bool Actor::isAlive() const
{
	return m_alive;
}

double Actor::getXSpeed() const
{
	return m_xspeed;
}

double Actor::getYSpeed() const
{
	return m_yspeed;
}

void Actor::setXSpeed(double speed)
{
	m_xspeed = speed;
}

void Actor::setYSpeed(double speed)
{
	m_yspeed = speed;
}

void Actor::die()
{
	m_alive = false;
}

void Actor::move()
{
	double vert_speed = m_yspeed - getWorld()->getGhostRacer()->getYSpeed();
	double new_y = getY() + vert_speed;
	double new_x = getX() + m_xspeed;
	moveTo(new_x, new_y);
	if (new_x < 0 || new_x > VIEW_WIDTH || new_y < 0 || new_y > VIEW_HEIGHT) die();
}

void Actor::damage(int amount)
{
	die(); // Non-living actors hit by projectiles will be destroyed
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////// LIVING ACTOR IMPLEMENTATION ////////////////////////
/////////////////////////////////////////////////////////////////////////////

LivingActor::LivingActor(int imageID, double startX, double startY, int dir, double size, unsigned int depth,
	int startHealth, StudentWorld* world, double xSpeed, double ySpeed) :
	Actor(imageID, startX, startY, dir, size, depth, world, xSpeed, ySpeed),
	m_health(startHealth)
{

}

LivingActor::~LivingActor()
{

}

bool LivingActor::isCollisionAvoidanceWorthy() const
{
	return true;
}

// NOTE: Currently classifies GhostRacer is projectile vulnerable... not currently an issue as not in container
bool LivingActor::isProjectileVulnerable() const
{
	return true;
}

int LivingActor::getHealth() const
{
	return m_health;
}


void LivingActor::damage(int amount)
{
	m_health -= amount;
	if (m_health > 0)
	{
		makeHurtSound();
	}
	else
	{
		die();
		makeDieSound();
	}
}


/////////////////////////////////////////////////////////////////////////////
///////////////////////// GHOSTRACER IMPLEMENTATION /////////////////////////
/////////////////////////////////////////////////////////////////////////////

GhostRacer::GhostRacer(StudentWorld* world) :
	LivingActor(IID_GHOST_RACER, 128, 32, 90, 4.0, 0, 100, world, 0, 0),
	m_sprays(10),
	m_forwardSpeed(0)
{

}

GhostRacer::~GhostRacer()
{

}

void GhostRacer::move()
{
	double max_shift_per_tick = 4.0;
	static const double PI = 4 * atan(1.0);
	double theta = getDirection() * 1.0 / 360 * 2 * PI;
	double delta_x = cos(theta)*max_shift_per_tick;
	moveTo(getX() + delta_x, getY());
}

void GhostRacer::doSomething()
{
	if (!isAlive()) return;

	int ch;
	if (getX() <= ROAD_CENTER - ROAD_WIDTH / 2)
	{
		if (getDirection() > 90) damage(10);
		setDirection(82);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else if (getX() >= ROAD_CENTER + ROAD_WIDTH / 2)
	{
		if (getDirection() < 90) damage(10);
		setDirection(98);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else if (getWorld()->getKey(ch))
	{
		switch (ch)
		{
		case KEY_PRESS_SPACE:
			if (m_sprays >= 1) {
				double dx, dy;
				int dir = getDirection();
				getPositionInThisDirection(dir, SPRITE_HEIGHT, dx, dy);
				getWorld()->spawnActor(new HolyWaterProjectile(getWorld(), dx, dy, dir));
				getWorld()->playSound(SOUND_PLAYER_SPRAY);
				m_sprays--;
			}
			break;
		case KEY_PRESS_LEFT:
			if (getDirection() < 114) { setDirection(getDirection() + 8); }
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() > 66) { setDirection(getDirection() - 8); }
			break;
		case KEY_PRESS_UP:
			if (m_forwardSpeed < 5) { m_forwardSpeed++; }
			break;
		case KEY_PRESS_DOWN:
			if (m_forwardSpeed > -1) { m_forwardSpeed--; }
			break;
		}
	}

	move();
}

double GhostRacer::getYSpeed() const
{
	static const double PI = 4 * atan(1.0);
	double theta = getDirection() * 1.0 / 360 * 2 * PI;
	return m_forwardSpeed * sin(theta);
}

// TODO: Check if Forward Speed = Vertical Speed
int GhostRacer::getSprays() const
{
	return m_sprays;
}

void GhostRacer::addSprays(int amount)
{
	m_sprays += amount;
}

void GhostRacer::heal(int amount)
{
	if (amount + getHealth() > 100) { amount = 100 - getHealth(); }
	damage(-1*amount);
}

void GhostRacer::spin()
{
	int randDegree = randInt(-11, 20);
	if (randDegree <= 4) randDegree -= 9;
	int newDegree = getDirection() + randDegree;
	if (newDegree > 120) newDegree = 120;
	else if (newDegree < 60) newDegree = 60;
	setDirection(newDegree);
}

void GhostRacer::makeHurtSound() const
{
	// Make no sound
}

void GhostRacer::makeDieSound() const
{
	getWorld()->playSound(SOUND_PLAYER_DIE);
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////// BORDERLINE IMPLEMENTATION /////////////////////////
/////////////////////////////////////////////////////////////////////////////

BorderLine::BorderLine(int imageID, StudentWorld* world, double startX, double startY) :
	Actor(imageID, startX, startY, 0, 2.0, 1, world, 0, -4)
{

}

BorderLine::~BorderLine()
{

}

void BorderLine::doSomething()
{
	move();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////// HOLY WATER PROJECTILE  IMPLEMENTATION ///////////////////
/////////////////////////////////////////////////////////////////////////////

HolyWaterProjectile::HolyWaterProjectile(StudentWorld* world, double startX, double startY, int dir) :
	Actor(IID_HOLY_WATER_PROJECTILE, startX, startY, dir, 1.0, 1, world, 0, 0),
	m_traveled(0)
{

}

HolyWaterProjectile::~HolyWaterProjectile()
{

}

void HolyWaterProjectile::doSomething()
{
	if (!isAlive()) return;

	Actor* a = nullptr;
	if (getWorld()->checkProjectileCollision(this, a)) {
		a->damage(1);
		die();
		return;
	}

	move();
}

void HolyWaterProjectile::move()
{
	m_traveled += SPRITE_HEIGHT;
	if (m_traveled > 160) {
		die();
		return;
	}
	moveForward(SPRITE_HEIGHT);
	double new_x = getX();
	double new_y = getY();
	if (new_x < 0 || new_x > VIEW_WIDTH || new_y < 0 || new_y > VIEW_HEIGHT) die();
}

/////////////////////////////////////////////////////////////////////////////
////////////////////////// HOSTILE  IMPLEMENTATION //////////////////////////
/////////////////////////////////////////////////////////////////////////////

Hostile::Hostile(int imageID, int dir, int size, int startHealth, StudentWorld* world, 
	double startX, double startY, double xSpeed, double ySpeed) :
	LivingActor(imageID, startX, startY, dir, size, 0, startHealth, world, xSpeed, ySpeed),
	m_plandistance(0)
{

}

Hostile::~Hostile()
{

}

void Hostile::doSomething()
{
	if (!isAlive()) return;
	interactWithGhostRacer();
	if (!isAlive()) return;
	move();
	changeMovement();
}

int Hostile::getPlanDistance() const
{
	return m_plandistance;
}

void Hostile::setPlanDistance(int amount)
{
	m_plandistance = amount;
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////// PEDESTRIAN IMPLEMENTATION /////////////////////////
/////////////////////////////////////////////////////////////////////////////

Pedestrian::Pedestrian(int imageID, int size, StudentWorld* world, double startX, double startY)
	: Hostile(imageID, 0, size, 2, world, startX, startY, 0, -4)
{

}

Pedestrian::~Pedestrian()
{

}

void Pedestrian::makeHurtSound() const
{
	getWorld()->playSound(SOUND_PED_HURT);
}

void Pedestrian::makeDieSound() const
{
	getWorld()->playSound(SOUND_PED_DIE);
}

void Pedestrian::changeMovement()
{
	int newPlanDistance = getPlanDistance() - 1;
	setPlanDistance(newPlanDistance);
	if (newPlanDistance > 0) return;
	int randSpeed = randInt(-3, 2);
	if (randSpeed >= 0) randSpeed++;
	setXSpeed(randSpeed);
	setPlanDistance(randInt(4, 32));
	if (randSpeed < 0) setDirection(180);
	else setDirection(0);
}


/////////////////////////////////////////////////////////////////////////////
////////////////////// HUMAN PEDESTRIAN IMPLEMENTATION //////////////////////
/////////////////////////////////////////////////////////////////////////////

HumanPedestrian::HumanPedestrian(StudentWorld* world, double startX, double startY) :
	Pedestrian(IID_HUMAN_PED, 2.0, world, startX, startY)
{

}

HumanPedestrian::~HumanPedestrian()
{

}

void HumanPedestrian::damage(int amount) 
{
	LivingActor::damage(0);
	setXSpeed(getXSpeed() * -1);
	setDirection(getDirection() + 180);
}

void HumanPedestrian::interactWithGhostRacer()
{
	if (getWorld()->checkGhostRacerCollision(this)) {
		getWorld()->getGhostRacer()->die();
		die();
	}
}

/////////////////////////////////////////////////////////////////////////////
///////////////////// ZOMBIE PEDESTRIAN  IMPLEMENTATION /////////////////////
/////////////////////////////////////////////////////////////////////////////

ZombiePedestrian::ZombiePedestrian(StudentWorld* world, double startX, double startY) :
	Pedestrian(IID_ZOMBIE_PED, 3.0, world, startX, startY),
	m_tickstogrunt(0)
{

}

ZombiePedestrian::~ZombiePedestrian()
{

}

void ZombiePedestrian::interactWithGhostRacer()
{
	if (getWorld()->checkGhostRacerCollision(this))
	{
		getWorld()->getGhostRacer()->damage(5);
		damage(2);
		return; // NOTE: should de dead...
	}

	double dx = getX() - getWorld()->getGhostRacer()->getX();
	double dy = getY() - getWorld()->getGhostRacer()->getY();
	if (dx >= -30 && dx <= 30 && dy > 0) {
		setDirection(270);
		if (dx < 0) { setXSpeed(1); } // To the left
		else if (dx > 0) { setXSpeed(-1); } // To the right
		else { setXSpeed(0); } 
		m_tickstogrunt--;
		if (m_tickstogrunt <= 0) {
			getWorld()->playSound(SOUND_ZOMBIE_ATTACK);
			m_tickstogrunt = 20;
		}
	}
}

void ZombiePedestrian::damage(int amount)
{
	LivingActor::damage(amount);
	if (!isAlive()) {
		int rand = randInt(1, 5);
		if (rand == 1) getWorld()->spawnActor(new HealingGoodie(getWorld(), getX(), getY()));
		if (!getWorld()->checkGhostRacerCollision(this)) { getWorld()->increaseScore(150); }
	}
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////// ZOMBIE CAB IMPLEMENTATION /////////////////////////
/////////////////////////////////////////////////////////////////////////////

ZombieCab::ZombieCab(StudentWorld* world, double startX, double startY) :
	Hostile(IID_ZOMBIE_CAB, 90, 4.0, 3, world, startX, startY, 0, 0),
	m_hit(false)
{

}

ZombieCab::~ZombieCab()
{

}

void ZombieCab::damage(int amount)
{
	LivingActor::damage(amount);
	if (!isAlive()) {
		int rand = randInt(1, 5);
		if (rand == 1) getWorld()->spawnActor(new OilSlick(randInt(2, 5), getWorld(), getX(), getY()));
		getWorld()->increaseScore(200);
	}
}

void ZombieCab::changeMovement()
{
	// Adjust speed
	double dv = getYSpeed() - getWorld()->getGhostRacer()->getYSpeed();
	if (dv > 0) {
		// TODO
	}
	else if (dv <= 0) {
		// TODO
	}

	int newPlanDistance = getPlanDistance() - 1;
	setPlanDistance(newPlanDistance);
	if (newPlanDistance > 0) return;
	int randSpeed = randInt(-2, 1);
	if (randSpeed >= 0) randSpeed++;
	setYSpeed(randSpeed);
	setPlanDistance(randInt(4, 32));
}


void ZombieCab::interactWithGhostRacer() 
{
	if (!m_hit && getWorld()->checkGhostRacerCollision(this)) {
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
		getWorld()->getGhostRacer()->damage(20);
		double dx = getX() - getWorld()->getGhostRacer()->getX();
		if (dx < 0) { // To the left
			setXSpeed(-5);
			setDirection(120 + randInt(0, 19));
		}
		else { // To the right
			setXSpeed(5);
			setDirection(60 - randInt(0, 19));
		}
		m_hit = true;
	}
}

void ZombieCab::makeHurtSound() const
{
	getWorld()->playSound(SOUND_VEHICLE_HURT);
}

void ZombieCab::makeDieSound() const
{
	getWorld()->playSound(SOUND_VEHICLE_DIE);
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////// INTERACTABLE IMPLEMENTATION ////////////////////////
/////////////////////////////////////////////////////////////////////////////

Interactable::Interactable(int imageID, int dir, int size, StudentWorld* world, double startX, double startY) :
	Actor(imageID, startX, startY, dir, size, 0, world, 0, -4)
{

}

Interactable::~Interactable()
{

}

void Interactable::doSomething()
{
	if (!isAlive()) return;
	move();
	interactWithGhostRacer();
}

/////////////////////////////////////////////////////////////////////////////
//////////////////// DESTRUCTABLE GOODIE  IMPLEMENTATION ////////////////////
/////////////////////////////////////////////////////////////////////////////

DestructibleGoodie::DestructibleGoodie(int imageID, int dir, int size, StudentWorld* world, double startX, double startY) :
	Interactable(imageID, dir, size,  world, startX, startY)
{

}

DestructibleGoodie::~DestructibleGoodie()
{

}

bool DestructibleGoodie::isProjectileVulnerable() const
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////
///////////////////// HOLY WATER GOODIE  IMPLEMENTATION /////////////////////
/////////////////////////////////////////////////////////////////////////////

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* world, double startX, double startY) :
	DestructibleGoodie(IID_HOLY_WATER_GOODIE, 90, 2.0, world, startX, startY)
{

}

HolyWaterGoodie::~HolyWaterGoodie()
{

}

void HolyWaterGoodie::interactWithGhostRacer()
{
	if (getWorld()->checkGhostRacerCollision(this)) {
		getWorld()->getGhostRacer()->addSprays(10);
		die();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(50);
	}
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////// HEALING GOODIE IMPLEMENTATION ///////////////////////
/////////////////////////////////////////////////////////////////////////////

HealingGoodie::HealingGoodie(StudentWorld* world, double startX, double startY) :
	DestructibleGoodie(IID_HEAL_GOODIE, 0, 1.0, world, startX, startY)
{

}

HealingGoodie::~HealingGoodie()
{

}

void HealingGoodie::interactWithGhostRacer()
{
	if (getWorld()->checkGhostRacerCollision(this)) {
		getWorld()->getGhostRacer()->heal(10);
		die();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(250);
	}
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////// SOUL GOODIE  IMPLEMENTATION ////////////////////////
/////////////////////////////////////////////////////////////////////////////

SoulGoodie::SoulGoodie(StudentWorld* world, double startX, double startY) :
	Interactable(IID_SOUL_GOODIE, 0, 4.0, world, startX, startY)
{

}

SoulGoodie::~SoulGoodie()
{

}

void SoulGoodie::interactWithGhostRacer()
{
	if (getWorld()->checkGhostRacerCollision(this)) {
		getWorld()->addSoul();
		die();
		getWorld()->playSound(SOUND_GOT_SOUL);
		getWorld()->increaseScore(100);
	}
	else { setDirection(getDirection() - 10); }
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////// OIL SLICK  IMPLEMENTATION /////////////////////////
/////////////////////////////////////////////////////////////////////////////

OilSlick::OilSlick(int size, StudentWorld* world, double startX, double startY) :
	Interactable(IID_OIL_SLICK, 0, size, world, startX, startY)
{

}

OilSlick::~OilSlick()
{

}

void OilSlick::interactWithGhostRacer()
{
	if (getWorld()->checkGhostRacerCollision(this)) {
		getWorld()->playSound(SOUND_OIL_SLICK);
		getWorld()->getGhostRacer()->spin();
	}
}