#ifndef TestHandler_hpp
#define TestHandler_hpp

#include "WindowEventHandler.hpp"
#include "Matrix4x4.hpp"
#include <SDL_opengl.h>

class TestHandler : public WindowEventHandler
{
    Matrix4x4F _projectionMatrix;
    GLfloat _vertices[6];
    float _rotation = 0.0f;

public:
    TestHandler();
    virtual ~TestHandler();

    virtual void OnPrepareRender();
    virtual void OnRender();
    virtual void OnUpdate();

    virtual void OnResize(Sint32 width, Sint32 height);
};

#endif
