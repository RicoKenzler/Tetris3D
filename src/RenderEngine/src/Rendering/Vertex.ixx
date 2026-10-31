export module RenderEngine.Rendering:Vertex;

export namespace RenderEngine
{
	struct Vertex
	{
		float x, y, z;
		float r, g, b;
		float nx, ny, nz;
		float u, v;
	};
}