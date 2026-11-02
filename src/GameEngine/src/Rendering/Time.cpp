module;

#include <chrono>

module Engine.Rendering;

import :Time;

namespace engine
{
	void Time::Init()
	{
		m_startTime = std::chrono::steady_clock::now();
	}

	void Time::Update()
	{
		const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		const std::chrono::duration<float> elapsed = now - m_startTime;
		m_elapsedTime = elapsed.count();
		m_deltaTime = m_elapsedTime - m_lastFrameTime;
		m_lastFrameTime = m_elapsedTime;
	}

	float Time::GetElapsedSeconds() const
	{
		return m_elapsedTime;
	}

	float Time::GetDeltaTime() const
	{
		return m_deltaTime;
	}
}