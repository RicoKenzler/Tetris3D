module GameLogic;

import Engine;
import :Game;

namespace game
{
	engine::Engine* Game::m_engine = nullptr;

	void Game::Initialize(engine::Engine* engine)
	{
		m_engine = engine;
	}

	void Game::Update(const float deltaTime)
	{
		m_cameraControl.UpdateCamera(deltaTime);
	}

	void Game::Render()
	{
	}
}