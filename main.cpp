#include "TestHandler.hpp"
#include "Debug.hpp"
#include <iostream>
#include <fstream>
#include <SDL_image.h>
using namespace std;

void RunWindow()
{
    ofstream fout("debug.txt", ofstream::binary);
    AddLogStream(cout);
    if (fout) AddLogStream(fout);

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

    auto context = SDL_GL_CreateContext(window);
    glewInit();

    Log() << "SDL_GL_SetSwapInterval ";
    if (SDL_GL_SetSwapInterval(1))
        Log() << "failed.\n";
    else
        Log() << "succeeded.\n";

    Log() << "OpenGL debug context flag ";
    GLint v;
    glGetIntegerv(GL_CONTEXT_FLAGS, &v);
    if (v & GL_CONTEXT_FLAG_DEBUG_BIT)
        Log() << "enabled\n";
    else
        Log() << "disabled\n";

    TestHandler().Run(window);

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    RemoveAllLogStreams();
}

int main(int argc, char** argv)
{
    RunWindow();
    return 0;
}
