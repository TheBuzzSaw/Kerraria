#include "TestHandler.hpp"
#include "Debug.hpp"
#include <memory>
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

static void SetIcon(SDL_Window* window)
{
    Uint16 pixels[16*16] =
    {
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0aab, 0x0789, 0x0bcc, 0x0eee, 0x09aa, 0x099a, 0x0ddd,
        0x0fff, 0x0eee, 0x0899, 0x0fff, 0x0fff, 0x1fff, 0x0dde, 0x0dee,
        0x0fff, 0xabbc, 0xf779, 0x8cdd, 0x3fff, 0x9bbc, 0xaaab, 0x6fff,
        0x0fff, 0x3fff, 0xbaab, 0x0fff, 0x0fff, 0x6689, 0x6fff, 0x0dee,
        0xe678, 0xf134, 0x8abb, 0xf235, 0xf678, 0xf013, 0xf568, 0xf001,
        0xd889, 0x7abc, 0xf001, 0x0fff, 0x0fff, 0x0bcc, 0x9124, 0x5fff,
        0xf124, 0xf356, 0x3eee, 0x0fff, 0x7bbc, 0xf124, 0x0789, 0x2fff,
        0xf002, 0xd789, 0xf024, 0x0fff, 0x0fff, 0x0002, 0x0134, 0xd79a,
        0x1fff, 0xf023, 0xf000, 0xf124, 0xc99a, 0xf024, 0x0567, 0x0fff,
        0xf002, 0xe678, 0xf013, 0x0fff, 0x0ddd, 0x0fff, 0x0fff, 0xb689,
        0x8abb, 0x0fff, 0x0fff, 0xf001, 0xf235, 0xf013, 0x0fff, 0xd789,
        0xf002, 0x9899, 0xf001, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0xe789,
        0xf023, 0xf000, 0xf001, 0xe456, 0x8bcc, 0xf013, 0xf002, 0xf012,
        0x1767, 0x5aaa, 0xf013, 0xf001, 0xf000, 0x0fff, 0x7fff, 0xf124,
        0x0fff, 0x089a, 0x0578, 0x0fff, 0x089a, 0x0013, 0x0245, 0x0eff,
        0x0223, 0x0dde, 0x0135, 0x0789, 0x0ddd, 0xbbbc, 0xf346, 0x0467,
        0x0fff, 0x4eee, 0x3ddd, 0x0edd, 0x0dee, 0x0fff, 0x0fff, 0x0dee,
        0x0def, 0x08ab, 0x0fff, 0x7fff, 0xfabc, 0xf356, 0x0457, 0x0467,
        0x0fff, 0x0bcd, 0x4bde, 0x9bcc, 0x8dee, 0x8eff, 0x8fff, 0x9fff,
        0xadee, 0xeccd, 0xf689, 0xc357, 0x2356, 0x0356, 0x0467, 0x0467,
        0x0fff, 0x0ccd, 0x0bdd, 0x0cdd, 0x0aaa, 0x2234, 0x4135, 0x4346,
        0x5356, 0x2246, 0x0346, 0x0356, 0x0467, 0x0356, 0x0467, 0x0467,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff,
        0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff, 0x0fff
    };

    auto surface = SDL_CreateRGBSurfaceFrom(pixels,16,16,16,16*2,0x0f00,0x00f0,0x000f,0xf000);

    // The icon is attached to the window pointer
    SDL_SetWindowIcon(window, surface);

    // ...and the surface containing the icon pixel data is no longer required.
    SDL_FreeSurface(surface);
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

    SetIcon(window);

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

    auto th = make_unique<TestHandler>();
    th->Run(window);
    th.reset(nullptr);

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
