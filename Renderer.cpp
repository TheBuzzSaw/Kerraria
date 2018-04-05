#include "Renderer.hpp"
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

Renderer::Renderer()
{
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

Renderer::~Renderer()
{
    glDeleteTextures(1, &_texture);
    glDeleteProgram(_program);
}

void Renderer::Render(const RenderGridBuffer& buffer)
{
    // --- Open() ---

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
    glUniformMatrix4fv(_matrixUniform, 1, GL_FALSE, buffer.matrix);

    // --- Render() ---

    glClear(GL_COLOR_BUFFER_BIT);

    constexpr auto Stride = sizeof(GLfloat) * 7;
    auto data = buffer.vertexData.data();

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

    glDrawArrays(GL_TRIANGLES, 0, buffer.vertexData.size() / 7);

    // --- Close() ---

    glDisableVertexAttribArray(_textureCoordinateAttribute);
    glDisableVertexAttribArray(_colorAttribute);
    glDisableVertexAttribArray(_positionAttribute);
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glUseProgram(0);
}
