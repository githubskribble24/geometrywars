#include "EntityManager.h"
#include <iostream>

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
	// we do not want a reference because we will be clearing the list at the end of the function
	// if we pass a reference this means we will get rid of the entities we added into the entity list
	for (auto entity : m_entitiesToAdd)
	{
		m_entities.push_back(entity);

		// if the tag is not in the map yet create a tag for it and then insert
		//if (!m_entityMap.contains(entity->tag()))
		//{
		//	EntityVec newMapInsert;
		//	m_entityMap.insert(std::map<std::string, EntityVec>::value_type(entity->tag(), newMapInsert));
		//	// map.find()->first 
		//	// returns the string
		//	EntityVec& entValue = m_entityMap.find(entity->tag())->second;
		//	entValue.push_back(entity);
		//}
		//else
		//{
		//	EntityVec& entValue = m_entityMap.find(entity->tag())->second;
		//	entValue.push_back(entity);
		//}
		// above is not needed!
		// From CPPreference: Inserts value_type(key, T()) if the key does not exist.
		// https://en.cppreference.com/w/cpp/container/map/operator_at
		m_entityMap[entity->tag()].push_back(entity);
	}

	// remove dead entities from the vector of all entities
	removeDeadEntities(m_entities);

	// remove dead entities from each vector in the entity map
	// Modern C++20 way of iterating through [key, value] pairs in map
	for (auto& [tag, entityVec] : m_entityMap)
	{
			
		removeDeadEntities(entityVec);
	}

	m_entitiesToAdd.clear();
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	EntityVec deadEntities;
	for (auto& e : vec)
	{
		if (!e->isActive())
			deadEntities.push_back(e);
		if (e->cLifespan == nullptr)
			continue;
		if (e->cLifespan->remaining <= 0)
			deadEntities.push_back(e);
	}

	for (auto& e : deadEntities)
	{
		for (auto& ent : vec)
		{
			if (ent->id() == e->id())
			{
				// https://dev.to/pgradot/lets-try-c20-erase-elements-in-a-container-with-stderase-2d08
				std::erase(vec, ent);
				break;
			}
		}
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

	m_entitiesToAdd.push_back(entity);

	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}

const std::map<std::string, EntityVec>& EntityManager::getEntityMap()
{
	return m_entityMap;
}