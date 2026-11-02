module;

#include <chrono>

export module Engine.Rendering:Time;

export namespace engine
{
	class Time
	{
	public:
		void Init();
		void Update();

		[[nodiscard]] float GetElapsedSeconds() const;
		[[nodiscard]] float GetDeltaTime() const;

	private:
		std::chrono::steady_clock::time_point m_startTime;
		float m_lastFrameTime = 0.0f;
		float m_deltaTime = 0.0f;
		float m_elapsedTime = 0.0f;
	};
}