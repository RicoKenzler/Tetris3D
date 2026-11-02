#include <Windows.h>
#include <stdexcept>

import Engine;
import GameLogic;

_Use_decl_annotations_
int WINAPI wWinMain(
    _In_ HINSTANCE instance,
    _In_opt_ HINSTANCE,
    _In_ PWSTR,
    _In_ int showCommand)
{
    try
    {
        engine::Engine engine{};
		game::Game game{};

        engine.Initialize(instance, showCommand, &game);

        game.Initialize(&engine);

		return engine.Run();
    }
    catch (const std::exception& exception)
    {
        MessageBoxA(
            nullptr,
            exception.what(),
            "Engine Error",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }
}