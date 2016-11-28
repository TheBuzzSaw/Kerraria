#include "TestHandler.hpp"
#include "Debug.hpp"
#include <iostream>
#include <fstream>
#include <SDL_image.h>
#ifdef KerrariaES2
#include <bcm_host.h>
#endif
using namespace std;

static void MyCallback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* msg,
    const void* data)
{
    if (msg && *msg) Log() << "[OpenGL] " << msg << '\n';
}

static const char* GetString(GLenum name)
{
    auto text = (const char*)glGetString(name);
    return text ? text : "(null)";
}

static void RunWindow()
{
    ofstream fout("debug.txt", ofstream::binary);
    AddLogStream(cout);
    if (fout) AddLogStream(fout);

#ifdef KerrariaES2
    bcm_host_init();
#endif

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER);
    IMG_Init(IMG_INIT_PNG);

#ifndef KerrariaES2
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_FLAGS,
        SDL_GL_CONTEXT_DEBUG_FLAG);
#else
    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(0, &mode);
    Log() << "mode: " << mode.w << "x" << mode.h << '\n';
#endif

    auto window = SDL_CreateWindow(
        "Kerraria",
#ifndef KerrariaES2
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1024,
        768,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#else
        0,
        0,
        mode.w,
        mode.h,
        SDL_WINDOW_OPENGL);

    SDL_SetWindowFullscreen(window, SDL_TRUE);
#endif

    auto context = SDL_GL_CreateContext(window);

#ifndef KerrariaES2
    glewInit();
#endif

    Log() << "SDL_GL_SetSwapInterval ";
    if (SDL_GL_SetSwapInterval(1))
        Log() << "failed.";
    else
        Log() << "succeeded.";

    Log()
        << "\nOpenGL Vendor: " << GetString(GL_VENDOR)
        << "\nOpenGL Renderer: " << GetString(GL_RENDERER)
        << "\nOpenGL Version: " << GetString(GL_VERSION)
        << "\nOpenGL Shading Language: "
        << GetString(GL_SHADING_LANGUAGE_VERSION)
        << '\n';

#ifndef KerrariaES2
    Log() << "OpenGL debug context flag ";
    GLint v;
    glGetIntegerv(GL_CONTEXT_FLAGS, &v);
    if (v & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        Log() << "enabled\n";
        glDebugMessageCallback((GLDEBUGPROC)MyCallback, nullptr);
    }
    else
    {
        Log() << "disabled\n";
    }
#endif

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
