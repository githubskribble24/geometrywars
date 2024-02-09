#pragma once

#include "Components.h"
#include <memory>
#include <string>

// Factory pattern
class Entity
{
	// we put the constructor inside of private so that we can not use this anywhere
	// BESIDES EntityManager class 
	// this is the only place that can make an Entity object
	friend class EntityManager;

	bool	m_active	= true;
	size_t	m_id = 0;
	std::string m_tag = "default";

	// Constructor and destructor
	// we put the constructor & destructor inside of private so that we can not use this anywhere
	Entity(const size_t id, const std::string& tag);

public:

	// component pointers -> these are all nullptr by default
	std::shared_ptr<CTransform>	cTransform;
	std::shared_ptr<CShape>	cShape;
	std::shared_ptr<CCollision>	cCollision;
	std::shared_ptr<CInput>	cInput;
	std::shared_ptr<CScore>	cScore;
	std::shared_ptr<CLifespan>	cLifespan;

	// private member access functions
	bool isActive() const;
	const std::string& tag() const;
	const size_t id() const;
	void destroy();
};

