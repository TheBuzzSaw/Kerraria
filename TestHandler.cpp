#include "TestHandler.hpp"
#include "Debug.hpp"
#include <SDL_image.h>
using namespace std;

static constexpr auto PixelFormat = SDL_PIXELFORMAT_ABGR8888;

static const GLenum TexParams[] = {
    GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE,
    GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE,
    GL_TEXTURE_MAG_FILTER, GL_NEAREST,
    GL_TEXTURE_MIN_FILTER, GL_NEAREST,
    0, 0 };

static void SetParams(const GLenum* params)
{
    for (int i = 0; params[i]; i += 2)
        glTexParameteri(GL_TEXTURE_2D, params[i], params[i + 1]);
}

static pair<float, float> GetTexCoords(int low, int high)
{
    return {
        float(low) / 1024.0f,
        float(high + 1) / 1024.0f};
}

static pair<float, float> GetTexCoords(int index)
{
    int offset = index * 72;
    return GetTexCoords(offset, offset + 69);
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
            Log() << "Loaded " << path << " successfully!\n";
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

    auto tcs = GetTexCoords(1);
    auto tct = GetTexCoords(8);

    for (int i = -3; i < 3; ++i)
    {
        for (int j = -3; j < 0; ++j)
        {
            _vertices.push_back(i);
            _vertices.push_back(j + 1);
            _vertices.push_back(tcs.first);
            _vertices.push_back(tct.first);

            _vertices.push_back(i + 1);
            _vertices.push_back(j + 1);
            _vertices.push_back(tcs.second);
            _vertices.push_back(tct.first);

            _vertices.push_back(i + 1);
            _vertices.push_back(j);
            _vertices.push_back(tcs.second);
            _vertices.push_back(tct.second);

            _vertices.push_back(i);
            _vertices.push_back(j);
            _vertices.push_back(tcs.first);
            _vertices.push_back(tct.second);
        }
    }
}

TestHandler::~TestHandler()
{
    glDeleteTextures(1, &_texture);
}

void TestHandler::OnOpen()
{
    glClearColor(0.0f, 0.5f, 0.5f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void TestHandler::OnClose()
{
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void TestHandler::OnPrepareRender()
{
    glLoadMatrixf(RotateZ(_rotation));
}

void TestHandler::OnRender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glVertexPointer(2, GL_FLOAT, sizeof(GLfloat) * 4, _vertices.data());
    glTexCoordPointer(2, GL_FLOAT, sizeof(GLfloat) * 4, _vertices.data() + 2);
    glDrawArrays(GL_QUADS, 0, _vertices.size() / 4);
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
    glMatrixMode(GL_PROJECTION);
    WindowEventHandler::OnResize(width, height);
    auto ratio = float(width) / float(height);
    _projectionMatrix = Orthographic(4.0f, ratio);
    glLoadMatrixf(_projectionMatrix);
    glMatrixMode(GL_MODELVIEW);
}
