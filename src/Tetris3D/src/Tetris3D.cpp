#include <iostream>

import RenderEngine;

int main()
{
    auto* const r = new RenderEngine::Renderer();
	r->Render();
    delete r;

    std::cout << "Hello World!\n";
}