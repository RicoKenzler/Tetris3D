module;

#include <memory>
#include <ranges>

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
		gameObject1->SetPosition({ -3, 0, 0 });
		gameObject1->SetScale({ 0.75f, 0.75f, 0.75f });
		m_gameObjects.emplace_back(std::move(gameObject1));

		id = m_engine->GetRenderer().CreateObject(true);
		auto gameObject2 = std::make_unique<GameObject>(id);
		gameObject2->SetPosition({ 0, 0, 0 });
		gameObject2->SetScale({ 0.75f, 0.75f, 0.75f });
		m_gameObjects.emplace_back(std::move(gameObject2));

		id = m_engine->GetRenderer().CreateObject(true);
		auto gameObject3 = std::make_unique<GameObject>(id);
		gameObject3->SetPosition({ 3, 0, 0 });
		gameObject3->SetScale({ 0.75f, 0.75f, 0.75f });
		m_gameObjects.emplace_back(std::move(gameObject3));

		id = m_engine->GetRenderer().CreateObject(false);
		auto gameObject4 = std::make_unique<GameObject>(id);
		gameObject4->SetPosition({ 6, 0, 0 });
		gameObject4->SetScale({ 0.75f, 0.75f, 0.75f });
		m_gameObjects.emplace_back(std::move(gameObject4));

		id = m_engine->GetRenderer().CreateObject(false);
		auto gameObject5 = std::make_unique<GameObject>(id);
		gameObject5->SetPosition({ -6, 0, 0 });
		gameObject5->SetScale({ 0.75f, 0.75f, 0.75f });
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

		core::Vector3 rotation = m_gameObjects[1]->GetRotation();
		rotation.x += angle;
		m_gameObjects[1]->SetRotation(rotation);

		rotation = m_gameObjects[2]->GetRotation();
		rotation.z += angle;
		m_gameObjects[2]->SetRotation(rotation);

		rotation = m_gameObjects[3]->GetRotation();
		rotation.y += angle;
		m_gameObjects[3]->SetRotation(rotation);

		if (m_gameObjects.size() >= 5)
		{
			rotation = m_gameObjects[4]->GetRotation();
			rotation.y += angle;
			m_gameObjects[4]->SetRotation(rotation);

			if (m_engine->GetTime().GetElapsedSeconds() > 5.0f)
			{
				m_engine->GetRenderer().DestroyObject(m_gameObjects[4]->GetId());
				m_gameObjects.pop_back();
			}
		}

		engine::Renderer& renderer = m_engine->GetRenderer();

		for (const auto& gameObject : std::views::filter(m_gameObjects, [](const auto& obj) { return obj->GetIsDirty(); }))
		{
			renderer.SetTransformData(gameObject->GetId(), gameObject->GetTransformData());
			gameObject->SetIsDirty(false);
		}
	}

	void Game::Render()
	{
	}
}