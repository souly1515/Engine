#include <iostream>
#include <Windows.h>
#include <random>
#include "MetaHelpers.h"
#include "EngineManager.h"
#include "Graphics/GraphicSystem.h"
#include "Graphics/Sprite/Sprite.h"

// using fixed dt
// will probably use real dt when i have time to deal with that
float dt = 0.016f;
constexpr float shipIdle = 0.125;
constexpr float bulletLife = 20;
constexpr float bulletSpeed = 30;
constexpr float shipSpeed = 10;
constexpr float shipShootRange = 100;


std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
std::uniform_real_distribution<> dis_negToPos(-1, 1);
std::uniform_real_distribution<> dis_pos(0, 1);

struct Velocity
{
	float x;
	float y;
};

struct Position
{
	float x;
	float y;
};

struct Ship
{
	float timeIdleLeft = 0;
};

struct Bullet
{
	float lifeLeft;
	Entity owner;
};

void CreateBullet(Engine::EntityManager::EntityManager& EM, Position& shipPos, Position& otherPos, Entity owner)
{
	auto ent = EM.AddEntity<Sprite, Velocity, Position, Bullet>();

	auto& pos    = EM.GetComponent<Position>(ent);
	auto& vel    = EM.GetComponent<Velocity>(ent);
	auto& spr    = EM.GetComponent<Sprite>(ent);
	auto& bullet = EM.GetComponent<Bullet>(ent);
	pos = shipPos;
	glm::vec2 dir = { otherPos.x - shipPos.x, otherPos.y - shipPos.y };
	dir = glm::normalize(dir);;
	vel.x = dir.x * bulletSpeed;
	vel.y = dir.y * bulletSpeed;
	bullet.lifeLeft = bulletLife;
	bullet.owner = owner;
	spr.m_mesh = GraphicSystem::GetInstance()->m_squareMesh;
	spr.m_shader = GraphicSystem::GetInstance()->ShaderMan.GetShader("default");
}

void CreateShip(Engine::EntityManager::EntityManager& EM)
{
	auto ent = EM.AddEntity<Position, Sprite, Ship, Velocity>();

	auto& pos = EM.GetComponent<Position>(ent);
	auto& vel = EM.GetComponent<Velocity>(ent);
	auto& spr = EM.GetComponent<Sprite>(ent);
	auto& ship = EM.GetComponent<Ship>(ent);
	pos.x = float(dis_pos(gen) * 1280);
	pos.y = float(dis_pos(gen) * 720);
	vel.x = float(dis_negToPos(gen) * shipSpeed);
	vel.y = float(dis_negToPos(gen) * shipSpeed);
	ship.timeIdleLeft = shipIdle;
	spr.m_mesh = GraphicSystem::GetInstance()->m_squareMesh;
	spr.m_shader = GraphicSystem::GetInstance()->ShaderMan.GetShader("default");
}

struct UpdateMovement
{
	void operator()(Position& pos, Velocity& vel)
	{
		pos.x += vel.x * dt;
		pos.y += vel.y * dt;

		// boundary check
		if (pos.x < 0)
			vel.x = abs(vel.x);
		if (pos.x > 1280)
			vel.x = -abs(vel.x);
		if (pos.y < 0)
			vel.y = abs(vel.y);
		if (pos.y > 720)
			vel.y = -abs(vel.y);
	}
};

struct ShipBehaviour
{
	Engine::Tools::Query shipQuery;
	ShipBehaviour()
	{
		using namespace Engine::Tools;
		shipQuery.SetFromTuple< Query::must<Position, Ship>, Query::none_of<Bullet>>(nullptr);
	}

	void Execute(Engine::EntityManager::EntityManager& GM)
	{
		auto archetypes = GM.Search(shipQuery);

		for (auto itr = archetypes.begin(); itr != archetypes.end(); ++itr)
		{
			Ship& ship = GM.GetComponent<Ship>(*itr);

			if (ship.timeIdleLeft > 0)
				ship.timeIdleLeft -= dt;
		}
		for(auto itr = archetypes.begin(); itr != archetypes.end(); ++itr)
		{
			Ship& ship1 = GM.GetComponent<Ship>(*itr);
			Position& pos1 = GM.GetComponent<Position>(*itr);

			auto itr2 = itr;
			++itr2;
			for (; itr2 != archetypes.end(); ++itr2)
			{
				Ship& ship2 = GM.GetComponent<Ship>(*itr2);
				Position& pos2 = GM.GetComponent<Position>(*itr2);
				if (ship2.timeIdleLeft <= 0)
				{
					float distx = pos1.x - pos2.x;
					float disty = pos1.y - pos2.y;
					if (distx * distx + disty * disty <= shipShootRange * shipShootRange)
					{
						//create bullet
						CreateBullet(GM, pos2, pos1, *itr2);
						ship2.timeIdleLeft = 10;
					}
				}
				if (ship1.timeIdleLeft <= 0)
				{
					float distx = pos1.x - pos2.x;
					float disty = pos1.y - pos2.y;
					if (distx * distx + disty * disty <= shipShootRange * shipShootRange)
					{
						//create bullet
						CreateBullet(GM, pos1, pos2, *itr);
						ship1.timeIdleLeft = 10;
					}
				}
			}
		}

	}
};

struct BulletBehaviour
{
	Engine::Tools::Query bulletQuery;
	Engine::Tools::Query shipQuery;

	BulletBehaviour()
	{
		using namespace Engine::Tools;
		bulletQuery.SetFromTuple< Query::must<Position, Bullet>, Query::none_of<Ship>>(nullptr);
		shipQuery.SetFromTuple< Query::must<Position, Ship>, Query::none_of<Bullet>>(nullptr);
	}

	void Execute(Engine::EntityManager::EntityManager& GM)
	{
		auto bulletArche = GM.Search(bulletQuery);

		for (auto itr = bulletArche.begin(); itr != bulletArche.end(); ++itr)
		{
			//check zombie
			if (GM.IsZombie(*itr))
				continue;


			Bullet& bul = GM.GetComponent<Bullet>(*itr);
			Position& pos1 = GM.GetComponent<Position>(*itr);

			bul.lifeLeft -= dt;
			if (bul.lifeLeft <= 0)
			{
				GM.RemoveEntity(*itr);
				continue;
			}

			auto shipArche = GM.Search(shipQuery);
			for (auto itr2 = shipArche.begin(); itr2 != shipArche.end(); ++itr2)
			{
				if (GM.IsZombie(*itr2))
					continue;
				if (*itr2 == bul.owner)
					continue;
				Position& pos2 = GM.GetComponent<Position>(*itr2);

				float distx = pos1.x - pos2.x;
				float disty = pos1.y - pos2.y;

				if (distx * distx + disty * disty <= 10 * 10)
				{
					GM.RemoveEntity(*itr);
					GM.RemoveEntity(*itr2);
					// destroy bullet
					// destroy ship

					break;
				}

			}
		}
	}
};

struct Render
{
	GraphicSystem* gs = nullptr;
	Engine::Tools::Query renderQuery;

	Render()
	{
		using namespace Engine::Tools;
		renderQuery.SetFromTuple< Query::must<Position>, Query::one_of<Ship, Bullet>>(nullptr);
	}

	void Execute(Engine::EntityManager::EntityManager& GM)
	{
		if (!gs)
			gs = GraphicSystem::GetInstance();
		gs->UpdateBegin();

		auto archetypes = GM.Search(renderQuery);

		for (auto itr = archetypes.begin(); itr != archetypes.end(); ++itr)
		{
			Position& pos = GM.GetComponent<Position>(*itr);
			Sprite& spr = GM.GetComponent<Sprite>(*itr);
			Ship* ship = GM.TryGetComponent<Ship>(*itr);
			Bullet* bullet = GM.TryGetComponent<Bullet>(*itr);

			// hack to get the sprite to stop crashing
			spr.m_shader = GraphicSystem::GetInstance()->ShaderMan.GetShader("default");

			spr.SetShader();

			glm::mat4 trans = {};
			for (int i = 0; i < 4; ++i)
			{
				trans[i][i] = 1;
			}
			trans[0][0] = 2.0f / 1280;
			trans[1][1] = 2.0f / 720;
			trans[3][0] = -1.f;
			trans[3][1] = -1.f;

			spr.m_shader->SetMat("worldTrans", trans);
			if (ship)
			{
				trans[1][1] = trans[0][0] = 10;

				trans[3][0] = pos.x;
				trans[3][1] = pos.y;

				spr.m_shader->SetMat("transform", trans);
				if (ship->timeIdleLeft > 0)
				{
					spr.m_shader->SetVector4("uColor", glm::vec4(1, 0, 0, 1));
				}
				else
				{
					spr.m_shader->SetVector4("uColor", glm::vec4(0, 1, 0, 1));
				}
			}
			else if (bullet)
			{
				trans[1][1] = trans[0][0] = 5;

				trans[3][0] = pos.x;
				trans[3][1] = pos.y;

				spr.m_shader->SetMat("transform", trans);
				spr.m_shader->SetVector4("uColor", glm::vec4(1, 1, 1, 1));
			}
			spr.Draw();
		}

	}
};

struct SwapBuffer
{
	GraphicSystem* gs = nullptr;
	void Execute(Engine::EntityManager::EntityManager& GM)
	{
		if (!gs)
			gs = GraphicSystem::GetInstance();
		gs->UpdateEnd();
	}
};


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE /*hPrevInstance*/,
	_In_ LPWSTR    /*lpCmdLine*/,
	_In_ int       nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Engine::EngineManager engineMan;

	engineMan.Init(hInstance, nCmdShow);
	GraphicSystem* gs = GraphicSystem::GetInstance();
	gs->ShaderMan.LoadShader("Resources/VertexShader.glsl", "Resources/FragmentShader.glsl", "default");

	engineMan.RegisterComponent<EntityComponent>();
	engineMan.RegisterComponent<Sprite>();

	engineMan.RegisterComponent<Velocity>();
	engineMan.RegisterComponent<Position>();
	engineMan.RegisterComponent<Ship>();
	engineMan.RegisterComponent<Bullet>();

	engineMan.RegisterSystem< UpdateMovement>();
	engineMan.RegisterSystem< ShipBehaviour>();
	engineMan.RegisterSystem< BulletBehaviour>();
	engineMan.RegisterSystem< Render>();
	engineMan.RegisterSystem< SwapBuffer>();

	for(int i = 0; i < 30; ++i)
		CreateShip(engineMan.EntMan);

	engineMan.Run();

	//clean up done by destructors
}