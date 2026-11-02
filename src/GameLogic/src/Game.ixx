module;

#include <vector>
#include <memory>

export module GameLogic:Game;

import Core;
import Engine;
import :CameraControl;
import :GameObject;

export namespace game
{
	class Game : public core::IGame
	{
	public:
		Game() = default;
		void Initialize(engine::Engine* engine);
		void Shutdown() override;
		void Update(float deltaTime) override;
		void Render() override;

		static engine::Engine* GetEngine() { return m_engine; }

	private:
		static engine::Engine* m_engine;
		CameraControl m_cameraControl;
		std::vector<std::unique_ptr<GameObject>> m_gameObjects;
	};
}