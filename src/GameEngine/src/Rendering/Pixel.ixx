module;

#include <cstdint>

export module Engine.Rendering:Pixel;

export namespace engine
{
	struct Pixel
	{
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
		std::uint8_t a;
	};
}