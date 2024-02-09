#include "Game.h"

#include <fstream>
#include <istream>
#include <format>
#include <cstdlib>
#include <ctime>

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "imgui/imgui_stdlib.h"
#include "EntityManager.h"

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& path)
{
	// set up default window parameters
	std::srand(std::time(NULL));

	std::ifstream fin(path);

	std::string buf;
	int width{ 0 }, height{ 0 }, fps{ 0 }, fullscreenMode{ 0 };

	while (fin >> buf)
	{
		if (buf == "Window")
		{
			fin >> width >> height >> fps >> fullscreenMode;
			continue;
		}

		else if (buf == "Font")
		{
			std::string filePath;
			int fontSize;
			Color3 fontColor;
			fin >> filePath >> fontSize >> fontColor.R >> fontColor.G >> fontColor.B;

			if (!m_font.loadFromFile(filePath))
			{
				std::cout << "Could not load font file\n";
				exit(-1);
			}

			m_text.setFont(m_font);
			m_text.setString("Score: 0");
			m_text.setFillColor(sf::Color(fontColor.R, fontColor.G, fontColor.B));
			m_text.setPosition(10, 10);

			continue;
		}

		else if (buf == "Player")
		{
			int shapeRadius{ 0 };
			int collisionRadius{ 0 };
			float speed{ 0 };
			Color3 fillColor{ 0 };
			Color3 outlineColor{ 0 };
			int outlineThickness{ 0 };
			int shapeVertices{ 0 };

			fin >> shapeRadius >> collisionRadius >> speed >> fillColor.R >> fillColor.G >> fillColor.B >> outlineColor.R >> outlineColor.G >> outlineColor.B
				>> outlineThickness >> shapeVertices;

			m_playerConfig.SR = shapeRadius;
			m_playerConfig.CR = collisionRadius;
			m_playerConfig.S = speed;
			m_playerConfig.FR = fillColor.R;
			m_playerConfig.FG = fillColor.G;
			m_playerConfig.FB = fillColor.B;
			m_playerConfig.OR = outlineColor.R;
			m_playerConfig.OG = outlineColor.G;
			m_playerConfig.OB = outlineColor.B;
			m_playerConfig.OT = outlineThickness;
			m_playerConfig.V = shapeVertices;
			continue;
		}

		else if (buf == "Enemy")
		{
			int shapeRadius{ 0 };
			int collisionRadius{ 0 };
			float speedMIN{ 0 };
			float speedMAX{ 0 };
			Color3 outlineColor{ 0 };
			int outlineThickness{ 0 };
			int minV{ 0 };
			int maxV{ 0 };
			int smallLifeSpan{ 0 };
			int spawnInterval{ 0 };

			fin >> shapeRadius >> collisionRadius >> speedMIN >> speedMAX >> outlineColor.R >> outlineColor.G >> outlineColor.B
				>> outlineThickness >> minV >> maxV >> smallLifeSpan >> spawnInterval;

			m_enemyConfig.SR = shapeRadius;
			m_enemyConfig.CR = collisionRadius;
			m_enemyConfig.SMIN = speedMIN;
			m_enemyConfig.SMAX = speedMAX;
			m_enemyConfig.OR = outlineColor.R;
			m_enemyConfig.OG = outlineColor.G;
			m_enemyConfig.OB = outlineColor.B;
			m_enemyConfig.OT = outlineThickness;
			m_enemyConfig.VMIN = minV;
			m_enemyConfig.VMAX = maxV;
			m_enemyConfig.L = smallLifeSpan;
			m_enemyConfig.SI = spawnInterval;
			continue;
		}

		else if (buf == "Bullet")
		{
			int shapeRadius{ 0 };
			int collisionRadius{ 0 };
			float speed{ 0 };
			Color3 fillColor{ 0 };
			Color3 outlineColor{ 0 };
			int outlineThickness{ 0 };
			int shapeVertices{ 0 };
			int smallLifeSpan{ 0 };

			fin >> shapeRadius >> collisionRadius >> speed >> fillColor.R >> fillColor.G >> fillColor.B >> outlineColor.R >> outlineColor.G >> outlineColor.B
				>> outlineThickness >> shapeVertices >> smallLifeSpan;

			m_bulletConfig.SR = shapeRadius;
			m_bulletConfig.CR = collisionRadius;
			m_bulletConfig.S = speed;
			m_bulletConfig.FR = fillColor.R;
			m_bulletConfig.FG = fillColor.G;
			m_bulletConfig.FB = fillColor.B;
			m_bulletConfig.OR = outlineColor.R;
			m_bulletConfig.OG = outlineColor.G;
			m_bulletConfig.OB = outlineColor.B;
			m_bulletConfig.OT = outlineThickness;
			m_bulletConfig.V = shapeVertices;
			m_bulletConfig.L = smallLifeSpan;
			continue;
		}
		else
		{
			std::cout << "Something went wrong while trying to parse the config.\nPlease check the config for anythings that should not be in there.\n";
			exit(-1);
		}
	}
	if (fullscreenMode)
		m_window.create(sf::VideoMode(), "Geometry Wars - Game", sf::Style::Fullscreen);
	else
		m_window.create(sf::VideoMode(width, height), "Geometry Wars - Game");
	m_window.setFramerateLimit(fps);

	ImGui::SFML::Init(m_window);

	spawnPlayer();
}

void Game::run()
{
	while (m_running)
	{
		m_currentFrame++;
		// update the entity manager
		m_entities.update();

		// required	update call to ImGui
		ImGui::SFML::Update(m_window, m_deltaClock.restart());



		if (m_spawnEnemies)
		{
			sEnemySpawner();
		}
		if (!m_paused)
			if (m_movementBool)
				sMovement();
		if (m_collisionBool)
			sCollision();
		if (m_lifespanBool)
			sLifeSpan();
		sUserInput();
		if (m_guiBool)
			sGUI();
		if (m_renderingBool)
			sRender();

	}
}

void Game::setPaused(bool paused)
{
	// TODO
}

void Game::spawnPlayer()
{
	// We create every entity by calling EntityManager.addEntity(tag);
	// This returns a std::shared_ptr<Entity>, so we use 'auto' to save typing
	auto entity = m_entities.addEntity("player");

	sf::Vector2u windowSize = m_window.getSize();

	float startX = windowSize.x / 2;
	float startY = windowSize.y / 2;

	// Give this entity a Transform so it spawns at (x, y) with velocity (1,1) and angle 0
	entity->cTransform = std::make_shared<CTransform>(Vec2(startX, startY), Vec2(m_playerConfig.S, m_playerConfig.S), 0.0f);

	// The entity's shape will have a radius 32, 8 sides (vertices), dark grey fill, and red outline thickness of 4
	entity->cShape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V, sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB), sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB), m_playerConfig.OT);

	// Add an input component to the player so that we can use inputs
	entity->cInput = std::make_shared<CInput>();

	// Since we want this Entity to be our player, set our Game's player variable to be this Entity
	// This goes slightly against the EntityManager paradigm, but we use the player so much it's worth it
	m_player = entity;
}

// spawn an enemy at a random position
void Game::spawnEnemy()
{
	auto entity = m_entities.addEntity("enemy");
	sf::Vector2u windowSize = m_window.getSize();
	std::rand();
	int randomX = m_enemyConfig.CR + (std::rand() % windowSize.x);
	int randomY = m_enemyConfig.CR + (std::rand() % windowSize.y);
	float randomSpeed = ((float)std::rand() / (float)RAND_MAX) * m_enemyConfig.SMAX;
	if (randomSpeed < m_enemyConfig.SMIN)
		randomSpeed + m_enemyConfig.SMIN;

	entity->cTransform = std::make_shared<CTransform>(Vec2(randomX, randomY), Vec2(randomSpeed, randomSpeed), 0.0f);

	int vertices = m_enemyConfig.VMIN + (std::rand() % m_enemyConfig.VMAX);
	sf::Color fillColor((int)(1 + std::rand() % 255), (int)(1 + std::rand() % 255), (int)(1 + std::rand() % 255));
	sf::Color outlineColor((int)(1 + std::rand() % 255), (int)(1 + std::rand() % 255), (int)(1 + std::rand() % 255));

	entity->cShape = std::make_shared<CShape>(m_enemyConfig.SR, vertices, fillColor, outlineColor, m_enemyConfig.OT);

	entity->cScore = std::make_shared<CScore>(vertices*100);

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	int vertices = e->cShape->circle.getPointCount();
	int degreeIntervals = 360 / (vertices * 10);

	// get the length of the vector
	double L = std::sqrtf(e->cTransform->pos.x * e->cTransform->pos.x + e->cTransform->pos.y * e->cTransform->pos.y);
	// normalize
	Vec2 normalizedEnemy(e->cTransform->pos.x / L, e->cTransform->pos.y / L);

	float angle = 0;

	for (size_t i = 0; i < vertices; i++)
	{

		double radians{ angle * 3.14159265359 / 180.0 };

		Vec2 velocity(
			normalizedEnemy.x * cos(radians) + normalizedEnemy.y * sin(radians),
			normalizedEnemy.x * sin(radians) + normalizedEnemy.y * cos(radians)
		);

		// get the lenght of the vector
		float L = { sqrtf(velocity.x * velocity.x + velocity.y * velocity.y) };

		// normalize the vector
		Vec2 normalizedVector(velocity.x / L, velocity.y / L);
		
		// scale the normalized vector by the parents velocity
		Vec2 newVelocity(normalizedVector.x * velocity.x, normalizedVector.y * velocity.y);

		auto enemyEnt = m_entities.addEntity("smallenemy");
		enemyEnt->cTransform = std::make_shared<CTransform>(Vec2(e->cTransform->pos.x, e->cTransform->pos.y), newVelocity, 0);
		enemyEnt->cShape = std::make_shared<CShape>(e->cShape->circle.getRadius() / 2, vertices, e->cShape->circle.getFillColor(), e->cShape->circle.getOutlineColor(), m_enemyConfig.OT);
		enemyEnt->cScore = std::make_shared<CScore>(vertices * 100 * 2);
		enemyEnt->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);

		angle += 360 / vertices;
	}
}


// target = mouse click (x, y)
void Game::spawnBullet(std::shared_ptr<Entity> entity, const Vec2& target)
{
	auto bulletEnt = m_entities.addEntity("bullet");

	bulletEnt->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
	bulletEnt->cTransform = std::make_shared<CTransform>(Vec2(entity->cTransform->pos.x, entity->cTransform->pos.y), Vec2(m_bulletConfig.S, m_bulletConfig.S), 0.0f);
	bulletEnt->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);


	// Doing it this way we avoid sin() and cosin()
	// this way is faster
	// D has some length L, we need it to be S
	// D is the distance in vector from the target to the player
	Vec2 D = target - bulletEnt->cTransform->pos;
	// length is the distance in scalar, think of this like how much meters the target is from the player in a straight line
	// instead of vector (x, y)
	float length = D.length();
	// Normalize distance (D)
	// this now has Length = 1
	// this means for every x+1 the distance jumps by L
	// http://blog.wolfire.com/2009/07/linear-algebra-for-game-developers-part-1/
	Vec2 normalized = D / length; // (D.x / length, D.y / length);
	// add the acceleration to the length jumps this way we have the veloctity
	Vec2 velocity = normalized * m_bulletConfig.S; // (speed * normalized.x, speed * normalized.y);

	float angle = atan2f(D.y, D.x);
	bulletEnt->cTransform->velocity = velocity;
	bulletEnt->cTransform->angle = angle;

}

void Game::spawnSpecialWeapon(std::shared_ptr<Entity> entity)
{
	// TODO: implement your own special weapon
	// shout out of all player vertices
	int vertices = m_player->cShape->circle.getPointCount();
	int degreeIntervals = 360 / (vertices * 10);

	for (int i = 0; i < vertices; i++)
	{
		float angle = degreeIntervals * i;

		Vec2 velocity(m_player->cTransform->velocity.x * cos(angle), m_player->cTransform->velocity.y * sin(angle));


		auto bulletEnt = m_entities.addEntity("bullet");
		bulletEnt->cTransform = std::make_shared<CTransform>(Vec2(m_player->cTransform->pos.x, m_player->cTransform->pos.y), velocity, angle);
		bulletEnt->cShape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V, sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB), sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB), m_bulletConfig.OT);
		bulletEnt->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
		bulletEnt->cTransform->angle = angle;
	}

	m_lastSpecialWeaponTime = m_currentFrame;
}

void Game::sMovement()
{
	sf::Vector2u windowSize = m_window.getSize();
	unsigned int windowX = windowSize.x;
	unsigned int windowY = windowSize.y;

	// Sample movement speed update
	for (auto& [tag, entityVec] : m_entities.getEntityMap())
	{
		if (tag == "player")
		{
			float radius = m_player->cShape->circle.getRadius();
			if (m_player->cInput->up)
			{
				if (m_player->cTransform->pos.y - radius <= 0)
					continue;
				m_player->cTransform->pos.y -= m_player->cTransform->velocity.y;
			}
			if (m_player->cInput->down)
			{
				if (m_player->cTransform->pos.y + radius >= windowY)
					continue;
				m_player->cTransform->pos.y += m_player->cTransform->velocity.y;
			}
			if (m_player->cInput->left)
			{
				if (m_player->cTransform->pos.x - radius <= 0)
					continue;
				m_player->cTransform->pos.x -= m_player->cTransform->velocity.x;
			}
			if (m_player->cInput->right)
			{
				if (m_player->cTransform->pos.x + radius >= windowX)
					continue;
				m_player->cTransform->pos.x += m_player->cTransform->velocity.x;
			}
			continue;
		}
		for (auto& entity : entityVec)
		{
			entity->cTransform->pos.x += entity->cTransform->velocity.x;
			entity->cTransform->pos.y += entity->cTransform->velocity.y;
		}
	}
}

void Game::sLifeSpan()
{
	// TODO: implement all lifespan functionality
	//
	// for all entities
	//	   if entity has no lifespan component, skip it
	//	   if entity has > 0 remaining lifespan, subtract 1
	//	   if it has lifespan and is alive
	//			scale its alpha channel properly
	//	   if it has lifespan and its time is up
	//			destroy the entity
	for (auto& e : m_entities.getEntities())
	{
		if (e->cLifespan == nullptr)
			continue;
		if (e->cLifespan > 0)
			e->cLifespan->remaining -= 1;
		if (e->isActive())
		{
			// get these from the config
			e->cShape->circle.setFillColor(sf::Color(0, 255, 10, 255 * e->cLifespan->remaining / e->cLifespan->total));
			e->cShape->circle.setOutlineColor(sf::Color(255, 0, 0, 255 * e->cLifespan->remaining / e->cLifespan->total));
		}
		if (e->isActive() && e->cLifespan->remaining <= 0)
		{
			e->destroy();
		}
	}
}

void Game::sCollision()
{
	// all collisions for bullets, window borders etc are done in here
	sf::Vector2u windowSize = m_window.getSize();
	unsigned int windowX = windowSize.x;
	unsigned int windowY = windowSize.y;

	for (auto& [tag, entityVec] : m_entities.getEntityMap())
	{
		if (tag == "bullet")
		{
			for (auto& bulletEnt : entityVec)
			{
				for (auto& enemyEnt : m_entities.getEntities("enemy"))
				{
					float distance = bulletEnt->cTransform->pos.dist(enemyEnt->cTransform->pos);
					if ( distance < (bulletEnt->cShape->circle.getRadius() + enemyEnt->cShape->circle.getRadius()) )
					{
						spawnSmallEnemies(enemyEnt);
						enemyEnt->destroy();
						bulletEnt->destroy();
						m_score += enemyEnt->cScore->score;
					}
				}
				for (auto& smallEnemyEnt : m_entities.getEntities("smallenemy"))
				{
					float distance = bulletEnt->cTransform->pos.dist(smallEnemyEnt->cTransform->pos);
					if (distance < (bulletEnt->cShape->circle.getRadius() + smallEnemyEnt->cShape->circle.getRadius()))
					{
						smallEnemyEnt->destroy();
						bulletEnt->destroy();
						m_score += smallEnemyEnt->cScore->score;
					}
				}

			}
			continue;
		}

		if (tag == "enemy" || tag == "smallenemy")
		{
			for (auto& enemyEnt : entityVec)
			{
				float radius = enemyEnt->cShape->circle.getRadius();

				float playerDistance = enemyEnt->cTransform->pos.dist(m_player->cTransform->pos);
				if (playerDistance < (enemyEnt->cShape->circle.getRadius() + m_player->cShape->circle.getRadius()))
				{
					if (m_player->isActive())
					{
						m_player->destroy();
						enemyEnt->destroy();
						m_score = 0;
						spawnPlayer();
					}
				}
				
				if (!(enemyEnt->cTransform->velocity.y < 0) && enemyEnt->cTransform->pos.y + radius >= windowY)
					enemyEnt->cTransform->velocity.y = -enemyEnt->cTransform->velocity.y;
				if (!(enemyEnt->cTransform->velocity.y > 0) && enemyEnt->cTransform->pos.y - radius <= 0)
					enemyEnt->cTransform->velocity.y = -enemyEnt->cTransform->velocity.y;

				if (!(enemyEnt->cTransform->velocity.x < 0) && enemyEnt->cTransform->pos.x + radius >= windowX)
					enemyEnt->cTransform->velocity.x = -enemyEnt->cTransform->velocity.x;
				if (!(enemyEnt->cTransform->velocity.x > 0) && enemyEnt->cTransform->pos.x - radius <= 0)
					enemyEnt->cTransform->velocity.x = -enemyEnt->cTransform->velocity.x;
			}

			continue;
		}
	}
}

void Game::sEnemySpawner()
{
	// TODO: code which implements enemy spawning should go here
	//spawnEnemy(50);
	//spawnEnemy(100);
	//spawnEnemy(150);
	//spawnEnemy();
	if (m_lastEnemySpawnTime == 0 || m_enemyConfig.SI == 0)
	{
		spawnEnemy();
		return;
	}

	int allowed = (m_currentFrame - (int)m_lastEnemySpawnTime) % (int)m_enemyConfig.SI;
	//spawnEnemy();

	if (allowed == 0)
	{
		spawnEnemy();
	}

}

void Game::sGUI()
{
	ImGui::Begin("Geometry Wars");

	//ImGui::ShowDemoWindow();
	
	if (ImGui::Button("Systems"))
		m_guiTabInt = 0;
	ImGui::SameLine(0.0f, 2.0f);
	if (ImGui::Button("Entity Manager"))
		m_guiTabInt = 1;

	switch (m_guiTabInt)
	{
	case 0:
		ImGui::Checkbox("Movement", &m_movementBool);
		ImGui::Checkbox("Lifespan", &m_lifespanBool);
		ImGui::Checkbox("Collision", &m_collisionBool);
		ImGui::Checkbox("Spawn Enemies", &m_spawnEnemies);
		ImGui::PushItemWidth(100.0f);
		ImGui::Indent();
		ImGui::SliderInt("Enemy Spawn Interval", &m_enemyConfig.SI, 0, 200);
		if (ImGui::Button("Spawn manual"))
			spawnEnemy();
		ImGui::Unindent();
		ImGui::PopItemWidth();
		ImGui::Checkbox("GUI", &m_guiBool);
		ImGui::Checkbox("Rendering", &m_renderingBool);
		break;

	case 1:
		if (ImGui::CollapsingHeader("Entities by Tag", ImGuiTreeNodeFlags_None))
		{
			ImGui::Indent();
			if (ImGui::CollapsingHeader("bullet", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();
				for (auto& [tag, e] : m_entities.getEntityMap())
				{
					if (tag != "bullet")
						continue;
					for (auto& entity : e)
					{
						sf::Color entColor = entity->cShape->circle.getFillColor();
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(entColor.r, entColor.g, entColor.b, 0.0f));
						if (ImGui::Button("D"))
							entity->destroy();
						ImGui::SameLine();
						ImGui::Text((std::format("{}", entity->id())).c_str());
						ImGui::SameLine();
						ImGui::Text(tag.c_str());
						ImGui::SameLine();
						ImGui::Text((std::format("({}, {})", entity->cTransform->pos.x, entity->cTransform->pos.y)).c_str());
						ImGui::PopStyleColor(1);
					}
				}
				ImGui::Unindent();

			}
			if (ImGui::CollapsingHeader("enemy", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();
				for (auto& [tag, e] : m_entities.getEntityMap())
				{
					if (tag != "enemy")
						continue;
					for (auto& entity : e)
					{
						sf::Color entColor = entity->cShape->circle.getFillColor();
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(entColor.r, entColor.g, entColor.b, 0.0f));
						if (ImGui::Button("D"))
							entity->destroy();
						ImGui::SameLine();
						ImGui::Text((std::format("{}", entity->id())).c_str());
						ImGui::SameLine();
						ImGui::Text(tag.c_str());
						ImGui::SameLine();
						ImGui::Text((std::format("({}, {})", entity->cTransform->pos.x, entity->cTransform->pos.y)).c_str());
						ImGui::PopStyleColor(1);
					}
				}
				ImGui::Unindent();
			}
			if (ImGui::CollapsingHeader("player", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();
				for (auto& [tag, e] : m_entities.getEntityMap())
				{
					if (tag != "player")
						continue;
					for (auto& entity : e)
					{
						sf::Color entColor = entity->cShape->circle.getFillColor();
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(entColor.r, entColor.g, entColor.b, 0.0f));
						if (ImGui::Button("D"))
							entity->destroy();
						ImGui::SameLine();
						ImGui::Text((std::format("{}", entity->id())).c_str());
						ImGui::SameLine();
						ImGui::Text(tag.c_str());
						ImGui::SameLine();
						ImGui::Text((std::format("({}, {})", entity->cTransform->pos.x, entity->cTransform->pos.y)).c_str());
						ImGui::PopStyleColor(1);
					}
				}
				ImGui::Unindent();
			}
			if (ImGui::CollapsingHeader("small", ImGuiTreeNodeFlags_None))
			{
				ImGui::Indent();
				for (auto& [tag, e] : m_entities.getEntityMap())
				{
					if (tag != "smallenemy")
						continue;
					for (auto& entity : e)
					{
						sf::Color entColor = entity->cShape->circle.getFillColor();
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(entColor.r, entColor.g, entColor.b, 0.0f));
						if (ImGui::Button("D"))
							entity->destroy();
						ImGui::SameLine();
						ImGui::Text((std::format("{}", entity->id())).c_str());
						ImGui::SameLine();
						ImGui::Text(tag.c_str());
						ImGui::SameLine();
						ImGui::Text((std::format("({}, {})", entity->cTransform->pos.x, entity->cTransform->pos.y)).c_str());
						ImGui::PopStyleColor(1);
					}
				}
				ImGui::Unindent();
			}
			ImGui::Unindent();
		}
		if (ImGui::CollapsingHeader("All Entities", ImGuiTreeNodeFlags_None))
		{
			for (auto& [tag, e] : m_entities.getEntityMap())
			{
				for (auto& entity : e)
				{
					sf::Color entColor = entity->cShape->circle.getFillColor();
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(entColor.r, entColor.g, entColor.b, 0.0f));
					if (ImGui::Button("D"))
						entity->destroy();
					ImGui::SameLine();
					ImGui::Text((std::format("{}", entity->id())).c_str());
					ImGui::SameLine();
					ImGui::Text(tag.c_str());
					ImGui::SameLine();
					ImGui::Text((std::format("({}, {})", entity->cTransform->pos.x, entity->cTransform->pos.y)).c_str());
					ImGui::PopStyleColor(1);
				}
			}
		}
		break;

	default:
		break;
	}
	

	ImGui::End();
}

void Game::sRender()
{
	m_window.clear();

	// for each entity do these things
	for (auto& e : m_entities.getEntities())
	{
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);
		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);
		m_window.draw(e->cShape->circle);
	}

	m_text.setString(std::format("{} {}", "Score:", m_score));
	m_window.draw(m_text);

	// draw the UI last
	ImGui::SFML::Render(m_window);

	m_window.display();
}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// pass the event to imgui to be parsed
		ImGui::SFML::ProcessEvent(m_window, event);

		// this event triggers when the window is closed
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		// this event is triggered when a key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				if (m_paused)
					return;
				m_player->cInput->up = true;
				break;
			case sf::Keyboard::A:
				if (m_paused)
					return;
				m_player->cInput->left = true;
				break;
			case sf::Keyboard::S:
				if (m_paused)
					return;
				m_player->cInput->down = true;
				break;
			case sf::Keyboard::D:
				if (m_paused)
					return;
				m_player->cInput->right = true;
				break;
			case sf::Keyboard::P:
				m_paused = !m_paused;
				break;
			case sf::Keyboard::Escape:
				m_paused = !m_paused;
				m_running = false;
				break;
			default:
				break;
			}
		}
		if (m_paused)
			return;
		// this event is triggered when a key is released
		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::W:
				m_player->cInput->up = false;
				break;
			case sf::Keyboard::A:
				m_player->cInput->left = false;
				break;
			case sf::Keyboard::S:
				m_player->cInput->down = false;
				break;
			case sf::Keyboard::D:
				m_player->cInput->right = false;
				break;
			default:
				break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			// this line ignores mouse events if ImGui is the thing being clicked
			if (ImGui::GetIO().WantCaptureMouse)
				continue;

			if (event.mouseButton.button == sf::Mouse::Left)
			{
				Vec2 target(event.mouseButton.x, event.mouseButton.y);
				Game::spawnBullet(m_player, target);
			}

			if (event.mouseButton.button == sf::Mouse::Right)
			{
				if (m_lastSpecialWeaponTime == 0)
				{
					spawnSpecialWeapon(m_player);
					return;
				}

				int allowed = (m_currentFrame - (int)m_lastSpecialWeaponTime) % 5;

				if (allowed == 0)
				{
					spawnSpecialWeapon(m_player);
				}
			}
		}
	}
}
