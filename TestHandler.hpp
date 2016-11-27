#ifndef TestHandler_hpp
#define TestHandler_hpp

#include "WindowEventHandler.hpp"
#include "Matrix4x4.hpp"
#include "OpenGL.hpp"
#include "Rectangle.hpp"
#include "Span.hpp"
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
    Span2D<uint8_t> _grid;
    Point<int> _panAnchor = {-1, -1};
    Point<int> _displaySize = {};
    Point<int> _tileViewSize = {};
    Point<float> _tileViewSpace = {};
    Point<float> _tileViewCenter = {};
    Point<float> _tileViewCenterAnchor = {};
    Point<float> _delta = {};
    float _multiplier = 1.0f;
    std::vector<GLfloat> _vertexData;
    std::vector<uint8_t> _tiles;
    bool _logDump = false;

public:
    TestHandler();
    virtual ~TestHandler();

    void OnOpen() override;
    void OnClose() override;
    void OnPrepareRender() override;
    void OnRender() override;
    void OnUpdate() override;

    void OnKeyDown(SDL_Keysym keysym) override;
    void OnKeyUp(SDL_Keysym keysym) override;

    /// Mouse events
    void OnMouseMove(SDL_MouseMotionEvent event) override;
    void OnMouseWheel(SDL_MouseWheelEvent event) override;
    void OnMouseButtonDown(SDL_MouseButtonEvent event) override;
    void OnMouseButtonUp(SDL_MouseButtonEvent event) override;

    void OnResize(Sint32 width, Sint32 height) override;
};

#endif
