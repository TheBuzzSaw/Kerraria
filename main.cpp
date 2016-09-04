#include "TestHandler.hpp"
#include <iostream>
#include <SDL_image.h>
using namespace std;

void RunWindow()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER);
    IMG_Init(IMG_INIT_PNG);

    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_FLAGS,
        SDL_GL_CONTEXT_DEBUG_FLAG);

    auto window = SDL_CreateWindow(
        "Kerraria",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1024,
        768,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    auto context = (SDL_GLContext*)SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    {
        TestHandler th;
        th.Run(window);
    }

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char** argv)
{
    RunWindow();
    return 0;
}
