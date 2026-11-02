export module Core:TransformData;

import :Vector3;

export namespace core
{
	struct TransformData
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale{ 1.0f, 1.0f, 1.0f };
	};
}