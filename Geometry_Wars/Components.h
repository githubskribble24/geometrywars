#pragma once

#include "Vec2.h"
// #include <SFML/Graphics.hpp>
#include <SFML/Graphics.hpp>

// Components are pure data

// this tells us where it is and the speed
class CTransform
{
public:
	Vec2 pos      = { 0.0f, 0.0f };
	Vec2 velocity = { 0.0f, 0.0f };
	float angle   = 0;

	CTransform(const Vec2& pos, const Vec2& velocity, float angle)
		: pos{ pos }, velocity{ velocity }, angle(angle)
	{}
};

class CShape
{
public:
	sf::CircleShape circle;

	CShape(float radius, int points, const sf::Color & fill, const sf::Color & outline, float thickness)
		: circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(thickness);
		circle.setOrigin(radius, radius);
	}
};

// this tells us how big it is
class CCollision
{
public:
	float radius = 0;
	CCollision(float r)
		: radius(r)
	{}
};

// score component for entity add this to score when killing entity
class CScore
{
public:
	int score = 0;

	CScore(int s)
		: score(s)
	{}
};


class CLifespan
{
public:
	// every frame take from remaining variable
	int remaining = 0;		// amount lifespan remaining on entity
	int total	  = 0;		// the total initial amount of lifespan
	CLifespan(int total)
		: remaining(total), total(total)
	{}
};


class CInput
{
public:
	bool up		= false;
	bool left	= false;
	bool right	= false;
	bool down	= false;
	bool shoot	= false;

	CInput() {}
};

// add new components here