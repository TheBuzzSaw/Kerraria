#ifndef TestHandler_hpp
#define TestHandler_hpp

#include "WindowEventHandler.hpp"
#include "Matrix4x4.hpp"
#include "OpenGL.hpp"
#include "Grid.hpp"
#include <vector>
#include <random>

class TestHandler : public WindowEventHandler
{
    std::mt19937 _mt;
    Matrix4x4F _projectionMatrix;
    Matrix4x4F _rotateMatrix;
    float _rotation = 0.0f;
    GLuint _texture;
    GLuint _program;
    GLint _textureUniform;
    GLint _positionAttribute;
    GLint _colorAttribute;
    GLint _textureCoordinateAttribute;
    Grid<uint8_t> _grid;
    std::vector<GLfloat> _vertexData;
    std::vector<uint8_t> _tiles;
    std::vector<GLfloat> _vertices;
    std::vector<GLfloat> _lights;

public:
    TestHandler();
    virtual ~TestHandler();

    virtual void OnOpen();
    virtual void OnClose();
    virtual void OnPrepareRender();
    virtual void OnRender();
    virtual void OnUpdate();

    virtual void OnKeyDown(SDL_Keysym keysym);

    virtual void OnResize(Sint32 width, Sint32 height);
};

#endif
