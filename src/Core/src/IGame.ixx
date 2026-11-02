export module Core:IGame;

export namespace core
{
	class IGame
	{
	public:
		virtual void Shutdown() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Render() = 0;

	protected:
		IGame() = default;
		virtual ~IGame() = default;
	};
}