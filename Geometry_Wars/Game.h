#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"
#include "EntityManager.h"

struct Color3
{
	int R, G, B;
};

struct PlayerConfig {
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V;
	float S;
};

struct EnemyConfig {
	int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
	float SMIN, SMAX;
};

struct BulletConfig {
	int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
	float S;
};

class Game
{
	sf::RenderWindow	m_window;		// the window we will draw to
	EntityManager		m_entities;		// vector of entities to maintain
	sf::Font			m_font;			// the font we will use to draw
	sf::Text			m_text;			// the score text to be drawn on screen
	PlayerConfig		m_playerConfig;
	EnemyConfig			m_enemyConfig;
	BulletConfig		m_bulletConfig;
	sf::Clock			m_deltaClock;
	int					m_score = 0;
	int					m_currentFrame = 0;
	bool				m_running = true;
	int					m_lastEnemySpawnTime = 0;
	int					m_lastSpecialWeaponTime = 0;
	bool				m_spawnEnemies = true;
	bool				m_movementBool = true;
	bool				m_collisionBool = true;
	bool				m_lifespanBool = true;
	bool				m_guiBool = true;
	bool				m_renderingBool = true;
	int					m_guiTabInt = 0;
	bool				m_paused = false;
	std::shared_ptr<Entity> m_player;

public:
	Game(const std::string& config);
	void init(const std::string& path);
	void run();
	void setPaused(bool paused);
	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> e);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target);
	void spawnSpecialWeapon(std::shared_ptr<Entity> entity);
	// Systems
	void sMovement();
	void sLifeSpan();
	void sCollision();
	void sEnemySpawner();
	void sGUI();
	void sRender();
	void sUserInput();
};