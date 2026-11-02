export module Core:Vector3;

export namespace core
{
	struct Vector3
	{
		Vector3() = default;
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		float x = 0;
		float y = 0;
		float z = 0;
	};
}