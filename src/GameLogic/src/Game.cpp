module;

#include <memory>

module GameLogic;

import Engine;
import :Game;

namespace game
{
	engine::Engine* Game::m_engine = nullptr;

	void Game::Initialize(engine::Engine* engine)
	{
		m_engine = engine;

		int id = m_engine->GetRenderer().CreateObject(true);
		auto gameObject1 = std::make_unique<GameObject>(id);
		gameObject1->GetTransformData().position = { -3, 0, 0 };
		gameObject1->GetTransformData().scale = { 0.75f, 0.75f, 0.75f };
		m_gameObjects.emplace_back(std::move(gameObject1));

		id = m_engine->GetRenderer().CreateObject(true);
		auto gameObject2 = std::make_unique<GameObject>(id);
		m_gameObjects.emplace_back(std::move(gameObject2));

		id = m_engine->GetRenderer().CreateObject(true);
		auto gameObject3 = std::make_unique<GameObject>(id);
		gameObject3->GetTransformData().position = { 3, 0, 0 };
		gameObject3->GetTransformData().scale = { 0.75f, 0.75f, 0.75f };
		m_gameObjects.emplace_back(std::move(gameObject3));

		id = m_engine->GetRenderer().CreateObject(false);
		auto gameObject4 = std::make_unique<GameObject>(id);
		gameObject4->GetTransformData().position = { 6, 0, 0 };
		gameObject4->GetTransformData().scale = { 0.75f, 0.75f, 0.75f };
		m_gameObjects.emplace_back(std::move(gameObject4));

		id = m_engine->GetRenderer().CreateObject(false);
		auto gameObject5 = std::make_unique<GameObject>(id);
		gameObject5->GetTransformData().position = { -6, 0, 0 };
		gameObject5->GetTransformData().scale = { 0.75f, 0.75f, 0.75f };
		m_gameObjects.emplace_back(std::move(gameObject5));
	}

	void Game::Shutdown()
	{
		m_gameObjects.clear();
	}

	void Game::Update(const float deltaTime)
	{
		m_cameraControl.UpdateCamera(deltaTime);

		// Rotate the cube at 45 degrees per second.
		constexpr float rotationSpeed = 45.0f;
		const float angle = deltaTime * rotationSpeed;

		m_gameObjects[1]->GetTransformData().rotation.x += angle;
		m_gameObjects[2]->GetTransformData().rotation.z += angle;
		m_gameObjects[3]->GetTransformData().rotation.y += angle;

		if (m_gameObjects.size() >= 5)
		{
			m_gameObjects[4]->GetTransformData().rotation.y += angle;

			if (m_engine->GetTime().GetElapsedSeconds() > 5.0f)
			{
				m_engine->GetRenderer().DestroyObject(m_gameObjects[4]->GetId());
				m_gameObjects.pop_back();
			}
		}

		engine::Renderer& renderer = m_engine->GetRenderer();

		for (const auto& gameObject : m_gameObjects)
		{
			renderer.SetTransformData(gameObject->GetId(), gameObject->GetTransformData());
		}
	}

	void Game::Render()
	{
	}
}