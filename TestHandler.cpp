#include "TestHandler.hpp"
#include <SDL_image.h>
using namespace std;

static constexpr auto PixelFormat = SDL_PIXELFORMAT_ABGR8888;

static const GLenum TexParams[] = {
    GL_TEXTURE_WRAP_S, GL_REPEAT,
    GL_TEXTURE_WRAP_T, GL_REPEAT,
    GL_TEXTURE_MAG_FILTER, GL_LINEAR,
    GL_TEXTURE_MIN_FILTER, GL_LINEAR,
    0 };

void SetParams(const GLenum* params)
{
    for (int i = 0; params[i]; i += 2)
        glTexParameteri(GL_TEXTURE_2D, params[i], params[i + 1]);
}

void LoadTexture(const char* path)
{
    auto surface = IMG_Load(path);

    if (surface)
    {
        if (surface->format->format != PixelFormat)
        {
            auto convertedSurface = SDL_ConvertSurfaceFormat(
                surface,
                PixelFormat,
                0);

            SDL_FreeSurface(surface);
            surface = convertedSurface;
        }

        if (surface)
        {
            cout << "Loaded " << path << " successfully!\n";
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGBA,
                surface->w,
                surface->h,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                surface->pixels);
        }

        SDL_FreeSurface(surface);
    }
}

TestHandler::TestHandler()
{
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    SetParams(TexParams);
    LoadTexture("../tiles_spritesheet.png");
    glDisable(GL_TEXTURE_2D);
}

TestHandler::~TestHandler()
{
    glDeleteTextures(1, &_texture);
}

void TestHandler::OnOpen()
{
    glClearColor(0.25f, 0.0f, 0.0f, 1.0f);
    //glEnable(GL_TEXTURE_2D);
}

void TestHandler::OnClose()
{
    //glDisable(GL_TEXTURE_2D);
}

void TestHandler::OnPrepareRender()
{
    auto third = Tau<float>() / 3.0f;

    for (int i = 0; i < 3; ++i)
    {
        Vector4F v = {{0.0f, 1.0f, 0.0f, 1.0f}};
        auto r = float(i) * third + _rotation;

        auto matrix = _projectionMatrix * RotateZ(r);
        v = Divided(matrix * v);
        _vertices[i * 2 + 0] = v.values[0];
        _vertices[i * 2 + 1] = v.values[1];
    }
}

void TestHandler::OnRender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 1.0f, 1.0f);
    for (int i = 0; i < 3; ++i) glVertex2fv(_vertices + i * 2);
    glEnd();
}

void TestHandler::OnUpdate()
{
    _rotation -= (1.0f / 128.0f);
}

void TestHandler::OnKeyDown(SDL_Keysym keysym)
{
    WindowEventHandler::OnKeyDown(keysym);
    switch (keysym.sym)
    {
        case SDLK_BACKSLASH:
            SDL_Delay(750);
            break;
        case SDLK_F11:
            if (SDL_GetWindowFlags(Window()) & SDL_WINDOW_FULLSCREEN_DESKTOP)
            {
                SDL_SetWindowFullscreen(Window(), 0);
            }
            else
            {
                SDL_SetWindowFullscreen(Window(), SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
            break;
        default: break;
    }
}

void TestHandler::OnResize(Sint32 width, Sint32 height)
{
    WindowEventHandler::OnResize(width, height);
    auto ratio = float(width) / float(height);
    _projectionMatrix = Orthographic(2.0f, ratio);
}
