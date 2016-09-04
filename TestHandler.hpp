#ifndef TestHandler_hpp
#define TestHandler_hpp

#include "WindowEventHandler.hpp"
#include "Matrix4x4.hpp"
#include <SDL_opengl.h>
#include <vector>

class TestHandler : public WindowEventHandler
{
    Matrix4x4F _projectionMatrix;
    float _rotation = 0.0f;
    GLuint _texture;
    std::vector<GLfloat> _vertices;

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
