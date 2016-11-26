#include "TestHandler.hpp"
#include "Debug.hpp"
#include <SDL_image.h>
#include <fstream>
#include <sstream>
using namespace std;

static constexpr auto PixelFormat = SDL_PIXELFORMAT_ABGR8888;
static constexpr float Delta = 1.0f / 8.0f;

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
    GLuint program = glCreateProgram();

     GLuint vertexShader =
        LoadShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader =
        LoadShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint isLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        string errors;
        errors.resize(length);
        glGetProgramInfoLog(program, length, &length, &errors[0]);

        Log() << "-- program linker errors --\n" << errors << '\n';
    }
    else
    {
        Log() << "successfully linked shader program\n";
    }

    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    return program;
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
    _vertexData.reserve(1024);

    _grid.width = 256;
    _grid.height = 64;
    _tiles.resize(_grid.Count(), 0xff);
    _grid.data = _tiles.data();
    _tileViewCenter = {
        static_cast<float>(_grid.width / 2),
        static_cast<float>(_grid.height / 2)};

    for (int i = 0; i < _grid.height / 2; ++i)
    {
        for (int j = 0; j < _grid.width; ++j)
        {
            _grid(j, i) = 0xc8;
        }
    }

    uniform_int_distribution<int> d(0, 8);

    for (int i = 0; i < _grid.width; ++i)
    {
        int size = d(_mt);
        for (int j = 0; j < size; ++j)
        {
            _grid(i, _grid.height / 2 + j) = 0xc8;
        }

        _grid(i, _grid.height / 2 + size) = 0x87;
    }

    _program = LoadProgramFromFiles(
        "vertex.shader",
        "fragment.shader");

    _textureUniform = glGetUniformLocation(_program, "theTexture");
    _positionAttribute = glGetAttribLocation(_program, "position");
    _colorAttribute = glGetAttribLocation(_program, "color");
    _textureCoordinateAttribute = glGetAttribLocation(_program, "textureCoordinates");

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
    glDeleteProgram(_program);
}

void TestHandler::OnOpen()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glUseProgram(_program);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnableVertexAttribArray(_positionAttribute);
    glEnableVertexAttribArray(_colorAttribute);
    glEnableVertexAttribArray(_textureCoordinateAttribute);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glUniform1i(_textureUniform, 0);
}

void TestHandler::OnClose()
{
    glDisableVertexAttribArray(_textureCoordinateAttribute);
    glDisableVertexAttribArray(_colorAttribute);
    glDisableVertexAttribArray(_positionAttribute);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glUseProgram(0);
}

void TestHandler::OnPrepareRender()
{
    _vertexData.clear();
    auto halfSpace = _tileViewSpace / 2.0f;
    Point<float> center = {
        Restricted(_tileViewCenter.x, halfSpace.x, _grid.width - halfSpace.x),
        Restricted(_tileViewCenter.y, halfSpace.y, _grid.height - halfSpace.y)};

    auto tileViewOffset = (center - halfSpace)
        .Cast<int>()
        .Restricted(
            0,
            _grid.width - _tileViewSize.x,
            0,
            _grid.height - _tileViewSize.y);

    for (int i = 0; i < _tileViewSize.y; ++i)
    {
        for (int j = 0; j < _tileViewSize.x; ++j)
        {
            uint8_t tile = _grid(tileViewOffset.x + j, tileViewOffset.y + i);
            if (tile == 0xff) continue;
            int xi = tile & 0xf;
            int yi = (tile >> 4) & 0xf;

            auto tcs = GetTexCoords(xi);
            auto tct = GetTexCoords(yi);

            auto x = static_cast<float>(j);
            auto y = static_cast<float>(i);

            _vertexData.push_back(x);
            _vertexData.push_back(y);
            _vertexData.push_back(tcs.first);
            _vertexData.push_back(tct.second);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);

            _vertexData.push_back(x);
            _vertexData.push_back(y + 1.0f);
            _vertexData.push_back(tcs.first);
            _vertexData.push_back(tct.first);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);

            _vertexData.push_back(x + 1.0f);
            _vertexData.push_back(y + 1.0f);
            _vertexData.push_back(tcs.second);
            _vertexData.push_back(tct.first);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);

            _vertexData.push_back(x);
            _vertexData.push_back(y);
            _vertexData.push_back(tcs.first);
            _vertexData.push_back(tct.second);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);

            _vertexData.push_back(x + 1.0f);
            _vertexData.push_back(y + 1.0f);
            _vertexData.push_back(tcs.second);
            _vertexData.push_back(tct.first);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);

            _vertexData.push_back(x + 1.0f);
            _vertexData.push_back(y);
            _vertexData.push_back(tcs.second);
            _vertexData.push_back(tct.second);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);
        }
    }

    auto translation = -center + tileViewOffset.Cast<float>();

    _rotateMatrix =
        //RotateZ(_rotation) *
        Translate(
            translation.x,
            translation.y,
            0.0f);
    glLoadMatrixf(_rotateMatrix);

    if (_logDump)
    {
        _logDump = false;
        Log() << "halfSpace -- " << halfSpace << '\n';
        Log() << "center -- " << center << '\n';
        Log() << "translation -- " << translation << '\n';
    }
}

void TestHandler::OnRender()
{
    glClear(GL_COLOR_BUFFER_BIT);

    constexpr auto Stride = sizeof(GLfloat) * 7;
    auto data = _vertexData.data();

    glVertexAttribPointer(
        _positionAttribute,
        2,
        GL_FLOAT,
        GL_FALSE,
        Stride,
        data);
    glVertexAttribPointer(
        _textureCoordinateAttribute,
        2,
        GL_FLOAT,
        GL_FALSE,
        Stride,
        data + 2);
    glVertexAttribPointer(
        _colorAttribute,
        3,
        GL_FLOAT,
        GL_FALSE,
        Stride,
        data + 4);

    glDrawArrays(GL_TRIANGLES, 0, _vertexData.size() / 7);
}

void TestHandler::OnUpdate()
{
    _tileViewCenter += _delta;
    _rotation -= (1.0f / 128.0f);
}

void TestHandler::OnKeyDown(SDL_Keysym keysym)
{
    WindowEventHandler::OnKeyDown(keysym);
    switch (keysym.sym)
    {
        case SDLK_BACKSLASH:
            //SDL_Delay(750);
            _logDump = true;
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

        case SDLK_a:
        case SDLK_LEFT:
            _delta.x = -Delta;
            break;
        case SDLK_d:
        case SDLK_RIGHT:
            _delta.x = Delta;
            break;
        case SDLK_w:
        case SDLK_UP:
            _delta.y = Delta;
            break;
        case SDLK_s:
        case SDLK_DOWN:
            _delta.y = -Delta;
            break;

        default: break;
    }
}

void TestHandler::OnKeyUp(SDL_Keysym keysym)
{
    WindowEventHandler::OnKeyUp(keysym);
    switch (keysym.sym)
    {
        case SDLK_a:
        case SDLK_LEFT:
            if (_delta.x < 0.0f) _delta.x = 0.0f;
            break;
        case SDLK_d:
        case SDLK_RIGHT:
            if (_delta.x > 0.0f) _delta.x = 0.0f;
            break;
        case SDLK_w:
        case SDLK_UP:
            if (_delta.y > 0.0f) _delta.y = 0.0f;
            break;
        case SDLK_s:
        case SDLK_DOWN:
            if (_delta.y < 0.0f) _delta.y = 0.0f;
            break;
    }
}

void TestHandler::OnResize(Sint32 width, Sint32 height)
{
    constexpr float Radius = 8.0f;
    constexpr float Diameter = Radius * 2.0f;

    glMatrixMode(GL_PROJECTION);
    WindowEventHandler::OnResize(width, height);
    auto ratio = float(width) / float(height);

    if (width > height)
    {
        _tileViewSpace = {Diameter * ratio, Diameter};
    }
    else
    {
        _tileViewSpace = {Diameter, Diameter / ratio};
    }

    _tileViewSize = (_tileViewSpace.Cast<int>() + Point<int>{2, 2})
        .Restricted(1, _grid.width, 1, _grid.height);
    _projectionMatrix = Orthographic(Radius, ratio);
    glLoadMatrixf(_projectionMatrix);
    glMatrixMode(GL_MODELVIEW);
}
