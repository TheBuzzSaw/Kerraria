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

static pair<float, float> GetTexCoords(int index)
{
    int offset = index * 72;

    return {
        offset / 1024.0f,
        (offset + 70) / 1024.0f};
}

void LoadTexture(const char* path)
{
    auto surface = IMG_Load(path);

    if (surface)
    {
        if (surface->format->format != PixelFormat)
        {
            Log() << "Converting format for " << path << '\n';
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
        else
        {
            Log() << "Failed to convert image format for " << path << '\n';
        }

        SDL_FreeSurface(surface);
    }
    else
    {
        Log() << "Failed to load image " << path << '\n';
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

    uniform_int_distribution<int> d(1, 8);

    for (int i = -6; i < 6; ++i)
    {
        for (int j = -6; j < 0; ++j)
        {
            auto tcs = GetTexCoords(d(_mt));
            auto tct = GetTexCoords(d(_mt));

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

    constexpr int EdgeCount = 32;
    float radius = 2.0f;
    _lightVertices[0].push_back(0.0f);
    _lightVertices[0].push_back(0.0f);
    _lightVertices[0].push_back(1.0f);
    _lightVertices[0].push_back(0.0f);
    _lightVertices[0].push_back(0.0f);
    _lightVertices[0].push_back(1.0f);

    _lightVertices[1].push_back(0.0f);
    _lightVertices[1].push_back(0.0f);
    _lightVertices[1].push_back(0.0f);
    _lightVertices[1].push_back(1.0f);
    _lightVertices[1].push_back(0.0f);
    _lightVertices[1].push_back(1.0f);

    for (int i = 0; i <= EdgeCount; ++i)
    {
        float radians = Tau<float>() * float(i) / float(EdgeCount);

        _lightVertices[0].push_back(sin(radians) * radius);
        _lightVertices[0].push_back(cos(radians) * radius);
        _lightVertices[0].push_back(1.0f);
        _lightVertices[0].push_back(0.0f);
        _lightVertices[0].push_back(0.0f);
        _lightVertices[0].push_back(1.0f);

        _lightVertices[1].push_back(sin(radians) * radius);
        _lightVertices[1].push_back(cos(radians) * radius);
        _lightVertices[1].push_back(0.0f);
        _lightVertices[1].push_back(1.0f);
        _lightVertices[1].push_back(0.0f);
        _lightVertices[1].push_back(1.0f);
    }
}

TestHandler::~TestHandler()
{
    glDeleteTextures(1, &_texture);
}

void TestHandler::OnOpen()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_BLEND);
    glEnableClientState(GL_VERTEX_ARRAY);
}

void TestHandler::OnClose()
{
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_BLEND);
}

void TestHandler::OnPrepareRender()
{
    _rotateMatrix = RotateZ(_rotation);

    auto n = sin(float(_lightStep) * Tau<float>() / 480.0f) * 4.0f;
    _lightMatrix[0] = Translate(n, 0.0f, 0.0f);
    _lightMatrix[1] = Translate(0.0f, n, 0.0f);
}

void TestHandler::OnRender()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // Render lights.
    glBlendFunc(GL_ONE, GL_ONE);
    glEnableClientState(GL_COLOR_ARRAY);

    for (int i = 0; i < 2; ++i)
    {
        glLoadMatrixf(_lightMatrix[i]);
        glVertexPointer(
            2, GL_FLOAT, sizeof(GLfloat) * 6, _lightVertices[i].data());
        glColorPointer(
            4, GL_FLOAT, sizeof(GLfloat) * 6, _lightVertices[i].data() + 2);
        glDrawArrays(GL_TRIANGLE_FAN, 0, _lightVertices[i].size() / 6);
    }

    glDisableClientState(GL_COLOR_ARRAY);

    // Render tiles.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glLoadMatrixf(_rotateMatrix);
    glVertexPointer(2, GL_FLOAT, sizeof(GLfloat) * 4, _vertices.data());
    glTexCoordPointer(2, GL_FLOAT, sizeof(GLfloat) * 4, _vertices.data() + 2);
    glDrawArrays(GL_QUADS, 0, _vertices.size() / 4);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

void TestHandler::OnUpdate()
{
    _rotation -= (1.0f / 128.0f);
    ++_lightStep;
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
        {
            auto flag = SDL_GetWindowFlags(
                Window()) & SDL_WINDOW_FULLSCREEN_DESKTOP;
            SDL_SetWindowFullscreen(
                Window(),
                flag ^ SDL_WINDOW_FULLSCREEN_DESKTOP);
            break;
        }
        default: break;
    }
}

void TestHandler::OnResize(Sint32 width, Sint32 height)
{
    glMatrixMode(GL_PROJECTION);
    WindowEventHandler::OnResize(width, height);
    auto ratio = float(width) / float(height);
    _projectionMatrix = Orthographic(8.0f, ratio);
    glLoadMatrixf(_projectionMatrix);
    glMatrixMode(GL_MODELVIEW);
}
