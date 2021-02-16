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

// NOTE: only used by GhostRacer
void LivingActor::heal(int amount)
{
	m_health += amount;
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
	if (getX() <= ROAD_CENTER - ROAD_WIDTH / 2 /* LEFT ROAD BOUNDARY */)
	{
		if (getDirection() > 90) damage(10);
		setDirection(82);
		getWorld()->playSound(SOUND_VEHICLE_CRASH);
	}
	else if (getX() >= ROAD_CENTER + ROAD_WIDTH / 2 /* RIGHT ROAD BOUNDARY */)
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
				/* Add holy water SPRITE_HEIGHT in front of ghost racer */
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
			if (m_forwardSpeed < 5) m_forwardSpeed++;
			break;
		case KEY_PRESS_DOWN:
			if (m_forwardSpeed > -1) m_forwardSpeed--;
			break;
		}
	}
	move();
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

WhiteBorderLine::WhiteBorderLine(StudentWorld* world, double startX, double startY) :
	BorderLine(IID_WHITE_BORDER_LINE, world, startX, startY)
{

}

WhiteBorderLine::~WhiteBorderLine()
{

}

void WhiteBorderLine::doSomething()
{
	move();
	if (isAlive()) getWorld()->setWhiteBorderY(getY());
}

YellowBorderLine::YellowBorderLine(StudentWorld* world, double startX, double startY) :
	BorderLine(IID_YELLOW_BORDER_LINE, world, startX, startY)
{

}

YellowBorderLine::~YellowBorderLine()
{

}

void YellowBorderLine::doSomething()
{
	move();
}