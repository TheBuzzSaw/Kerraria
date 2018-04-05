#include "TestHandler.hpp"
#include "Debug.hpp"
#include <SDL_image.h>
#include <fstream>
#include <sstream>
#include <ctime>
using namespace std;

static constexpr auto PixelFormat = SDL_PIXELFORMAT_ABGR8888;
static constexpr float Delta = 1.0f / 8.0f;
static constexpr float PixelsPerSpace = 64.0f;

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
    //int offset = index * 72;
    int offset = index * 64;

    return {
        offset / 1024.0f,
        (offset + 64) / 1024.0f};
}

static void LoadTexture(const char* path)
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

static GLuint LoadShader(const char* source, GLenum shaderType)
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

static GLuint LoadProgram(
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

static string FileToString(const char* path)
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

static GLuint LoadProgramFromFiles(
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
    : _mt(time(nullptr))
{
    _vertexData.reserve(1024);

    _grid.minor = 128;
    _grid.major = 256;
    _tiles.resize(_grid.Count(), 0xff);
    _grid.data = _tiles.data();
    _tileViewCenter = {
        static_cast<float>(_grid.major / 2),
        static_cast<float>(_grid.minor / 2)};

    normal_distribution<double> slopeDistribution(0.0, 2.0);
    double previousSlope = 0.0;
    double previousHeight = double(_grid.minor) / 2.0;
    auto middle = previousHeight;
    int step = 8;

    uniform_int_distribution<Uint8> grassTopDist(1, 5);
    uniform_int_distribution<Uint8> stoneDist(0x11, 0x15);
    
    for (int i = 0; i < _grid.major; i += step)
    {
        double randomSlope = slopeDistribution(_mt) +
            (previousHeight > middle ? -1.0 : 1.0);
        double slope = (randomSlope + previousSlope) / 2.0;
        double height = slope * double(step) + previousHeight;
        
        for (int j = 0; j < step; ++j)
        {
            double midHeight = double(j) * slope + previousHeight;
            auto n = min<int>(_grid.minor, int(midHeight));
            n = max<int>(n, 1);
            
            for (int k = 0; k < n; ++k)
            {
                _grid(i + j, n - 1 - k) = k ? stoneDist(_mt) : grassTopDist(_mt);
            }
        }
        
        previousSlope = slope;
        previousHeight = height;
    }

    _program = LoadProgramFromFiles(
#ifdef KerrariaES2
        "es2.vertex.shader",
        "es2.fragment.shader");
#else
        "vertex.shader",
        "fragment.shader");
#endif

    _matrixUniform = glGetUniformLocation(_program, "theMatrix");
    _textureUniform = glGetUniformLocation(_program, "theTexture");
    _positionAttribute = glGetAttribLocation(_program, "position");
    _colorAttribute = glGetAttribLocation(_program, "color");
    _textureCoordinateAttribute = glGetAttribLocation(_program, "textureCoordinates");

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    SetParams(TexParams);
    LoadTexture("images/sheet.png");
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
        Restricted(_tileViewCenter.x, halfSpace.x, _grid.major - halfSpace.x),
        Restricted(_tileViewCenter.y, halfSpace.y, _grid.minor - halfSpace.y)};

    auto tileViewOffset = (center - halfSpace)
        .Cast<int>()
        .Restricted(
            0,
            _grid.major - _tileViewSize.x,
            0,
            _grid.minor - _tileViewSize.y);

    for (int j = 0; j < _tileViewSize.x; ++j)
    {
        for (int i = 0; i < _tileViewSize.y; ++i)
        {
            uint8_t tile = _grid(tileViewOffset.x + j, tileViewOffset.y + i);
            if (tile == 0xff) continue;
            int xi = tile & 0xf;
            int yi = (tile >> 4) & 0xf;

            auto tcs = GetTexCoords(xi);
            auto tct = GetTexCoords(yi);

            // I don't know how else to close the gaps.
            constexpr float Lip = 1.0f / 1024.0f;
            auto x = static_cast<float>(j) - Lip;
            auto y = static_cast<float>(i) - Lip;
            auto xx = static_cast<float>(j + 1) + Lip;
            auto yy = static_cast<float>(i + 1) + Lip;

            _vertexData.push_back(x);
            _vertexData.push_back(y);
            _vertexData.push_back(tcs.first);
            _vertexData.push_back(tct.second);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);

            _vertexData.push_back(x);
            _vertexData.push_back(yy);
            _vertexData.push_back(tcs.first);
            _vertexData.push_back(tct.first);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);

            _vertexData.push_back(xx);
            _vertexData.push_back(yy);
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

            _vertexData.push_back(xx);
            _vertexData.push_back(yy);
            _vertexData.push_back(tcs.second);
            _vertexData.push_back(tct.first);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);
            _vertexData.push_back(1.0f);

            _vertexData.push_back(xx);
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

    glUniformMatrix4fv(
        _matrixUniform,
        1,
        GL_FALSE,
        _projectionMatrix * _rotateMatrix);


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
    if (_panAnchor.x < 0) _tileViewCenter += _delta * _multiplier;
    _rotation -= (1.0f / 128.0f);
}

void TestHandler::OnKeyDown(SDL_Keysym keysym)
{
    WindowEventHandler::OnKeyDown(keysym);
    switch (keysym.sym)
    {
        case SDLK_l:
            _logStats = !_logStats;
            break;

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

        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            _multiplier = 12.0f;
            break;

        case SDLK_LCTRL:
        case SDLK_RCTRL:
            _multiplier = 0.25f;
            break;

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
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            if (_multiplier > 1.0f) _multiplier = 1.0f;
            break;

        case SDLK_LCTRL:
        case SDLK_RCTRL:
            if (_multiplier < 1.0f) _multiplier = 1.0f;
            break;

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

void TestHandler::OnMouseMove(SDL_MouseMotionEvent event)
{
    if (_panAnchor.x >= 0)
    {
        Point<int> mouse{event.x, event.y};
        auto delta = (_panAnchor - mouse).Cast<float>() / PixelsPerSpace;
        delta.y = -delta.y;
        _tileViewCenter = _tileViewCenterAnchor + delta;
    }
}

void TestHandler::OnMouseWheel(SDL_MouseWheelEvent event)
{
    (void)event;
}

void TestHandler::OnMouseButtonDown(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT)
    {
        _panAnchor = {event.x, event.y};
        _tileViewCenterAnchor = _tileViewCenter;
    }
    else if (event.button == SDL_BUTTON_RIGHT)
    {
        Point<int> position = {event.x, _displaySize.y - 1 - event.y};
        auto halfSpace = _tileViewSpace / 2.0f;
        auto spaceOffset = position.Cast<float>() / PixelsPerSpace - halfSpace;
        auto worldCoordinates = (_tileViewCenter + spaceOffset).Cast<int>();
        
        if (worldCoordinates.x >= 0 &&
            worldCoordinates.x < _grid.major &&
            worldCoordinates.y >= 0 &&
            worldCoordinates.y < _grid.minor)
        {
            _grid(worldCoordinates.x, worldCoordinates.y) = 0x41;
        }
        else
        {
            Log() << "out of bounds -- " << worldCoordinates << '\n';
        }
    }
}

void TestHandler::OnMouseButtonUp(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_LEFT)
    {
        _panAnchor = {-1, -1};
    }
}

void TestHandler::OnResize(Sint32 width, Sint32 height)
{
    _displaySize = {width, height};

    WindowEventHandler::OnResize(width, height);
    _tileViewSpace = {
        float(width) / PixelsPerSpace,
        float(height) / PixelsPerSpace};

    _tileViewSize = (_tileViewSpace.Cast<int>() + Point<int>{2, 2})
        .Restricted(1, _grid.major, 1, _grid.minor);

    auto halfSpace = _tileViewSpace / 2.0f;
    _projectionMatrix = Orthographic(
        -halfSpace.x,
        halfSpace.x,
        -halfSpace.y,
        halfSpace.y,
        1.0f,
        -1.0f);
}

