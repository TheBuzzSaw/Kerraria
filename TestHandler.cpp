#include "TestHandler.hpp"
#include "Debug.hpp"
#include <SDL_image.h>
#include <fstream>
#include <sstream>
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

GLuint LoadShader(const char* source, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        string errors;
        errors.resize(length);
        glGetShaderInfoLog(shader, length, &length, &errors[0]);

        Log() << "-- shader compilation errors --\n" << errors << '\n';
    }
    else
    {
        Log() << "successfully compiled shader\n";
    }

    return shader;
}

GLuint LoadProgram(
    const char* vertexShaderSource,
    const char* fragmentShaderSource)
{
    GLuint result = glCreateProgram();

     GLuint vertexShader =
        LoadShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader =
        LoadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(result, vertexShader);
    glAttachShader(result, fragmentShader);
    glLinkProgram(result);

    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    return result;
}

string FileToString(const char* path)
{
    string result;

    if (path && *path)
    {
        ifstream stream(path, ifstream::binary);

        if (stream)
        {
            ostringstream oss;
            oss << stream.rdbuf();
            stream.close();
            result = oss.str();
        }
    }

    return result;
}

GLuint LoadProgramFromFiles(
    const char* vertexShaderPath,
    const char* fragmentShaderPath)
{
    string vertexShaderSource = FileToString(vertexShaderPath);
    string fragmentShaderSource = FileToString(fragmentShaderPath);

    return LoadProgram(
        vertexShaderSource.c_str(),
        fragmentShaderSource.c_str());
}

TestHandler::TestHandler()
{
    _program = LoadProgramFromFiles(
        "vertex.shader",
        "fragment.shader");

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    SetParams(TexParams);
    LoadTexture("../tiles_spritesheet.png");
    glDisable(GL_TEXTURE_2D);

    uniform_int_distribution<int> d(1, 8);
    constexpr int N = 16;

    for (int i = -N; i < N; ++i)
    {
        for (int j = -N; j < 0; ++j)
        {
            auto tcs = GetTexCoords(d(_mt));
            auto tct = GetTexCoords(d(_mt));

            _vertices.push_back(i);
            _vertices.push_back(j + 1);
            _vertices.push_back(tcs.first);
            _vertices.push_back(tct.first);

            _lights.push_back(1.0f);
            _lights.push_back(1.0f);
            _lights.push_back(1.0f);

            _vertices.push_back(i + 1);
            _vertices.push_back(j + 1);
            _vertices.push_back(tcs.second);
            _vertices.push_back(tct.first);

            _lights.push_back(1.0f);
            _lights.push_back(1.0f);
            _lights.push_back(1.0f);

            _vertices.push_back(i + 1);
            _vertices.push_back(j);
            _vertices.push_back(tcs.second);
            _vertices.push_back(tct.second);

            _lights.push_back(0.0f);
            _lights.push_back(1.0f);
            _lights.push_back(0.0f);

            _vertices.push_back(i);
            _vertices.push_back(j);
            _vertices.push_back(tcs.first);
            _vertices.push_back(tct.second);

            _lights.push_back(0.0f);
            _lights.push_back(0.0f);
            _lights.push_back(1.0f);
        }
    }
}

TestHandler::~TestHandler()
{
    glDeleteTextures(1, &_texture);
    glDeleteProgram(_program);
}

void TestHandler::OnOpen()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void TestHandler::OnClose()
{
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void TestHandler::OnPrepareRender()
{
    _rotateMatrix = RotateZ(_rotation);
}

void TestHandler::OnRender()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadMatrixf(_rotateMatrix);
    constexpr auto Stride = sizeof(GLfloat) * 4;
    glVertexPointer(2, GL_FLOAT, Stride, _vertices.data());
    glColorPointer(3, GL_FLOAT, 0, _lights.data());
    glTexCoordPointer(2, GL_FLOAT, Stride, _vertices.data() + 2);
    glDrawArrays(GL_QUADS, 0, _vertices.size() / 4);
}

void TestHandler::OnUpdate()
{
    _rotation -= (1.0f / 128.0f);

    static int n = 0;
    n = (n + 1) % 30;
    //if (!n)
    {
        uniform_real_distribution<float> d(0.0f, 1.0f);

        for (auto& v : _lights) v = d(_mt);
    }
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
