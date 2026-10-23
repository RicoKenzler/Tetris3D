#include <Windows.h>
#include <stdexcept>

import RenderEngine;

_Use_decl_annotations_
int WINAPI wWinMain(
    _In_ HINSTANCE instance,
    _In_opt_ HINSTANCE,
    _In_ PWSTR,
    _In_ int showCommand)
{
    try
    {
        RenderEngine::Engine engine{};
        engine.Start(instance, showCommand);
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