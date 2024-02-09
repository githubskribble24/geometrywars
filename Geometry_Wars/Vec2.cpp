#include "Vec2.h"
#include <math.h>

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin)
{

}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator / (float val) const
{
	return Vec2(x / val, y / val);
}

Vec2 Vec2::operator * (float val) const
{
	return Vec2(x * val, y * val);
}

void Vec2::operator += (const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator /= (float val)
{
	x /= val;
	y /= val;
}

void Vec2::operator *= (float val)
{
	x *= val;
	y *= val;
}

float Vec2::dist(const Vec2& rhs) const
{
	// also called length
	//float x = this->x - rhs.x;
	//float y = this->y - rhs.x;
	//float length = sqrtf(x * x + y * y);

	return (rhs - *this).length();
}

float Vec2::length() const
{
	return sqrtf(x * x + y * y);
}